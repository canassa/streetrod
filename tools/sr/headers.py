"""`./sr headers`: the include blocks of the C sources (fileheaders.py) and the header
dependencies of src/MAKEFILE, kept in step with the hand-written headers in the source folders.
The files in git are the source the build compiles; nothing here runs during `./sr build` or
`./sr check`. `check()` (`./sr headers --check`) reports what a run would change without
writing, and the headers' problems (a name declared twice, or by a header whose file does not
define it)."""

import os

from . import config

_dry = None     # {path: new bytes} while checking


def generate():
    """Rewrite the include blocks and the MAKEFILE dependencies; returns the headers' problems."""
    from . import fileheaders, makefile
    problems = fileheaders.generate()
    makefile.sync_deps()
    text = config.source_path('STRUCTS.H').read_text(errors="replace")
    if text.rstrip().rsplit("\n", 1)[-1].strip() != "#endif":
        problems.append("src/SYS/STRUCTS.H: something was added after the closing #endif; "
                        "move it inside the include guard")
    return problems


def check():
    """({path: new bytes} that `./sr headers` would write, problems); both empty: no drift."""
    global _dry
    _dry = {}
    try:
        problems = generate()
        return _dry, problems
    finally:
        _dry = None


def _write(path, text, crlf=False):
    """Write a maintained file if it changed. Sources and headers have LF line ends (CL reads
    either); the MAKEFILE and SR.LNK are DOS text files for MAKE and LINK (CRLF)."""
    text = text.replace("\r\n", "\n")
    data = (text.replace("\n", "\r\n") if crlf else text).encode("latin-1")
    if path.exists() and path.read_bytes() == data:
        return
    if path.suffix.upper() in ('.C', '.H'):
        from .format import format_bytes
        data = format_bytes(data, path)
        if path.exists() and path.read_bytes() == data:
            return
    if _dry is not None:
        _dry[path] = data
        return
    # Write-then-rename: parallel `sr` runs may read the file meanwhile.
    tmp = path.with_name(f".{path.name}.{os.getpid()}")
    tmp.write_bytes(data)
    os.replace(tmp, path)
