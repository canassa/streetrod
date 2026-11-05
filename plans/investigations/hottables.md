# Hotspot table boundaries

## Purpose and decision

Represent every screen's drawing and hit-test records in the one absolute index
space the menu code consumes. `g_hots[HOTS_COUNT]` now covers all 365 18-byte
records at DGROUP 09F4..239E (exclusive end). One complete table with per-screen
`HOTS_*` indexes matches the executable and makes each record's screen explicit.
The source groups initializers by screen; each link uses its target screen base
plus a relative offset. `KM_*` remain the existing screen/keymap IDs.

The investigation draft was written in `work/investigations/hottables.md` before
source edits. The initial single-table probe retained GARAGE.C and RACE.C's
complete function spans, including all four address-difference lookup/sort
routines. The full gates below establish the final representation's bytes.
This does not establish the original declaration spellings or boundaries.

## How the table is consumed

- `MENU.C:139-149` uses `g_groupHot[id]` to select a header. Its immediately
  following record starts hit testing only when its kind exceeds HOT_END.
  Drawing instead begins at the header's drawNext index.
- `MENU.C:119` follows absolute drawNext links; `MENU.C:195-199` follows the
  same links for bounding boxes. Zero ends drawing, including at header zero.
- `MENU.C:249-273` walks contiguous hit records until a nonclickable kind.
  HOT_LINK_BOX.hitCode is an absolute skip index, often the current header
  (ending a scan), another control after child rows, or an explicit HOT_END.
  These fields are indexes, unlike other kinds' decoded input results.
- `SYS.C:812-821` also reads global indexes while looking for a keyboard result.
- `MESSAGES.ASM:g_groupHot` has 42 entries, indexed by ScreenId through
  KM_END_MENU. KM_NONE's value is 41, a spare-parts scroll button, not a header;
  it is retained as HOTS_NONE. KM_MUSIC has a keyboard map but no hotspot slot.
- Int index fields remain int: runtime sorting/list building and the mixed
  meanings of hitCode do not form the header enum's closed set. The group
  table's C declaration now uses enum HotspotIndex (the same two-byte storage).

## Complete screen map

Ranges include the header and any terminator, labels or templates up to the
next header. The index is not the screen ID: IDs 38/39/40/41 are particularly
out of storage order. Bounds are inclusive here.

| Screen ID | Header index | Records |
|---|---:|---|
| `KM_RACE_OUTCOME` | 0 | 0..1 |
| `KM_START_GAME` | 2 | 2..5 |
| `KM_LOAD` | 6 | 6..25 |
| `KM_SPARE_PARTS` | 26 | 26..45 |
| `KM_SAVE` | 46 | 46..66 |
| `KM_SAVE_ERROR` | 67 | 67..71 |
| `KM_NEWSPAPER` | 72 | 72..76 |
| `KM_ADVERTS` | 77 | 77..107 |
| `KM_BUY` | 108 | 108..114 |
| `KM_GARAGE` | 115 | 115..134 |
| `KM_YOUR_CARS` | 135 | 135..155 |
| `KM_SELL_CAR_OFFER` | 156 | 156..162 |
| `KM_COUNTER_OFFER` | 163 | 163..165 |
| `KM_PAINT` | 166 | 166..172 |
| `KM_NO_SAVED_GAMES` | 173 | 173..177 |
| `KM_SELL_PART_OFFER` | 178 | 178..182 |
| `KM_UNUSED_17` | 183 | 183..184 |
| `KM_DRAG_WAGER` | 185 | 185..190 |
| `KM_ROAD_WAGER` | 191 | 191..196 |
| `KM_TRANSMISSION` | 197 | 197..202 |
| `KM_ENGINE` | 203 | 203..220 |
| `KM_RACE_TYPE` | 221 | 221..225 |
| `KM_GAS_STATION` | 226 | 226..233 |
| `KM_MESSAGE` | 234 | 234..236 |
| `KM_REPAIR` | 237 | 237..243 |
| `KM_GET_RUNNING` | 244 | 244..249 |
| `KM_SAVE_RESTART_QUIT` | 250 | 250..257 |
| `KM_TIMING` | 258 | 258..262 |
| `KM_MESSAGE_BOX` | 263 | 263..267 |
| `KM_OVERWRITE_SAVE` | 268 | 268..272 |
| `KM_LOAD_ERROR` | 273 | 273..277 |
| `KM_READY` | 278 | 278..282 |
| `KM_RACE_STATS` | 283 | 283..286 |
| `KM_BODY_CHANGE` | 287 | 287..291 |
| `KM_STICKERS` | 292 | 292..302 |
| `KM_CALL_RIVAL` | 303 | 303..321 |
| `KM_BOBS` | 322 | 322..328 |
| `KM_MANUAL_COLORS` | 329 | 329..341 |
| `KM_UNUSED_40` | 342 | 342..343 |
| `KM_END_MENU` | 344 | 344..348 |
| `KM_CAR_INFO` | 349 | 349..364 |

