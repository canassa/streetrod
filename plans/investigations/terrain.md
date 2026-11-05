# Terrain flag words

## Purpose and storage

The road builder creates two roadside scenery words and one shared road word per
track sample; projection, front/rear rendering, cruise control, collisions and
rival steering consume them. The five parallel 1120-entry far arrays in
`src/DATA/SLOTS.C:5-9` are runtime scratch, rebuilt by `rd_generate`, not loaded
from pictures or saved as a road format. `g_roadsideLeftBits` is left,
`g_roadsideRightBits` right, `g_roadBitsArray` the shared word; `g_roadCurve` is
horizontal curve displacement and `g_roadHeight` height. Each flag element is
an unsigned 16-bit word. Keep sizes, signedness and array order unchanged.

`rd_seek` (`ROAD.C:626-633`) positions `g_sampleLeftRoadside`,
`g_hereRoadsideRightRowPtr`, and `g_trackRoadWordsCursor` at the same index. The
projection routines advance/retreat the latter; callers seek again between passes
(`ROAD.C:893-906`). VIEW casts the unsigned pointers to signed word pointers,
then walks far to near: ahead from sample +14 downward, mirror from sample -8
upward (`VIEW.C:1225-1227,1340-1342,1386-1388,1513-1515`). This is still the same
format, not a second set of bits. No other aliases/readers were found by a
whole-tree search for all arrays and pointers.

Evidence line numbers below refer to the source before this investigation's edits.

## Shared road word: g_roadBitsArray

| Bit | Meaning and evidence |
| --- | --- |
| 0 / 0001 | Town cross-section: sidewalks and building geometry, not a country post. Set over samples 0..179 (`ROAD.C:411-438`); selects sidewalk fill, building/scenery interpretation (`VIEW.C:535-577,699-711,794-804,1242-1295,1399-1456`) and building-top projection (`ROAD.C:729,765,815,851`). |
| 1 / 0002 | Right-side utility pole (`ROAD.C:518-519`, `VIEW.C:1293-1294,1454-1455`). Also a right-shoulder collision hazard (`ROAD.C:991`). |
| 2 / 0004 | Town cross street: suppresses longitudinal sidewalk/building/scenery drawing and draws exposed building ends next to intersections (`ROAD.C:420-438`; `VIEW.C:552,649,711-740,804-832,1234,1395`). |
| 3 / 0008 | Center-line dash for this sample (`ROAD.C:418-419,512-513`; `VIEW.C:544-547,579-583,641-644,670-674`). Forced on curves, cleared through constrictions. |
| 4 / 0010 | White road stripe, placed one sample before each race finish index, alongside an overhead sign (`ROAD.C:572-577`; `VIEW.C:577`). Mirror road fill does not read this bit. |
| 5 / 0020 | Left side closed: left edge moves from 1500 to 2000 (`ROAD.C:321-323,737-744,925-929`). |
| 6 / 0040 | Right side closed: right edge moves from 3300 to 2800 (`ROAD.C:279-281,744-747,929-932`). |
| 7 / 0080 | Both sides narrowed: left 2000, right 2800 (`ROAD.C:301-303,733-737,921-925`). Takes precedence over left/right closure if combined. |
| 8 / 0100 | Speed sign (`ROAD.C:604`; `VIEW.C:1319-1320,1492-1493`). |
| 9 / 0200 | Left-chevron sign (`ROAD.C:92-96`; `VIEW.C:1321-1322,1496-1497`). |
| 10 / 0400 | Right-chevron sign (`ROAD.C:117-119`; `VIEW.C:1323-1324,1498-1499`). |
| 11 / 0800 | Road-narrows warning sign (`ROAD.C:295-299`; `VIEW.C:1325-1326,1494-1495`). |
| 12 / 1000 | City-limits overhead sign using vector scenery (`ROAD.C:574`; `VIEW.C:991-992`). |
| 13 / 2000 | Overhead current scene bitmap (county line while racing), the same bitmap drawing mode also serves town destinations (`ROAD.C:577`; `VIEW.C:207-214,993-996,1422-1428,1480-1486`). Resource 1258 was decoded with `sr pic`: it reads COUNTY LINE. Do not name this FINISH: the same bitmap drawing mode serves town destinations. |
| 14 / 4000 | Overhead left-arrow sign in country; Vegas Gambler billboard in town (`ROAD.C:491,493,495,594`; `VIEW.C:215-222,997-1003`, `SCENERY.H:20`). |
| 15 / 8000 | Overhead straight-arrow sign in country; Block Out billboard in town (`ROAD.C:492,494,496,583,588`; `VIEW.C:223-230,997-1003`, `SCENERY.H:22`). |

