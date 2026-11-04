"""Per-file headers, as the original was organized: every source file (one link unit, i.e.
one original translation unit, or one standalone data/assembler file) declares what it
defines in its same-folder <STEM>.H, and each C source includes the headers of the files whose
globals and functions it uses. The headers are hand-written source (declarations joined by name
to original addresses in symbols.json); `./sr headers` rewrites the C sources'
include blocks from them and checks that each header's names are defined by its file."""
import re

from . import config
from .headers import _write

_STANDALONE = {'STRUCTS', 'PC'}

# runtime variables the game reads directly; MSC's own headers declare them
_SYSTEM = {'errno': 'stdlib.h', '_doserrno': 'stdlib.h', '_amblksiz': 'malloc.h'}


def owners_layout():
    """[(stem, [source path])] for every original file: the code files (in address order),
    the data-only files outside DATA (in link order), then the standalone data files."""
    from . import makefile, module
    code = [f.path for f in module.files()]
    order = {o: k for k, o in enumerate(makefile.link_order())}
    data = sorted((r.path for r in makefile.compile_rules() if r.path.parent.name != 'DATA' and r.path not in code),
                  key=lambda p: order.get(p.stem.upper() + '.OBJ', len(order)))
    out, seen = [], set()
    for p in code + data:
        out.append((p.stem.upper(), [p])); seen.add(p)
    for p in config.source_files('.C', '.ASM'):
        if p.suffix.upper() in ('.C', '.ASM') and p not in seen:
            out.append((p.stem.upper(), [p]))
    return out


def _strip(text):
    text = re.sub(r'/\*.*?\*/', ' ', text, flags=re.S)
    return re.sub(r'"(\\.|[^"\\])*"|\'(\\.|[^\'\\])*\'', '""', text)


def file_statics(path):
    """Names of the uninitialized file-scope `static` variables a C source defines."""
    if path.suffix.upper() != '.C':
        return set()
    out = set()
    for ln in _strip(path.read_text(encoding='latin-1')).splitlines():
        if re.match(r'static\s[^;{}=()]*;\s*$', ln):
            out.add(re.search(r'(\w+)\s*(\[[^]]*\]\s*)*;', ln).group(1))
    return out


def local_statics():
    """Every file-local uninitialized static of the game sources."""
    return {n for _, srcs in owners_layout() for p in srcs for n in file_statics(p)}


def _defines(path, names):
    """Names from `names` that this source defines (C file-scope definitions, MASM PUBLICs)."""
    text = path.read_text(encoding='latin-1')
    if path.suffix.upper() == '.ASM':
        # MASM strings have no escapes (a backslash is a character), a ';' outside one starts
        # a comment, and an apostrophe in a comment is not a quote
        text = re.sub(r"'[^'\n]*'|\"[^\"\n]*\"|;[^\n]*", '', text)
        pubs = {m.lower() for m in re.findall(r'^\s*PUBLIC\s+_(\w+)', text, re.M | re.I)}
        return {n for n in names if n.lower() in pubs}
    found = names & set(re.findall(r'^#[ \t]*define[ \t]+([A-Za-z_]\w*)', _strip(text), re.M))
    for ln in text.splitlines():
        if not ln[:1].isalpha() or ln.startswith(('extern', 'static', 'typedef')):
            continue
        ln = ln.split("=")[0]
        for m in re.finditer(r'\b([A-Za-z_]\w*)\b', ln):
            n = m.group(1)
            if n not in names:
                continue
            rest = ln[m.end():]
            # data: `T n;`, `T n[..] = ...`, `T (far *n)(...);`; function: `T n(args)` without `;`
            if re.match(r"\s*(\[[^]]*\]\s*)*(=|;|,|$)", rest) or re.match(r'\s*(\[[^]]*\]\s*)*\)\s*\(', rest) \
                    or (re.match(r'\s*\(', rest) and not ln.rstrip().endswith(';')):
                found.add(n)
                break
    return found


