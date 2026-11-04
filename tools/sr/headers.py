"""Generate the headers from the DB: the per-file headers the game sources include (fileheaders.py).
STRUCTS.H is hand-written."""

import os

from . import config


def generate(con):
    inc = config.INCLUDE
    inc.mkdir(parents=True, exist_ok=True)

    from . import fileheaders
    missing = fileheaders.generate(con)
    if missing:
        print("warning: no source defines " + ", ".join(missing[:12]) + (" ..." if len(missing) > 12 else ""))

    structs = inc / "STRUCTS.H"
    if structs.exists():
        text = structs.read_text(errors="replace")
        if text.rstrip().rsplit("\n", 1)[-1].strip() != "#endif":
            raise SystemExit("src/include/STRUCTS.H: something was added after the closing #endif; "
                             "move it inside the include guard")
    if not structs.exists():
        structs.write_text("/* STRUCTS.H - shared struct types. Hand-written source: one definition per type. */\n"
                           "#ifndef STRUCTS_H\n#define STRUCTS_H\n\n#endif\n")


def _write(path, text):
    data = text.replace("\n", "\r\n").encode("latin-1")
    if path.exists() and path.read_bytes() == data:
        return
    # Write-then-rename: parallel `sr` runs regenerate the headers while others compile.
    tmp = path.with_name(f".{path.name}.{os.getpid()}")
    tmp.write_bytes(data)
    os.replace(tmp, path)
