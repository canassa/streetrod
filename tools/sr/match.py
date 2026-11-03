"""Compare one compiled procedure against the original bytes."""

import difflib
import re

from .disasm import branch_target, linear


class Result:
    def __init__(self, size_exe, size_obj, mismatches, nearcalls, masked):
        self.size_exe = size_exe
        self.size_obj = size_obj
        self.mismatches = mismatches
        self.nearcalls = nearcalls
        self.masked = masked
        self.refs = None
        self.lint = []

    @property
    def verdict(self):
        if self.size_exe != self.size_obj or self.mismatches:
            return "DIFF"
        if self.refs is not None and self.refs.problems or self.lint:
            return "DIFF"
        return "MATCH*" if self.nearcalls else "MATCH"

    def line(self):
        v = self.verdict
        if v == "DIFF" and not self.mismatches and self.size_exe == self.size_obj:
            v = "DIFF (bytes match; " + ("refs" if self.refs is not None and self.refs.problems else "lint") + " failed)"
        s = (f"{v}: {len(self.mismatches)} mismatched bytes, {self.masked} fixup bytes masked, "
             f"size {self.size_obj} vs original {self.size_exe}")
        if self.nearcalls:
            s += f", {self.nearcalls} near call displacement(s) not verifiable in isolation"
        if self.refs is not None and (self.mismatches or self.size_exe != self.size_obj):
            s += "; refs: not checked until the bytes line up"
        elif self.refs is not None:
            s += "; " + self.refs.line()
            for p in self.refs.problems:
                s += "\n    " + p
        for p in self.lint:
            s += "\n    lint: " + p
        return s


def compare(obj, proc, img, addr, size):
    o0 = proc.start
    olen = (proc.end if proc.end is not None else len(obj.code)) - o0
    exe = linear(img, addr, addr + size)
    near_ok = set()
    nearcalls = 0
    for ins in exe:
        rel = ins.addr - addr
        if ins.bytes[0] != 0xE8 or ins.size != 3 or rel + 3 > olen or obj.code[o0 + rel] != 0xE8:
            continue
        if bytes(obj.code[o0 + rel + 1 : o0 + rel + 3]) == ins.bytes[1:3]:
            continue
        # A near call out of the function: its displacement depends on where the callee
        # sits in the original module, which a one-function compile can't reproduce.
        if not addr <= branch_target(ins) < addr + size:
            nearcalls += 1
            near_ok.update((rel + 1, rel + 2))
    mism, masked = [], 0
    for i in range(min(olen, size)):
        if obj.mask[o0 + i]:
            masked += 1
            continue
        if i in near_ok:
            continue
        if obj.code[o0 + i] != img[addr + i]:
            mism.append(i)
    return Result(size, olen, mism, nearcalls, masked)


def _key(ins, base_ip):
    t = branch_target(ins)
    if t is not None:
        return f"{ins.mnemonic} L+{t - base_ip:x}"
    return ins.mnemonic + " " + re.sub(r"0x[0-9a-f]{3,}", "#", ins.op_str)


def diff_rows(obj, proc, img, addr, size, off):
    """Instruction-aligned rows (marker, ours, theirs) for a two-column diff."""
    o0 = proc.start
    oend = proc.end if proc.end is not None else len(obj.code)
    ours = linear(obj.code, o0, oend, ip0=o0)
    theirs = linear(img, addr, addr + size, ip0=off)
    ka = [_key(i, o0) for i in ours]
    kb = [_key(i, off) for i in theirs]
    rows = []

    def fmt(ins, buf, start, mask=None):
        raw = []
        for k, b in enumerate(ins.bytes):
            raw.append(".." if mask is not None and mask[ins.addr + k] else f"{b:02x}")
        return f"{ins.addr - start:04x} {''.join(raw):<14} {ins.text()}"

    def same_bytes(a, b):
        if a.size != b.size:
            return False
        return all(obj.mask[a.addr + k] or a.bytes[k] == b.bytes[k] for k in range(a.size))

    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, ka, kb, autojunk=False).get_opcodes():
        if tag == "equal":
            for a, b in zip(ours[i1:i2], theirs[j1:j2]):
                mark = " " if same_bytes(a, b) else "~"
                rows.append((mark, fmt(a, obj.code, o0, obj.mask), fmt(b, img, addr)))
        else:
            n = max(i2 - i1, j2 - j1)
            for k in range(n):
                a = ours[i1 + k] if i1 + k < i2 else None
                b = theirs[j1 + k] if j1 + k < j2 else None
                # Paired instructions whose only difference is a masked fixup are equal.
                mark = (" " if same_bytes(a, b) else "≠") if a and b else ("+" if a else "-")
                rows.append((mark, fmt(a, obj.code, o0, obj.mask) if a else "", fmt(b, img, addr) if b else ""))
    return rows
