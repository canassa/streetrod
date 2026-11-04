"""Locate MSC 5.1 library modules inside the EXE so CRT functions get their real names."""

import re
import struct

from . import config
from .exe import image


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


def _placements(obj, base, img, strict=True):
    """{segment index: linear address} of a member placed at `base`: its code, and the DGROUP
    segments its code refers to (from the original words at its fixups). None when two
    references disagree (unless not strict: then the last one wins)."""
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
        if strict and fix.index in placements and placements[fix.index] != location:
            return None
        placements[fix.index] = location
    return placements


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
            placements = _placements(obj, base, img)
            if placements is None:
                invalid.add((base, id(obj)))
                placements = _placements(obj, base, img, strict=False)
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
    return locate_all(libs, lo, hi)[0]


def locate_all(libs, lo=config.CRT[0], hi=config.CRT[1]):
    """locate(), and {C name: DGROUP offset} of the located modules' data publics (placed by
    their code's references; a public placed at two offsets is left out)."""
    img = image().img
    found, data = {}, {}
    for base, name, obj in candidates(libs, img, lo, hi):
        for pub, off in obj.publics.items():
            found.setdefault(base+off, []).append((pub.lstrip('_'), name))
        placements = _placements(obj, base, img, strict=False)
        for pub, (seg, off) in obj.all_publics.items():
            if seg and seg != obj.code_seg and seg in placements:
                data.setdefault(pub[1:] if pub.startswith('_') else pub, set()).add(
                    placements[seg] + off - config.DGROUP_SEG*16)
    return found, {n: min(v) for n, v in data.items() if len(v) == 1 and 0 <= min(v) < 0x10000}
