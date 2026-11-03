# Street Rod

Reverse engineering of Street Rod (Logical Design Works / California Dreams, 1989), DOS version 1.0.

## Findings so far

- Built with Microsoft C 5.1, medium model, 8087 emulator: `/AM /FPi`. The C runtime matches `MLIBCR.LIB` at 93.6% and the emulator matches `EM.LIB` at 96.8%. The rest are fixup bytes the matcher doesn't mask yet.
- `0x032D0` (`src/T32D0.C`, 36 bytes) and the copy-protection routine `0x2FC8` (`src/PROTECT.C`, 403 bytes) byte-match with `/AM /Oas /Gs /FPi`.
- MSC 5.1 assigns stack slots to locals in an order that depends on the identifier names, not on declaration order, so local names are load-bearing. `src/PROTECT.C` uses single letters, which MSC lays out alphabetically from the top of the frame. The hash behind this ordering hasn't been worked out yet.
- Other modules may use other flags. Some have intrinsic `strlen`, `loop` and `_disable`/`_enable`, which point to `/Oi` and `/Ol`.
- The EXE is packed with Microsoft EXEPACK. About 92% of the game's own code is compiled C and about 7% is hand-written asm (line drawing, unrolled sprite blitters, video helpers).
- The copy protection is the manual color lookup in the far function at `0x2FC8`.

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
| `src/funcs/` | Reconstructed functions, one per file |
| `tools/sr/` | The `sr` CLI; `tools/unexepack.py` removes EXEPACK |
| `db/sr.sqlite` | Function database |
| `work/` | Scratch output (ignored) |

## Usage

`CLAUDE.md` describes the workflow and the `./sr` CLI, which drives everything (compile in DOSBox-X, byte compare, the function DB, progress in `docs/PROGRESS.md`). `./dosbox.sh` opens an interactive DOSBox-X with the toolchain on `D:`.
