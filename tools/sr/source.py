"""Symbol tables from source declarations and tools/sr/symbols.json.

Only original function extents and data addresses are stored. Names connect those
addresses to the current definitions and headers; source files, declarations and
near/far distance are derived. Runtime symbols come from the EXE and MSC libraries.
"""
import functools
import hashlib
import json
import os
import re
from pathlib import Path

from . import config

SYMBOLS = Path(__file__).with_name('symbols.json')
_EXTERN = re.compile(r'^extern\s+(.+?);\s*$')
_PROTO = re.compile(r'^(.+\));\s*$')
_STATIC = re.compile(r'^static\s+([^;{}=()]*?)\s*;\s*$')


def symbol_addresses():
    return json.loads(SYMBOLS.read_text())


_DECLARED = re.compile(r'\b([A-Za-z_]\w*)\s*(?=(?:\[[^\]]*\]\s*)*(?:;|$|\)\s*\())')
LIBS = ('MLIBCR.LIB', 'MLIBFP.LIB', 'LIBH.LIB', 'EM.LIB')


def reload():
    """Forget what was read (after a command edited the sources)."""
    for f in (game_functions, headers, statics, runtime):
        f.cache_clear()


def source_files():
    """The game's C and MASM sources (src/, src/DATA)."""
    return config.source_files('.C', '.ASM')


def _rel(path):
    return path.relative_to(config.ROOT).as_posix()


def declared_name(decl):
    """The identifier a declaration declares: `int (far *g)(void)` -> g, `char *t[][2]` -> t."""
    m = _DECLARED.search(decl)
    if m is None:
        raise ValueError(f'no declared name in {decl!r}')
    return m.group(1)


def _template(decl, name):
    """`int g_x[4]` -> `int @[4]`."""
    return re.sub(rf'\b{re.escape(name)}\b', '@', decl, count=1)


def _c_far(text, at, name):
    m = re.compile(rf'^([^\n;{{}}]*?)\b{re.escape(name)}\s*\(', re.M).search(text, at)
    if m is None:
        raise SystemExit(f'{name}: no definition')
    words = re.findall(r'\w+', m.group(1))
    return 0 if words and words[-1] == 'near' else 1


def _asm_far(text, name):
    m = re.search(rf'^_{re.escape(name)}\s+PROC\s+(FAR|NEAR)\b', text, re.M | re.I)
    if m is None:
        raise SystemExit(f'{name}: no PROC FAR/NEAR')
    return int(m.group(1).upper() == 'FAR')


@functools.cache
def game_functions():
    """Every recorded game function, located in its current source file."""
    from .fileheaders import _strip
    addresses = symbol_addresses()['functions']
    protos = {f['name']: f['proto'] for f in headers()['functions']}
    rows = []
    for path in source_files():
        text = path.read_text(encoding='latin-1')
        kind = 'c' if path.suffix.upper() == '.C' else 'asm'
        if kind == 'c':
            clean = _strip(text)
            definitions = re.findall(r'^([A-Za-z_][^\n;{}]*?\b([A-Za-z_]\w*)[ \t]*\([^;{}]*\))\s*\{', clean, re.M)
            names = [name for _, name in definitions]
        else:
            names = re.findall(r'^_(\w+)\s+PROC\s+(?:FAR|NEAR)\b', text, re.M | re.I)
        for name in names:
            if name not in addresses:
                raise SystemExit(f'{path.name}: {name}: missing function in {SYMBOLS.name}')
            loc = addresses[name]
            seg, off, size = (loc[k] for k in ('seg', 'off', 'size'))
            far = _c_far(clean, 0, name) if kind == 'c' else _asm_far(text, name)
            rows.append(dict(addr=seg * 16 + off, seg=seg, off=off, size=size, kind=kind,
                             far=far, name=name, src=_rel(path), proto=protos.get(name)))
    found = [r['name'] for r in rows]
    missing = addresses.keys() - set(found)
    if missing:
        raise SystemExit('functions without definitions: ' + ', '.join(sorted(missing)))
    if len(found) != len(set(found)):
        raise SystemExit('duplicate function definitions')
    rows.sort(key=lambda r: r['addr'])
    for a, b in zip(rows, rows[1:]):
        if a['addr'] == b['addr']:
            raise SystemExit(f"{a['name']} and {b['name']}: two functions at one address")
    return rows


@functools.cache
def headers():
    """Declarations from per-file headers, joined to the original addresses by name."""
    from .fileheaders import _strip
    addresses = symbol_addresses()
    out = dict(globals=[], fardata=[], functions=[])
    for path in config.source_files('.H'):
        if path.stem.upper() == 'STRUCTS':
            continue
        stem = path.stem.upper()
        clean = _strip(path.read_text(encoding='latin-1'))
        declarations = re.finditer(
            r'^(?:extern\s+[^;{}]+|[A-Za-z_][^;{}]*\([^;{}]*\))\s*;', clean, re.M)
        for declaration in declarations:
            ln = ' '.join(declaration.group().split())
            m = _EXTERN.match(ln)
            if m:
                name = declared_name(m[1])
                if name not in addresses['data']:
                    raise SystemExit(f'{path.name}: {name}: missing data address in {SYMBOLS.name}')
                row = dict(**addresses['data'][name], name=name, decl=_template(m[1], name), header=stem)
                out['fardata' if 'seg' in row else 'globals'].append(row)
                continue
            m = _PROTO.match(ln)
            if m:
                name = re.search(r'(\w+)\s*\(', m[1]).group(1)
                if name not in addresses['functions']:
                    raise SystemExit(f'{path.name}: {name}: missing function in {SYMBOLS.name}')
                loc = addresses['functions'][name]
                out['functions'].append(dict(name=name, proto=_template(m[1], name),
                                             addr=loc['seg'] * 16 + loc['off'], header=stem))
    return out


