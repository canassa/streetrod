# Menu, screen, bitmap, sprite and video types

Investigation started 2026-11-05 on branch `st-video`. Each type below was
traced in the requested order before changing names or declarations. Evidence
pins refer to the pre-edit source; function names remain searchable afterward.
Only the eighteen requested records, their member uses and associated value
constants are in scope. No resource or save-file layout is changed.

## 1. ListDialog (MENU.H)

Purpose: retain the current scrolling list's row geometry, contents and choice.
Storage: communal `g_menuList` in UI/MENU.C:28, transient and not saved.
`mn_push` resets the row/header discovery fields (MENU.C:138-149);
`mn_draw_hot` discovers them while traversing menu hotspots (103-109).

| Member | Role, values and evidence |
|---|---|
| lineBase | Near address of the first contiguous row Hotspot, stored as an int, not an index or Y coordinate. MENU.C:108 writes `(int)p`; 435-444 cast it back; 459,480,490,521,529,574 add 18-byte row strides. SAVEGAME.C:437-442 uses the same address for save-row editing. Rename to rowBaseAddress; retain int size and signedness. |
| rows | Number of visible row hotspots discovered by drawing, not item count. MENU.C:107 and 418; page/scroll arithmetic at 421,558-576. |
| list | Borrowed counted list: element zero is item count; subsequent words are description offsets, or negative external-string selectors. MENU.C:408-420,451. No other reader currently uses this stored pointer. |
| selection | One-based selected item across the whole list, not visible row. MENU.C:420,505,585-587. |
| header | Borrowed editable/centered header hotspot, or zero. MENU.C:103-104,423-433,545-551; EDIT.C:161-166,195-196. |

The list and header storage belong to the caller/menu tables. Counts and item
indices are numeric quantities, not enums. Empty lists return before setup.

## 2. Lims (MENU.H)

Purpose: cache the cursor region in which the current hover result remains valid.
Storage: communal `g_boxSpotHitData` (SYS.C:48), reset by menu lifecycle and
`sy_hover_reset`; never serialized. Rename the vague tag to `HoverBounds`.
Members are ordered x1,x2,y1,y2, unlike Rect.

| Member | Role, values and evidence |
|---|---|
| x1 | Included left screen-pixel limit. MENU.C:268-270,288; SYS.C:484,511. |
| x2 | Excluded right limit for the cache test. MENU.C:257,265,289; SYS.C:484,511. |
| y1 | Included top row. MENU.C:280-282,290; SYS.C:484. |
| y2 | Excluded bottom row. MENU.C:278-279,291; SYS.C:485,512. |

On a hit these are hotspot bounds; on a miss they bound nearby free space.
The initial miss range uses right=319/bottom=199 despite exclusive readers;
no-menu uses -1/1000 (MENU.C:249-258). Resets use -1 or zero (138,174,185).
Do not silently normalize those edge cases or swap the member order.

## 3. DriverState (SCREEN.H)

Purpose: player cash, owned inventory, allocation lists and menu/race selection.
Storage: communal `g_driver` (SCREEN.C:40); POOLS.C:8-35 rebuilds pool links.
SAVEGAME.C:337-342,464-469 writes a 36-byte region starting at the separate
16-byte `g_driverName`, then serializes current/other cars and spare parts
explicitly. The block includes the first twenty bytes of DriverState (money through
partCount), including runtime pointers. `pl_init(0)` then rebuilds free lists
and clears counts; `read_cars`/`read_nodes` replace loaded list pointers and
recount allocations. The adIndex/raceType tail is not in that saved block.
The original symbol offsets confirm g_driverName immediately precedes g_driver
by sixteen bytes (tools/sr/symbols.json).

