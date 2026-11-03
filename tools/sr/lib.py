"""Locate MSC 5.1 library modules inside the EXE so CRT functions get their real names."""

import re
import struct

from . import config
from .exe import image


def modules(path):
    """Yield (module name, code bytes, fixup mask, {public: code offset}) for each LIB module."""
    d = path.read_bytes()
    page = struct.unpack("<H", d[1:3])[0] + 3 if d[0] == 0xF0 else 16
    i = page if d[0] == 0xF0 else 0
    while i + 3 <= len(d) and d[i] == 0x80:
        name, code, mask, pubs = None, bytearray(), bytearray(), {}
        lnames, segs, code_seg, last = [], [], None, None
        while i + 3 <= len(d):
            t = d[i]
            n = struct.unpack("<H", d[i + 1 : i + 3])[0]
            rec = d[i + 3 : i + 2 + n]
            if t == 0x80:
                name = rec[1 : 1 + rec[0]].decode("latin-1")
            elif t == 0x96:
                k = 0
                while k < len(rec):
                    lnames.append(rec[k + 1 : k + 1 + rec[k]].decode("latin-1"))
                    k += 1 + rec[k]
            elif t == 0x98:
                segs.append(lnames[rec[3] - 1])
                if code_seg is None and segs[-1].endswith("_TEXT"):
                    code_seg = len(segs)
            elif t == 0x90 and rec[1] == code_seg and code_seg:
                k = 2
                while k < len(rec):
                    ln = rec[k]
                    pub = rec[k + 1 : k + 1 + ln].decode("latin-1")
                    pubs[pub] = struct.unpack("<H", rec[k + 1 + ln : k + 3 + ln])[0]
                    k += 3 + ln
                    k += 1 if rec[k] < 0x80 else 2
            elif t == 0xA0:
                last = None
                if rec[0] == code_seg:
                    off = struct.unpack("<H", rec[1:3])[0]
                    body = rec[3:]
                    if len(code) < off + len(body):
                        code.extend(b"\0" * (off + len(body) - len(code)))
                        mask.extend(b"\1" * (off + len(body) - len(mask)))
                    code[off : off + len(body)] = body
                    mask[off : off + len(body)] = b"\0" * len(body)
                    last = off
            elif t == 0x9C and last is not None:
                k = 0
                while k < len(rec):
                    b = rec[k]
                    if b & 0x80:
                        loc = (b >> 2) & 0xF
                        o = last + (((b & 3) << 8) | rec[k + 1])
                        k += 2
                        for z in range({0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2}.get(loc, 2)):
                            if o + z < len(mask):
                                mask[o + z] = 1
                        fd = rec[k]
                        k += 1
                        if not fd & 0x80 and (fd >> 4) & 7 < 3:
                            k += 1 if rec[k] < 0x80 else 2
                        if not fd & 0x08 and fd & 3 < 3:
                            k += 1 if rec[k] < 0x80 else 2
                        if not fd & 0x04:
                            k += 2
                    else:
                        k += 2 if rec[k + 1] < 0x80 else 3
            if t not in (0x9C, 0xA0):
                last = None
            i += 3 + n
            if t in (0x8A, 0x8B):
                i = (i + page - 1) // page * page
                break
        if code and pubs:
            yield name, bytes(code), bytes(mask), pubs


def locate(libs, lo=config.CRT[0], hi=config.CRT[1]):
    """{linear address: [(public name, module), ...]} for library code found in [lo, hi).

    A module may publish several names for one offset (LIBH's `__aFlmul`/`__aFulmul`)."""
    img = image().img
    region = img[lo:hi]
    found = {}
    for lib in libs:
        for name, code, mask, pubs in modules(lib):
            if len(code) < 8:
                continue
            pat = b"".join(b"." if mask[k] else re.escape(code[k : k + 1]) for k in range(len(code)))
            m = re.search(pat, region, re.S)
            if m:
                for pub, off in pubs.items():
                    found.setdefault(lo + m.start() + off, []).append((pub.lstrip("_"), name))
    return found
