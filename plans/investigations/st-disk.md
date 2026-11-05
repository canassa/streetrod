# Disk, picture-slot, font and needle type investigation

Investigated in the requested order: `Offs`, `Panel`, `Spr12`, `Spr6`, `Pos6`,
`PicLibSelection`, `Slot`, `Glyph`, `NeedlePoint`. This note was written before
renaming. Evidence pins below refer to the starting sources; function names and
old member names identify the statements after documentation moves the lines.
Only these types, their member accesses and their value-set constants are in
scope. Unrelated identifiers are recorded at the end.

## Shared loading and layout evidence

`src/SYS/DISK.C:46-85` defines the near DGROUP car tables, the 13 destination
pointers and their exact read lengths. `di_load_data` (`DISK.C:502-532`) reads
those blocks sequentially from `hot_data`, with no conversion or write-back.
These are model/specification tables, not save-game records. The original
`orig/freeware/HOT_DATA` is 4064 bytes. Its relevant blocks are:

| Original type | Table | File offset | Records | Bytes per record |
| --- | --- | ---: | ---: | ---: |
| Spr12 | g_layoutAutoSprArray | 916 | 26 | 12 |
| Panel, embedding Offs | g_bodyRects | 1228 | 26 | 20 |
| Spr6 | g_pointsModelService | 1748 | 26 | 6 |
| Pos6 | g_imageDecalLayout | 4016 | 8 | 6 |

All field widths, signedness and order remain fixed. DOS ints and near pointers
are two bytes; longs and far pointers are four bytes. `unused` fields stay
present, including those that receive initialization but have no readers.

## 1. Offs -> CarHotRect

Purpose: one model-relative interactive rectangle on the car side image.
Storage: embedded five times in each Panel, loaded as part of its 520-byte block.
The four unsigned bytes at offsets 0..3 are x, y, width and height in pixels.

| Member | Role and actual loaded range | Readers/writers |
| --- | --- | --- |
| x | Left offset from car image origin; 0..246 | GARAGE.C:220-246 adds car X; BOBS.C:659,668 adds displayed car X |
| y | Top offset from car image origin, downwards; 0..47 | GARAGE.C:221-247 and BOBS.C:660,669 add car Y |
| w | Horizontal extent; 17..126 | GARAGE.C:222-246 forms x2=x1+w; BOBS.C:661,670 uses x1+w-1 |
| h | Vertical extent; 10..15 | GARAGE.C:223-247 forms y2=y1+h; BOBS.C:671 uses y1+h-1 |

The sole writer is the bulk data loader. Bounds are not universally inclusive:
the garage copies exclusive limits into Hotspot; Bob's subtracts one. Bob's roof
region also replaces its bottom with another hotspot's top minus one. Do not
claim these bytes are already screen endpoints or normalize that behavior.
No enum is warranted for coordinates or extents.

## 2. Panel -> CarHotspots

Purpose: the five car-image interaction regions selected by model. It is a
20-byte record, with `r` renamed `regions`, holding five CarHotRect records.
`place_spots` (`GARAGE.C:213-249`) maps each index to a garage hit code;
`KEYMAPS.H:77-81`, `HOTSPOT.C:141-145` and the `GAME.C:915-934` dispatch prove:

| Index | Region | Garage hit code | Other readers |
| ---: | --- | ---: | --- |
| 0 | Rear bumper | 11 | none |
| 1 | Roof | 13 | BOBS.C:659-661 |
| 2 | Transmission | 9 | none |
| 3 | Hood | 10 | BOBS.C:668-671 |
| 4 | Front bumper | 12 | none |

Name this set with CarRegion and CAR_REGION_COUNT. Replace all five numeric
subscripts in GARAGE.C and both subscripts in BOBS.C, the embedded array bound,
and the parallel `g_hIds` bound and hide_spots loop limit. Keep the existing
hit-code identifiers and other UI arrays outside this type unchanged.

## 3. Spr12 -> CarPicLayout

Purpose: the twelve pixel offsets used to assemble a model's side-view car.
All fields are unsigned bytes, loaded only by di_load_data; there are no runtime
member writers. `cp_build` (`CARPIC.C:280-420`) reads every field;
`TIRES.C:207-208` also reads both wheel positions. All coordinates use the
unshifted car image's top-left as origin, with Y downwards.