| Member | Role, values and evidence |
|---|---|
| money | Signed long cash in dollars. SAVEGAME.C:125-127 initializes; SCREEN.C:83 changes/displays; GARAGE.C:428 and RACE.C:438-441 spend/clamp. |
| unused | Written 10 by POOLS.C:34 and SAVEGAME.C:128, no semantic reader. Preserve name and word. |
| currentCar | Selected owned car or zero; GAME.C:383-387,613-639 selects/relinks; SAVEGAME.C:338/465 reconstructs/serializes. |
| otherCars | Head of the other owned cars; GAME.C:384-385,617,632-637; GARAGE.C:315-317 promotes first when needed. |
| freeCars | Free Node2 pool head, POOLS.C:14-20; GAME.C:282-295 pops/pushes. |
| carCount | Allocated car-node count, including current car; POOLS.C:28, GAME.C:285,295; checked against MAX_OWNED_CARS. |
| spareParts | Uninstalled part-list head; GAME.C:208,266,309-310 and GARAGE.C:639-686 manipulate it; SAVEGAME.C:339/466 loads/saves. |
| freeParts | Free Node pool head; POOLS.C:21-27; GAME.C:158-171 pops/pushes. |
| partCount | Allocated part-node count, including fitted parts, not spare-list length; GAME.C:161,171,383-402; checked against MAX_OWNED_PARTS. |
| adIndex | Newspaper advertisement resume index; ADS.C:103-107,164-177. |
| raceType | Existing RaceType enum; BOBS.C:528-568 selects drag/road; RACE.C:686,935 chooses fuel/course. |

No new inventory enums: sizes, counts and indices remain arithmetic quantities.
The unused word's original purpose is unknown.

## 4. TextEnd (SCREEN.H)

Purpose: return the pen position from the last mode-specific text renderer.
Storage: communal `g_penPoint` (SCREEN.C:41), transient, no loader/save path.

| Member | Role, values and evidence |
|---|---|
| x | Pixel column immediately after rendered text; EGA.C:104, CGA.C:122, TANDY.C:45. SCREEN.C:84,98,246 and MENU.C:454 use it for value placement/clearing/copy width; ADS.C:123 appends text. |
| y | Original text's top row, not a baseline or bottom; EGA.C:105, CGA.C:123, TANDY.C:46. EDIT.C:46,147 and MENU.C:454 reuse it. |

## 5. StatusText (SCREEN.H)

Purpose: retain status-strip text placement and whether the next draw must erase.
Storage: communal `g_statusLine` (SCREEN.C:42); sc_status_init initializes the
message fields at SCREEN.C:28-37. Never saved.

| Member | Role, values and evidence |
|---|---|
| messageX | Default centered-message area left edge, initialized 88; SCREEN.C:32,58,96,105; RACE.C:761,769,777 sizes offers. |
| y | Text top pixel row, status-strip top + 1; SCREEN.C:30,63,96-105. |
| messageWidth | Default centering width, initialized 160; SCREEN.C:31,55-59. Does not limit clearing, which extends to screen right. |
| shown | Erase-before-next-write flag, not a character count. SCREEN.C:65-68 tests it, then stores the first string byte (possibly any nonzero value); 81-85,113-116 temporarily force it; SYS.C:405-407,517-519 and BOBS.C:1061 force clearing. Rename to needsErase and use BOOL without normalizing the first-byte assignment. |
| valueX | Aligned money/gear column after label; SCREEN.C:98 sets it from pen X, 83/115 draws there. |

Keep all truth tests and the first-character store; TRUE means any nonzero.

## 6. PicturePlacement (SCREEN.H)

Purpose: hold the destination and height cap for the next loaded scene picture.
Storage: initialized `g_picturePlacement={0,0,SCREEN_BODY_HEIGHT}`
(SCREEN.C:144), transient.

| Member | Role, values and evidence |
|---|---|
| x | Destination left pixel column; SCREEN.C:221,226,235; reset to zero after drawing at 228. |
| y | Destination top row; SCREEN.C:222,226-228,236. |
| maxHeight | Picture height cap and optional clear height, not an absolute bottom coordinate; SCREEN.C:217-219. BOBS.C:848-852, DASH.C:359-383 and FAME.C:194-200 temporarily allow SCREEN_HEIGHT. |

The cap is not reduced by y; document the implemented cap, not inferred clipping.

## 7. FarPtr (BITMAP.H)

Purpose: view the same real-mode far data pointer as a pointer or its two words.
Storage: embedded in Bitmap.bits/mask and SLOTS.H's cached-picture pointer;
EGASLIDE.C and TANDYBLT.C also have automatic copies. Not an on-disk pointer.

