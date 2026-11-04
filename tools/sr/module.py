"""Contiguous C or MASM compilation groups under src/mods/.

Members first match individually. The module gate verifies their positions, intervening
bytes, internal near-call displacements and code-segment fixups. External relocations
remain reference-checked; external near calls are counted as outstanding layout work.
Groups may be parts of original files, whose historical boundaries are not established.
"""

import re

from . import config, db
from .disasm import branch_target, linear

MODS = config.SRC / "mods"
_NAME = re.compile(r"^[A-Z0-9_]{1,8}\.(C|ASM)$")
_HEAD = re.compile(r"^/\* (\w+) @ [0-9A-Fa-f]{4}:[0-9A-Fa-f]{4} \(linear 0x([0-9A-Fa-f]{5})", re.M)
_HOIST = re.compile(r"^(#include .*|static [^;{]*\bnear\b[^;{]*\([^;{]*\);)\s*$")
# a file-local uninitialized variable: every member source that uses it repeats the definition,
# the merged file has it once at the top (its position only breaks _BSS hash ties)
_STATIC = re.compile(r"^static [^;{}=()]*;(\s*/\*.*\*/)?\s*$")


def fname(row):
    return row["name"] or f"F_{row['addr']:05X}"


def members(con, name):
    return con.execute("SELECT * FROM functions WHERE module = ? ORDER BY addr", (name,)).fetchall()


def path(name):
    return MODS / name


# ---------------------------------------------------------------- evidence


def same_module_edges(con, im):
    """Near-call edges suggesting source groups within the same code segment.

    These are candidate spans, not proof of one C translation unit: explicit near
    interfaces can cross object files, and the first span includes MASM routines.
    """
    rows = con.execute("SELECT * FROM functions WHERE kind = 'c'").fetchall()
    by_addr = {r["addr"]: r for r in con.execute("SELECT * FROM functions")}
    edges = set()
    for r in rows:
        for ins in linear(im.img, r["addr"], r["addr"] + r["size"], ip0=r["off"]):
            if ins.bytes[0] != 0xE8 or ins.size != 3:
                continue
            t = r["seg"] * 16 + (branch_target(ins) & 0xFFFF)
            if r["addr"] <= t < r["addr"] + r["size"] or t not in by_addr:
                continue
            if by_addr[t]["far"] and im.img[ins.addr - 1] != 0x0E:
                continue
            edges.add((r["addr"], t))
    return edges


def units(con, im):
    """Candidate spans [lo, hi): hulls of connected near-call groups.

    Split at language, alignment and compiler limits; verify each resulting module.
    """
    spans = []
    for a, b in same_module_edges(con, im):
        lo, hi = min(a, b), max(a, b)
        end = con.execute("SELECT size FROM functions WHERE addr = ?", (hi,)).fetchone()[0]
        spans.append([lo, hi + end])
    spans.sort()
    out = []
    for lo, hi in spans:
        if out and lo < out[-1][1]:
            out[-1][1] = max(out[-1][1], hi)
        else:
            out.append([lo, hi])
    return out


# ---------------------------------------------------------------- source merge/split


def merge(texts):
    """Concatenate member sources in address order. `#include`s and static near forward
    declarations move to the top once; everything else stays with its function."""
    top, seen, bodies = [], set(), []
    for t in texts:
        keep = []
        for ln in t.splitlines():
            if _HOIST.match(ln) or _STATIC.match(ln):
                if ln.strip() not in seen:
                    seen.add(ln.strip())
                    top.append(ln.rstrip())
                continue
            keep.append(ln.rstrip())
        bodies.append("\n".join(keep).strip("\n"))
    inc = [ln for ln in top if ln.startswith("#include")]
    fwd = [ln for ln in top if not ln.startswith("#include")]
    head = "\n".join(inc) + ("\n\n" + "\n".join(fwd) if fwd else "")
    return head + "\n\n" + "\n\n".join(bodies) + "\n"


