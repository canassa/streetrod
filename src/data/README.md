# Reconstructed game storage

These MASM 5.1 files provide initialized data and working storage for the real
linked executable. The build assembles these sources directly; it does not read
or extract sections from the original executable. No check-only `.STB` definitions
are linked into the game.

`DGROUP.ASM` describes the named globals, literal strings, typed table fields,
car-mask Run/List records, and symbolic pointers. Original offsets remain as
annotations and overlapping aliases. The linker may move the whole block.
Near pointers explicitly use DGROUP. Overlapping globals are real LABELs
placed with ORG: MASM EQU aliases lose the PUBDEF group association. Code and far-data
externals are declared outside segments to avoid imposing an incorrect frame.

The old runtime intervals DS:0000–0041 and DS:629C–6C21 are reserved as zero
storage to preserve relative game-data offsets. Their original contents are not
copied. MSC libraries supply their own startup, heap, stdio and floating-point
state. `CRT.H` maps three previously unnamed runtime references to `errno`,
`_doserrno` and `_amblksiz`. Game storage beginning at old DS:6C22 is zeroed:
the unpacked file's FF filler is not an initializer.

PARTS2 ends at 3884:0000. The following 0x170-byte block belongs to
EM.LIB (via MLIBCE.LIB), which supplies it in the layout build before SLOTS at 389B:0000.
Those bytes were previously misidentified as the end of the Part array.

Far-data modules contain font glyphs, Bitmap descriptors, track coordinates,
sound instruction words, car-mask Part records, Slot records and terrain arrays.
The EGA font has 91 nine-byte glyphs. The CGA font has 91 eighteen-byte
glyphs and WORD alignment. Its original segment frame is 0ED2 and its first
glyph is at offset 4: the preceding three bytes belong to the last EGA glyph,
followed by one alignment byte. There is no CGA header and the arrays do not
overlap. The earlier header/overlap interpretation was incorrect.

The reconstruction catalog and its checks live in
`tools/scripts/reconstruct_data.py`. It refuses unidentified nonzero data.
It is an audit/authoring tool, not a build dependency:

```
nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'
nix-shell shell.nix --run 'python3 tools/scripts/reconstruct_data.py --verify-built'
```

The first check compares 74 type sizes and 408 field offsets with actual MSC
output. The second checks source records against the reconstruction and then
compares every byte of linked game data, resolving pointers through `SR.MAP`
instead of ignoring relocation bytes. Use `--write-source` only when deliberately
updating the recovered records. Array bounds and identified continuation ranges
are recorded in that script; they are not inferred during normal compilation.