Screens without an explicit HOT_END stop hit testing at their next drawing
label/header; a terminator is not a universal array boundary. Engine templates
218..220 belong to the engine, not the race-type group beginning at 221.

## Former declarations and why they were misleading

| Original name | Absolute records |
|---|---|
| `g_hots` | 0..78 |
| `g_hotPageRows` | 79..109 |
| `g_hotBuy` | 110..115 |
| `g_gar` | 116..132 |
| `g_hotCalendar` | 133..199 |
| `g_transBolts` | 200..203 |
| `g_engineHots` | 204..217 |
| `g_engineHotTemplate` | 218..226 |
| `g_hotGasStation` | 227..231 |
| `g_hotGasExit` | 232..238 |
| `g_hotRepair` | 239..277 |
| `g_hotReadyBox` | 278..287 |
| `g_bodyAsk` | 288..322 |
| `g_hotBobs` | 323..364 |

The task's g_hotTransBolts and g_hotBodyChange are named g_transBolts and
g_bodyAsk in the starting branch. All 14 declarations formed one contiguous
run; none of their names established a screen boundary. For example,
g_hotPageRows ended after the buy header and box; g_hotBuy ended at the garage
header; g_hotCalendar included the calendar, car name and ten later groups;
g_transBolts ended at the engine header; g_engineHotTemplate included the next
race-type group and gas header; g_hotGasExit included message and repair
headers; g_hotReadyBox ended at the body-change header; g_bodyAsk ended at the
Bob's header. No artificial alias declarations remain.

## Mutable regions and replacement inventory

All source locations in this section describe the starting branch.

| Users | Former view | Final view / extent |
|---|---|---|
| `ADS.C:105` | g_hotPageRows | HOTS_ADVERTS + 2; list box 79, 25 mutable rows 80..104 |
| `GAME.C:473,477` | g_hotBuy[0] | HOTS_BUY + 2 (See It button) |
| `GARAGE.C:185-198` | g_gar, g_hotCalendar | HOTS_GARAGE + 1 through +18 (116..133), 18 sortable regions |
| `GARAGE.C:510` | g_bodyAsk[0] | HOTS_BODY_CHANGE + 1, caption box 288 |
| `GARAGE.C:810,860` | g_transBolts[k] | HOTS_TRANSMISSION + 3 + k, bolts 200..201 |
| `GARGFX.C:632-664` | g_engineHots | HOTS_ENGINE + 1, mutable buffer 204..217 |
| `GARGFX.C:632-664` | g_engineHotTemplate | HOTS_ENGINE_TEMPLATE, three controls 218..220 |
| `RACE.C:80-93` | g_hotGasStation, g_hotGasExit | HOTS_GAS_STATION + 1 through +6, six sortable regions 227..232 |
| `RACE.C:858,863` | g_hotRepair[0], [7] | HOTS_REPAIR + 2 and HOTS_GET_RUNNING + 2 (239 and 246) |
| `SCREEN.C:170-184` | g_hotReadyBox[i] | HOTS_READY + i, five records 278..282 |
| `BOBS.C:644-648,1166-1168` | g_hotBobs | HOTS_BOBS + 1, mutable regions 323..327 |

Garage's calendar is inside the sorted region run and can move. The car name
at 134 is drawn independently by header 115 and is outside the sort. Gas's
exit is similarly inside its six-record sort; 233 is its HOT_END. Initializer
positions within these runs are not permanent identities after sorting.