| Member | Role, values and evidence |
|---|---|
| p | Dereferenceable segment:offset char far pointer; BITMAP.C:37-44,96-113 and DISK.C's slot loader. |
| w | Overlay of the same four bytes, not another pointer allocation. |
| w.off | Low offset word, allowed to advance without segment normalization; CARPIC.C:198, EGASLIDE.C:86, EGABLIT.C:144-149. |
| w.seg | High segment word; EGABLIT.C:143, DRAW.C:23,44, DISK.C:260. VIDEO.C:155-160 builds EGA page addresses. |

Both words remain unsigned. Union size is four bytes; order is offset, segment.

## 8. Rect (BITMAP.H)

Purpose: hold inclusive pixel bounds for sprite clipping, erasure and overlays.
Storage: automatic rectangles; Sprite/Page/cursor/tire records; CARPIC.C's
static overlays and last-car rectangle; CARHOP.C's saved rectangle. Transient.

| Member | Role, values and evidence |
|---|---|
| x1 | Included left column; SPRITE.C:22,44,77; driver restore calls. |
| y1 | Included top row; SPRITE.C:26,46,79. |
| x2 | Included right column; SPRITE.C:41,45,78; CGASPR.C:20 computes width with +1. |
| y2 | Included bottom row; SPRITE.C:43,47,80; EGASPR.C:83-84 computes height with +1. |

SPRITE.C:162-165 marks empty subtraction with minima 1000 and maxima -1;
sp_setup uses maxima -1 for undrawn pages (115-124). CURSOR.H also uses
negative x2 for no drawn cursor. sp_rect_and rounds horizontal bounds using
the video byte mask (28-30); it is not ordinary half-open rectangle math.
CARPIC.C:239-243, CGASPR.C:88-105, EGASPR.C:74-94 and TANDYSPR.C:100-119
show overlay bounds may be relative to one moving axis until translated.

## 9. Blt16 (BITMAP.H)

Purpose: sixteen-byte copy packet used for sprite composition and shifter drawing.
Storage: locals in EGASPR.C:31 and DASH.C:632; sp_clip_span also receives a
cast view of a full Blt in CGASPR.C:73/TANDYSPR.C:84. Never saved/loaded.

| Member | Role, values and evidence |
|---|---|
| w | Width in pixels on entry to drivers; SPRITE.C:338, EGASPR.C:61,83, DASH.C:649,657. |
| h | Height in rows; EGASPR.C:57,84, DASH.C:641. |
| sx | Source left pixel offset; SPRITE.C:336, EGASPR.C:62,86,89, DASH.C:643,652. |
| sy | Source top row; EGASPR.C:58,87,90, DASH.C:644,653. |
| dx | Destination pixel column; SPRITE.C:337, EGASPR.C:63,92, DASH.C:646,655. |
| dy | Destination top row; EGASPR.C:59,93, DASH.C:647-648,656. |
| unused1, unused2 | Spare words; no Blt16 access. Preserve both names/layout. |

Only first six words have consumers. da_clip_blt converts a separate output
packet's X/width to byte units; it does not reinterpret this input as bytes.
sp_clip_span changes only sx,dx,w; caller must provide the vertical fields.

## 10. Seq (SPRITE.H)

Purpose: one mutable movement phase, with per-update displacement and countdown.
Storage: communal g_seqSpriteMoveData[10] (SPRITE.C:52), pointed to by a Sprite;
sp_set_step builds it (203-210); motion planners fill successive phases.

| Member | Role, values and evidence |
|---|---|
| count | Remaining countdown; -1 terminates, decremented before testing/advancing (SPRITE.C:309-312). Drivers reject negative counts before rendering. A count of one advances to the next phase without moving by the old phase's dx/dy. |
| dx | Signed horizontal pixels per advance; SPRITE.C:206,313,315-319. Magnitudes 2/4/8 select ring links via >>2; sign controls movement direction. |
| dy | Signed rows per advance; SPRITE.C:207,314; sp_slide_v sets +/-1 at 275-285. |

Keep signed chars and numeric motion/count ranges; name the lone terminator.

## 11. Frame (SPRITE.H)

Purpose: one sprite image plus driver-dependent bottom strip and ring links.
Storage: communal g_frmTable[12] (SPRITE.C:51), rebuilt by sp_setup (92-104),
not loaded from pictures or serialized. Rename misleading member names.

