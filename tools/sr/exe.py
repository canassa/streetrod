"""The unpacked SR.EXE load image."""

import struct
from functools import cache

from . import config


class Image:
    def __init__(self, path=config.EXE):
        d = path.read_bytes()
        h = struct.unpack("<14H", d[:28])
        self.header = h
        self.hdr = h[4] * 16
        self.img = d[self.hdr :]
        self.entry = (h[11], h[10])
        self.relocs = []
        for i in range(h[3]):
            off, seg = struct.unpack("<HH", d[h[12] + 4 * i : h[12] + 4 * i + 4])
            self.relocs.append(seg * 16 + off)
        self.reloc_pos = set(self.relocs)

    def word(self, a):
        return struct.unpack("<H", self.img[a : a + 2])[0]

    def far_call_targets(self):
        """(seg, off) of every `call far seg:off` whose segment word is relocated."""
        out = set()
        for p in self.relocs:
            if self.img[p - 3] == 0x9A:
                out.add((self.word(p), self.word(p - 2)))
        return out

    def dgroup_string(self, off, minlen=3):
        """The NUL-terminated string at DS:off, or None if it doesn't look like text."""
        base = config.DGROUP_SEG * 16 + off
        if base >= len(self.img):
            return None
        end = self.img.find(b"\0", base, base + 200)
        if end < 0:
            return None
        s = self.img[base:end]
        if base > 0 and self.img[base - 1] != 0 and len(s) < 8:
            return None
        if len(s) < minlen or any(c < 0x20 and c not in (9, 10, 13) or c > 0x7E for c in s):
            return None
        return s.decode("latin-1")


@cache
def image():
    return Image()