All four high bits request gantry/building-top projection and make either
shoulder hazardous. Gantry display priority is city limits, scene bitmap, then
left arrow versus straight arrow; these are independent flags, not a numeric
kind (`VIEW.C:979-1003`). `rd_check_crash` checks the next shared sample and the
current/previous roadside barricade (`ROAD.C:974-998`). Width bits are read by
both projection directions, current/four-sample-ahead player edges and the
rival's four-sample lookahead (`RACE.C:1247-1279,1334-1344`): any narrowing stops
the rival choosing an ordinary passing lane.

## Roadside words: g_roadsideLeftBits / g_roadsideRightBits

The town bit in the shared word selects the interpretations below; no separate
per-side tag exists. Extend the existing `RoadsideFlags` in ROAD.H and reuse its
existing constants wherever their meaning is correct.

| Bit | Country | Town / other use and evidence |
| --- | --- | --- |
| 0 / 0001 | Road-edge post | Destination sign, cruise exit and approach turn trigger. Town placements at right 44 / left 108,156 (`ROAD.C:497,500,503`), draw bitmap (`VIEW.C:1422-1428,1480-1486`); stop at +1 (`DRVTICK.C:103-104`), steer at left +7/right +6 (`ROAD.C:1044-1051`). Rear view only draws it in country. |
| 1 / 0002 | Not generated | Read solely to shift a gantry placement back one sample (`ROAD.C:580,585,591`). Never written anywhere. Its intended authoring meaning cannot be recovered; name only that observed effect. It is NOT the shared word's utility-pole bit. |
| 2 / 0004 | Small rock | Raised building span. Builder sets two/three consecutive entries (`ROAD.C:439-477`); building roof and end walls use it (`VIEW.C:701-709,718-721,737-780,796-802,810-814,829-859`). |
| 3 / 0008 | Large rock | Tall building: doubles roof rise rather than half rise (`ROAD.C:442-454,464-476`; `VIEW.C:702,707,719-721,741-779`). Mirror ignores this height bit. |
| 4 / 0010 | Not generated/used as country scenery | Building wall-shading phase, selects fill_quad color 2 versus 3 (`ROAD.C:445,455,467,477,481-488`; `VIEW.C:704,709,712-715,798,802,805-808`). Does not delimit the building's span. |
| 5 / 0020 | Tree | Same tree, at road edge in town versus outer verge in country (`VIEW.C:1246-1251,1296-1301,1403-1408,1457-1462`). |
| 6 / 0040 | Second tree shape | Hydrant (`ROAD.C:424-425,525-547`; `VIEW.C:1265-1270,1277-1279,1288-1289,1430-1435,1440-1450`). |
| 7 / 0080 | Third tree shape | Sidewalk inset/grass strip: fills inner half in ground color (`ROAD.C:458,480,549-552`; `VIEW.C:554-574,651-665`). |
| 8 / 0100 | Bush | Bush (`ROAD.C:485-488,561-569`; `VIEW.C:1252-1260,1302-1310,1409-1417,1463-1471`). |
| 9 / 0200 | Move bush farther from road | Never generated for town; ignored there (`ROAD.C:563-569`; `VIEW.C:1255-1259,1305-1309,1412-1416,1466-1470`). |
| 10 / 0400 | Unused | No set/test; only preserved by some masks. |
| 11 / 0800 | Warning sign | Same warning before fixed town barricades (`ROAD.C:275,317,499,502,505`; `VIEW.C:1261-1262,1315-1316,1418-1419,1476-1477`). |
| 12 / 1000 | Barricade | Same; collision tests current and previous entry (`ROAD.C:278,320,498,501,504,981-990`; `VIEW.C:1263-1264,1317-1318,1420-1421,1478-1479`). |
| 13 / 2000 | Left-then-right S-curve sign | Only generated on right (`ROAD.C:141-143`; `VIEW.C:1311-1312,1472-1473`). |
| 14 / 4000 | Right-then-left S-curve sign | Only generated on right (`ROAD.C:172-174`; `VIEW.C:1313-1314,1474-1475`). |
| 15 / 8000 | Unused | No set/test; preserved by layout clearing masks. |