| Member | Role, values and evidence |
|---|---|
| pos | Borrowed far image pointer, not a position; SPRITE.C:93,128; EGASPR.C:52,57-64, CGASPR.C:53-65, TANDYSPR.C:56-75. Rename image. |
| data | Optional bottom-band view: EGA points at an eight-byte Band through a Bitmap pointer; CGA/Tandy points to a near Bitmap header. SPRITE.C:94-97 chooses it; EGASPR.C:64 casts to Mask, CGASPR.C:52,57,66-69 and TANDYSPR.C:55,60,76-79 draw Bitmap bands. Rename bottomBand; preserve the pointer type/size and document the mode discriminator. |
| nxt | Three near ring links for advancing 1,2,4 images; SPRITE.C:98-104 builds, 178-186 reverses/rebuilds, 319 selects by abs(dx)>>2. Rename next. |

No image ownership transfer. A null band means normal full-height composition.

## 12. Box (VIDEO.H)

Purpose: mutable rectangle used by animation-script blits and front-page refresh.
Storage: embedded in each Obj; g_animRun.box borrows it (VIDEO.C:214).
No loader or save path. Keep the tag, whose animation context is now documented.

| Member | Role, values and evidence |
|---|---|
| w,h | Pixel width/row height from ANIM_SET_SIZE (VIDEO.C:262-264), used by tiled clipping and copy at 229-241,274-283. |
| sx,sy | Source bitmap origin in pixels/rows; anim_set_box at 187-192. |
| dx,dy | Destination origin in pixels/rows; anim_set_box at 193-195; page refresh at 274-283. |
| unused | Sixteen reserved bytes after the six-word Blt-compatible prefix; no access. Preserve. |

Not a general four-edge bounding box. ANIM_TILE_CAR deliberately computes
width as x2-x1 rather than +1 (231); no behavioral correction is in scope.

## 13. Obj (VIDEO.H)

Purpose: runtime state of one active bitmap-animation script.
Storage: communal g_scriptObjsData[5] (VIDEO.C:28), initialized by vi_init
(142-146), allocated by vi_anim_start (296-302), not persisted.

| Member | Role, values and evidence |
|---|---|
| id | Script identity (index into g_animScriptStart), not this object's slot; -1 frees the slot. VIDEO.C:145,175,297-300; timers carry it at 269 and hook dispatch uses 2 at 330-331. Rename scriptId. |
| scriptIndex | Resume index into the flat command array; VIDEO.C:213,267-268,300. Rename resumeIndex. It is not continuously updated as every command executes. |
| pendingTimers | Number of outstanding animation timer entries; start zero (301), wait increments (270), resume decrements (315), stop tests to cancel (176-177). Count, not BOOL. |
| unused1 | Unread/unwritten spare word. Preserve. |
| img | Borrowed arena bitmap loaded by ANIM_LOAD_IMAGE (256), read by blits/tiling (222,226,237-241), pointer cleared on stop (170-171); no free. Rename image. |
| unused2 | Otherwise unused far pointer; stop tests/clears at 172-173. Preserve name and type; no invented resource semantics. |
| box | Embedded persistent rectangle state, used through g_animRun.box. |

vi_anim_start's slot index 0..4 differs from the script identity 0..8.
Name the slot capacity and inactive-script sentinel; script identities remain
indices. Current start callers use 2 for the fuel-pump sound/indicator callback
(RACE.C:332,403) and 8 for the timing display (GARAGE.C:959,978). The other
table entries have no current start caller, so do not invent game-level names.

## 14. Colors (VIDEO.H)

Purpose: current text, fill, line and bevel choices for a drawing operation.
Storage: x_colors (VIDEO.C:26), two initialized mode Style tables (34-65),
and temporary saved copies throughout UI. No asset/save loader. Five chars;
standalone size 5, automatic stack reservation may round to 6.