def split(text, rows):
    """Module text -> {addr: singleton text}. A member's chunk runs from the line after the
    previous member's closing brace to its own closing brace, so comments, pragmas and
    file-scope declarations just above a function stay with it."""
    lines = text.splitlines()
    inc = [ln for ln in lines if _HOIST.match(ln)]
    out, start = {}, 0
    for r in sorted(rows, key=lambda r: r["addr"]):
        name = fname(r)
        d = next((i for i in range(start, len(lines)) if not lines[i][:1].isspace()
                  and re.search(rf"\b{name}\s*\(", lines[i]) and not lines[i].rstrip().endswith(";")), None)
        if d is None:
            raise SystemExit(f"{name}: no definition found in the module text")
        e = next(i for i in range(d, len(lines)) if lines[i].startswith("}"))
        body = [ln for ln in lines[start : e + 1] if not _HOIST.match(ln)]
        out[r["addr"]] = "\n".join(inc) + "\n\n" + "\n".join(body).strip("\n") + "\n"
        start = e + 1
    return out


# ---------------------------------------------------------------- validation


def validate(con, rows, name):
    """Fail closed on anything that can't be one contiguous run of one source file."""
    if not _NAME.match(name):
        raise SystemExit(f"{name!r}: a module is named by its DOS 8.3 file name, e.g. GARAGE.C")
    rows = sorted(rows, key=lambda r: r["addr"])
    for r in rows:
        kind = "asm" if name.endswith(".ASM") else "c"
        if r["kind"] != kind:
            raise SystemExit(f"{fname(r)} is kind {r['kind']}; {name} holds {kind} functions")
        if r["status"] != "matched" or r["mismatches"]:
            raise SystemExit(f"{fname(r)} is {r['status']}: only byte-matched functions join a module")
        if r["module"] and r["module"] != name:
            raise SystemExit(f"{fname(r)} is already in {r['module']}")
    for a, b in zip(rows, rows[1:]):
        between = con.execute("SELECT * FROM functions WHERE addr > ? AND addr < ?",
                              (a["addr"], b["addr"])).fetchall()
        # A retired scanner false positive wholly inside the previous member is
        # not an intervening function. Keep its history in the DB, but require the
        # real member to cover its complete extent before excluding it here.
        between = [r for r in between if not (r["status"] == "skip"
                   and a["addr"] <= r["addr"]
                   and r["addr"] + r["size"] <= a["addr"] + a["size"])]
        if between:
            raise SystemExit(f"{', '.join(fname(x) for x in between)} sit between {fname(a)} and {fname(b)}: "
                             "a module is a contiguous run")
        gap = b["addr"] - (a["addr"] + a["size"])
        if gap > 1:
            raise SystemExit(f"{gap} bytes between {fname(a)} and {fname(b)} belong to no known function "
                             f"(0x{a['addr'] + a['size']:05x}): find out what they are first")
    return rows


def unit_flags(rows):
    """One CL line for the whole file: the union of the members' /O letters. A member
    matched with fewer letters only had no code those letters change (no loop for l, no
    intrinsic call for i); the module build re-verifies that."""
    if all(r["kind"] == "asm" for r in rows):
        flags = {r["flags"] or config.DEFAULT_ASM_FLAGS for r in rows}
        if len(flags) != 1:
            raise SystemExit("assembly module members must use the same MASM flags")
        return flags.pop()
    letters = set()
    for r in rows:
        m = re.search(r"/O(\w+)", r["flags"] or config.DEFAULT_FLAGS)
        letters.update(m.group(1) if m else "")
    order = "alsitdx"
    o = "".join(c for c in order if c in letters) + "".join(sorted(letters - set(order)))
    return f"/AM {'/O' + o + ' ' if o else ''}/Gs /FPi"


# ---------------------------------------------------------------- the module gate


