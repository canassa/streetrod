"""The checked-in build: src/MAKEFILE (for MSC MAKE 4.07) and src/SR.LNK (LINK's response file).

They are the build definition, the way Microsoft's own 1988 STARTUP\\MAKEFILE is written:
macros, one description block per object with its real command line, the source and the
headers it includes as dependencies, and a link through a response file. MAKE 4.07 runs the
blocks in file order and does not recurse, so the library comes first and the link last.
`MAKE MAKEFILE` in src\\ (with MSC SETUP's environment) builds SR.EXE.

The host tools read these files and nothing else for the build: `./sr build` runs each
block's command exactly as written (CL, MASM and LIB under emu2, LINK under DOSBox-X), and
the per-file flags, code segments and link order come from here (`module.files()`).

Grammar used by the parser (a subset of MAKE 4.07): `#` comment lines, `NAME=value` macros
expanded as `$(NAME)`, `target: dependencies` with `\\` continuation lines, and command lines
that start with white space. A C object's command is `$(CC) <its flags> [/NT<segment>]
<source>`; a MASM object's is `$(ASM) <its flags> <source>,<obj>,<lst>;`.
"""
import os
import re
from pathlib import Path

from . import config

MAKEFILE = config.SRC / 'MAKEFILE'
RESPONSE = config.SRC / 'SR.LNK'
COMPILERS = {'$(CC)': 'CL', '$(ASM)': 'MASM'}


class Rule:
    """One description block: `target: deps` and its command lines."""

    def __init__(self, target, deps, commands, first, last, macros):
        self.target = target
        self.deps = deps                # as written (macros expanded)
        self.raw = commands             # command lines as written
        self.first, self.last = first, last   # physical line span [first, last)
        self._macros = macros

    @property
    def stem(self):
        return self.target.rsplit('.', 1)[0].upper()

    @property
    def command(self):
        """The (single) command line, macros expanded: what MAKE runs."""
        if len(self.raw) != 1:
            raise ValueError(f'{self.target}: expected one command line, found {len(self.raw)}')
        return expand(self.raw[0], self._macros)

    @property
    def program(self):
        return self.command.split()[0].upper()

    @property
    def args(self):
        return self.command.split()[1:]

    # -------------------------------------------------- compile rules ($(CC) / $(ASM))
    @property
    def compiler(self):
        """CL or MASM for an object's block, else None."""
        words = self.raw[0].split() if len(self.raw) == 1 else []
        return COMPILERS.get(words[0]) if words else None

    @property
    def source(self):
        """The DOS path of the compiled source (MODS\\M_00000.C)."""
        last = self.raw[0].split()[-1]
        return last.split(',')[0] if self.compiler == 'MASM' else last

    @property
    def path(self):
        return host_path(self.source)

    @property
    def flags(self):
        """The file's own options: the command without the macro, /NT and the source."""
        words = self.raw[0].split()[1:-1]
        return ' '.join(w for w in words if not w.upper().startswith('/NT'))

    @property
    def segment(self):
        """The code segment a C file is compiled into (`/NTname`), or None."""
        words = [w for w in self.raw[0].split()[1:-1] if w.upper().startswith('/NT')]
        return words[0][3:] if words else None


def expand(text, macros):
    def one(m):
        if m.group(1) not in macros:
            raise ValueError(f'MAKEFILE: undefined macro $({m.group(1)})')
        return expand(macros[m.group(1)], macros)
    out = re.sub(r'\$\((\w+)\)', one, text)
    if '$' in out:
        raise ValueError(f'MAKEFILE: unsupported macro use in {text!r}')
    return out


def parse(text):
    """(macros, [Rule]) of a MAKE 4.07 description file."""
    lines = text.replace('\r\n', '\n').split('\n')
    macros, rules = {}, []
    cur = None
    i = 0
    while i < len(lines):
        first = i
        line = lines[i]
        i += 1
        while line.endswith('\\') and i < len(lines):
            line = line[:-1] + ' ' + lines[i].strip()
            i += 1
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        if line[0] in ' \t':
            if cur is None:
                raise ValueError(f'MAKEFILE line {first + 1}: command outside a description block')
            cur.raw.append(line.strip())
            cur.last = i
            continue
        m = re.match(r'^(\w+)\s*=\s*(.*)$', line)
        if m:
            macros[m.group(1)] = m.group(2).strip()
            cur = None
            continue
        m = re.match(r'^([^\s:]+)\s*:(?!\\)\s*(.*)$', line)
        if not m:
            raise ValueError(f'MAKEFILE line {first + 1}: cannot parse {line!r}')
        cur = Rule(m.group(1).upper(), expand(m.group(2), macros).split(), [], first, i, macros)
        rules.append(cur)
    return macros, rules


