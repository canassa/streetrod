"""Find every function in the code region and classify it as C, asm or CRT.

Recursive descent from all relocated far-call targets, far function pointers in data,
the entry point, and near calls; then repeated passes over the gaps for prologues the
descent didn't reach (functions only called through pointers or jump tables).
"""

import re
import struct

from . import config
from .disasm import branch_target, decode
from .exe import image

_HARD = {"iret", "pushf", "popf", "xlatb", "jcxz", "lodsb", "lodsw", "lahf",
         "clc", "stc", "cmc", "rcl", "rcr", "daa", "das"}
_INT_PROLOGUE = bytes.fromhex("50515253545556571e06")


def _entries(im):
    segvals, entries, weak = set(), set(), set()
    img = im.img
    for p in im.relocs:
        v = im.word(p)
        segvals.add(v)
        if img[p - 3] == 0x9A:
            entries.add((v, im.word(p - 2)))
        elif p < config.CODE_END and v < config.CODE_END // 16 and img[p - 1] in (0xB8, 0xBA):
            # A code address loaded as a value: `mov ax,OFF; mov dx,SEG` (MSC function pointer) or
            # `mov ax,SEG; mov ds,ax; mov dx,OFF` (asm handing an interrupt handler to int 21h/25h).
            for q in (p - 3, p + 3, p + 5):
                if img[q - 1] in (0xB8, 0xBA) and img[q - 1] != img[p - 1]:
                    t = v * 16 + im.word(q)
                    if t < config.CODE_END and (img[t : t + 3] == b"\x55\x8b\xec" or img[t] in (0x1E, 0x50, 0x56, 0x57)):
                        entries.add((v, im.word(q)))
                    break
        elif v < config.CODE_END // 16 and p >= config.CODE_END:
            off = im.word(p - 2)
            t = v * 16 + off
            if im.img[t : t + 3] == b"\x55\x8b\xec" or im.img[t] in (0x1E, 0x50, 0x56, 0x57):
                # Weak: any relocated data word pair can look like a far pointer. Only taken
                # where the strong entries' descent didn't already reach (0x0F8C2 was data).
                weak.add((v, off))
    entries.add(im.entry)
    return entries, weak, sorted(s for s in segvals if s * 16 < config.CODE_END)


def scan():
    im = image()
    img = im.img
    end = config.CODE_END
    entries, weak, segl = _entries(im)
    funcs, covered = {}, {}

    def descend(work):
        while work:
            seg, off = work.pop()
            start = seg * 16 + off
            if start in funcs or start >= end or start in covered:
                continue
            insns, q, callees = {}, [off], set()
            while q:
                o = q.pop()
                while True:
                    a = seg * 16 + o
                    if a in insns or a >= end or o > 0xFFFF or a in covered:
                        break
                    ins = decode(img, a, o)
                    if ins is None:
                        break
                    insns[a] = ins
                    b = ins.bytes
                    m = ins.mnemonic
                    if m == "call" and b[0] == 0xE8:
                        t = branch_target(ins) & 0xFFFF  # near targets wrap within the segment
                        work.append((seg, t))
                        callees.add(seg * 16 + t)
                    if m == "lcall" or (m == "call" and b[0] == 0x9A):
                        if a + 3 in im.reloc_pos:
                            callees.add(im.word(a + 3) * 16 + im.word(a + 1))
                    if m == "jmp":
                        if b[0] in (0xE9, 0xEB):
                            o = branch_target(ins) & 0xFFFF
                            continue
                        mm = re.match(r"word ptr cs:\[(bx|si|di) \+ 0x([0-9a-f]+)\]", ins.op_str)
                        if mm:
                            t = int(mm.group(2), 16)
                            for k in range(256):
                                p = seg * 16 + t + 2 * k
                                if p in insns or p in covered:
                                    break
                                tgt = im.word(p)
                                if abs(tgt - o) > 0x1000 or tgt < off:
                                    break
                                q.append(tgt)
                        break
                    if branch_target(ins) is not None and m != "call":
                        q.append(branch_target(ins) & 0xFFFF)
                    if m in ("ret", "retf", "iret"):
                        break
                    o += ins.size
            if not insns:
                continue
            funcs[start] = (seg, off, insns, callees)
            for a, ins in insns.items():
                for k in range(ins.size):
                    covered[a + k] = start

    def segof(a):
        for s in segl:
            if s * 16 <= a < s * 16 + 0x10000:
                return s
        return None

    descend(list(entries))
    descend(list(weak - entries))
    pat = re.compile(rb"\x55\x8b\xec|\x50\x51\x52\x53\x54\x55\x56\x57|\x1e\x06\x50|\x55\x1e\x06")
    for _ in range(6):
        new = []
        for m in pat.finditer(img[:end]):
            a = m.start()
            if a not in covered and (a - 1 in covered or img[a - 1] in (0x90, 0x00, 0xCB, 0xC3)):
                s = segof(a)
                if s is not None:
                    new.append((s, a - s * 16))
        if not new:
            break
        descend(new)

    rows = []
    for start in sorted(funcs):
        seg, off, insns, callees = funcs[start]
        last = max(insns)
        size = last + insns[last].size - start
        rows.append({
            "addr": start, "seg": seg, "off": off, "size": size,
            "kind": classify(start, insns),
            "far": int(any(i.mnemonic == "retf" for i in insns.values())),
            "callees": sorted(callees),
        })
    return rows


def classify(start, insns):
    if config.CRT[0] <= start < config.CRT[1]:
        return "crt"
    seq = [insns[a] for a in sorted(insns) if a >= start]
    head = b"".join(i.bytes for i in seq)[:12]
    if head[:10] == _INT_PROLOGUE:
        return "c"
    k = 3
    if head[k : k + 1] == b"\x83":
        k += 3
    elif head[k : k + 1] == b"\x81":
        k += 4
    if head[k : k + 1] == b"\x1e":
        k += 1
    if head[:3] == b"\x55\x8b\xec" and head[k : k + 2] == b"\x56\x57":
        return "asm"
    hard = fp = 0
    for i in insns.values():
        rep = i.bytes[0] in (0xF2, 0xF3)
        if not rep and i.mnemonic in _HARD:
            hard += 1
        if i.mnemonic in ("ret", "retf") and i.op_str:
            hard += 1
        if i.mnemonic == "int" and i.op_str in ("0x10", "0x21", "0x16", "0x33", "0x13"):
            hard += 1
        if i.mnemonic == "sub" and i.op_str in ("ax, ax", "al, al", "cx, cx", "dx, dx", "si, si", "di, di", "bx, bx"):
            fp += 1
        if i.mnemonic == "add" and i.op_str.startswith("sp, "):
            fp += 1
        if "[bp - 0x" in i.op_str or i.mnemonic.startswith("emu_"):
            fp += 1
    if hard:
        return "asm"
    if head[:3] == b"\x55\x8b\xec":
        return "c"
    return "c" if fp >= 2 else "asm"


def far_call_operand(im, ins):
    """Linear target of a relocated `call far` instruction, else None."""
    if ins.bytes[0] == 0x9A and ins.addr + 3 in im.reloc_pos:
        return im.word(ins.addr + 3) * 16 + im.word(ins.addr + 1)
    return None


def unpack_word(b, i):
    return struct.unpack("<H", b[i : i + 2])[0]
