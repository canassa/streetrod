"""Verify a compiled module's own initialized data against the original DGROUP.

The code's references to the module's _DATA give its original base (every reference must
agree on one). The data bytes are compared at that base; each data fixup is resolved and
compared symbolically: a near pointer to the module's own string must name the same text,
one to another global the same DGROUP offset, a far pointer the same function or far-data
item, a DGROUP segment word the DGROUP paragraph.
"""
import struct

from . import config


def _cstring(buf, off):
    end = buf.find(b"\0", off)
    return bytes(buf[off:end if end >= 0 else len(buf)])


def check(obj, con, im, codebase):
    """problems (list of str) and (base, size) of the module's _DATA in the original."""
    problems = []
    if '_DATA' not in obj.segments:
        return problems, None
    dseg = obj.segments.index('_DATA') + 1
    data = obj.data.get(dseg, bytearray())
    if not data:
        return problems, None
    img = im.img
    ds = config.DGROUP_SEG * 16
    bases = {}
    for f in obj.fixups:
        if f.seg != obj.code_seg or f.kind != 'seg' or f.index != dseg or f.loc not in (1, 5) or f.self_rel:
            continue
        inline = int.from_bytes(obj.code[f.pos:f.pos + 2], 'little')
        orig = struct.unpack('<H', img[codebase + f.pos:codebase + f.pos + 2])[0]
        base = (orig - inline - f.disp) & 0xFFFF
        bases.setdefault(base, []).append(f.pos)
    if not bases:
        return problems, None
    globals_by_name = {g['name']: g['off'] for g in con.execute('SELECT off, name FROM globals')}
    if len(bases) > 1:
        shown = ', '.join(f'{b:04x} ({len(p)} refs)' for b, p in sorted(bases.items(), key=lambda kv: -len(kv[1])))
        problems.append(f'data layout: the code refers to our _DATA with different original bases: {shown}')
        base = max(bases, key=lambda b: len(bases[b]))
        pubs = sorted((off, name) for name, (seg, off) in obj.all_publics.items() if seg == dseg)
        for off, name in pubs:
            sym = name.lstrip('_')
            if sym in globals_by_name and globals_by_name[sym] != base + off:
                problems.append(f'    {sym}: ours +{off:04x} (DS:{base + off:04x}), original DS:{globals_by_name[sym]:04x}')
        return problems, None
    base = next(iter(bases))
    if base % 2:
        # bytes are still compared, but alignment of later items may differ standalone
        pass
    if base + len(data) > 0xFFFF or ds + base + len(data) > len(img):
        problems.append(f'data layout: base {base:04x} plus {len(data)} bytes runs past the image')
        return problems, (base, len(data))
    masked = bytearray(len(data))
    dfix = [f for f in obj.fixups if f.seg == dseg]
    for f in dfix:
        for z in range(f.size):
            if f.pos + z < len(masked):
                masked[f.pos + z] = 1
    orig = img[ds + base:ds + base + len(data)]
    bad = [i for i in range(len(data)) if not masked[i] and data[i] != orig[i]]
    if bad:
        problems.append(f'data: {len(bad)} byte(s) differ from the original at DS:{base + bad[0]:04x}'
                        + (f', DS:{base + bad[1]:04x}' if len(bad) > 1 else '') + ' ...' * (len(bad) > 2))
    far_by_name = {g['name']: (g['seg'], g['off']) for g in con.execute('SELECT seg, off, name FROM fardata')}
    funcs = {f['addr']: (f['name'] or f'F_{f["addr"]:05X}') for f in con.execute('SELECT addr, name FROM functions')}
    aliases = {}
    for a, n in con.execute('SELECT addr, name FROM aliases'):
        aliases.setdefault(a, set()).add(n)
    for f in dfix:
        at = ds + base + f.pos
        name = obj.target_name(f) or ''
        sym = name.lstrip('_')
        inline = int.from_bytes(data[f.pos:f.pos + 2], 'little')
        where = f'DS:{base + f.pos:04x}'
        if f.loc == 3 and f.kind == 'ext':
            off, seg = struct.unpack('<HH', img[at:at + 4])
            lin = seg * 16 + off
            if sym in far_by_name:
                fseg, foff = far_by_name[sym]
                if (seg, off) != (fseg, foff + inline + f.disp):
                    problems.append(f'{where}: far pointer to {sym}, original {seg:04x}:{off:04x}')
            elif lin in funcs and (funcs[lin] == sym or sym in aliases.get(lin, ())):
                pass
            elif sym in globals_by_name and seg == config.DGROUP_SEG and off == globals_by_name[sym] + inline + f.disp:
                pass
            else:
                problems.append(f'{where}: far pointer to {sym}, original {seg:04x}:{off:04x}' +
                                (f' ({funcs[lin]})' if lin in funcs else ''))
        elif f.loc in (1, 5) and f.kind == 'ext':
            orig_w = struct.unpack('<H', img[at:at + 2])[0]
            if sym not in globals_by_name:
                problems.append(f'{where}: near pointer to {sym}, which is not a known global (original DS:{orig_w:04x})')
            elif (globals_by_name[sym] + inline + f.disp) & 0xFFFF != orig_w:
                problems.append(f'{where}: near pointer to {sym} (DS:{globals_by_name[sym]:04x}+{inline + f.disp}), original DS:{orig_w:04x}')
        elif f.loc in (1, 5) and f.kind == 'seg' and f.index == dseg:
            orig_w = struct.unpack('<H', img[at:at + 2])[0]
            off = (inline + f.disp) & 0xFFFF
            mine, theirs = _cstring(data, off), _cstring(img, ds + orig_w)
            if (orig_w - off) & 0xFFFF != base:
                # a pointer into our own data: the same relative position in the original block
                problems.append(f'{where}: pointer into our data at +{off:04x} (DS:{base + off:04x}), original DS:{orig_w:04x}')
            elif mine != theirs:
                problems.append(f'{where}: our data at +{off:04x} holds {mine[:30]!r}, original DS:{orig_w:04x} holds {theirs[:30]!r}')
        elif f.loc == 2 and f.kind == 'grp':
            orig_w = struct.unpack('<H', img[at:at + 2])[0]
            if orig_w != config.DGROUP_SEG:
                problems.append(f'{where}: DGROUP segment word, original {orig_w:04x}')
        elif f.loc == 2 and f.kind == 'ext':
            orig_w = struct.unpack('<H', img[at:at + 2])[0]
            far = far_by_name.get(sym)
            if far is None or far[0] != orig_w:
                problems.append(f'{where}: SEG {sym}, original {orig_w:04x}')
        else:
            problems.append(f'{where}: unexpected data fixup kind {f.kind} loc {f.loc} to {sym}')
    return problems, (base, len(data))
