# Bitmap investigation

## Purpose and scope

`Bitmap` describes a pixel rectangle, its optional transparency mask and the
storage kind that the active video driver must use when accessing its pixels.
Only this type, its member accesses and its kind values are in scope. Evidence
line numbers below refer to the source before the changes in this investigation.
This note was written before renaming any members.

## Storage, initialization and lifetime

The MSC record is 48 bytes, word-aligned, with a final alignment byte. It is
not the picture-library directory record (`Slot`), and it is not serialized in
the save game (`src/GAME/SAVEGAME.C:337-342,464-469`). `di_read_index` reads 30-byte directory records into `g_slot`
(`src/SYS/DISK.C:176-209`); `di_unpack_pic` constructs a runtime bitmap from its
width, height and allocation flags, then decodes the compressed payload into
the runtime buffers (`DISK.C:308-363`). The disk flags and the Bitmap kind byte
are separate values: the former controls color/mask allocation, the latter is
explicitly passed as 1 by the unpacker.

There are several storage forms:

- Far heap blocks: `me_new_bitmap` allocates the header and optional color/mask
  payload together, unless the caller supplies a header and buffers
  (`src/SYS/MEM.C:310-351`). `me_wrap_bitmap` allocates only a header for caller
  supplied pointers (`MEM.C:355-374`); no callers currently use that function.
  `me_set_bitmap` initializes an existing header (`MEM.C:377-391`).
- Arena blocks: `ar_bm` reserves the header, color bytes and optional mask bytes,
  seeds the color pointer, and calls `me_new_bitmap` with kind -1
  (`src/SYS/ARENA.C:76-95`). Arena marker operations determine their lifetime;
  the mask does not own a separate allocation.
- Fixed descriptors: `src/DATA/SCREENS.ASM:8-26` contains three 48-byte records;
  `SCREEN2.ASM:7-12` contains another EGA page; `SCRATCH.ASM:5-10` contains an
  initially zero descriptor. `src/GAME/ROAD.C:63-72` defines two initialized
  near EGA descriptors with the same layout.
- Embedded/communal headers: `g_bandRecordsSprArray` and
  `g_carBackdropHdrBmp` (`src/GAME/CARPIC.C:32-35`) and the four embedded headers
  in `src/GAME/TIRES.H:9-39`. The source constructs views and copies headers;
  copying a header aliases its buffers, it does not copy pixels. Examples are
  `cp_backdrop` (`CARPIC.C:194-198`), tire animation (`TIRES.C:97,162,297-305`),
  and EGA page setup (`src/VIDEO/VIDEO.C:153-161`).
- The cursor puts its image, mask, save descriptor and save data in one far
  block (`src/UI/CURSOR.C:39-49`), with an EGA save buffer in video memory.

`vi_init` selects driver geometry from the front descriptor's kind. RAM bitmaps
use the active driver's geometry, rather than storing their own bpp or plane
count (`src/VIDEO/VIDEO.C:104-141`). A bitmap must therefore be used with the
video mode for which its buffers were prepared.

## Members and fixed layout

| Offset | Original member / type | Meaning and evidence | Change |
| --- | --- | --- | --- |
| 0x00 | `width`, signed int | Width in pixels. Constructors copy w; blit wrappers use it as packet width (`MEM.C:347,369,387`; `src/VIDEO/BITMAP.C:21,230,313`). | Keep. |
| 0x02 | `height`, signed int | Height in pixel rows. Constructors copy h; clipping and row loops bound against it (`MEM.C:348,370,388`; `src/GAME/DASH.C:681-714`; `BITMAP.C:36,96`). | Keep. |
| 0x04 | `planesize`, unsigned int | Pixel-buffer span in bytes. Ordinary bitmaps: pitch * height * active plane count (`MEM.C:319-321,346,360-368,382-386`). EGA hardware pages instead initialize it to the size of one addressed plane (`ROAD.C:63-72`; `SCREENS.ASM:9,16`; `SCREEN2.ASM:8`). CGA/Tandy pages store full banked spans (`src/SYS/MAIN.C:192,214`). | Rename to `pixelBytes`. |
| 0x06 | `unused[0x1e]`, char array | Thirty bytes never read/written by member access. Fixed descriptors contain zeroes and FF sentinels at differing positions; whole-record assignment copies them. Their original purpose is unknown. | Keep name, bytes and order. |
| 0x24 | `bits`, `union FarPtr` | Color data pointer, or zero for mask-only images (`MEM.C:332-344`; `DISK.C:336-352`). The union permits pointer and segment/offset access without changing the four-byte representation. | Keep. |
| 0x28 | `alt`, `union FarPtr` | Optional transparency-mask pointer, also the sole data pointer of mask-only images. EGA makes it by complementing the OR of all color planes; Tandy makes transparent nibbles for zero-color pixels (`DISK.C:336-352`; `src/VIDEO/MASKBLT.ASM:145-212`; `src/TANDY/TANDYPIC.ASM:343-379`). | Rename to `mask`. |
| 0x2c | `pitch`, signed int | Bytes in one pixel row in a plane/bank. Constructors use ceil(width / pixels-per-byte) (`MEM.C:319,359,381`). Page setup uses the hardware row stride (`MAIN.C:193,215`). | Keep. |
| 0x2e | `flags`, signed char | Kind/provenance and driver storage selector, not bit flags. Work bitmaps get -1; decoded pictures get 1; display pages use the existing VID_* constants. The signedness is required by comparisons. | Rename to `kind`, retain signed char, document `BitmapKind`. |
| 0x2f | implicit alignment | No named member. Header size/allocation and fixed records include it. | Preserve. |

