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


def find(procs, name):
    for p in procs:
        if p.c_name == name or p.name == name:
            return p
    return None


def from_publics(obj, name):
    """A Proc for an assembled function: from its PUBLIC to the next PUBLIC or the end."""
    for sym in (name, "_" + name):
        if sym in obj.publics:
            start = obj.publics[sym]
            later = sorted(o for o in obj.publics.values() if o > start)
            p = Proc(sym, "?")
            p.start = start
            p.end = later[0] if later else len(obj.code)
            return p
    return None