| Member | Role, values and evidence |
|---|---|
| text | Foreground palette index, EGA.C text expansion, CGA.C:draw_text and TANDY.C:39-44. |
| textBg | Glyph background palette index, same renderers. |
| fill | Rectangle background palette index, DRAW.C:25-28, MENU.C:57,75,97. |
| line | Line/frame highlight palette index, DRAW.C:46,57-59,94-102,121-124. |
| frame | Bevel selector, not a color: 0 suppresses menu bevel, 1 draws one highlight, 2 doubles it, 3 fills the border with line color. MENU.C:58-59; DRAW.C:69,85-103. Rename bevelStyle and name every value with BevelStyle constants; retain char. |

Palette indices are numeric register indices, not fixed RGB colors: EGA/Tandy
use 0..15, CGA 0..3; palettes are mutable (VIDEO.C:71-100,355-398). Do not
invent color names that would become wrong after palette remapping.

## 15. AnimRun (VIDEO.H)

Purpose: transient interpreter cursor for the currently running animation object.
Storage: private static g_animRun (VIDEO.C:21), overwritten on each anim_run;
not a queue and not saved. Six bytes of near pointers.

| Member | Role, values and evidence |
|---|---|
| command | Borrowed current command; VIDEO.C:213,253,257,286 advances/jumps. |
| object | Borrowed active slot; VIDEO.C:212,222-226,249,256,267-270. |
| box | Borrowed rectangle inside that same object, assigned 214 and used by anim_set_box. |

A wait stores the resume index in Obj and returns; the static cursor itself
is not resumed or retained per object. Interpreter is not reentrant.

## 16. Hooks (VIDEO.H)

Purpose: mode-selected table of fifteen far video-driver entry points.
Storage: communal g_videoOps (DASH.C:49); copied from private initialized
tables in EGA.C:21-28, CGA.C:22-32, TANDY.C:17-26. No on-disk table.
All fifteen pointers remain far and four bytes; callback return declarations
are retained even where implementations are void and consumers ignore return.

| Member | Contract and evidence |
|---|---|
| copy | Source, destination, pixel rectangle, plane/mask flags. eb_copy/cb_copy/tb_copy clip and convert units; BITMAP.C:24, MENU.C's callers. |
| shiftBlit | Source, destination, rectangle; preserves partial pixel-byte edges. EGASHIFT.C:148, CGASHIFT.C:74, TANDYBLT.C:250; GARGFX.C:300,347. |
| maskedBlit | Source, destination, rectangle, compositing selector; combines image/mask and existing destination. EGABLIT.C:174-217; CARPIC.C:175,272-276. |
| drawText | Destination, left/top pixels, near string; updates TextEnd. EGA.C:34-105, CGA.C:37-123, TANDY.C:31-53. |
| cursorBlit | Cursor image/mask/save/target pointers, byte X, pixel Y, byte width, rows; CURSOR.C:156-162 and CURSBLT.ASM. |
| copyPage | Source then destination far display pointers; EGACOPY.C:12-21; CGA.C:380 and TANDY.C:130. |
| recolorRegion | Bitmap, old/new palette indices, rectangle edges; MENU.C:485,492; EGA er_rect, CGA cg_recolor_rect, Tandy ta_recolor_rect. |
| blank | Disable display or blacken its palette; CGA.C:436; DASH.C:305. |
| show | Enable display or restore palette; CGA.C:450; DASH.C:312. |
| spriteStep | Draw/advance Sprite, return nonzero while active; EGASPR.C:26-99, CGASPR.C:30-128, TANDYSPR.C:35-127. |
| reveal | Destination then source display pointers; timed interleaved-row reveal (opposite copyPage argument order). EGACOPY.C:25-80, CGA.C:390, TANDY.C:139. |
| slideSprite | Refresh top/bottom, X, start/end Y, bitmap; EGASLIDE.C:ev_sprite and SPRITE.C:264-298; GARGFX.C:61,103. |
| drawLine | Pixel endpoints then color, targeting shared segment/offset state. DRAW.C:23-28,44-46 and mode line ASM. |
| fillSpan | Left X, right X, Y, color with shared target state; ROAD/VIEW call sites and EGASPAN.ASM/CGA/TANDY equivalents. |
| fillBelow | First row and color, fill through lower clip row using shared target state; EGASPAN.ASM:155-214 and CGALINE/TANDYLIN equivalents. |

Callback names are accurate. Do not turn ignored callback returns into a
new success/error API. The Tandy drawText implementation's last parameter is
an int used as a near string address; that misleading parameter is out of scope.