### Size, row and mask consumers

`pixelBytes` is not the size of one RAM plane: EGA shifts right twice to get
that (`src/EGA/EGASHIFT.C:166`), and mask callers divide by `r_vid.planes`
(`CARPIC.C:233-234,317`; `src/GAME/SHRINK.C:59-60,134`;
`src/GAME/TIRES.C:65,242`). EGA mask generation likewise divides the input
count by four (`MASKBLT.ASM:156-160`). RAM mask bytes are pitch * height.

On CGA pages, half of the span is the odd-row bank displacement
(`src/CGA/CGA.C:159,288`; `src/CGA/CGASHIFT.C:101`). On Tandy pages, a quarter
is a bank displacement (`src/TANDY/TANDY.C:51,97`;
`src/TANDY/TANDYBLT.C:277-279`). These spans include unused bytes at bank ends.
An EGA hardware view constructed by `me_set_bitmap` can retain the normal
four-plane computed span; EGA blitters use kind, pitch and height instead of
this field to select hardware plane addressing (`src/EGA/EGABLIT.C:51-60`).

Mask semantics follow the operation, not merely nonzero pointer presence.
`mk_copy` preserves destination where mask bits are set
(`MASKBLT.ASM:60-101`); CGA and Tandy masks have repeated bits/nibbles for each
packed pixel. The EGA cursor deliberately inverts its mask for its specialized
cursor hook (`CURSOR.C:67-74`), so the member documentation must not promise
one polarity for all consumers. Shift, underlay and mask-AND operations all
use this pointer (`BITMAP.C:35-55,88-118,150-151,202-220,284-285`). Some frames
share a mask (`CARPIC.C:353`); tire views clear it (`TIRES.C:299,302,305`).

Other buffer/row consumers are the compose/masked/copy functions in
`src/CGA/CGABLT.C`, `src/EGA/EGABLIT.C`, `src/TANDY/TANDYBLT.C`; shifted blits
in `CGASHIFT.C` and `EGASHIFT.C`; sprite stepping in `CGASPR.C`, `EGASPR.C`
and `TANDYSPR.C`; EGA sliding in `EGASLIDE.C`; car strips in `CARPIC.C`;
all three shrink paths in `SHRINK.C`; glyph/recolor drawing in `CGA.C`,
`EGA.C`, `EGARECOL.C` and `TANDY.C`. `GARGFX.C`, `TIRES.C`, `POPUP.C` and
bitmap grab helpers propagate mask presence into allocation requests.
`DRAW.C`, `DASH.C` and the sprite modules use width/height for clipping.

### Kind value set and every reader/writer

Existing `VID_EGA`, `VID_CGA`, `VID_HERCULES`, `VID_TANDY` are defined in
`src/SYS/MAIN.H:23-26` and mirrored as EQU constants in `MAIN.INC`. Reuse their
names/values by moving the C definitions into the BitmapKind enum owned by
BITMAP.H; retain the assembler definitions unchanged. Add only the two proven
RAM categories, `BITMAP_WORK` (-1) and `BITMAP_PICTURE` (1). Neither category
stores the physical pixel format by itself. No count is warranted: no code
indexes/loops over these sparse kind codes. Zero in the scratch descriptor is
uninitialized storage, not an active rendering category.

Writers and forwarded arguments:

- `me_new_bitmap` stores c; callers are `ar_bm`, `di_unpack_pic`, startup dash
  allocation, cursor setup, popup grabs, bitmap copy and bitmap shift. All pass
  -1 except the unpacker (1). Replace each of those raw kind arguments.
