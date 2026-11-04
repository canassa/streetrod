from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

EXE_PACKED = ROOT / "orig" / "SR.EXE"
EXE = ROOT / "orig" / "SR_unpacked.EXE"
SRC = ROOT / "src"
TOOLCHAIN = ROOT / "toolchain" / "msc51"
MASM = ROOT / "toolchain" / "masm51"
WORK = ROOT / "work"

# Linear addresses in the unpacked load image.
CODE_END = 0x2BEB0
CRT = (0x1E160, 0x21960)
DGROUP_SEG = 0x3E96

KINDS = ("c", "asm", "crt")


def source_index():
    out = {}
    for path in sorted(SRC.rglob('*')):
        if path.is_file() and path.suffix.upper() in ('.C', '.ASM', '.H', '.INC'):
            name = path.name.upper()
            if name in out:
                raise ValueError(f'duplicate source name: {out[name]} and {path}')
            out[name] = path
    return out


def source_files(*extensions):
    return [p for p in source_index().values() if p.suffix.upper() in extensions]


def source_path(name):
    return source_index()[name.upper()]


def stage_headers(dest, flat=False):
    import shutil
    for path in source_files('.H', '.INC'):
        target = dest / (path.name if flat else path.relative_to(SRC))
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(path, target)


def dos_environment():
    lines = (SRC / 'SRENV.BAT').read_text().splitlines()
    return tuple(line[4:] for line in lines if line.upper().startswith('SET '))
