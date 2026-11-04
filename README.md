# Street Rod

Reverse engineering of Street Rod (Logical Design Works / California Dreams, 1989), DOS version 1.0.

## Build the game

On the host, with Nix and the checked-out MSC/MASM toolchains:

```sh
nix-shell shell.nix --run make   # or ./sr build
```

This compiles 147 game modules and 12 reconstructed data modules, then links
`work/game/SR.EXE` with the original MSC 5.1 libraries. It also writes `SR.MAP`,
compiler logs and linker diagnostics. All game data comes from `src/data/` source;
the build does not extract it from the original EXE.

For a DOS-native build, run `./sr build --prepare-only` on the host, then mount these directories
in DOSBox-X (substitute the checkout's absolute path):

```text
mount c /path/to/streetrod/work/game
mount d /path/to/streetrod/toolchain/msc51
mount e /path/to/streetrod/work/game/INCLUDE
mount f /path/to/streetrod/toolchain/masm51
c:
ENV.BAT
MAKE MAKEFILE
```

`BUILD.BAT` is also provided as an unconditional rebuild. MSC's MAKE 4.07 takes
its input filename positionally and processes rules in order. The generated
Makefile uses this syntax rather than NMAKE syntax.

Run the resulting EXE from a directory containing the assets in `orig/freeware/`.
Keep the original EXE intact; copy the assets and rebuilt EXE to a separate run
directory. The retail manual-color protection is preserved.

`make objects` compiles without linking; `make check` runs the reconstruction
regression gate. See [src/data/README.md](src/data/README.md) for the data audits.
The linked build is not yet a byte-identical reproduction of the shipped EXE:
its segment layout and cross-module call encodings differ. All 572 reconstructed
game functions still pass their original function/module checks.

## Findings so far

- Microsoft C 5.1, medium model, with the 8087 emulator; flags vary by module.
- All 506 C and 66 assembly game functions are grouped in 147 verified modules.
- The 147 MSC runtime entries are supplied by the original libraries.
- MSC local-variable names affect stack layout; the CLI models that rule.
- The original EXE is EXEPACK-packed. The rebuilt executable is unpacked.

Addresses are linear offsets into the unpacked load image (`orig/SR_unpacked.EXE` minus its `0x22D0` byte header).

## Layout

| Path | What |
|---|---|
| `orig/SR.EXE` | Reconstructed retail v1.0 EXE: the freeware EXE with the crack reverted and the `MsDos` trailer removed. SHA-1 `3a5e28b795313028b7e77d147122f41cb29de334` |
| `orig/SR_unpacked.EXE` | The same EXE with EXEPACK removed (`tools/unexepack.py`) |
| `orig/freeware/` | The files from the freeware download, as shipped. Its `SR.EXE` has the protection NOPed out at file offset `0x334E` |
| `orig/extra/` | `LIB2C` and `README.VGA` from the original disks, missing from the freeware copy (taken from the archive.org copy) |
| `dist/` | The downloads as fetched: streetrodonline.com `sr1pc.zip`, archive.org `msdos_Street_Rod_1989`, and The Good Old Days `001593` (a 720K image with an uncracked EXE whose CGA/Hercules menu strings were edited) |
| `toolchain/msc51/` | MSC 5.1 `BIN`, `INCLUDE` and `LIB`, unpacked from the WinWorld 5.25" disk set in `toolchain/msc51/dist/` |
| `src/mods/` | Reconstructed C and assembly compilation groups |
| `src/data/` | Reconstructed initialized data and working storage |
| `tools/sr/` | The `sr` CLI; `tools/unexepack.py` removes EXEPACK |
| `db/sr.sqlite` | Function database |
| `work/` | Scratch output (ignored) |

## Usage

`CLAUDE.md` describes the workflow and the `./sr` CLI, which drives everything (compile in DOSBox-X, byte compare, the function DB, progress in `docs/PROGRESS.md`). `./dosbox.sh` opens an interactive DOSBox-X with the toolchain on `D:`.

### Original binary layout

`./sr build --layout` rebuilds the recovered code/link order into `work/layout`.
All 572 game functions land at their original addresses, and all 748 near-call
encodings match. `./sr dgroup` recovers which module owned each DGROUP item from the
original data positions and writes the definitions into the module sources. The linked
load image is byte-identical to the original, and the `/EXEPACK` link
(`work/layout/SRPACKED.EXE`) differs from `orig/SR.EXE` only in the order of its packed
relocation table (331 bytes; one object boundary is left). `./sr linkcheck work/layout/SR.EXE` compares the linked file
without masking relocations or padding and returns nonzero until it is identical.
The detailed report is `work/layout/BYTE-MATCH.JSON`. Against the unpacked reference the
header and relocation order still differ: the unpacker synthesized both.