## 17. Video (VIDEO.H)

Purpose: active display pages, linear pixel geometry and automatic-refresh flag.
Storage: communal r_vid (MAIN.C:36), mode initially selected in MAIN.C:184-213,
initialized by vi_init (VIDEO.C:104-161); never saved.

| Member | Role, values and evidence |
|---|---|
| front | Visible-page Bitmap, VIDEO.C:108,150. Hercules changes its kind to VID_CGA at 130 while mode retains VID_HERCULES. |
| back | Current drawing Bitmap, VIDEO.C:109,151; SCREEN.C:178-181 can temporarily redirect it to front. |
| hoodBg | Lazy engine-bay working background; VIDEO.C:110 clears; GARGFX.C:522-525 loads/unpacks PIC_ENGINE_BAY, other accesses restore/patch it. |
| mode | Existing BitmapKind display selector, including VID_HERCULES independently of front.kind. Retype int as enum BitmapKind; MAIN.C and VIDEO.C writers already use its constants. |
| planes | Color-plane count in linear bitmaps: 4 EGA, 1 packed CGA/Tandy; VIDEO.C:114,125,133; ARENA.C:90 and MEM.C:321 use it for allocation, BITMAP.C:97 for iteration. |
| shift | Log2 pixels per stored byte in one plane: 3/2/1; VIDEO.C:115,126,134, ARENA.C:83 and MEM.C:319 compute pitch. |
| bpp | Bits per pixel per stored plane (1 EGA, 2 CGA, 4 Tandy), not total EGA color depth. VIDEO.C:116,127,135, BITMAP.C:49,54,78. Rename planeBitsPerPixel. |
| mask | Signed X byte-alignment mask (-8/-4/-2); VIDEO.C:117,128,136, SPRITE.C:28-30,68,76 and BITMAP.C:93; complement supplies within-byte phase. Rename xByteMask. |
| copyToFront | BOOL controlling automatic rectangle refresh, not whether back exists. VIDEO.C:111,273-283; UI/GAME writers temporarily suppress it during batching. Replace literal assignments with TRUE/FALSE everywhere, preserving chained assignments and expressions. |

Plane count, shifts, bit widths and alignment masks are related physical
quantities, not categorical enum codes. Preserve arithmetic signedness and order.

## 18. Mask (EGABLIT.H)

Purpose: EGA driver's eight-byte view of a precomposed bottom sprite band.
Storage: no standalone Mask objects. Frame.data is cast at EGASPR.C:64,68;
sp_setup borrows `char [8]` views of CARPIC.C's g_carSpriteStrips (567-568),
whose separate Band type is populated by make_bands (117-141). Never serialized.
Rename the misleading tag to EgaSpriteBand; keep Band outside this task.

| Member | Role, values and evidence |
|---|---|
| data | Far pointer to packed-width rows in planar VRAM, not transparency bits. CARPIC.C:123-140 precomposes through latch writes; EGABLIT.C:122 offsets horizontally,168 copies all planes with ea_to_page. |
| y | Band's source top row, matching Band.y written by CARPIC.C:126/139; no reader through this view. Preserve word, document that it is ignored here. |
| h | Bottom-band row count; CARPIC.C:127, EGABLIT.C:121,136,166-168 subtracts it from normal composition height then latch-copies it. |

The consumer computes the destination band position from clipped height minus
h, not from y, and doesn't vertically adjust the band for clipping. Preserve.

## Scope and unresolved names

- Band (CARPIC.H) and Mask are layout-compatible views, but the latter name
  misleadingly suggests a transparency mask. Only the requested view is renamed.
- `g_boxSpotHitData` is also a miss-region hover cache, not just a hit box
  (MENU.C:249-299). Global rename is outside scope.
- `g_frmTable`, `g_seqSpriteMoveData`, `g_bandRecordsSprArray` and
  `g_carSpriteStrips` are runtime storage, not loaded frame/motion resources.
- `ta_draw_text`'s `glyph` parameter is actually a near string passed to
  tt_render; TANDY.C:31-44, TANDYTXT.ASM. Outside requested structs.
- Existing EGABLIT.C comments say VGA and label eb_copy's source/destination
  backwards (arguments are source first in actual call sites). Keep function
  and parameter renames outside scope; callback documentation follows data flow.