The engine builder copies three controls, inserts up to nine part bolts and
one wire target, and appends HOT_END: 14 mutable records suffice. Its fixed
templates follow that run but are never reached by its hit scan. The template
copy count is HOTS_RACE_TYPE - HOTS_ENGINE_TEMPLATE. The ready translation
count is HOTS_RACE_STATS - HOTS_READY; neither reaches the following screen.

All drawNext literals and HOT_LINK_BOX skip-index literals became screen-based
expressions. The numeric g_groupHot entries became HOTS_* constants from
HOTSPOT.INC. HotspotIndex in HOTSPOT.H and those assembler EQU constants have
agreement tests; tests also verify every slot's matching header, complete
header coverage and links staying inside their screen. Normal hit results,
coordinates, text offsets, styles and kinds are unchanged. No equivalent
hotspot-index constants existed in headers/INC files before this work.

Former partial-array declarations were removed from HOTSPOT.H and their 13
interior entries removed from symbols.json; g_hots retains its original base.
The unused communal definitions and names remain intact. MESSAGES.H includes
the enum's owning header, and sr headers regenerates affected dependencies.
ScreenId members gained brief documentation so explicit Doxygen references to
them resolve. RECONSTRUCTION.md and the recipe book correct the earlier claim
that the runtime differences prove separate symbols.

## Representation and byte evidence

A table per screen would leave runtime g_hots[absoluteIndex] traversing beyond
the initial declared C array, or require a different indexing mechanism. The
complete table directly describes those reads and also keeps each screen's
header, regions and drawing-only labels together. The three templates are an
explicit named subrange within the engine screen.

Contrary to the old reconstruction note, the measured expressions
`((unsigned)&g_hots[A] - (unsigned)&g_hots[B]) / sizeof(struct Hotspot)` preserve
the loads, subtraction and division in ga_hotspot, sort_spots, find_gas_spot
and sort_gas_spots. Plain pointer subtraction or substituting constant lengths
was not introduced. Enum names do not spend MSC's external-symbol name budget;
removing the old views also reduces the referenced-global inventory.

The hotspot initializers contain integers and indexes, no pointer fixups.
Combining declarations can change OBJ data-record boundaries; no hotspot MZ
relocations depend on those boundaries. Only the strict packed comparison,
including the relocation table, establishes that consumers and link layout
remain identical. See validation below.

## Other findings and limits

- KM_NONE selects a nonheader and KM_MUSIC has no group slot; retained exactly.
- SYS.C's 32-record keyboard-result search can cross screen boundaries. This
  behavior is outside scope and unchanged.
- Paint's box is not on its header's draw chain (header 166 goes directly to
  button 168; box 167 has drawNext zero). Retained exactly.
- The original symbol names and C declaration boundaries remain unknown.

## Validation

- `./sr headers --check`: pass.
- `./sr format --check`: pass.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- `rm -rf work/layout && ./sr build`: 96/96 object gates, 572/572
  mapped functions raw-byte exact at original positions, 748/748 near calls,
  and **Packed EXE vs orig/SR.EXE: IDENTICAL**.
- `./sr dgroup check`: every initialized global in its owner unit.
- `./sr dgroup order`: agrees with SR.LNK.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`:
  109 tests pass, including three new hotspot agreement/boundary tests.
- `./dosbox.sh --make`: **DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL**.
  Both SHA-1 hashes: `3a5e28b795313028b7e77d147122f41cb29de334`.

The build's separate uncompressed diagnostic comparison reports a different
MZ header/relocation order (2218 entries in each) and 14 missing tail bytes
against SR_unpacked.EXE. `linklayout.py:146-160` runs that diagnostic separately
from the packed comparison; no comparison was relaxed. The complete packed
file and native MAKE file, including their packed relocation encoding, match
the shipped SR.EXE exactly. The requested packed/DGROUP gates pass.

MAKEFILE remains CRLF as required by the repository; default git diff --check
reports those CR characters as trailing whitespace. With CR recognized at EOL,
`git -c core.whitespace=cr-at-eol diff --check` passes.