@functools.cache
def statics():
    """Uninitialized file statics, with original addresses joined by name."""
    from .fileheaders import _strip
    addresses = symbol_addresses()['data']
    out = []
    for path in source_files():
        if path.suffix.upper() != '.C':
            continue
        for ln in _strip(path.read_text(encoding='latin-1')).splitlines():
            m = _STATIC.match(ln)
            if m:
                name = declared_name(m[1])
                if name not in addresses:
                    raise SystemExit(f'{path.name}: {name}: missing static address in {SYMBOLS.name}')
                out.append(dict(off=addresses[name]['off'], name=name, decl=_template(m[1], name),
                                header=None, static=_rel(path)))
    return out


def globals_(runtime_data=True):
    """{DGROUP offset: row} of every named DGROUP global: the headers' and the statics, then
    the runtime's data publics at the offsets left (their rows have no decl; `runtime` set)."""
    out = {}
    for g in headers()['globals'] + statics():
        if g['off'] in out:
            raise SystemExit(f"DS:{g['off']:04X} is both {out[g['off']]['name']} and {g['name']}")
        out[g['off']] = g
    if runtime_data:
        for name, off in sorted(runtime()['data'].items(), key=lambda kv: (kv[1], kv[0])):
            out.setdefault(off, dict(off=off, name=name, decl=None, header=None, runtime=True))
    return dict(sorted(out.items()))


def global_offsets():
    """{name: DGROUP offset} of every global, the runtime's included (all names of one offset)."""
    out = {name: off for name, off in runtime()['data'].items()}
    out.update((g['name'], off) for off, g in globals_(runtime_data=False).items())
    return out


def fardata():
    return sorted(headers()['fardata'], key=lambda g: (g['seg'], g['off']))


# ---------------------------------------------------------------- the runtime (orig/, toolchain/)


def _runtime_key():
    from . import lib, scan
    h = hashlib.sha256()
    for p in [config.EXE, *(config.TOOLCHAIN / 'LIB' / n for n in LIBS), lib.__file__, scan.__file__,
              __file__]:
        h.update(Path(p).read_bytes())
    return h.hexdigest()


@functools.cache
def runtime():
    """{'functions': runtime rows, 'aliases': {addr: [names]}, 'callees': {addr: [addr]},
    'data': {C name: DGROUP offset}}: the scanner's view of the EXE and the library modules
    located in it. A located address with several publics takes the last as its name."""
    from . import lib, scan
    cache = config.WORK / 'cache' / 'runtime.json'
    key = _runtime_key()
    try:
        data = json.loads(cache.read_text())
        if data['key'] == key:
            return dict(functions=data['functions'], aliases={int(a): v for a, v in data['aliases'].items()},
                        callees={int(a): v for a, v in data['callees'].items()}, data=data['data'])
    except (OSError, ValueError, KeyError):
        pass
    rows = scan.scan()
    found, libdata = lib.locate_all([config.TOOLCHAIN / 'LIB' / n for n in LIBS])
    functions, aliases = [], {}
    for r in rows:
        if r['kind'] != 'crt':
            continue
        pubs = found.get(r['addr'], [])
        name, module = pubs[-1] if pubs else (None, None)
        functions.append(dict(addr=r['addr'], seg=r['seg'], off=r['off'], size=r['size'], kind='crt',
                              far=r['far'], name=name, src=None, proto=None, module=module))
        if len(pubs) > 1:
            aliases[r['addr']] = sorted({p for p, _ in pubs} - {name})
    callees = {r['addr']: r['callees'] for r in rows}
    data = dict(key=key, functions=functions, aliases=aliases, callees=callees, data=libdata)
    try:
        cache.parent.mkdir(parents=True, exist_ok=True)
        tmp = cache.with_suffix(f'.tmp{os.getpid()}')
        tmp.write_text(json.dumps(data))
        tmp.replace(cache)
    except OSError:
        pass
    return dict(functions=functions, aliases=aliases, callees=callees, data=libdata)


def functions():
    """Every function of the EXE: the sources' and the runtime's, by address."""
    return sorted(game_functions() + runtime()['functions'], key=lambda r: r['addr'])


def aliases():
    return runtime()['aliases']


def callees():
    return runtime()['callees']


# ---------------------------------------------------------------- lookups


def fname(row):
    return row['name'] or f"F_{row['addr']:05X}"


def resolve(ref):
    """A function by name, or by its address (hex, with or without 0x)."""
    fs = functions()
    for r in fs:
        if fname(r) == ref:
            return r
    try:
        addr = int(ref, 16)
    except ValueError:
        raise SystemExit(f'unknown function: {ref}') from None
    for r in fs:
        if r['addr'] == addr:
            return r
    raise SystemExit(f'no function at {addr:#x}')


def label(addr):
    for r in functions():
        if r['addr'] == addr:
            return fname(r)
    return f'F_{addr:05X}'


def members(src):
    """The functions a source file (its path relative to the root) defines, by address."""
    return [r for r in game_functions() if r['src'] == src]