| Offset | Old -> new member | Role | Loaded range |
| ---: | --- | --- | --- |
| 0 | wheelX1 | First tire image's left X | 14..60 |
| 1 | wheelX2 | Second tire image's left X | 152..218 |
| 2 | b2 -> portraitX | Portrait underlay's left X | 75..138 |
| 3 | b3 -> portraitBottom | Inclusive portrait bottom before roof correction | 17..21 |
| 4 | roofX | Roof-change patch's left X; Y is fixed at zero | 0..72 |
| 5 | b5 -> engineX | Large-engine overlay's left X | 0..207 |
| 6 | b6 -> engineY | Large-engine overlay's top Y | 0..11 |
| 7 | rearBumperY | Rear-removal patch's top Y; X is zero | 0..43 |
| 8 | frontBumperX | Front-removal patch's left X | 0..240 |
| 9 | frontBumperY | Front-removal patch's top Y | 0..42 |
| 10 | stickerX | Sticker-background origin X | 0..133 |
| 11 | stickerY | Sticker-background origin Y | 0..27 |

Portrait Y is `b3 - portrait.height + roofCut + 1`, proving a bottom anchor
rather than a top. Engine offsets go to bm_draw_pic with the model's engine
picture; roof/bumper offsets go to bm_put_pic (BITMAP.C:242-270). Wheel Y is
computed by bottom-aligning tire and car images; frame phase is added to wheel
X separately. Sticker X is split into byte-aligned and residual positions for
interactive drawing. Zero is a valid coordinate, not a blanket absence marker;
body-picture availability comes from Model. The King's unavailable overlays
and sticker coordinates are zero in the supplied data.

## 4. Spr6 -> ServicePoints

Purpose: model-relative fuel-cap and window-service anchors for the gas station.
Six unsigned bytes, no padding, loaded in the 156-byte block. Only cp_build
(`CARPIC.C:299-307`) reads these fields; it copies them into g_featurePos.

| Offset | Member | Role | Loaded range |
| ---: | --- | --- | --- |
| 0 | fuelCapX | Fuel cap X | 0..85 |
| 1 | fuelCapY | Fuel cap Y | 0..36 |
| 2 | rearGlassX | Rear-window service X | 0..76 |
| 3 | rearGlassY | Rear-window service Y | 0..6 |
| 4 | frontGlassX | Windshield service X | 0..176 |
| 5 | frontGlassY | Windshield service Y | 0..10 |

Coordinates are image-relative pixels, Y downwards. `hasRearGlass` is true only
when both rear coordinates are nonzero. cp_show_car translates the windows
only when frontGlassX>0, and always translates the cap (CARPIC.C:499-506).
The King's row is all zero. Downstream evidence: RACE.C:144-166 builds fuel and
window hotspots, CARPIC.C:591-596 draws the fuel cap, and RACE.C:197-200,265-268
uses its anchor for the cap/nozzle animation. These fields are coordinates,
not Boolean flags; retain unsigned char even where zero disables a feature.

## 5. Pos6 -> StickerPos

Purpose: the picture and placement within the common car sticker background.
Eight six-byte records loaded from HOT_DATA; never changed or saved. The order
is menu choice order, not numeric picture order. `id` becomes `pictureId` and
keeps enum PictureId (two-byte signed enum); `dx` and `dy` stay signed ints.

| Offset | Member | Role / values | Readers |
| ---: | --- | --- | --- |
| 0 | id -> pictureId | Sticker pictures 1186..1193, all eight once | CARPIC.C:333; GARAGE.C:593 |
| 2 | dx | Rightward pixel offset within sticker background; 0,1,2,4,8 | same draw calls |
| 4 | dy | Downward pixel offset; 0,1,2,5,6 | same draw calls |

cp_build indexes sticker-1 and adds these offsets to the model's sticker origin.
ga_stickers indexes the chosen menu number minus one and adds the residual
unaligned X phase when rebuilding the saved sticker background. The source
values happen to be nonnegative; that is no basis to change signedness or
invent an enum for spatial offsets. Existing PictureId names suffice.

## 6. PicLibSelection

Purpose: a transient view of whichever picture library a requested ID needs.
One eight-byte file-static record `g_picLib` (DISK.C:44), zero-initialized at
startup. It is constructed by open_lib for each selection, not read from disk
or saved. Every member is a two-byte near pointer; none owns an allocation.

| Offset | Member | Writer and pointed-to state | Readers |
| ---: | --- | --- | --- |
| 0 | count -> entryCount | open_lib selects g_lib1Count or g_countLibBmB | open_lib reads the file's count through it; load_pic computes directory extent |
| 2 | handle | open_lib selects address of g_lib1Handle or g_lib2Handle | open_lib, di_read_index and load_pic read/seek/close/reopen the selected DOS handle |
| 4 | path | open_lib selects g_library1Path or g_picLibBPath | open_lib/load_pic open the path and use its drive letter in prompts |
| 6 | otherHandle | open_lib selects the other handle's address | open_lib closes and clears it when the selected file must be opened |

Evidence: DISK.C:134-171, 195-197, 248-284. The count is entries in the selected
file, not total pictures, loaded bytes or a cached allocation count. Handle
zero is the program's closed sentinel. Selecting a library doesn't close the
other handle if the selected handle is already open. Paths are borrowed from
di_open_libs's mutable drive-prefixed names. Retain the accurate type name.

