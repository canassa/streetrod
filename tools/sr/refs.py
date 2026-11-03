"""Verify what every fixup in a compiled procedure refers to.

The byte compare masks fixups, so on its own it can't tell `g_score` from `g_count`.
Here each fixup is resolved against the original instruction:

- data (offset into DGROUP through an EXTDEF): the original's word must be the DB offset
  of that global, plus the displacement and the addend compiled into the instruction;
- far calls (ptr32 to an EXTDEF): the original's seg:off must be the function of that name;
- near calls to an EXTDEF (asm): the original's rel16 must land on the function of that name;
- our own data (string literals, statics): the bytes must equal the original's at that offset;
- DGROUP segment bases: the original's word must be the DGROUP paragraph.

Near calls the compiler resolved inside our OBJ (to an .STB placeholder) are checked by
name against the original call target.
"""

import struct

from . import config
from .disasm import branch_target, linear


# MSC 5.1 EM.LIB absolute symbols for the emulator's `int 34h`-`3Dh` patches.
_EMU = {"FIDRQQ": 0x5C32, "FIERQQ": 0x1632, "FIWRQQ": 0xA23D, "FICRQQ": 0x0E32, "FISRQQ": 0x0632,
        "FIARQQ": 0xFE32, "FJARQQ": 0x4000, "FJCRQQ": 0xC000, "FJSRQQ": 0x8000}


def _canon(sym):
    # C symbols get one leading underscore; CRT helpers (`__aFldiv`) more. `crtnames` strips them all.
    return sym.lstrip("_")


def _cstring(buf, off):
    end = buf.find(b"\0", off)
    return bytes(buf[off : end if end >= 0 else len(buf)])


class Refs:
    def __init__(self):
        self.ok = 0
        self.problems = []
        self.unchecked = 0

    def line(self):
        s = f"refs: {self.ok} verified"
        if self.unchecked:
            s += f", {self.unchecked} not checkable"
        if self.problems:
            s += f", {len(self.problems)} WRONG"
        return s