## Builder masks and edge cases

- `1801`: preserve posts/destinations, warning signs, barricades while clearing
  incidental scenery under signs (`ROAD.C:94-95` and all equivalent sites).
- `0fff`: remove all gantries; `0ff7`: remove gantries AND center-line dash
  through closures (`ROAD.C:277-305,319-325`). Preserve the high-bit clearing;
  replacing this with just `~CENTER_LINE` would change behavior.
- `f117`: clear center dashes, all width flags, chevrons and narrow warning;
  keep town/pole/cross-street/stripe/speed/gantries (`ROAD.C:343`).
- `87ff`: clear right warning/barricade/both S-curve signs; `e7ff`: clear left
  warning/barricade (`ROAD.C:344-345`). Preserve these asymmetric masks.
- `e0`: three width flags; `f000`: four gantry flags; `f002`: gantries or pole;
  `1010` / `2010`: finish stripe plus the respective gantry.
- Generator initializes every flag element to zero before setting features;
  town intersection setup preinitializes the next three samples and skips them.
  CPU speed changes random density and periodic post/dash spacing.
- A shared word with town OR any gantry requires building-top projection, even
  when buildings themselves are absent. Mirror intentionally omits the white
  stripe, town billboard destination signs and tall-building height variation.

## Planned replacement inventory (recorded before editing)

1. ROAD.H: document/extend RoadsideFlags with town aliases, the observed dormant
   previous-entry bit and unused bits; add shared TerrainFlags and composed
   masks. Keep the existing roadside constant spellings.
2. ROAD.C: every terrain initializer, set, clear and test in all builders,
   rd_build_layout, rd_generate, both projections, rd_advance, rd_check_crash,
   rd_update; rename width-mask locals bits/bits7 and projection flag8.
3. VIEW.C: every pointer test in both road fills and building routines, both
   scene traversals, and render_gantry; rename w36/w11/w2/f3/flagk and building
   flag locals a41/b8l/b4/c10/t8l to their roles. Type signed word snapshots as
   enum TerrainFlags, truth-valued int masks as BOOL; keep far storage signedness.
   Replace gantry scenery ids 12/14 with the existing SceneryId members.
4. RACE.C move_rival: replace width mask and rename flag to its width role.
   DRVTICK.C cruise termination: use the town destination alias.
5. SLOTS.H, ROAD.H, VIEW.H: document storage/pointer formats; retain declarations'
   sizes/signedness. Document all RenderRows members if documenting its type.

## Out-of-scope misleading identifiers and uncertainties

- The original array/cursor naming issues were resolved in the naming follow-up
  below; the renderer still uses a signed view of the same shared flag words.
- ROADSIDE_POST/SMALL_ROCK/LARGE_ROCK/TREE_3 are accurate only for country;
  explicit town aliases avoid silently misnaming building and sidewalk tests.