## 7. Slot -> PictureSlot

Purpose: one picture's disk directory header plus its runtime packed-data cache.
`src/DATA/SLOTS.C:4` allocates 277 far communal 48-byte records. di_read_index
reads exactly the first 30 bytes from `2 + i*30` in the selected library, then
initializes selected runtime fields. No slot is saved. Only DISK.C accesses its
members. Picture IDs map to shared indices via pic_index; CGA additionally
remaps indices, so this is not generally directly indexed by PictureId.

| Offset | Member | Role and evidence |
| ---: | --- | --- |
| 0 | width | Signed int pixel width; read by di_pic_size and di_unpack_pic |
| 2 | height | Signed int pixel height; same readers |
| 4 | unused1[2] | Unused on-disk word, not a decoder size limit |
| 6 | bitmapFlags | Unsigned byte selecting opaque, masked or mask-only allocation; passed to me_new_bitmap |
| 7 | pal -> rleTokens[16] | Per-picture byte dictionary tokens, copied to decoder stack buffer |
| 23 | unused2 | Unused directory byte; zero in supplied libraries |
| 24 | dataOffset | Signed long offset relative to end of selected library directory |
| 28 | packedSize -> packedSpan | Signed int span from opening BC marker to closing BC marker, excluding the closing byte |
| 30 | vramPlane | Unsigned byte: EGA plane index for video-cache data, or NOT_IN_VRAM for ordinary RAM |
| 31 | unused3 | Unread runtime byte; not initialized explicitly |
| 32 | ptr -> packed | Four-byte FarPtr to opening BC marker; zero means uncached |
| 36 | unused4 | Unread runtime long, reset to zero |
| 40 | unused5 | Unread runtime long, reset to zero |
| 44 | unused6 | Unread runtime int, reset to -1 |
| 46 | unused7 | Unread runtime byte, reset to 1; no proof it is a Boolean |
| 47 | unused8 | Unread runtime byte; not initialized explicitly |

Source evidence: DISK.C:174-210 initializes, 219-289 loads, 294-303 frees,
308-360 unpacks, 366-372 reports dimensions. load_pic computes absolute file
position as `2 + entryCount*30 + dataOffset`, reads packedSpan+1 bytes, and
validates BC at both ends. The actual compressed stream is at packed.p+1 with
length packedSpan-1. Adjacent entries share a framing byte. The video-cache
path allocates/copies only packedSpan bytes, discarding the final BC after
validation; the initial framing byte remains. Do not call packedSpan the
full framed length or the decoded size.

`MEM.C:310-352` interprets allocation flags: zero is color only, one reserves
a mask, two is mask only. The supplied EGA/CGA directories use exactly these
three values; name their wire value set PictureMask, keeping bitmapFlags a
byte. The generic allocator's flags parameter and literal callers are outside
the requested types; its broader integer interface is not retyped here.
`RLE.ASM:35-55,75-113` maps the sixteen tokens to g_rleCounts/g_rleValues,
proving that the dictionary is unrelated to color-palette remapping.
TANDYPIC's unpackers receive the same tokens.

Resource audit: EGA libraries have 9+268 entries, CGA 4+273. Across both sets,
width=8..320, height=4..200, packedSpan=13..17336; all directory flag values
are 0,1,2. Unused word +4 ranges 28..32000 and is never read by game code.

me_vram_alloc (`MEM.C:394-409`) returns a plane index 0..3, or -1 on failure.
load_pic uses existing NOT_IN_VRAM when not caching, selects the plane's map
mask before copying, and selects its read map while decoding. di_free_pic
clears only packed.p; the plane byte need not reset. A temporary buffer load
is detached after unpacking when LOAD_READ is returned, while preloaded cache
pointers persist. Do not describe the far pointer as always owned heap memory.
Keep all existing spare members, their types and initialization unchanged.

## 8. Glyph

Purpose: one variable-advance, eight-row monochrome glyph used by EGA and Tandy.
A nine-byte, byte-aligned record in the linked FONTDATA segment. FONTDATA.ASM's
GLYPH macro emits one width byte and eight row bytes for each character 32..122
(91 records). No runtime writes, disk loads or save-game serialization.

| Offset | Member | Role / value set |
| ---: | --- | --- |
| 0 | w -> advance | Unsigned byte text advance in pixels, supplied values 2..7 |
| 1 | rows[8] | Signed char storage for eight raster bytes; top to bottom, high bit left, set means foreground |

EGA.C:58-80 takes a byte pointer to the record, reads the advance, then walks
the row bytes, and falls back to the space for unsupported characters.
TANDYTXT.ASM:33-109 computes index*9, reads the advance and rows, and tests
80h downward, proving the bit orientation. DASH.C:736-743 and RACE.C:486-497
read the advance to measure/truncate text. These measurement paths don't do
the renderer's bounds check; don't promise that the type validates character
codes. Retain row char signedness; readers reinterpret bytes explicitly.
The CGA font is a separate CgaGlyph format and is outside this task.

