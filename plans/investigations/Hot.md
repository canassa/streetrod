# Hot investigation

## Purpose and scope

One record describes a menu header, drawable element, list row, clickable
region, skip box or terminator in the game's mutable hotspot tables.
This note was written before source edits. Evidence pins refer to that revision.
Rename `Hot` to `Hotspot`, `palIndex` to `styleIndex`, `id` to `hitCode`, and
`next` to `drawNext`. Retain the other accurate member names and the existing
`HotKind` enum. No function, global, unrelated type, algorithm or layout changes.

## Storage, construction and persistence

`src/UI/HOTSPOT.C:4-360` supplies fourteen initialized near-data arrays, in
order. They form a contiguous sequence of 365 eighteen-byte records at link
time; the code deliberately indexes beyond the declared first array through
`g_hots`. The fixed layout is nine signed words, with the enum occupying the
first word, word aligned, no padding: offsets 0,2,4,6,8,10,12,14,16.

| Global | Count | Index relative to g_hots |
| --- | ---: | --- |
| g_hots | 79 | 0..78 |
| g_hotPageRows | 31 | 79..109 |
| g_hotBuy | 6 | 110..115 |
| g_gar | 17 | 116..132 |
| g_hotCalendar | 67 | 133..199 |
| g_transBolts | 4 | 200..203 |
| g_engineHots | 14 | 204..217 |
| g_engineHotTemplate | 9 | 218..226 |
| g_hotGasStation | 5 | 227..231 |
| g_hotGasExit | 7 | 232..238 |
| g_hotRepair | 39 | 239..277 |
| g_hotReadyBox | 10 | 278..287 |
| g_bodyAsk | 35 | 288..322 |
| g_hotBobs | 42 | 323..364 |

The two pointer-subtraction pairs in `GARAGE.C:185-199` and
`RACE.C:80-94` establish separate globals and runtime indexing across their
boundaries. Preserve these declarations, rather than inventing one aggregate.
`MESSAGES.ASM:179-184` initializes `g_groupHot`, mapping screen IDs to menu
header indices. `mn_push` and `mn_bbox` read that mapping (`MENU.C:139,195`).
There is no disk loader for these records: compiled initializers supply them.
Save/load serializes driver state, cars/parts, race records, clock and road
selection (`SAVEGAME.C:337-342,464-469`), not hotspot tables. UI state is rebuilt
by its scene setup routines. Text offsets refer to the separately stored,
mutable `g_txt` block and can point to writable prompt buffers.

`GARGFX.C:38` also owns a static bolt-region template. `gx_build_hots`
(`620-667`) copies the first three fixed engine actions, inserts up to nine
bolt regions plus a wires region, then writes a terminator. Its capacity is
thirteen live entries plus the final sentinel. Stack copies in
`BOBS.C:640-648,682-684` and `GARGFX.C:678-687` preserve whole records while
sorting or rearranging them; they do not allocate or persist hotspots.

## Members, values and evidence

