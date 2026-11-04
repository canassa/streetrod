# Far data and far storage

These files hold the game's far data segments as source records. The build assembles or
compiles each one as an object of its own and links it at its recovered place. Nothing is
extracted from the original executable. The names of the files, segments and labels are
placeholders: the shipped EXE does not record them.

| File | What |
|---|---|
| `FONTDATA.ASM` | EGA font: 91 nine-byte glyphs (`'CODE'` class, linked before the code region at 0F38A) |
| `CGAFONT.ASM` | CGA font: 91 eighteen-byte glyphs, WORD aligned |
| `SCREENS.ASM`, `SCREEN2.ASM`, `SCRATCH.ASM` | Screen and scratch Bitmap descriptors |
| `NEEDLE.ASM` | Gauge needle points (tip and hub, 131 positions) |
| `SONG.ASM`, `EFFECTS.ASM` | Sound instruction words |
| `SPANS1.ASM`, `SPANS2.ASM` | Span records of the roadside sprites |
| `SLOTS.C` | 277 Slot records and five terrain work arrays: far communals (`FAR_BSS`) |

DGROUP is not here. Each C source defines its own initialized globals and literals
(addresses in `tools/sr/symbols.json`; `./sr dgroup check` verifies the ownership). Its uninitialized storage is
file statics (`_BSS`) or communals. `src/UI/MESSAGES.ASM` is an assembler data file
for DS:239E..4718. The runtime's DGROUP comes from MLIBCE.LIB.

The CGA font's segment frame is 0ED2 and its first glyph is at offset 4. The three bytes
before it belong to the last EGA glyph, and one alignment byte follows them. There is no
CGA header, and the arrays do not overlap.

SPANS2 ends at 3884:0000. The next 0x170 bytes are EM.LIB's EMULATOR_DATA, which comes
from MLIBCE.LIB. Earlier these bytes were misread as the end of the Part array. LINK puts
the far communals of `SLOTS.C` after every object (389B:0000), and it orders them by the
hash bucket of each symbol. That is why the names were chosen to fit the original order.

`tools/scripts/check_data_types.py` compiles `sizeof`/`offsetof` for every struct in
`STRUCTS.H` with MSC 5.1 and compares them with the Python type model (`tools/sr/cdata.py`)
that `./sr dgroup` uses to lay out and check the data definitions:

```
nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'
```