- `me_set_bitmap` stores its last argument; callers in CARHOP, CARPIC, TIRES
  and CURSOR pass VID_EGA or -1. Replace every -1 kind argument, including the
  conditional branches. `me_wrap_bitmap` has no callers. Type these three
  kind parameters as enum BitmapKind without renaming their parameters.
- `init_screens` writes the existing VID_* constants to fixed screen records
  (`MAIN.C:189-216`). `vi_init` normalizes the front kind to VID_CGA for the
  default driver path (`VIDEO.C:130`). Initialized C/ASM pages use VID_*
  already. Whole-header copies propagate the kind unchanged.

Readers (complete direct-access list):

- `cb_masked`, `cb_copy` (`CGABLT.C:109,145-153`) and `tb_masked`, `tb_copy`
  (`TANDYBLT.C:151,179-187`) test < -1 or >= -1 to choose banked or linear
  accesses. Replace their boundary literal with BITMAP_WORK; do not alter
  the comparison form.
- `eb_copy`, `eb_sprite`, `eb_masked` (`EGABLIT.C:51-60,142,200-201`) compare
  VID_EGA to use hardware registers and no contiguous color-plane advance.
- `eg_draw_text`, `er_rect` (`EGA.C:55`; `EGARECOL.C:25`) and `ed_line`
  (`src/UI/EDIT.C:35`) compute <= -1. The first two assigned locals are dead;
  `mono` is also never read. Retain these assignments, using BITMAP_WORK.
- `vi_init` reads the front kind into global mode (`VIDEO.C:112`). The
  `VIDEO.H:117` comment refers to that relation and needs its member name
  updated. Global `Video.mode` remains outside this type investigation.

## Edge cases and limits

- Allocators do not validate nonpositive dimensions, overflow or buffer capacity.
  Preserve signed widths/pitches, unsigned size and all original arithmetic.
- Mask-only allocation moves/clears the seeded color pointer and reserves only
  the mask payload; pixelBytes can describe hypothetical color storage that
  is absent. A null bits pointer is intentional, not an allocation failure.
- A supplied header does not imply contiguous or owned buffers. Callers can
  put pixels in hardware memory and reuse/shared masks elsewhere.
- Initializers use 48 bytes, including unused bytes and final alignment;
  never replace the signed kind byte with an int-sized enum.
- The unused region's former purpose and the reason for the -1/1 provenance
  distinction are not known. Runtime evidence proves the value assignments
  and dispatch behavior, not a recovered original field name or more behavior.

## Other misleading identifiers/comments (not renamed)

- `me_new_bitmap`'s c and `me_wrap_bitmap`/`me_set_bitmap`'s flags are the kind
  value, while the other flags argument controls allocation. `ar_bm`'s flags
  and `Slot.bitmapFlags` likewise describe mask/color allocation, not kind
  (`MEM.C:324-350`; `ARENA.C:85-93`; `DISK.C:324`).
- `di_unpack_pic`'s colors16 and `Slot.pal` are RLE dictionary tokens, not a
  display palette: they are passed to `rl_decode` as its token dictionary
  (`DISK.C:327-341`; `src/SYS/RLE.ASM`), with run counts/values in DISK.C.
- `eb_copy` has dst/src parameter names reversed relative to its copy direction:
  it reads dst->bits and writes src->bits (`EGABLIT.C:42-44,80-89`), matching
  other copy hooks' source-first calling convention.
- `ed_line`'s mono is a dead kind comparison, not a monochrome-mode test
  (`EDIT.C:35`, no later reader); EGA/CGA/Tandy page kinds all satisfy it.
- `ta_copy_page`'s EGA/VGA wording is misleading: it copies TANDY_PAGE_BYTES
  (`TANDY.C:144-149`).

## Changes and validation

Completed member replacements: planesize -> pixelBytes, alt -> mask,
flags -> kind, only for Bitmap accesses; keep unrelated locals/parameters and
other structs unchanged. Update descriptor operand comments to the same member
names. Preserve all member types, order, widths and signedness. The replacement inventory and validation results follow.


### Resource evidence

`./sr pic --list` was read as part of this investigation. For example, EGA
picture 1 is 320x200 with allocation flag 0, picture 2 is 24x64 with flag 1.
Their runtime kind is nevertheless always BITMAP_PICTURE when a header is
created by the unpacker. Mask-only entries are decoded through the mask pointer;
none of these library flags is stored as the Bitmap kind. The directory's
unused size word is not the runtime pixelBytes member.

