from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

EXE_PACKED = ROOT / "orig" / "SR.EXE"
EXE = ROOT / "orig" / "SR_unpacked.EXE"
DB = ROOT / "db" / "sr.sqlite"
SRC = ROOT / "src"
INCLUDE = SRC / "include"
TOOLCHAIN = ROOT / "toolchain" / "msc51"
MASM = ROOT / "toolchain" / "masm51"
WORK = ROOT / "work"
PROGRESS = ROOT / "docs" / "PROGRESS.md"

# Linear addresses in the unpacked load image.
CODE_END = 0x2BEB0
CRT = (0x1E160, 0x21960)
EMULATOR = (0x2BEB0, 0x2E3E0)
DGROUP_SEG = 0x3E96

STATUSES = ("todo", "wip", "matched", "partial", "blocked", "skip")
KINDS = ("c", "asm", "crt")