class Gate:
    def __init__(self):
        self.data = None            # (base, size) of the module's _DATA in the original
        self.notes = []             # data layout differences of a module that owns no data yet
        self.mismatches = []
        self.masked = 0
        self.selfrefs = 0
        self.nearcalls = 0
        self.problems = []
        self.size_obj = self.size_exe = 0

    @property
    def ok(self):
        return not self.mismatches and not self.problems and self.size_obj == self.size_exe

    def line(self, name):
        v = "MODULE MATCH" if self.ok else "MODULE DIFF"
        s = (f"{name}: {v}: {len(self.mismatches)} mismatched bytes over the whole span, "
             f"size {self.size_obj} vs original {self.size_exe}, {self.masked} external fixup bytes masked, "
             f"{self.selfrefs} in-module references verified")
        if self.nearcalls:
            s += f", {self.nearcalls} near call(s) outside the module"
        for p in self.problems:
            s += "\n    " + p
        for p in self.notes:
            s += "\n    (data, not gated yet) " + p
        if self.mismatches:
            s += "\n    first mismatches at linear " + ", ".join(f"0x{a:05x}" for a in self.mismatches[:8])
        return s


def gate(obj, procs, img, rows, con=None):
    """Compare the members' whole span in the object with the original image."""
    from .cod import find

    g = Gate()
    rows = sorted(rows, key=lambda r: r["addr"])
    first, last = rows[0], rows[-1]
    p0, pn = find(procs, fname(first)), find(procs, fname(last))
    if p0 is None or pn is None:
        g.problems.append("member procedure missing from the object")
        return g
    base = first["addr"] - p0.start          # linear address of object offset 0
    for r in rows:
        p = find(procs, fname(r))
        if p is None or p.start + base != r["addr"]:
            g.problems.append(f"{fname(r)} is at object offset {hex(p.start) if p else 'missing'}, original needs "
                              f"{r['addr'] - base:#x} (order or size of an earlier member)")
    lo, hi = p0.start, pn.end if pn.end is not None else len(obj.code)
    end_exe = last["addr"] + last["size"]
    if hi - lo == end_exe - first["addr"] + 1 and obj.code[hi - 1] == 0x90 and img[end_exe] == 0x90:
        end_exe += 1                         # the trailing alignment nop
    g.size_obj, g.size_exe = hi - lo, end_exe - first["addr"]
    off0 = first["off"] - p0.start          # segment offset of object offset 0
    # Callers may reach members through different frames (the DB keeps the one they use),
    # so a code-segment offset is relative to the frame of the member that holds it.
    frames = [(find(procs, fname(r)).start, r["off"] - find(procs, fname(r)).start) for r in rows
              if find(procs, fname(r)) is not None]

    def frame_off(pos):
        return max((s for s in frames if s[0] <= pos), default=(0, off0))[1]

    skip = set()
    for f in obj.fixups:
        if f.seg != obj.code_seg or not lo <= f.pos < hi:
            continue
        own = obj.publics.get(obj.target_name(f)) if f.kind == "ext" else None
        if own is not None and lo <= own < hi and f.loc in (1, 5):
            # A call or a near pointer to another member: MSC leaves it to the linker as an
            # EXTDEF, so the object holds 0000. Resolve it inside the module instead.
            word = own + f.disp + (obj.code[f.pos] | obj.code[f.pos + 1] << 8) - (f.pos + 2 if f.self_rel else 0) + (0 if f.self_rel else frame_off(f.pos))
            have = img[base + f.pos] | img[base + f.pos + 1] << 8
            g.selfrefs += 1
            if word & 0xFFFF != have:
                g.problems.append(f"reference to {obj.target_name(f)} at 0x{base + f.pos:05x}: "
                                  f"ours {word & 0xFFFF:#06x}, original {have:#06x}")
        elif f.kind == "seg" and f.index == obj.code_seg and f.loc in (1, 5) and not f.self_rel:
            # An offset into our own code segment (a jump table, a near function pointer):
            # with the members placed as in the original it must equal the original word.
            want = (frame_off(f.pos) + f.disp + (obj.code[f.pos] | obj.code[f.pos + 1] << 8)) & 0xFFFF
            have = img[base + f.pos] | img[base + f.pos + 1] << 8
            g.selfrefs += 1
            if want != have:
                g.problems.append(f"code-segment fixup at 0x{base + f.pos:05x}: ours {want:#06x}, original {have:#06x}")
        else:
            g.masked += f.size
            # An EXTDEF may name a prepended placeholder. Its raw displacement
            # is zero, which looks like an in-span call until the linker fills it.
            if f.self_rel and f.pos > 0 and obj.code[f.pos - 1] == 0xE8:
                g.nearcalls += 1
        skip.update(range(f.pos, f.pos + f.size))
    for ins in linear(obj.code, lo, hi, ip0=off0 + lo):
        t = branch_target(ins)
        if (ins.bytes[0] == 0xE8 and ins.size == 3 and ins.addr + 1 not in skip
                and t is not None and not off0 + lo <= t < off0 + hi):
            g.nearcalls += 1                 # an .STB placeholder: the callee isn't in the module yet
            skip.update((ins.addr + 1, ins.addr + 2))
    for i in range(lo, min(hi, lo + g.size_exe)):
        if i not in skip and obj.code[i] != img[base + i]:
            g.mismatches.append(base + i)
    if con is not None:
        from .datacheck import check
        from .exe import image
        problems, g.data = check(obj, con, image(), base)
        dseg = obj.segments.index('_DATA') + 1 if '_DATA' in obj.segments else None
        owns = dseg is not None and any(s == dseg for s, _ in obj.all_publics.values())
        if owns and g.data is not None and g.data[0] % 2:
            # an odd original base: this file is the middle of a larger translation unit
            # (every object's _DATA starts word-aligned); the layout build checks the whole unit
            g.notes.extend(['odd base, verified by the layout build only: ' + p for p in problems])
        elif owns:
            g.problems.extend(problems)
        else:
            # a module that does not define its data yet: its literal pool may legitimately differ
            g.notes.extend(problems)
    return g


