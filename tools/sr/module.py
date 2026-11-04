"""The game's source files: one per original object (a link unit in src/link/LAYOUT.JSON).

Each file has one command line: the unit's recorded `flags`, plus `/NT S<region>_TEXT`
for C (an assembler file names its code segment in its `.CODE` directive). `./sr check`
and `./sr build --layout` compile the file as it is in git with that line. The module gate
compares the file's whole span with the image: bytes between functions, internal near-call
displacements, internal calls and code-segment offsets. References to other objects and
DGROUP stay relocation-masked and are checked by `refs`.
"""

import json

from . import config
from .disasm import branch_target, linear

MODS = config.SRC / "mods"
PLAN = config.SRC / "link" / "LAYOUT.JSON"


def fname(row):
    return row["name"] or f"F_{row['addr']:05X}"


class File:
    """A code unit of LAYOUT.JSON: one source file, one object."""

    def __init__(self, unit, code_starts):
        self.unit = unit
        self.rel = unit["sources"][0]
        self.path = config.ROOT / self.rel
        self.kind = unit["kind"]
        self.start, self.end = unit["start"], unit["end"]
        self.flags = unit["flags"]
        self.segment = f"S{max(s for s in code_starts if s <= self.start) // 16:04X}_TEXT"

    @property
    def command(self):
        """The CL or MASM options this file is built with (the MAKEFILE line)."""
        return self.with_flags(self.flags)

    def with_flags(self, flags):
        return f"{flags} /NT{self.segment}" if self.kind == "c" else flags


def plan():
    return json.loads(PLAN.read_text())


def write_plan(p):
    PLAN.write_text(json.dumps(p, indent=2) + "\n")


def files(p=None):
    p = p or plan()
    return [File(u, p["code_starts"]) for u in p["units"]]


def file_of(row, fs=None):
    """The source file that defines a function (its DB `src`)."""
    for f in fs or files():
        if f.rel == row["src"]:
            return f
    raise SystemExit(f"{fname(row)}: src {row['src']!r} is not a source file of src/link/LAYOUT.JSON")


def find_file(name, fs=None):
    """A source file by its name (M_0914C.C) or path, or None."""
    for f in fs or files():
        if name.upper() in (f.rel.upper(), f.path.name.upper()):
            return f
    return None


def members(con, src):
    return con.execute("SELECT * FROM functions WHERE src = ? ORDER BY addr", (src,)).fetchall()


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
            # An EXTDEF near call to a function of another object. Its raw displacement
            # is zero, which looks like an in-span call until the linker fills it.
            if f.self_rel and f.pos > 0 and obj.code[f.pos - 1] == 0xE8:
                g.nearcalls += 1
        skip.update(range(f.pos, f.pos + f.size))
    for ins in linear(obj.code, lo, hi, ip0=off0 + lo):
        t = branch_target(ins)
        if (ins.bytes[0] == 0xE8 and ins.size == 3 and ins.addr + 1 not in skip
                and t is not None and not off0 + lo <= t < off0 + hi):
            g.nearcalls += 1                 # a near call resolved to a callee outside the file
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
        if owns:
            g.problems.extend(problems)
        else:
            # a file that defines no data: only its literal pool, reported, not gated
            g.notes.extend(problems)
    return g
