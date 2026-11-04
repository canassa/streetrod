from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

EXE_PACKED = ROOT / "orig" / "SR.EXE"
EXE = ROOT / "orig" / "SR_unpacked.EXE"
SRC = ROOT / "src"
INCLUDE = SRC / "include"
TOOLCHAIN = ROOT / "toolchain" / "msc51"
MASM = ROOT / "toolchain" / "masm51"
WORK = ROOT / "work"

# Linear addresses in the unpacked load image.
CODE_END = 0x2BEB0
CRT = (0x1E160, 0x21960)
DGROUP_SEG = 0x3E96

KINDS = ("c", "asm", "crt")