## 9. NeedlePoint

Purpose: signed pixel offset of a dashboard needle endpoint from its hub center.
Two signed chars at offsets 0 (x) and 1 (y), two bytes total. NEEDLE.ASM embeds
two arrays of 131 points under one g_needlePts far symbol. The outer dimension
already has NeedleEnd constants; retain them. No disk load, saves or writers.

x is positive rightwards; y is positive upwards. DASH.C:105-114 and 153-162
clamp the shared angle index to 0..NEEDLE_MAX_POS, add x to the corresponding
gauge center X and subtract y from its center row. Speedometer and tachometer
share both tip and inner endpoint arrays. The supplied tip coordinates span
x=-18..18, y=-13..18; inner coordinates x=-5..5, y=-3..5.

Retain accurate type and member names. Existing endpoint enum plus index-limit
macro express the value sets; add the missing endpoint count and use it for
the table's outer bound. The PROVISIONAL unreachable far references in DASH.C
and NEEDLE.ASM remain intact: their identity cannot be recovered from live
reads. Inner points form a short radius around the center rather than the
center itself; do not document NEEDLE_HUB as a fixed (0,0) pivot.

## Other misleading names and out-of-scope findings

- g_bodyRects stores transmission and hood hit rectangles as well as body
  modification regions (GARAGE.C:213-249); it isn't a collection of body panels.
- g_layoutAutoSprArray and g_imageDecalLayout hide car assembly and sticker
  placement meanings; their declarations and readers are listed above.
- DISK.C:di_unpack_pic's colors16 and RLE.H's palette wording describe dictionary
  token bytes as colors. RLE.ASM proves the actual dictionary mapping.
- DISK.C:di_read_index's height local tracks the maximum packed span, not height.
- Generic bitmap constructor `flags` parameters/literals in MEM/ARENA/BITMAP
  express allocation-mask choices; naming that entire allocator interface is
  outside these types. PictureMask describes only the library byte values.
- PicLibSelection stores borrowed pointers, not copies of open handles/counts.
  g_countLibBmB/g_picLibBPath use an arbitrary B suffix for the second library.
- TANDYTXT.ASM's opening comment calls this an 8x8 font, although its advance
  varies; its hard-coded segment commentary is unrelated to the type contract.
- Existing unused Slot bytes have no demonstrated semantics. The loader's
  writes do not establish callback, linked-list, animation or Boolean roles.

## Changes and validation

Applied all type/member mappings described above, retaining every member's
size, signedness, order and all spare fields. Updated existing references to
the renamed picture-slot type in the data README and reconstruction notes.
No globals, functions, parameters, locals or other types were renamed.

Value-set replacement inventory:

- CarRegion: all twenty component reads in place_spots (five indices), all
  seven component reads in setup_bobs_spots (two indices), the embedded region
  bound, g_hIds bound and hide_spots loop limit.
- PictureMask: documents the three on-disk byte values. These are loaded as
  bytes; no literal assignments/comparisons of bitmapFlags exist in the tree.
  The enum does not widen the field or alter the generic allocator interface.
- NOT_IN_VRAM: reused the existing sentinel without changing any value or use;
  its documented definition now accompanies the slot field in SLOTS.H.
- NEEDLE_END_COUNT: replaces the sole raw outer array bound; all live endpoint
  accesses already used NEEDLE_TIP/NEEDLE_HUB.

Pre-merge validation:

- Header/include dependencies and C formatting pass.
- Doxygen reports zero diagnostics.
- All 572 functions match; all 96 object gates pass, with all 748 near calls exact.
- Clean `rm -rf work/layout && ./sr build`: packed EXE IDENTICAL to orig/SR.EXE.
- DOS-native `./dosbox.sh --make`: IDENTICAL, SHA1
  3a5e28b795313028b7e77d147122f41cb29de334.
- DGROUP source ownership and link order pass.
- `python3 -m unittest discover -s tests` under shell.nix: 109 tests, OK.
- Direct MSC 5.1 audit of these nine records: 59 sizeof/offsetof checks, zero
  mismatches (sizes 4,20,12,6,6,8,48,9,2 in the requested order). Every expected
  offset is listed in the member tables above.

The whole-tree tools/scripts/check_data_types.py audit cannot parse an existing
Car array dimension using TRANSMISSION_BOLT_COUNT, as already documented in
CLAUDE.md and sr-cli. No parser change or guessed layout was introduced. The
targeted audit compiled all nine records with the real compiler and compared
each sizeof/offsetof result to the fixed layouts above.

Final origin/main merge and repeated gate results follow below.
