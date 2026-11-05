"""Parse an MSC 5.1 /Fc listing (.COD) for procedure offsets and local layouts."""

import re

_PROC = re.compile(r"^(\w+)\s+PROC\s+(NEAR|FAR)")
_OFF = re.compile(r"^\s*\*\*\*\s+([0-9a-f]{6})\s+((?:[0-9a-f]{2} )+)")
_LOCAL = re.compile(r"^;\s+(\w+) = (-?\d+)\s*$")


class Proc:
    def __init__(self, name, kind):
        self.name = name
        self.c_name = name[1:] if name.startswith("_") else name
        self.kind = kind
        self.start = None
        self.end = None
        self.locals = {}
        self.last_end = None


def parse(path):
    procs, cur = [], None
    for line in path.read_text(errors="replace").splitlines():
        m = _PROC.match(line)
        if m:
            cur = Proc(m.group(1), m.group(2))
            procs.append(cur)
            continue
        if cur is None:
            continue
        if re.match(r"^\w+\s+ENDP", line):
            cur = None
            continue
        m = _LOCAL.match(line)
        if m:
            cur.locals[m.group(1)] = int(m.group(2))
            continue
        m = _OFF.match(line)
        if m:
            off = int(m.group(1), 16)
            if cur.start is None:
                cur.start = off
            cur.last_end = off + len(m.group(2).split())
    for p in procs:
        p.end = p.last_end
    return procs


_SIZES = {"DB": 1, "DW": 2, "DD": 4, "DQ": 8, "DT": 10}


def statics(path):
    """{offset: (name, size)} of the _BSS items in a listing. MSC writes them in symbol-table
    order with `ORG $+n` / `ORG $-n` steps; a file-scope static is `$Snnn_name` (or `_name`)."""
    out, loc, inside = {}, 0, False
    for line in path.read_text(errors="replace").splitlines():
        if re.match(r"^_BSS\s+SEGMENT\s*$", line):
            inside, loc = True, 0
            continue
        if not inside:
            continue
        if re.match(r"^_BSS\s+ENDS", line):
            inside = False
            continue
        m = re.match(r"^\s*ORG\s+\$([+-]\d+)", line)
        if m:
            loc += int(m.group(1))
            continue
        m = re.match(r"^(\S+)\s+(DB|DW|DD|DQ|DT)\s+0*([0-9A-Fa-f]+)H\s+DUP\s*\(\?\)", line)
        if m:
            name = re.sub(r"^(\$S\d+_|_)", "", m.group(1))
            size = _SIZES[m.group(2)] * int(m.group(3), 16)
            out[loc] = (name, size)
            loc += size
    return out


def data_labels(path):
    """Named _DATA offsets, including MSC's private $Snnn_name labels.

    /Fc emits data without byte offsets, so follow its directives and relative ORGs.
    Segment blocks resume their previous location; strings and DUP contribute storage.
    """
    def count(body):
        # Protect commas inside quoted byte strings and DUP parentheses.
        terms = re.findall(r"'(?:[^']|'')*'|[^,'()]+|[(),]", body)
        def group(at=0):
            total = 0
            while at < len(terms):
                term = terms[at].strip(); at += 1
                if not term or term == ',':
                    continue
                if term == ')':
                    return total, at
                dup = re.fullmatch(r'(\d+)\s+DUP', term, re.I)
                if dup:
                    if at >= len(terms) or terms[at] != '(':
                        raise ValueError(body)
                    size, at = group(at + 1)
                    total += int(dup[1]) * size
                elif term.startswith("'"):
                    total += len(term[1:-1].replace("''", "'"))
                else:
                    total += 1
            return total, at
        return group()[0]

    out, loc, inside = {}, 0, False
    for line in path.read_text(errors="replace").splitlines():
        if re.match(r'^_DATA\s+SEGMENT', line):
            inside = True
            continue
        if not inside:
            continue
        if re.match(r'^_DATA\s+ENDS', line):
            inside = False
            continue
        m = re.match(r'^\s*ORG\s+\$([+-]\d+)', line)
        if m:
            loc += int(m[1])
            continue
        m = re.match(r'^(?:(\S+)\s+)?\s*(DB|DW|DD|DQ|DT)\s+(.+)', line)
        if m:
            label, directive, body = m.groups()
            if label and (label.startswith('$S') and not label.startswith('$SG') or label.startswith('_')):
                out[re.sub(r'^(\$S\d+_|_)', '', label)] = loc
            loc += _SIZES[directive] * count(body)
    return out


def find(procs, name):
    for p in procs:
        if p.c_name == name or p.name == name:
            return p
    return None


def asm_lengths(path):
    """{PROC name: length} from the symbol table of a MASM 5.1 listing."""
    out = {}
    if path is None:
        return out
    for line in path.read_text(errors="replace").splitlines():
        m = re.match(r"^(\S+) [ .]*\s+[FN] PROC\s+[0-9A-F]{4}\s.*Length = ([0-9A-F]{4})", line)
        if m:
            out[m.group(1)] = int(m.group(2), 16)
    return out


def from_publics(obj, name, lengths=None):
    """A Proc for an assembled function: from its PUBLIC over its PROC length (the listing),
    else to the next PUBLIC or the end. Data between two PROCs (a flag byte in the code
    segment) belongs to neither; the file's gate compares it."""
    for sym in (name, "_" + name):
        if sym in obj.publics:
            start = obj.publics[sym]
            later = sorted(o for o in obj.publics.values() if o > start)
            p = Proc(sym, "?")
            p.start = start
            p.end = later[0] if later else len(obj.code)
            if lengths and sym in lengths:
                p.end = start + lengths[sym]
            return p
    return None
