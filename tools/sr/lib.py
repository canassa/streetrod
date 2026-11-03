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


def objects(path):
    """Yield complete archive members, retaining relocation targets for identity.

    Wrappers for /=, %= and signed/unsigned operations can have identical code
    after masking their far call. They are not aliases of the same function.
    """
    from .omf import Obj
    data = path.read_bytes()
    page = struct.unpack_from('<H', data, 1)[0]+3 if data[0] == 0xf0 else 16
    pos = page if data[0] == 0xf0 else 0
    while pos+3 <= len(data) and data[pos] == 0x80:
        start = pos
        size = struct.unpack_from('<H', data, pos+1)[0]
        rec = data[pos+3:pos+2+size]
        name = rec[1:1+rec[0]].decode('latin-1')
        while pos+3 <= len(data):
            kind = data[pos]
            size = struct.unpack_from('<H', data, pos+1)[0]
            pos += 3+size
            if kind in (0x8a, 0x8b):
                yield name, Obj(data[start:pos])
                pos = (pos+page-1)//page*page
                break
        else:
            raise ValueError(f'{path}: unterminated archive member {name}')


def candidates(libs, img, lo, hi):
    """Locate code, then reject candidates whose symbolic call targets disagree."""
    inventory = [(name, obj) for lib in libs for name, obj in objects(lib) if obj.code]
    all_names = {symbol for _, obj in inventory for symbol in obj.publics}
    data_names = {symbol for _, obj in inventory for symbol, (seg, _) in obj.all_publics.items()
                  if seg and seg != obj.code_seg}
    possible = []
    for name, obj in inventory:
        if len(obj.code) < 3:
            continue
        pat = b''.join(b'.' if obj.mask[k] else re.escape(obj.code[k:k+1]) for k in range(len(obj.code)))
        for match in re.finditer(pat, img, re.S):
            possible.append((match.start(), name, obj))
    # A candidate may call another ambiguous wrapper. Iterate until stable so
    # rejecting a callee also rejects callers that depended on that identity.
    while True:
        symbols = {}
        invalid = set()
        for base, _, obj in possible:
            placements = {obj.code_seg: base}
            for fix in obj.fixups:
                if (fix.seg != obj.code_seg or fix.kind != 'seg' or fix.self_rel
                        or fix.loc not in (1, 5) or fix.index == obj.code_seg):
                    continue
                if obj.segments[fix.index-1] not in ('_DATA', 'CONST', '_BSS'):
                    continue
                actual = struct.unpack_from('<H', img, base+fix.pos)[0]
                inline = int.from_bytes(obj.code[fix.pos:fix.pos+2], 'little')
                location = config.DGROUP_SEG*16+actual-inline-fix.disp
                if fix.index in placements and placements[fix.index] != location:
                    invalid.add((base, id(obj)))
                placements[fix.index] = location
            for symbol, (seg, offset) in obj.all_publics.items():
                if seg in placements:
                    symbols.setdefault(symbol, set()).add(placements[seg]+offset)
        accepted = []
        for base, name, obj in possible:
            valid = (base, id(obj)) not in invalid
            for fix in obj.fixups:
                if fix.seg != obj.code_seg or fix.kind != 'ext':
                    continue
                symbol = obj.target_name(fix)
                if symbol not in all_names and symbol not in data_names:
                    continue  # application callbacks/data are not library identities
                at = base+fix.pos
                inline = int.from_bytes(obj.code[fix.pos:fix.pos+2], 'little')
                if fix.loc == 3:
                    off, seg = struct.unpack_from('<HH', img, at)
                    target = seg*16+off-inline-fix.disp
                elif fix.loc in (1, 5) and fix.self_rel:
                    relative = struct.unpack_from('<h', img, at)[0]
                    target = at+2+relative-inline-fix.disp
                elif fix.loc in (1, 5) and symbol in data_names:
                    offset = struct.unpack_from('<H', img, at)[0]
                    target = config.DGROUP_SEG*16+offset-inline-fix.disp
                else:
                    continue
                if target not in symbols.get(symbol, set()):
                    valid = False
                    break
            if valid:
                accepted.append((base, name, obj))
        if len(accepted) == len(possible):
            # An archive member cannot occupy bytes inside another member.
            # Short jump shims otherwise match incidental instructions in big TUs.
            return [(base, name, obj) for base, name, obj in accepted if lo <= base < hi
                    and not any(start <= base and base+len(obj.code) <= start+len(other.code)
                                and len(other.code) > len(obj.code)
                                for start, _, other in accepted)]
        possible = accepted


def locate(libs, lo=config.CRT[0], hi=config.CRT[1]):
    """{linear address: [(public name, module), ...]} with callee identities verified.

    Aliases must be actual publics at one offset, or independently verified
    identical implementations. Relocation-masked wrapper similarity is insufficient.
    """
    found = {}
    for base, name, obj in candidates(libs, image().img, lo, hi):
        for pub, off in obj.publics.items():
            found.setdefault(base+off, []).append((pub.lstrip('_'), name))
    return found