| Offset | Original declaration | Meaning, readers and writers |
| --- | --- | --- |
| 0 | enum HotKind kind | Rendering and traversal discriminator, already correctly enum typed. All initializer tags and dynamic stores use existing HOT constants. `MENU.C:51-117` dispatches drawing. `140,259-263` use the existing HOT_LAST_NONCLICKABLE boundary; HOT_DISABLED is above that boundary but skipped. ADS.C:116,146 changes newspaper rows to regions or disabled entries. GAME.C:473,477 and RACE.C:858,863 disable/enable purchase and repair buttons. GARAGE.C:208,224-248,810,860 enables/hides scene and transmission regions; GARGFX.C:664 terminates the engine list. BOBS.C:654-678,1171-1174 enables/hides Bob's regions. Values are the already named HOT_UNSET through HOT_DISABLED; no new enum is needed. |
| 2 | int palIndex | Index of a complete five-byte Colors style, not a hardware palette colour. MENU.C:54-56,73-74,81-82,86-87,95-96 copies `bustyle->pal[index]` into `x_colors`; EDIT.C:161,195 and MENU.C:546 select the header's style. Stored initializer indices are 0,1,3,4,5,6,7,8,10,12,13,14,15; no direct runtime writer was found. Zero suppresses box drawing (MENU.C:54) and otherwise preserves the current style on the first record because the cached index starts at zero. Rename styleIndex. These are bank positions, not independently proven semantic categories: retain int rather than inventing a second palette/colour enum or changing the separately investigated Style record. |
| 4 | int textOffset | Byte offset from g_txt, used for rendered captions/text (MENU.C:60-62,77-100), editable list-header buffers (422,506-514), and status text on hover (SYS.C:521). Menu headers also supply g_textOfsMenu (139). Zero means no box title but is still a valid offset for other kinds. Dynamic writers: SYS.C:98,109 changes music hover text; GARAGE.C:510 chooses a body-modification prompt; GARGFX.C:635,656 selects bolt/wires captions; BOBS.C:1168,1170 changes the exit caption. Values are offsets, including runtime pointer differences, rather than a closed enum. Keep the name/type. |
| 6 | int id | For ordinary clickable records, the signed result returned by mn_hit (MENU.C:293); list rows use one-based visible row numbers (MENU.C:469-503), while scene records use context-specific positive and negative decoded input codes. ADS.C:118 writes an advert-list index, or zero for an unselectable heading, so InputResult alone is not its type. GARGFX.C:644,657 writes bolt indices and the wires action. GARAGE.C:188 and RACE.C:83 locate regions by this value after sorting, BOBS.C:645 restores ordering by action, SYS.C:812-814 matches demo actions. For HOT_LINK_BOX only, MENU.C:272-273 interprets the same word as a g_hots skip index, not an action; text and box records also contain unused identifiers such as 51..54. Rename hitCode, document the skip exception, retain signed int. |
| 8 | int x1 | Left screen-pixel coordinate, inclusive for hits. MENU.C:264 early-exits because clickable lists are sorted by x1; 288 caches it on a hit. Drawing and editor placement use it (57-100, EDIT.C:163,196). Writers: ADS changes only vertical row bounds; GARAGE.C:220-246 places body features; RACE.C:122 places gas-station regions; BOBS.C:650,663,674 places rival regions; GARGFX.C:645,659 places bolts/wires. Keep name/type. |
| 10 | int y1 | Top screen-pixel coordinate for drawing. Hit testing accepts from y1-3 (MENU.C:277), although the cached hit bounds use y1 (290). Writers are the corresponding scene setup routines, ADS.C:115, GARGFX.C:646,660 and SCREEN.C:171,183 (temporary prompt translation). Keep name/type; do not silently normalize coordinates. |
| 12 | int x2 | Right screen-pixel limit. Rendering uses x2-x1 for width (MENU.C:57,75,97); hit testing rejects x>=x2 (268), so the stored limit is exclusive there even when a producer computes x+w-1 (RACE.C:122, BOBS.C:661). Same setup writers as x1; GARGFX.C:645,659 derives bolt/wires widths. Some header bounds are stale/unusual; drawing bounds come from the linked elements. Keep name/type. |
| 14 | int y2 | Bottom screen-pixel limit, exclusive for hits (MENU.C:280); rendering uses y2-y1 for box/button height (57,75). ADS.C:133,141 adjusts variable-height advert rows; SCREEN.C:172,184 translates prompts; other setup writers correspond to y1. Initial row limits can exceed the display (HOTSPOT.C:83-109 has rows through 250); scene code replaces active advert bounds. Keep name/type. |
| 16 | int next | Absolute record index relative to g_hots for the drawing chain, zero ending the chain. It is not the next hit-test record: mn_draw_hot follows it only when g_hotChain is -1 (MENU.C:119,145-150); mn_bbox follows it unconditionally (198-199). mn_hit instead advances physically or uses the HOT_LINK_BOX skip index. All values come from static initializers; runtime record copies/sorts can move them, but no direct writer exists. Rename drawNext. |

No member is boolean, unsigned, a pointer, or a bit-flag set. Keep every word's
size, signedness and order. `kind` already names its closed set; text offsets,
context-dependent hit/skip codes, chain indices and coordinates are numeric
values, not candidates for speculative categorical enums. No raw kind values
need replacing, and no new constant or enum is added. Searches covered headers
and assembler includes for existing hotspot/style/chain constants.

## Traversal and caller contexts

1. `mn_push` selects a header, takes the physically following record as the
   first clickable entry only when its kind exceeds the nonclickable boundary,
   resets list/header tracking, and draws the header's drawNext chain. The
   header is not itself drawn. HOT_LIST_NO_FRAME suppresses the selection frame.
2. Rendering boxes with a positive style fills/bevels them and optionally
   draws a centered title. Buttons draw centered labels; plain/centered text
   and editable/centered headers have their separate paths. List-row records
   establish row count and the base pointer rather than drawing row text.
3. `mn_list` renders actual list contents later. It casts eighteen-byte
   Hot records to eighteen-byte Win records (`MENU.C:433-459,478-494,519-520`):
   Win.lines reads palIndex, Win.w/h read x1/y1, and Win.tx/ty read x2/y2.
   SAVEGAME.C:436-445 uses that same view for its existing-save highlight.
   Preserve this layout alias; changing Win is outside scope.
4. Hit testing walks physical records until kind<=HOT_LAST_NONCLICKABLE,
   skips HOT_DISABLED, and can skip a whole linked box through its id. Inside
   a link box it advances into the children, never returns the link's id.
   On a hit it writes the hovered pointer/code and cached rectangle; on a miss
   it clears the hovered pointer/code and derives a free rectangle.
5. `gx_sort_hots` sorts by x1 (`GARGFX.C:672-689`); insert_hot orders by x1
   then y1 (`594-613`). GARAGE/RACE find records by action after sorting.
   Bob's scene selectively swaps records to maintain this ordering.

