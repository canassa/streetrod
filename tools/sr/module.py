"""Modules: one source file under src/mods/ holding a run of matched functions, compiled
as one object and gated as a whole.

A module is the reconstruction of (part of) one original source file. Its members are
consecutive functions of the original image, all `matched` as singletons first. Built
together, calls between members become real `push cs; call near` / near calls, so the
module gate verifies every byte of the span: member bodies, the bytes between them,
near-call displacements and the code-segment offsets in jump tables. Only fixups to other
modules and to DGROUP stay masked; the per-function refs check resolves those.
"""

import re

from . import config, db
from .disasm import branch_target, linear

MODS = config.SRC / "mods"
_NAME = re.compile(r"^[A-Z0-9_]{1,8}\.C$")
_HEAD = re.compile(r"^/\* (\w+) @ [0-9A-Fa-f]{4}:[0-9A-Fa-f]{4} \(linear 0x([0-9A-Fa-f]{5})", re.M)
_HOIST = re.compile(r"^(#include .*|static [^;{]*\bnear\b[^;{]*\([^;{]*\);)\s*$")


def fname(row):
    return row["name"] or f"F_{row['addr']:05X}"


def members(con, name):
    return con.execute("SELECT * FROM functions WHERE module = ? ORDER BY addr", (name,)).fetchall()


def path(name):
    return MODS / name


# ---------------------------------------------------------------- evidence


def same_module_edges(con, im):
    """(caller, callee) pairs the original proves are in one source file: a near call to a
    near function, or `push cs; call near` to a far one. MSC only emits either for a
    function of the same compilation unit."""
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
    """Spans [lo, hi) that must each lie inside one source file: the hull of every
    connected group of same-module edges (the linker keeps a module's code together)."""
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
            if _HOIST.match(ln):
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
        if r["kind"] != "c":
            raise SystemExit(f"{fname(r)} is kind {r['kind']}; modules hold C functions")
        if r["status"] != "matched" or r["mismatches"]:
            raise SystemExit(f"{fname(r)} is {r['status']}: only byte-matched functions join a module")
        if r["module"] and r["module"] != name:
            raise SystemExit(f"{fname(r)} is already in {r['module']}")
    for a, b in zip(rows, rows[1:]):
        between = con.execute("SELECT * FROM functions WHERE addr > ? AND addr < ?",
                              (a["addr"], b["addr"])).fetchall()
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
            s += f", {self.nearcalls} near call(s) to a placeholder outside the module"
        for p in self.problems:
            s += "\n    " + p
        if self.mismatches:
            s += "\n    first mismatches at linear " + ", ".join(f"0x{a:05x}" for a in self.mismatches[:8])
        return s


def gate(obj, procs, img, rows):
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
            g.problems.append(f"{fname(r)} is at object offset {p and p.start:#x}, original needs "
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
        if own is not None and f.loc in (1, 5):
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
        skip.update(range(f.pos, f.pos + f.size))
    for ins in linear(obj.code, lo, hi, ip0=off0 + lo):
        t = branch_target(ins)
        if ins.bytes[0] == 0xE8 and ins.size == 3 and t is not None and not off0 + lo <= t < off0 + hi:
            g.nearcalls += 1                 # an .STB placeholder: the callee isn't in the module yet
            skip.update((ins.addr + 1, ins.addr + 2))
    for i in range(lo, min(hi, lo + g.size_exe)):
        if i not in skip and obj.code[i] != img[base + i]:
            g.mismatches.append(base + i)
    return g


def set_members(con, name, rows):
    rel = str(path(name).relative_to(config.ROOT))
    for r in rows:
        db.update(con, r["addr"], module=name, src=rel)