### Replacement inventory

Every member access was changed, including field copies and segment/offset
views. The following counts are occurrences of the original member tokens in
C accesses (not local variables or unrelated identifiers):

| Source | planesize -> pixelBytes | alt -> mask | flags -> kind |
| --- | ---: | ---: | ---: |
| `src/CGA/CGA.C` | 2 | 0 | 0 |
| `src/CGA/CGABLT.C` | 0 | 6 | 5 |
| `src/CGA/CGASHIFT.C` | 1 | 0 | 0 |
| `src/EGA/EGA.C` | 0 | 0 | 1 |
| `src/EGA/EGABLIT.C` | 0 | 6 | 9 |
| `src/EGA/EGARECOL.C` | 0 | 0 | 1 |
| `src/EGA/EGASHIFT.C` | 1 | 0 | 0 |
| `src/EGA/EGASLIDE.C` | 1 | 2 | 0 |
| `src/GAME/CARPIC.C` | 7 | 6 | 0 |
| `src/GAME/GARGFX.C` | 0 | 3 | 0 |
| `src/GAME/SHRINK.C` | 4 | 7 | 0 |
| `src/GAME/TIRES.C` | 2 | 6 | 0 |
| `src/SYS/DISK.C` | 4 | 7 | 0 |
| `src/SYS/MAIN.C` | 5 | 0 | 7 |
| `src/SYS/MEM.C` | 3 | 5 | 3 |
| `src/TANDY/TANDY.C` | 2 | 0 | 0 |
| `src/TANDY/TANDYBLT.C` | 3 | 6 | 5 |
| `src/UI/CURSOR.C` | 1 | 2 | 0 |
| `src/UI/EDIT.C` | 0 | 0 | 1 |
| `src/UI/POPUP.C` | 0 | 1 | 0 |
| `src/VIDEO/BITMAP.C` | 0 | 17 | 0 |
| `src/VIDEO/VIDEO.C` | 0 | 0 | 2 |
| `src/VIDEO/VIDEO.H` | 0 | 0 | 1 |
| Total (including the access in the existing VIDEO.H comment) | 36 | 74 | 35 |

The BITMAP.H declarations and three DATA assembler descriptor comments were
updated too. No obsolete member accesses remain in src. The single local
`planesize` and the `alt`/`flags` parameters of memory functions are unchanged.

The raw kind arguments replaced are: `ar_bm`, `di_unpack_pic`, `main`,
`cu_init` (both constructor calls), `pu_grab`, `bm_copy`, `bm_shift_right`,
`grab_strip`, and the non-EGA `cp_show_car` backdrop setup. Kind comparison
boundaries are replaced in `cb_masked`, `cb_copy`, `tb_masked`, `tb_copy`,
`eg_draw_text`, `er_rect`, and `ed_line`. Existing VID_* C uses keep their names
and values, now from BITMAP.H's enum; the MASM EQU definitions stay in MAIN.INC.
`me_new_bitmap`'s c and both other constructors' flags parameters are now enum
BitmapKind; the byte member retains its original signed-char declaration.
`Video.mode` stays int because it belongs to the separate Video investigation.

Include blocks and MAKEFILE dependencies were regenerated with `sr headers`:
CRUISE.C and SYS.C now directly include BITMAP.H for the existing VID_* names,
and EGABLIT.C no longer needs MAIN.H just for those constants.

### Gates

- `./sr headers --check`: pass.
- `./sr format --check`: 157 files checked, pass.
- `./sr docs`: zero diagnostics.
- Real MSC 5.1 audit: 18/18 size/offset/kind checks passed. Bitmap is 48 bytes;
  FarPtr is 4 bytes; BitmapKind is 2 bytes; the kind member is still 1 byte at
  offset 46. Member offsets remain 0,2,4,6,36,40,44,46.
- `./sr check --all`: 572/572 match (551 MATCH, 21 MATCH*).
- `rm -rf work/layout && ./sr build`: 96/96 objects gated; all 572 functions
  exact and at original positions; all 748 near calls exact; packed EXE
  IDENTICAL to orig/SR.EXE. The additional unpacked-file diagnostic reports
  different MZ header/relocation ordering versus the unpacked reference; the
  packed executable comparison is byte exact.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`: all
  106 tests pass.
- `./dosbox.sh --make`: DOS-native packed executable IDENTICAL, SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334 (same as orig/SR.EXE).
- `./sr show me_new_bitmap`: checked constructor context and arithmetic.

Scratch logs and the standalone compiler audit are in work/investigations;
this committed note contains the evidence and outcomes needed for review.