These record consumers feed menu drawing, hover/status feedback, list selection,
numeric/text editing, automatic demo cursor placement and screen-area grabs.
They do not own the records or free them.

## Edge cases and unresolved intent

- Zero hit codes occur intentionally (unselectable advert headings). mn_hit
  can set the hovered pointer yet return zero; its caller treats zero as no
  action. Negative hit codes are ordinary input results, not terminators.
- HOT_UNSET records and any kind<=HOT_LAST_NONCLICKABLE stop physical hit
  scanning. HOT_DISABLED continues it. Neither case means free storage.
- Zero drawNext ends drawing; HOT_END is a separate kind discriminator.
  Chains have no cycle/range checks. No validation or behavior change is added.
- HOT_MENU and HOT_LIST_MENU are treated identically by all code readers;
  an intended additional distinction is unproven. Keep their existing values.
- The right/bottom exclusivity, the three-row upper hit extension, and some
  producers' inclusive-looking arithmetic are actual behavior, not a uniform
  rectangle convention that can be repaired within a naming investigation.
- `Style.pal` is declared seven entries but hotspot code indexes banks through
  15, across subsequent Colors members and unused bytes (`VIDEO.H:45-55`,
  VIDEO.C:34-65). This limits semantic names for the banks; do not redesign it
  or assign imagined category names in this branch.
- Historical spellings and intent of unused text/box IDs are unknown. The
  active member roles and fixed layout are otherwise established by users.

## Other misleading names outside scope

- `Win.w/h/tx/ty/lines` describe popup dimensions/offsets/line count in
  POPUP.C:107-115, but in the list alias they mean left/top/right/bottom/style
  index (`MENU.C:433-459`, `SAVEGAME.C:436-445`). A single prose meaning for
  Win without distinguishing those views would be wrong.
- `Style.pal[7]` suggests seven banks or hardware palette entries, whereas
  indices through 15 select complete Colors records across the rest of Style
  (`VIDEO.H:45-55`, `HOTSPOT.C` initializers, `MENU.C:54-96`). `Colors.frame`
  is used as a condition for beveling here (`MENU.C:58`); its wider semantics
  belong to the Colors investigation.
- `g_hotCalendar`, `g_hotGasExit`, `g_bodyAsk` and `g_hotBobs` are not homogeneous
  calendar/exit/body/Bob's-only arrays: their contiguous storage includes many
  later menus (`HOTSPOT.C:137-203,228-360`). Keep global names/layout intact.
- `set_jail_spots`'s parameter `id` actually holds HotKind, not the record's
  returned ID (`BOBS.C:1162-1174`). `mn_bbox`'s comment says `f10` for the
  drawing-chain member (`MENU.C:188`), an obsolete reconstruction name.

## Validation

Results will be appended after the source changes and required gates.

## Changes and verification results

- Renamed the tag at every declaration, pointer, local, template, array and
  sizeof use in ADS, BOBS, GARAGE, GARGFX, RACE, SYS, HOTSPOT and MENU sources
  and their affected headers. Existing reconstruction references now use the
  current tag and member names. The investigation's historical pins above
  intentionally retain the names being investigated.
- Replaced every actual `palIndex` access in MENU.C and EDIT.C with
  `styleIndex`; every actual hotspot `id` access in ADS.C, BOBS.C, GARAGE.C,
  GARGFX.C, RACE.C, SYS.C and MENU.C with `hitCode`; and every hotspot `next`
  access in MENU.C with `drawNext`. Unrelated fields with the same spelling
  (part/car list links, picture IDs and Win fields) were left intact.
- Documented Hotspot, all nine members, the existing HotKind values and the
  shared table base in HOTSPOT.H. No new closed value set or raw-value
  replacement was required: kind already uses the existing enum throughout.
- The pinned formatter realigned HOTSPOT.C's initializer continuation lines
  after the longer type name. Initializer tokens and every data value are
  unchanged. No global, function, local-variable name or representation changed.
- `./sr headers --check`: clean; `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- Direct MSC 5.1 sizeof/offsetof probe: size 18; member offsets
  0,2,4,6,8,10,12,14,16, exactly the established layout. The probe compiles
  HOTSPOT.H itself with `/AM /Oas /Gs /FPi` and reads its ten-word result
  from the OBJ's data segment; it is not a modern-host sizeof assumption.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- Clean `rm -rf work/layout && ./sr build`: 96/96 objects pass;
  Packed EXE vs orig/SR.EXE: IDENTICAL. All 572 mapped functions are raw-byte
  exact at the original positions and all 748 near-call encodings are exact.
  The auxiliary comparison with the separately unpacked original reports
  different MZ relocation ordering (2218 entries on each side); the packed
  retail executable comparison is byte-identical.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests, OK.
- `./dosbox.sh --make`: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL;
  both SHA-1 hashes are `3a5e28b795313028b7e77d147122f41cb29de334`.

The final post-build source edits only shortened documentation and added
explicit links to the documented table base; headers, formatting and docs
were checked again afterward. No required gate is blocked.