- The fill_quad forward declaration calls its first argument mode; its
  definition calls it col and passes it directly as the fillSpan color
  (VIEW.C:36,409-487). The building shade bit selects colors, not geometry.
- build_dip, build_hill_dip, build_dip_hill still have generic "hill section"
  comments; not part of this flag investigation.
- Existing SCENERY_ARROW_LEFT/UP names identify the gantry's country sign art;
  the same bits select unrelated bitmap billboards in town.
- Unwritten roadside bit 1 and unused bits 10/15 have no proven original design
  meaning. Naming their observed status is the complete evidence available.

## Verification and final replacement inventory

Implemented the planned inventory in ROAD.C, VIEW.C, RACE.C and DRVTICK.C,
with format/cursor documentation in ROAD.H, VIEW.H and SLOTS.H. Every raw flag
operation (including zero initializations and complement masks) now uses the
existing or new names. Mixed town/country post tests retain ROADSIDE_POST,
whose documentation covers both meanings; town-only sites use DESTINATION.
Array and pointer storage declarations retain their original sizes/signedness.
That flag-decoding pass made no global/function renames, data layout changes
or algorithm changes. The naming follow-up below renames the arrays/cursors.

Local replacement list:

| Functions | Old | New / type | Role |
| --- | --- | --- | --- |
| rd_project_ahead, rd_project_behind | bits | widthBits / TerrainFlags | Three constriction bits for the projected row. |
| same | flag8 | needTops / BOOL | Town or gantry needs the elevated Y projection. |
| rd_advance | bits7 | narrow / TerrainFlags | Current then four-sample-ahead constriction bits. |
| move_rival | flag | widthFlags / TerrainFlags | Rival four-sample lookahead constriction bits. |
| vw_road_row, vw_mirror_row | flagk | isCity / BOOL | Town cross-section selection. |
| render_buildings, render_mirror_buildings | a41 | roof / BOOL | Current sample has a raised building roof. |
| render_buildings | b8l | highRoof / BOOL | Full-height roof rather than half height. |
| both building functions | b4 | prevRaised / BOOL | Adjacent near sample has a raised roof. |
| both building functions | c10 | shadeFace / BOOL | Select wall fill color. |
| both building functions | t8l | town / BOOL | Town cross-section, including the unused forward snapshot. |
| render_mirror, vw_draw | w36 | roadFlags / TerrainFlags | Shared word snapshot. |
| same | w11 | leftBits / RoadsideFlags | Left roadside snapshot. |
| same | w2 | rightRoadBits / RoadsideFlags | Right roadside snapshot. |
| same | f3 | city / BOOL | Town scenery interpretation. |

render_gantry's flags parameter now uses enum TerrainFlags; its country scenery
choices reuse SCENERY_ARROW_LEFT and SCENERY_ARROW_UP instead of 12/14.

MSC 5.1 rejects the unsigned 0x8000 enum value (C2141). Keep bit 15 as a
#define for each format, with lower bits in the enums. The original unsigned
far storage is unchanged; the signed renderer snapshots retain two-byte enums.
See the recipe-book lesson. Local names were selected against the actual frame
layout; declaration order for the projection routines is preserved across the
new enum/BOOL declarations. No flags/compiler settings were changed.

Doxygen cannot resolve a bare cross-reference to these arrays when it has both
the unsized header declaration and sized source definition, even though it
renders their documentation. The enum format paragraphs describe the words
without naming their storage; the owning SLOTS.H declarations link to the enum
formats directly. No warning suppression or layout edits were needed.

Validation:

- headers --check and format --check pass.
- docs: zero diagnostics.
- check --all: 572/572, including exact module spans and reference targets.
- Clean build: packed EXE IDENTICAL to orig/SR.EXE; 572/572 mapped game
  functions at original positions, all 748 near-call encodings exact.