- `PIC_FUEL_PUMP_DIGITS` (DISK.H, resource 1075) is a misleading image name:
  script 8 uses it in GARAGE.C:959-978 for engine timing. `./sr pic 1075`
  decodes a 56x94 tuning strip with a vertical scale and arrow frames, not
  numerical pump digits. Reported here; the resource enum is outside scope.
- `sp_reverse`'s comment says heading, but it reverses frame order and selects
  pixel phase from g_xCoordSprite, not a heading (SPRITE.C:172-198).
- Obj.unused2 and DriverState.unused have insufficient evidence for semantic
  names. Keep them, and preserve all other unused members.

## Planned changes and replacement inventory

Member/tag renames: ListDialog.lineBase -> rowBaseAddress; Lims -> HoverBounds;
StatusText.shown -> needsErase; Frame.pos/data/nxt -> image/bottomBand/next;
Obj.id/scriptIndex/img -> scriptId/resumeIndex/image; Colors.frame -> bevelStyle;
Video.bpp/mask -> planeBitsPerPixel/xByteMask; Mask -> EgaSpriteBand.
All readers/writers of these members change together; no locals/functions/globals
are renamed. New constants: BevelStyle for every style-table initializer and
DRAW.C comparison; sprite motion terminator for every Seq count sentinel;
animation slot capacity/inactive identity for VIDEO.C allocation/reset logic.
StatusText.needsErase and Video.copyToFront use BOOL; Video.mode reuses
BitmapKind. Geometry remains numerical and color channels stay char palette
indices. All spare fields and declaration orders stay intact.

## Validation

Before merging main:

- Headers and formatting: clean; Doxygen: 0 diagnostics.
- Function gate: 572/572 match (551 MATCH, 21 MATCH*).
- Clean packed build: 96/96 objects pass, 572/572 functions at original
  positions and raw-byte exact, 748/748 near calls exact; packed executable
  IDENTICAL to orig/SR.EXE. The separate unpacked-file diagnostic reports the
  pre-existing header/relocation-order/trailing-image differences; the packed
  identity is the required executable gate.
- DGROUP ownership and object order: pass.
- Unit tests: 109 pass.
- The generic check_data_types.py audit hits the already documented
  TRANSMISSION_BOLT_COUNT macro-parser limitation in the unrelated Car type.
  A targeted real MSC 5.1 compile checks all eighteen requested types:
  117 sizeof/offsetof expressions, zero mismatches. Sizes, in investigation
  order: 10,8,24,4,10,6,4,8,16,4,12,28,44,5,6,60,24,8 bytes.
  The shape probe and JSON result are in ignored work/st-video-audit.

Post-merge gates and native DOS build results will be appended below.

An independent comparison with the pre-edit headers also confirms unchanged
size, alignment, member order, every member offset/size and pointer distance
for all eighteen targets. This includes the unsigned FarPtr words, signed Seq
bytes, char color/style fields and int-sized BOOL/BitmapKind substitutions.

### Final integration and verification

Committed the implementation as 3b0092b. Ran `git fetch origin` and
`git merge origin/main`; main was already included (3848b38), with no conflicts.
Reran every required gate after that merge:

- `./sr headers --check`: up to date.
- `./sr format --check`: 157 files clean.
- `./sr docs`: zero diagnostics. An XML audit confirms all 18 requested types
  and all 101 members (including the FarPtr word overlay) have documentation.
- `./sr check --all`: 572/572 match, including all 21 MATCH* functions.
- Removed only work/layout, then `./sr build`: 96/96 objects pass,
  572/572 mapped functions exact and in place, 748/748 near calls exact,
  packed executable IDENTICAL.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  109 tests pass.
- Repeated the targeted MSC sizeof/offsetof audit: 117 checks, zero mismatches.
- `./dosbox.sh --make`: DOS-native executable IDENTICAL; SHA-1 matches
  orig/SR.EXE: 3a5e28b795313028b7e77d147122f41cb29de334.

All unused fields, signedness, field ordering, storage extents and pointer
qualifiers are preserved. No unproved behavior fixes were made. The unrelated
whole-tree macro-parser limitation remains documented rather than broadened
into this task.
