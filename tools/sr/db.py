import sqlite3
import time

from . import config

SCHEMA = """
CREATE TABLE IF NOT EXISTS functions (
    addr       INTEGER PRIMARY KEY,   -- linear address in the unpacked image
    seg        INTEGER NOT NULL,      -- segment value callers use
    off        INTEGER NOT NULL,
    size       INTEGER NOT NULL,      -- extent in bytes (first to last instruction)
    kind       TEXT NOT NULL,         -- c | asm | crt (scanner's guess, editable)
    far        INTEGER NOT NULL,      -- 1 if it returns with RETF
    name       TEXT,                  -- C identifier once known
    status     TEXT NOT NULL DEFAULT 'todo',  -- todo | wip | matched | partial | blocked | skip
    src        TEXT,                  -- source file, relative to the project root
    flags      TEXT,                  -- CL flags when not the default
    mismatches INTEGER,               -- last check: differing bytes (excl. fixups)
    nearcalls  INTEGER,               -- last check: near-call displacements not verifiable
    notes      TEXT,
    updated    TEXT
);
CREATE TABLE IF NOT EXISTS callees (
    caller INTEGER NOT NULL,
    callee INTEGER NOT NULL,
    PRIMARY KEY (caller, callee)
);
CREATE TABLE IF NOT EXISTS globals (
    off   INTEGER PRIMARY KEY,        -- DGROUP offset
    name  TEXT NOT NULL UNIQUE,
    type  TEXT,
    notes TEXT,
    decl  TEXT                        -- C declaration with @ for the name, e.g. "int @[10]"
);
"""

_SCHEMA_FAR = """
CREATE TABLE IF NOT EXISTS fardata (
    seg   INTEGER NOT NULL,           -- paragraph of its far segment in the load image
    off   INTEGER NOT NULL,           -- offset within that segment
    name  TEXT NOT NULL UNIQUE,
    decl  TEXT,                       -- C declaration with @ for the name, e.g. "struct Slot far @[]"
    notes TEXT,
    PRIMARY KEY (seg, off)
);
"""

_SCHEMA_ALIASES = """
CREATE TABLE IF NOT EXISTS aliases (
    addr  INTEGER NOT NULL,           -- a function's linear address
    name  TEXT NOT NULL,              -- another public name the library gives that address
    PRIMARY KEY (addr, name)
);
"""

_MIGRATIONS = (
    ("globals", "decl", "TEXT"),
    ("functions", "proto", "TEXT"),  # prototype with @ for the name, e.g. "int far @(int n)"
)


def connect():
    config.DB.parent.mkdir(exist_ok=True)
    con = sqlite3.connect(config.DB, timeout=60)  # parallel workers share the DB
    con.row_factory = sqlite3.Row
    con.execute("PRAGMA journal_mode=WAL")
    con.executescript(SCHEMA)
    con.executescript(_SCHEMA_FAR)
    con.executescript(_SCHEMA_ALIASES)
    for table, col, typ in _MIGRATIONS:
        cols = {r[1] for r in con.execute(f"PRAGMA table_info({table})")}
        if col not in cols:
            con.execute(f"ALTER TABLE {table} ADD COLUMN {col} {typ}")
    return con


def now():
    return time.strftime("%Y-%m-%d %H:%M")


def resolve(con, ref):
    """An address (hex, with or without 0x) or a function name -> row."""
    row = con.execute("SELECT * FROM functions WHERE name = ?", (ref,)).fetchone()
    if row:
        return row
    try:
        addr = int(ref, 16)
    except ValueError:
        raise SystemExit(f"unknown function: {ref}") from None
    row = con.execute("SELECT * FROM functions WHERE addr = ?", (addr,)).fetchone()
    if not row:
        raise SystemExit(f"no function at {addr:#x} (run `sr scan`?)")
    return row


def update(con, addr, **fields):
    fields["updated"] = now()
    cols = ", ".join(f"{k} = ?" for k in fields)
    con.execute(f"UPDATE functions SET {cols} WHERE addr = ?", (*fields.values(), addr))
    con.commit()


def names_at(con, addr):
    """Every name a call to this address may use: the DB name plus library aliases."""
    names = {label(con, addr)}
    names.update(r[0] for r in con.execute("SELECT name FROM aliases WHERE addr = ?", (addr,)))
    return names


def label(con, addr):
    row = con.execute("SELECT name FROM functions WHERE addr = ?", (addr,)).fetchone()
    if row and row["name"]:
        return row["name"]
    return f"F_{addr:05X}"