- DGROUP ownership and link order checks pass.
- 106 unit tests pass.
- DOS-native build: IDENTICAL to orig/SR.EXE (matching SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334).
- Whole-tree search finds no remaining raw terrain-mask operations or old
  flag-local names in ROAD.C/VIEW.C (bitmap struct members named bits are
  unrelated and remain unchanged).


## Array/cursor naming investigation (2026-11-05, before renaming)

The requested scope is the five numbered arrays and their six standalone
cursors. The original flag decoding above still applies. All readers/writers
were searched across src, headers, metadata and documentation before editing.

| Original array | Proven role | Evidence in the original source |
| --- | --- | --- |
| g_terrain0 | Left roadside words | rd_generate assigns left town destinations at 108/156 and left scenery; rd_check_crash tests the left shoulder; rd_seek maps the left renderer cursor (ROAD.C:440-459,501-506,629,991-994; VIEW.C:1225,1386). |
| g_terrain1 | Right roadside words | Right destination at 44, right scenery/collision tests, and right renderer cursor (ROAD.C:462-481,498,630,998-1001; VIEW.C:1226,1387). |
| g_terrain2 | Signed horizontal curve displacement | Curve builders accumulate g_buildX; projection subtracts this sample from current curveX; rival projection uses the same difference (ROAD.C:86-190,633,744,775,834,865; VIEW.C:1139). |
| g_terrain3 | Shared road flags | Builder writes TerrainFlags; projection reads width/town/gantry bits; renderer uses the same unsigned words through a signed pointer (ROAD.C:631,734-735; VIEW.C:1227,1388). |
| g_terrain4 | Signed road height profile | Hill/dip and bump builders write signed profiles; rd_advance takes the current height, then projection compares sampled height to it (ROAD.C:194-265,599-602,632,743,833,961; VIEW.C:1138). |

rd_seek assigns all five ROAD cursors at index n. Forward projection advances
only shared flags, height and curve; backward projection retreats them.
rd_draw_frame seeks again before each projection and before rendering
(ROAD.C:899-912). The roadside cursors remain at the seek position; VIEW seeds
RenderRows.leftFlags/rightFlags and its separate shared-word cursor at -8 for
the mirror and +14 for the forward view, traversing far to near. These signed
renderer pointers do not introduce another word format. Keep their types.

Proposed array meanings are correct. Their exact spellings cannot all fit:
SLOTS.OBJ contains six far COMDEFs (g_slot is 13296 bytes, each track array is
2240 bytes), rather than ordered initialized segment data. The baseline clean
source builds IDENTICAL. LINK buckets for the existing far symbols are
64,158,166,174,182,190. Proposed roadsideLeft/right hash to 0/39 before g_slot;
roadCurve/roadFlags/roadHeight hash to 188/255/241, also reversing the last two.
The definitions and header declarations must each remain separate: dt_step's
two destination reads use distinct cached SEG words, even though both arrays
share a linked segment. A struct or combined declaration can change ES reloads.

Tool limitation: ./sr fit only models DGROUP COMMON and private BSS; it wrongly
claims all far data are definition-placed. For this task inspect SLOTS.OBJ's
COMDEF records and calculate tools/sr/linkhash.bucket("_" + name), then require
the full packed build. No relaxation of byte/reference gates is permitted.

Planned array spellings, chosen to preserve increasing far hash order:
g_roadsideLeftBits (160), g_roadsideRightBits (166), g_roadCurve (188),
g_roadBitsArray (192), g_roadHeight (241), after unchanged g_slot (64).
All retain their original order, signedness, far qualifier and 1120 bound.

Planned cursor spellings (subject to ./sr fit and the actual file/build gates):

