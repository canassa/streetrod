"""Expand an .STB sidecar: `@NAME` lines become placeholder definitions built from the DB prototype.

A hand-written far placeholder goes stale (C2086) whenever someone refines the callee's
prototype in the DB. `@F_13DE6` always repeats the current one; so does `@F_03C1B` for a
public near function (`T near @(...)`, declared in its file's header) defined by another
source of the same link unit. Static near placeholders have no DB prototype and stay
hand-written.
"""

import re

from . import db

_LINE = re.compile(r"^@([A-Za-z_]\w*)\s*$", re.M)


def placeholder(proto, name):
    head = proto.replace("@", name)
    ret = proto.split("@")[0].strip()
    body = "{ }" if re.fullmatch(r"void(\s+(far|near|interrupt))*", ret) else "{ return 0; }"
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
