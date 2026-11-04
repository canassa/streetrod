"""Maintain the headers from the DB (`./sr headers`, and `./sr global` / `./sr proto`): the
per-file headers the game sources include (fileheaders.py), the sources' include blocks and
the header dependencies of src/MAKEFILE. The files in git are the source the build compiles;
nothing here runs during `./sr build` or `./sr check`. `check()` (`./sr headers --check`)
reports what a regeneration would change without writing. STRUCTS.H is hand-written."""

import os

from . import config

_dry = None     # {path: new bytes, or None for a deletion} while checking


def generate(con, rewrite_sources=False):
    from . import fileheaders
    missing = fileheaders.generate(con, rewrite_sources=rewrite_sources)
    if missing:
        print("warning: no source defines " + ", ".join(missing[:12]) + (" ..." if len(missing) > 12 else ""))

    # the MAKEFILE lists each C source's headers as its dependencies
    from . import makefile
    makefile.sync_deps()

    text = (config.INCLUDE / "STRUCTS.H").read_text(errors="replace")
    if text.rstrip().rsplit("\n", 1)[-1].strip() != "#endif":
        raise SystemExit("src/include/STRUCTS.H: something was added after the closing #endif; "
                         "move it inside the include guard")


def check(con):
    """{path: new bytes or None} that `./sr headers` would write (empty: no drift)."""
    global _dry
    _dry = {}
    try:
        generate(con, rewrite_sources=True)
        return _dry
    finally:
        _dry = None


def _write(path, text, crlf=False):
    """Write a maintained file if it changed. Sources and headers have LF line ends (CL reads
    either); the MAKEFILE and SR.LNK are DOS text files for MAKE and LINK (CRLF)."""
    text = text.replace("\r\n", "\n")
    data = (text.replace("\n", "\r\n") if crlf else text).encode("latin-1")
    if path.exists() and path.read_bytes() == data:
        return
    if _dry is not None:
        _dry[path] = data
        return
    # Write-then-rename: parallel `sr` runs may read the file meanwhile.
    tmp = path.with_name(f".{path.name}.{os.getpid()}")
    tmp.write_bytes(data)
    os.replace(tmp, path)


def _remove(path):
    if _dry is not None:
        _dry[path] = None
    else:
        path.unlink()