| Old | Planned | Role |
| --- | --- | --- |
| g_roadTerrain | g_sampleLeftRoadside | Left roadside sample at rd_seek position. |
| g_rightSideTerrainRow | g_hereRoadsideRightRowPtr | Right roadside sample at rd_seek position. |
| g_hereCurvePtr | g_seekRoadCurvePtr | Curve cursor advanced/retreated by projection. |
| g_heightRoadPtr | g_elevationCursor | Height cursor advanced/retreated by projection. |
| g_hereFlagsPtr | g_seekRoadFlagsPtr | Shared flag cursor for projection/driving. |
| g_viewTerrainBitsData | g_renderRoadFlagsWordPtr | Signed shared flag cursor for rendering. |

The natural shorter Ptr/Row spellings do not fit the communal bucket windows.
Name growth consumes compiler storage once per contributing symbol (4d), so
whole-file and all-file gates must validate these together. Do not change any
unrelated symbol to make budget room. RenderRows members already describe
left/right flags and require no rename. Existing TerrainFlags constants name
the decoded format rather than an array number and remain unchanged.

Other identifiers: build_dip/build_hill_dip/build_dip_hill still have inaccurate
hill-only comments, as recorded above; outside this naming task. No new
semantic uncertainty was found for the array/cursor roles.

### Linked placement trial and final cursor spellings

The first renamed build passed all 572 function/object gates and all five far
arrays linked at their original offsets (33F0,3CB0,4570,4E30,56F0 in 389B).
However, the two near-common ties failed: g_seekRoadCurvePtr (bucket 44) moved
from DS:7560 to 755A, and g_seekRoadFlagsPtr (111) from DS:7D62 to 7D42. This
changed 76 loaded/packed bytes. Keep the original buckets for those two:

| Original | Final name | LINK bucket / original address |
| --- | --- | --- |
| g_roadTerrain | g_sampleLeftRoadside | 23 / DS:72EE |
| g_rightSideTerrainRow | g_hereRoadsideRightRowPtr | 203 / DS:8634 |
| g_hereCurvePtr | g_sampleCurveXReadPtr | 45 / DS:7560 |
| g_heightRoadPtr | g_elevationCursor | 59 / DS:7880 |
| g_hereFlagsPtr | g_trackRoadWordsCursor | 112 / DS:7D62 |
| g_viewTerrainBitsData | g_renderRoadFlagsWordPtr | 215 / DS:8B82 |

The renderer cursor's equal-bucket tie with its following communal preserves
placement. No source types, declaration order, sizes, flags, expressions or
algorithms changed. An inverse-rename token comparison of every changed .C
file against HEAD confirms identifier changes and formatting only.

### Naming follow-up verification

Final array names are the five planned spellings above; the final six cursor
names are in the linked-placement table. Every rename used ./sr rename,
including the two corrections after the first linked trial. Sources, headers,
metadata and reconstruction docs contain no old numbered arrays or cursor
names; the old spellings remain only in this investigation's historical
mapping/trial evidence and the append-only diary.

- headers --check and format --check pass; docs has zero diagnostics.
- check --all: 572/572 (551 MATCH, 21 MATCH*), with whole-file/reference gates.
  The combined name growth fits the actual contributing-symbol budget; no
  flags, unrelated names or compiler assumptions were changed to achieve it.
- Clean build: 96/96 objects pass; Packed EXE vs orig/SR.EXE: IDENTICAL.
- All 572 mapped game functions are raw-byte exact at original positions;
  all 748 near-call encodings are exact.
- SR.MAP independently confirms all eleven renamed data symbols retain their
  original addresses, including each far array's segment and offset.
- DGROUP ownership and object/link order pass.
- 106 unit tests pass.
- Final inverse-rename token comparison confirms every changed C file differs
  only in the requested identifiers and formatting; all storage types, sizes,
  ordering, expressions and algorithms are preserved.

The sr-cli skill and RECONSTRUCTION.md now document fit's far-COMDEF limitation
and the need to preserve distinct far declarations/cached SEG words. The
limitation does not prevent this task: emitted records, LINK hash calculations
and the full identical packed build establish the placement directly.
