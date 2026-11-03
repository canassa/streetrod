"""16-bit x86 decoding, including Microsoft's INT 34h-3Dh 8087 emulator calls."""

from capstone import CS_ARCH_X86, CS_MODE_16, Cs

_md = Cs(CS_ARCH_X86, CS_MODE_16)

# Capstone names a few opcodes by their 32-bit form even in 16-bit mode.
_MNEMONIC_16 = {"cwde": "cbw", "cdq": "cwd"}


class Insn:
    __slots__ = ("addr", "bytes", "mnemonic", "op_str")

    def __init__(self, addr, raw, mnemonic, op_str):
        self.addr = addr
        self.bytes = raw
        self.mnemonic = mnemonic
        self.op_str = op_str

    @property
    def size(self):
        return len(self.bytes)

    def text(self):
        return f"{self.mnemonic} {self.op_str}".strip()


def decode(buf, pos, ip):
    """Decode one instruction at buf[pos]; ip is its offset within its segment."""
    if buf[pos] == 0xCD and pos + 1 < len(buf) and 0x34 <= buf[pos + 1] <= 0x3C:
        if buf[pos + 1] == 0x3C:
            x = bytes([0xD8 | (buf[pos + 2] & 7)]) + buf[pos + 3 : pos + 10]
            extra = 3
        else:
            x = bytes([0xD8 + buf[pos + 1] - 0x34]) + buf[pos + 2 : pos + 9]
            extra = 2
        for _, size, m, o in _md.disasm_lite(x, 0, 1):
            n = extra + size - 1
            return Insn(pos, bytes(buf[pos : pos + n]), "emu_" + m, o)
    if buf[pos] == 0xCD and pos + 1 < len(buf) and buf[pos + 1] == 0x3D:
        return Insn(pos, bytes(buf[pos : pos + 2]), "emu_fwait", "")
    for _, size, m, o in _md.disasm_lite(bytes(buf[pos : pos + 16]), ip, 1):
        return Insn(pos, bytes(buf[pos : pos + size]), _MNEMONIC_16.get(m, m), o)
    return None


def linear(buf, start, end, ip0=None):
    """Linear sweep of buf[start:end]; undecodable bytes become `db`."""
    out = []
    pos = start
    ip0 = start if ip0 is None else ip0
    while pos < end:
        ins = decode(buf, pos, ip0 + pos - start)
        if ins is None or pos + ins.size > end:
            ins = Insn(pos, bytes(buf[pos : pos + 1]), "db", f"0x{buf[pos]:02x}")
        out.append(ins)
        pos += ins.size
    return out


def branch_target(ins):
    """Target ip of a short/near relative jump or call, else None."""
    b = ins.bytes
    if ins.mnemonic == "db":
        return None
    if b[0] in (0xE8, 0xE9) and len(b) == 3:
        return int(ins.op_str, 16)
    if (b[0] == 0xEB or 0x70 <= b[0] <= 0x7F or 0xE0 <= b[0] <= 0xE3) and len(b) == 2:
        return int(ins.op_str, 16)
    if b[0] == 0x0F and len(b) == 4 and 0x80 <= b[1] <= 0x8F:
        return int(ins.op_str, 16)
    return None