def verify(con, obj, proc, im, addr, row, procs=()):
    img = im.img
    r = Refs()
    lo, hi = proc.start, proc.end if proc.end is not None else len(obj.code)
    globals_by_name = {g["name"]: g["off"] for g in con.execute("SELECT off, name FROM globals")}
    far_by_name = {g["name"]: (g["seg"], g["off"]) for g in con.execute("SELECT seg, off, name FROM fardata")}
    funcs_by_addr = {f["addr"]: f for f in con.execute("SELECT addr, name, seg, off FROM functions")}
    dgroup = config.DGROUP_SEG * 16

    funcs_by_name = {(f["name"] or f"F_{a:05X}"): f for a, f in funcs_by_addr.items()}
    aliases = {}
    for a, n in con.execute("SELECT addr, name FROM aliases"):
        aliases.setdefault(a, set()).add(n)

    def fn_at(lin):
        f = funcs_by_addr.get(lin)
        if f is None:
            return None
        return f["name"] or f"F_{lin:05X}"

    def is_fn(lin, sym):
        """Does `sym` name the function at `lin`? Library modules publish aliases (`aFlmul` = `aFulmul`)."""
        return sym == fn_at(lin) or sym in aliases.get(lin, ())

    fnptr = {}
    for f in obj.fixups:
        if f.seg != obj.code_seg or not lo <= f.pos < hi:
            continue
        at = addr + (f.pos - lo)
        name = obj.target_name(f)
        inline = int.from_bytes(obj.code[f.pos : f.pos + min(f.size, 2)], "little")
        where = f"+{f.pos - lo:04x}"
        if f.loc == 3 and f.kind == "ext":
            off, seg = struct.unpack("<HH", img[at : at + 4])
            want = fn_at(seg * 16 + off)
            if want is None:
                r.problems.append(f"{where}: far call to {_canon(name)}, original calls {seg:04x}:{off:04x}, not a known function")
            elif not is_fn(seg * 16 + off, _canon(name)):
                r.problems.append(f"{where}: far call to {_canon(name)}, original calls {want}")
            else:
                r.ok += 1
        elif f.loc in (1, 5) and f.self_rel and f.kind == "ext":
            rel = struct.unpack("<H", img[at : at + 2])[0]
            ip = row["off"] + (at - addr) + 2
            lin = row["seg"] * 16 + ((ip + rel) & 0xFFFF)
            want = fn_at(lin)
            if not is_fn(lin, _canon(name)):
                r.problems.append(f"{where}: near call to {_canon(name)}, original calls {want or 'an unknown address'}")
            else:
                r.ok += 1
        elif f.loc in (1, 5) and f.kind == "ext":
            orig = struct.unpack("<H", img[at : at + 2])[0]
            sym = _canon(name)
            if sym in far_by_name:
                base = (orig - f.disp - inline) & 0xFFFF
                if base != far_by_name[sym][1]:
                    r.problems.append(f"{where}: {sym} is at offset {far_by_name[sym][1]:04x} of its segment, "
                                      f"original refers to offset {base:04x}")
                else:
                    r.ok += 1
                continue
            if sym in funcs_by_name and sym not in globals_by_name:
                # A far function's address taken as a value: checked with its SEG half below.
                fnptr.setdefault(sym, {})["off"] = ((orig - f.disp - inline) & 0xFFFF, where)
                continue
            if sym in _EMU:
                # 8087 emulator fixup: the linker adds the absolute FIxRQQ constant to the
                # `fwait; esc` bytes, turning them into `int 34h`-`3Dh`.
                if (inline + _EMU[sym]) & 0xFFFF != orig:
                    r.problems.append(f"{where}: {sym} patch gives {(inline + _EMU[sym]) & 0xFFFF:04x}, original {orig:04x}")
                else:
                    r.ok += 1
                continue
            if sym not in globals_by_name:
                r.problems.append(f"{where}: {sym} is not a known global (original refers to DS:{orig:04x})")
                continue
            base = (orig - f.disp - inline) & 0xFFFF
            if base != globals_by_name[sym]:
                r.problems.append(f"{where}: {sym} is DS:{globals_by_name[sym]:04x}, original refers to "
                                  f"DS:{orig:04x} (base DS:{base:04x})")
            else:
                r.ok += 1
        elif f.loc in (1, 5) and f.kind == "seg" and not f.self_rel:
            seg_name = obj.segments[f.index - 1]
            orig = struct.unpack("<H", img[at : at + 2])[0]
            ours = obj.data.get(f.index, bytearray())
            off = (inline + f.disp) & 0xFFFF
            if seg_name.endswith("_TEXT"):
                r.unchecked += 1
                continue
            segconst = next((g for g in obj.fixups if g.seg == f.index and g.pos == off and g.loc == 2), None)
            if segconst is not None:
                # MSC's segment constant for far data: a DGROUP word the loader relocates.
                far = far_by_name.get(_canon(obj.target_name(segconst) or ""))
                dpos = dgroup + orig
                if dpos not in im.reloc_pos:
                    r.problems.append(f"{where}: segment constant, but original DS:{orig:04x} is not relocated")
                elif far is None:
                    r.problems.append(f"{where}: segment of {obj.target_name(segconst)}, which is not declared far data")
                elif im.word(dpos) != far[0]:
                    r.problems.append(f"{where}: segment of {_canon(obj.target_name(segconst))} ({far[0]:04x}), "
                                      f"original DS:{orig:04x} holds segment {im.word(dpos):04x}")
                else:
                    r.ok += 1
                continue
            if seg_name == "CONST":
                # Float constants: compare the constant itself (up to the next one we reference,
                # at most a double), not a NUL-terminated run that spills into its neighbours.
                refd = sorted({(g.disp + int.from_bytes(obj.code[g.pos:g.pos + 2], "little")) & 0xFFFF
                               for g in obj.fixups if g.seg == obj.code_seg and g.kind == "seg" and g.index == f.index})
                nxt = next((o for o in refd if o > off), off + 8)
                n = max(1, min(8, nxt - off, len(ours) - off))
                mine, theirs = bytes(ours[off:off + n]), bytes(img[dgroup + orig:dgroup + orig + n])
            else:
                mine = _cstring(ours, off)
                theirs = _cstring(img, dgroup + orig)
            if mine != theirs:
                r.problems.append(f"{where}: {seg_name}+{off:04x} holds {mine[:40]!r}, original DS:{orig:04x} holds {theirs[:40]!r}")
            else:
                r.ok += 1
        elif f.loc == 2 and f.kind == "ext":
            # `mov reg, SEG sym`: the original's word is the far symbol's segment.
            orig = struct.unpack("<H", img[at : at + 2])[0]
            far = far_by_name.get(_canon(name))
            if far is None and _canon(name) in funcs_by_name:
                fnptr.setdefault(_canon(name), {})["seg"] = (orig, where)
                continue
            want = far[0] if far else None
            if want is None:
                r.problems.append(f"{where}: SEG {_canon(name)}, which is not declared far data or a function "
                                  f"(original segment {orig:04x})")
            elif want != orig:
                r.problems.append(f"{where}: SEG {_canon(name)} is {want:04x}, original segment {orig:04x}")
            else:
                r.ok += 1
        elif f.loc == 2 and f.kind == "grp" and obj.groups[f.index - 1] == "DGROUP":
            orig = struct.unpack("<H", img[at : at + 2])[0]
            if orig != config.DGROUP_SEG:
                r.problems.append(f"{where}: DGROUP base, original has segment {orig:04x}")
            else:
                r.ok += 1
        else:
            r.unchecked += 1

    # A function address as a value: seg and offset may be split differently from the DB's
    # seg:off (the linker normalises to the callee's own segment), so compare linear addresses.
    for sym, half in fnptr.items():
        where = (half.get("off") or half.get("seg"))[1]
        if "off" not in half or "seg" not in half:
            r.unchecked += 1
            continue
        lin = half["seg"][0] * 16 + half["off"][0]
        if lin != funcs_by_name[sym]["addr"]:
            r.problems.append(f"{where}: address of {sym} is {funcs_by_name[sym]['addr']:05x}, "
                              f"original has {half['seg'][0]:04x}:{half['off'][0]:04x}")
        else:
            r.ok += 1

    # Near calls resolved inside our OBJ (to placeholders): check the callee by name.
    starts = {p.start: p.c_name for p in procs if p.start is not None}
    for ins in linear(obj.code, lo, hi, ip0=lo):
        if ins.bytes[0] != 0xE8 or ins.size != 3 or obj.mask[ins.addr + 1]:
            continue
        t = branch_target(ins)
        if lo <= t < hi:
            continue
        ours = starts.get(t)
        at = addr + (ins.addr - lo)
        if img[at] != 0xE8:
            continue
        rel = struct.unpack("<H", img[at + 1 : at + 3])[0]
        ip = row["off"] + (at - addr) + 3
        want = fn_at(row["seg"] * 16 + ((ip + rel) & 0xFFFF))
        if ours is None:
            r.unchecked += 1
        elif want != ours:
            r.problems.append(f"+{ins.addr - lo:04x}: near call to {ours}, original calls {want or 'an unknown address'}")
        else:
            r.ok += 1
    return r