def set_members(con, name, rows):
    rel = str(path(name).relative_to(config.ROOT))
    for r in rows:
        db.update(con, r["addr"], module=name, src=rel)


def merge_asm(texts, rows):
    """Combine MASM sources; local label names are scoped by original function address.

    Each member keeps its DATA/CODE directives. Calls to members use their real PUBLIC
    definitions, not EXTRNs. No bytes, instruction encodings or alignment are inserted.
    """
    publics = set()
    for text in texts:
        publics.update(re.findall(r"^\s*PUBLIC\s+(\w+)", text, re.M | re.I))
    chunks = []
    for text, row in zip(texts, rows):
        labels = set(re.findall(r"^\s*(\w+):", text, re.M)) - publics
        for label in sorted(labels, key=len, reverse=True):
            if label.startswith(f"L{row['addr']:05X}_"):
                continue
            text = re.sub(rf"\b{re.escape(label)}\b", f"L{row['addr']:05X}_{label}", text)
        lines = []
        for line in text.splitlines():
            if re.match(r"^\s*(?:\.MODEL\b|END\s*(?:;.*)?$)", line, re.I):
                continue
            ext = re.match(r"^\s*EXTRN\s+(\w+):", line, re.I)
            if ext and ext.group(1) in publics:
                continue
            lines.append(line)
        chunks.append(f"; sr-member 0x{row['addr']:05X}\n" + "\n".join(lines).strip())
    model = next((m.group(0).strip() for m in [re.search(r"(?im)^\s*\.MODEL\s+\w+.*$", texts[0])] if m),
                 ".MODEL MEDIUM")
    return f"        {model}\n\n" + "\n\n".join(chunks) + "\n        END\n"


def split_asm(text, rows):
    parts = re.split(r"(?m)^; sr-member 0x([0-9A-Fa-f]+)\s*$", text)
    chunks = {int(parts[i], 16): parts[i + 1] for i in range(1, len(parts), 2)}
    out = {}
    for row in rows:
        body = re.sub(r"(?im)^\s*END\s*$", "", chunks[row["addr"]])
        declarations = []
        for other in rows:
            if other["addr"] != row["addr"]:
                # MASM members may export either spelling (or both aliases).
                symbols = re.findall(r"^\s*PUBLIC\s+(\w+)", chunks[other["addr"]], re.M | re.I)
                for sym in symbols:
                    if re.search(rf"\b{re.escape(sym)}\b", body):
                        declarations.append(f"        EXTRN {sym}:{'FAR' if other['far'] else 'NEAR'}")
        out[row["addr"]] = ("        .MODEL MEDIUM\n        .CODE\n" +
                            "\n".join(declarations) + "\n" + body.strip() + "\n        END\n")
    return out