_cache = {}


def read(path=None):
    """(macros, rules) of src/MAKEFILE, cached by modification time."""
    path = Path(path or MAKEFILE)
    st = path.stat()
    key = (str(path), st.st_mtime_ns, st.st_size)
    if key not in _cache:
        _cache.clear()
        _cache[key] = parse(path.read_text(encoding='latin-1'))
    return _cache[key]


def rules(path=None):
    return read(path)[1]


def compile_rules(path=None):
    """The object blocks ($(CC) / $(ASM)), in file order."""
    return [r for r in rules(path) if r.compiler]


def rule(program, path=None):
    """The one block whose command runs `program` (LIB, LINK)."""
    found = [r for r in rules(path) if len(r.raw) == 1 and r.program == program]
    if len(found) != 1:
        raise ValueError(f'MAKEFILE: expected one {program} block, found {len(found)}')
    return found[0]


def host_path(dos):
    """The file under src/ that a DOS path relative to the build directory names
    (DOS names are case-insensitive)."""
    p = config.SRC
    for part in dos.split('\\'):
        match = [c for c in p.iterdir() if c.name.upper() == part.upper()] if p.is_dir() else []
        p = match[0] if match else p / part
    return p


def response(path=None):
    """(objects in link order, exe, map, libraries) of src/SR.LNK."""
    text = Path(path or RESPONSE).read_text(encoding='latin-1').replace('\r\n', '\n')
    lines = text.split('\n')
    objects = []
    i = 0
    while i < len(lines):
        ln = lines[i].strip()
        i += 1
        more = ln.endswith('+')
        objects += ln.rstrip('+').split()
        if not more:
            break
    rest = [ln.strip() for ln in lines[i:]]
    rest += [''] * (3 - len(rest))
    return [o.upper() for o in objects], rest[0], rest[1], rest[2].rstrip(';')


def link_order(path=None):
    return response(path)[0]


def header_deps(source):
    """INCLUDE\\X.H for every project header a C source includes (directly or through
    another project header), sorted."""
    seen, todo = set(), [source]
    while todo:
        text = todo.pop().read_text(encoding='latin-1')
        for h in re.findall(r'^#include\s+"(\w+\.H)"', text, re.M | re.I):
            p = config.INCLUDE / h.upper()
            if h.upper() not in seen and p.exists():
                seen.add(h.upper())
                todo.append(p)
    return ['INCLUDE\\' + h for h in sorted(seen)]


def _block(target, deps, command):
    """A description block, dependency lines wrapped at 78 columns like STARTUP\\MAKEFILE."""
    out, line = [], f'{target}:'
    for d in deps:
        if len(line) + 1 + len(d) > 76:
            out.append(line + ' \\')
            line = '\t'
        line += ('' if line == '\t' else ' ') + d
    out.append(line)
    out.append('\t' + command)
    return out


def _replace(rule_, new_lines, path):
    path = Path(path or MAKEFILE)
    lines = path.read_text(encoding='latin-1').replace('\r\n', '\n').split('\n')
    lines[rule_.first:rule_.last] = new_lines
    _write(path, '\n'.join(lines))


def set_flags(stem, flags, path=None):
    """Give one object's command line other options (`./sr flags FILE FLAGS`)."""
    r = next(r for r in compile_rules(path) if r.stem == stem.upper())
    words = r.raw[0].split()
    nt = [w for w in words[1:-1] if w.upper().startswith('/NT')]
    command = ' '.join([words[0], *flags.split(), *nt, words[-1]])
    _replace(r, _block(r.target, r.deps, command), path)


def sync_deps(path=None):
    """Make every C object's dependencies its source plus the headers it includes
    (run by `./sr headers` and every header regeneration). Returns the changed targets."""
    path = Path(path or MAKEFILE)
    changed = []
    for r in reversed(compile_rules(path)):
        if r.compiler != 'CL':
            continue
        want = [r.source, *header_deps(r.path)]
        if r.deps != want:
            _replace(r, _block(r.target, want, r.raw[0]), path)
            changed.append(r.target)
    return changed[::-1]


def _write(path, text):
    from .headers import _write as write
    write(Path(path), text, crlf=True)
    _cache.clear()
