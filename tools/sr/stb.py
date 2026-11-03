"""Expand an .STB sidecar: `@NAME` lines become placeholder definitions built from the DB prototype.

A hand-written far placeholder goes stale (C2086) whenever someone refines the callee's
prototype in PROTOS.H. `@F_13DE6` always repeats the current one. Near (static) placeholders
have no DB prototype and stay hand-written.
"""

import re

from . import db

_LINE = re.compile(r"^@([A-Za-z_]\w*)\s*$", re.M)


def placeholder(proto, name):
    head = proto.replace("@", name)
    ret = proto.split("@")[0].strip()
    body = "{ }" if ret in ("void", "void far", "void interrupt far") else "{ return 0; }"
    return f"{head} {body}"


def expand(text):
    if "@" not in text:
        return text
    con = db.connect()

    def sub(m):
        name = m.group(1)
        row = con.execute("SELECT * FROM functions WHERE name = ? OR printf('F_%05X', addr) = ?",
                          (name, name)).fetchone()
        if row is None or not row["proto"]:
            raise SystemExit(f"STB: @{name} has no prototype in the DB (./sr proto ADDR \"...\")")
        return placeholder(row["proto"], name)

    return "#include \"STRUCTS.H\"\n" + _LINE.sub(sub, text)