def generate():
    """Rewrite every C source's include block from the headers' declarations. Returns the
    problems: a name declared by two headers, or by a header whose file does not define it."""
    from . import source
    decls = {}
    header_constants = set()
    type_owners = {}
    problems = []
    for kind in ('globals', 'fardata', 'functions'):
        for d in source.headers()[kind]:
            decls.setdefault(d['name'], []).append(d['header'])
    for path in config.source_files('.H'):
        text = _strip(path.read_text(encoding='latin-1'))
        for kind, name in re.findall(r'\b(struct|union)\s+(\w+)\s*\{', text):
            key = (kind, name)
            if key in type_owners:
                problems.append(f'{kind} {name} is defined in {type_owners[key]}.H and {path.name}')
            type_owners[key] = path.stem.upper()
        macros = re.findall(r'^#[ \t]*define[ \t]+([A-Za-z_]\w*)(?:\([^\n)]*\))?[ \t]+[^\s]',
                            _strip(path.read_text(encoding='latin-1')), re.M)
        enums = []
        for tag, body in re.findall(r'\benum\s*(\w+)?\s*\{([^}]+)\}', _strip(path.read_text(encoding='latin-1')), re.S):
            if tag:
                enums.append(tag)
            enums.extend(re.findall(r'(?:^|,)\s*([A-Za-z_]\w*)', body))
        for name in macros + enums:
            header_constants.add(name)
            decls.setdefault(name, []).append(path.stem.upper())
    files = owners_layout()
    stems = {stem for stem, _ in files}
    owner = {}
    for name, hs in sorted(decls.items()):
        if len(hs) > 1:
            problems.append(f'{name} is declared in ' + ' and '.join(f'{h}.H' for h in hs))
        owner[name] = hs[0]
    made = ({h for hs in decls.values() for h in hs} | set(type_owners.values())) & (stems | _STANDALONE)
    for h in sorted(({h for hs in decls.values() for h in hs} | set(type_owners.values())) - stems - _STANDALONE):
        problems.append(f'{config.source_path(h + ".H").relative_to(config.SRC)}: no source file {h}.C or {h}.ASM')
    for stem, srcs in files:
        mine = {n for n, h in owner.items() if h == stem}
        defined = set().union(*(_defines(p, mine) for p in srcs))
        for n in sorted(mine - defined - header_constants):
            problems.append(f'{n}: declared in {stem}.H, but {", ".join(p.name for p in srcs)} does not define it')
    for stem, srcs in files:
        for p in srcs:
            if p.suffix.upper() == '.C':
                _rewrite_includes(p, stem, {**owner, **type_owners}, made)
    return problems


def _rewrite_includes(path, stem, owner, made):
    """The include block as a C programmer of 1989 wrote it: the system headers first (in the
    order the file has them; one the game's runtime variables need is appended), then
    the file's own header, and the headers of the files whose symbols or types it uses, alphabetically.
    The block is the run of #include lines at the top of the file."""
    raw = path.read_text(encoding='latin-1')
    ids = set(re.findall(r'\b[A-Za-z_]\w*\b', _strip(raw)))
    tags = set(re.findall(r'\b(struct|union)\s+(\w+)', _strip(raw)))
    need = sorted({owner[i] for i in ids | tags if i in owner} & made - {stem})
    lines = raw.split('\n')
    at = [i for i, ln in enumerate(lines) if ln.startswith('#include')]
    if not at:
        at = [0]
        lines.insert(0, '')
    if any(lines[i].strip() for i in range(at[0], at[-1]) if i not in at):
        raise SystemExit(f'{path}: the #include lines are not one block at the top')
    system = [lines[i] for i in at if lines[i].startswith('#include <')]
    for h in sorted({_SYSTEM[i] for i in ids if i in _SYSTEM}):
        if not any(re.match(rf'#include\s+<{re.escape(h)}>', ln, re.I) for ln in system):
            system.append(f'#include <{h}>')
    block = system + ([f'#include "{stem}.H"'] if stem in made else []) \
        + [f'#include "{h}.H"' for h in need]
    new = lines[:at[0]] + block + [ln for i, ln in enumerate(lines) if i > at[0] and i not in at]
    text = '\n'.join(new)
    if text != raw:
        _write(path, text)
