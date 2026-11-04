# Street Rod

Reverse engineering of Street Rod (Logical Design Works / California Dreams, 1989), DOS version 1.0.
The C and assembly sources rebuild the shipped `SR.EXE` byte for byte with the original tools.

## Build the game

The build uses the tools the game was built with, as unmodified binaries in `toolchain/`:
Microsoft C 5.1 (`CL`, `LIB`, `LINK`, `MAKE`) and MASM 5.1. The build definition is checked
in: [`src/MAKEFILE`](src/MAKEFILE) for MSC MAKE 4.07 and the LINK response file
[`src/SR.LNK`](src/SR.LNK). `MAKE MAKEFILE` in `src/` runs its blocks in order:

1. LIB combines MSC's medium-model emulator libraries into `MLIBCE.LIB`, as MSC SETUP
   does: `LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;` (the component order
   matters).
2. CL and MASM compile each original object from its source once, with that file's flags
   (each subsystem folder holds its sources and headers; `DATA\` holds far data).
3. `LINK /MAP /STACK:2560 /EXEPACK @SR.LNK` links the objects in the recovered order. It takes
   the runtime from `MLIBCE.LIB`, which the objects name as their default library.

The result is byte-identical to `orig/SR.EXE` (SHA-1 `3a5e28b795313028b7e77d147122f41cb29de334`).
All game data, including fonts, sprite records, gauge needle points and music, comes from source.
The build never reads data out of the original executable.

### Natively in DOS

Run `SRENV.BAT` before MAKE: MSC searches the including file's folder first, but MASM searches only INCLUDE. The 87-character batch line covers every project folder without lengthening compiler commands.

The DOS drives are `C:` the build directory (a copy of `src/`), `D:` `toolchain/msc51` and
`F:` `toolchain/masm51`, with the environment MSC SETUP sets up:

```text
set PATH=D:\BIN;F:\BIN
set LIB=D:\LIB
set TMP=C:\TMP
c:
call SRENV.BAT
MAKE MAKEFILE
```

`./dosbox.sh` opens DOSBox-X like this on a fresh copy of `src/` in `work/dos`.
`./dosbox.sh --make` runs `MAKE MAKEFILE` headless and compares the resulting `C:\SR.EXE` with
`orig/SR.EXE` (about 20 s with `cycles=max`). MAKE 4.07 reads its makefile name as an argument
and runs the blocks in file order (no NMAKE syntax).

### On the host

```sh
./sr build          # or: nix-shell shell.nix --run make
```

This copies `src/` to `work/layout` and runs every MAKEFILE block exactly as written: CL,
MASM and LIB under emu2, in parallel, and LINK in DOSBox-X. That part does not need `orig/`
(`./sr build --no-verify` stops after it). Then it verifies: each
code object is checked against the original, the link is repeated without `/EXEPACK` for
`./sr linkcheck`, and it ends with `Packed EXE vs orig/SR.EXE: IDENTICAL`. The packed EXE is
`work/layout/SRPACKED.EXE`; `work/layout/SR.EXE` is the unpacked link.
`./sr build --objects-only` stops before linking.

To play the build, copy it and the assets in `orig/freeware/` to a separate directory. Leave
`orig/` untouched. The retail manual-color copy protection is still in the EXE.

## Findings so far

- Microsoft C 5.1, medium model, with the 8087 emulator. Each source file has its own flags.
- All 509 C and 63 assembly game functions are reconstructed. Each one sits at its original
  address in the rebuilt EXE.
- The 147 MSC runtime entries come from the original libraries.
- MSC's local-variable names affect stack layout, and LINK's symbol hash affects where
  communal storage goes. The CLI models both.
- The original EXE is EXEPACK-packed by LINK `/EXEPACK`.

Addresses are linear offsets into the unpacked load image (`orig/SR_unpacked.EXE` minus its `0x22D0` byte header).

## Layout

| Path | What |
|---|---|
| `orig/SR.EXE` | Reconstructed retail v1.0 EXE: the freeware EXE with the crack reverted and the `MsDos` trailer removed. SHA-1 `3a5e28b795313028b7e77d147122f41cb29de334` |
| `orig/SR_unpacked.EXE` | The same EXE with EXEPACK removed (`tools/unexepack.py`) |
| `orig/freeware/` | The files from the freeware download, as shipped. Its `SR.EXE` has the protection NOPed out at file offset `0x334E` |
| `orig/extra/` | `LIB2C` and `README.VGA` from the original disks, missing from the freeware copy (taken from the archive.org copy) |
| `dist/` | The downloads as fetched: streetrodonline.com `sr1pc.zip`, archive.org `msdos_Street_Rod_1989`, and The Good Old Days `001593` (a 720K image with an uncracked EXE whose CGA/Hercules menu strings were edited) |
| `toolchain/msc51/` | MSC 5.1 `BIN` (CL, LIB, LINK, MAKE), `INCLUDE` and `LIB`, unpacked from the WinWorld 5.25" disk set in `toolchain/msc51/dist/` |
| `toolchain/masm51/` | MASM 5.1 |
| `src/SYS/`, `UI/`, `GAME/`, `VIDEO/`, `CGA/`, `EGA/`, `TANDY/`, `HERC/`, `INPUT/`, `SOUND/` | C and assembly sources with their same-name headers |
| `src/DATA/` | Far data and storage as source records (see [src/DATA/README.MD](src/DATA/README.MD)) |
| `src/SRENV.BAT` | DOS include search path for the subsystem folders, DATA and the compiler headers |
| `src/MAKEFILE`, `src/SR.LNK` | The build: MSC MAKE 4.07 makefile and LINK response file (the recovered link order) |
| `tools/sr/` | The `sr` CLI; `tools/unexepack.py` removes EXEPACK |
| `docs/` | `RECONSTRUCTION.md` (the reconstruction choices) |
| `work/` | Scratch output (ignored) |

## Usage

`CLAUDE.md` describes the workflow and the `./sr` CLI, which drives everything: compiling,
byte comparison and the gates. Original function extents and data addresses live in `tools/sr/symbols.json`; the tools
join them by name to definitions and declarations in the sources and headers. `make check` runs the
per-function regression gate (`./sr check --all`).

After merging annotated sources, extract their current symbol names and addresses before stripping:

```sh
nix-shell shell.nix --run 'python3 tools/strip_address_comments.py --extract'
nix-shell shell.nix --run 'python3 tools/strip_address_comments.py'
```

Extraction merges annotations with metadata for already stripped symbols, dropping stale names absent
from source. Conflicting annotations abort without writing. Stripping refuses missing or conflicting
metadata before changing any source; `--check` checks without writing.

`./sr linkcheck work/layout/SR.EXE` compares the unpacked link with `orig/SR_unpacked.EXE`
and does not mask any bytes. Its report is `work/layout/BYTE-MATCH.JSON`. The header, the
relocation order and 14 bytes at the end of the load image still differ there (`load_image:
0 differing, 14 missing`): the unpacker synthesized the header and relocation table and left
the 14 filler bytes in `orig/SR_unpacked.EXE`. The packed comparison above is the one that
counts.

## What is reconstruction

[docs/RECONSTRUCTION.md](docs/RECONSTRUCTION.md) lists everything in the tree that is a
reconstruction choice rather than recovered fact: names, file and segment names, data-only
files, provisional dummies, source forms the code cannot decide, and the build file's form.

## C formatting

`./sr format` formats every `.C` and `.H` under `src/`; pass file paths to limit
it. `./sr format --check` writes nothing and exits nonzero on drift. Assembly and
`.INC` files are excluded, including when passed explicitly. The pinned style is
`tools/sr/clang-format.yaml`, applied explicitly so editors cannot discover it as
`.clang-format`. `./sr headers` and `./sr rename` format the C text they change.

The nix shell supplies clang-format and lefthook. Install the staged-C formatting
hook once with `nix-shell shell.nix --run "lefthook install"`; it formats staged
C sources and headers and re-stages them. Run the byte-match gates before committing.
