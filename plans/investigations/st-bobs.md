# Bob's, carhop and fame record investigations

Scope, in requested order: Btn, Step, Cand, Cell, Wager (BOBS.H), Sheet
(CARHOP.H), ManualEntry and Entry (FAME.H). Investigation completed before
renaming. Evidence locations below refer to the pre-edit source; function names
and data names identify the same code after line numbers move. No game logic,
storage order, member size or signedness is to change.

## 1. Btn -> JailOverlay

Purpose: describe one rectangular piece of the jail foreground, not a button.
Storage: seven initialized public records in BOBS.C:973-976, g_jailBars;
five bar strips and two wall edges. They are compiled data, never saved/loaded
as records. The table stores near pointers to global far bitmap pointers,
resolved when bo_jail loads PIC_JAIL_WALL and PIC_JAIL_BAR (BOBS.C:1196-1197).

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| src -> bitmapSlot | 0/2 | Near address of g_barJailPixels (first five) or g_edgeBmpJail (last two); bo_jail dereferences it for copy, BOBS.C:1216. The slot's bitmap is borrowed for the jail scene. |
| sx -> sourceX | 2/2 signed | Source pixel column, 0 except 32 on the right wall; BOBS.C:1212. |
| sy -> sourceY | 4/2 signed | Source pixel row, always 0 in the table; BOBS.C:1213. |
| w -> width | 6/2 signed | Pixel width, 16 for bars, 32 for walls; BOBS.C:1210 and CARPIC.C:647. |
| h -> height | 8/2 signed | Pixel height, 149; BOBS.C:1211. |
| x -> screenX | 10/2 signed | Screen destination column: 40,96,152,208,264,0,288; BOBS.C:1214 and CARPIC.C:648. |
| y -> screenY | 12/2 signed | Screen destination row, 5; BOBS.C:1215. |

Size 14, word aligned. Only the initializer writes these members.
cp_patch_bg additionally restores the foreground from the screen using width
and screenX, so cars remain behind the bars. Keep pointer indirection: replacing
it with an embedded bitmap pointer would change initialization and lifetime.

## 2. Step -> IdleAnimStep

Purpose: choose one pose and its dwell time for the background character at
Bob's and behind the jail window. Storage: thirty initialized private records,
g_animSteps, BOBS.C:689-692; not a file format or save-game record.

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| frame -> poseIndex | 0/1 unsigned | Values 2..14 index g_animCells at poseIndex+1; the result is the sheet cell passed to blit_cell, BOBS.C:799. It is not directly the bitmap cell number. |
| delay -> delayTicks | 1/1 unsigned | Dwell ticks (7,15,22,30,37,45,75,90); added to g_tick for g_waitUntil, BOBS.C:800. |

Size 2, byte aligned. The initializer is the sole writer. blit_cell
(BOBS.C:742-773) divides the physical cell index by seven, uses 40x50 cells,
and treats indices >=14 as cell zero. Its jail branch copies a bar over the
source cell first. poseIndex is an ordinal into a remapping table, not a
proven set of semantic animation states; do not invent pose names.

Edge case: play_cell_anim tests i>=31, although g_animSteps has only 30
records. On a timeout with i==30 it reads one record past the table, into the
following g_animCells bytes. Those bytes are 1,15, so the incidental final step
maps g_animCells[2] and waits 15 ticks. Preserve the thirty-record extent and
existing condition; do not silently repair behavior. This also disproves the
old comment's claim that frame is already the physical frame number.

## 3. Cand -> RivalCandidate

Purpose: temporary per-rival distances and exclusion state while building the
encounter pool. Storage: automatic map[RACER_KING] in build_rival_pool,
BOBS.C:273-321. Every member is initialized on each call, then discarded.

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| dr -> speedDifference | 0/2 signed | abs(pf_vmax(rivalSpec)-pf_vmax(-1)), in mph, BOBS.C:282; compared to the running minimum, :296-298. |
| taken -> excluded | 2/2 signed | Boolean: initially true for RACER_EMPTY, :285; false candidates can be selected, :296; set true after selection, :304. Use BOOL and TRUE/FALSE. |
| dk -> rankDifference | 4/2 signed | Absolute rival/current-car model-rank gap, :283-284; checked against RIVAL_RANK_DIFFERENCE at :311. |

Size 6, word aligned. pf_vmax computes attainable speed (PERF.C), whereas the
old comment calls dr a rating difference and dk a kind difference. Equal speed
gaps keep the first rival in table order because the comparison is strict.
Excluded means unavailable OR already selected, not merely already taken.

Edge case: the post-selection rank filter reads map[k-1], indexed by pool
position, rather than map[g_poolRivals[k-1]]. Document the rank gap as per-rival
state; do not claim the current filter necessarily examines the selected
rival's rank. The flag permits all ordinary rivals while g_poolRivals has only
12 slots; this separate function/table issue remains out of scope.

## 4. Cell -> EndingCell

Purpose: one crop and placement in either ending-animation sheet. Storage:
private initialized g_endCellsA[20] and g_endCellsB[9], BOBS.C:694-703;
read only by play_ending (BOBS.C:817-971), never saved/loaded as records.

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| sx -> sourceX | 0/1 unsigned | Sheet pixel column; A:0..160, B:0..208; BOBS.C:878,902,944. |
| sy -> sourceY | 1/1 unsigned | Sheet pixel row; A:0,24,68,112,156, B:1,54,65,86,99; :879,903,945. |
| w -> width | 2/1 unsigned | Crop width in pixels; A:32..64, B:8..224; :876,883,900,931,942. |
| h -> height | 3/1 unsigned | Crop height in pixels; A:24/44, B:2..101; :877,881,884,886,901,905,932,943. |
| x -> destinationX | 4/1 unsigned | A:80..96, destination column biased by 80 in the 80x44 work bitmap; B:80..136, direct screen column. :880,885,904,931,944. |
| y -> destinationY | 5/1 unsigned | A:all zero and ignored (crop bottom aligned at row44); B:51..113, direct screen row, used for union rectangle and destination :932-933,946. |

Size 6, byte aligned. A crops PIC_ENDING_COUPLE; the work bitmap is ultimately
copied to screen (120,69). B crops PIC_ENDING_LIPS and builds the union of the
previous/current screen rectangles to avoid leaving an older lips image.
Tandy omits B's last cell. There is no general x bias shared by both tables,
and these are ending records, not intro-picture records as the old comment said.
Only initializers write them; preserve unsigned bytes and both conventions.

## 5. Wager (name retained)

Purpose: accepted stake shared between negotiation and race settlement.
Storage: communal g_raceWagerTerms, BOBS.C:402; no save/load references. It is
set only on an accepted challenge, then copied into ra_outcome locals
(RACE.C:663-664). It remains live between those calls.

| Member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| kind | 0/2 signed | enum BetKind, BET_NONE/BET_10/BET_50/BET_25/BET_100/BET_CAR. King sets NONE or CAR (:530,534); ordinary accepted wager sets kind (:612). RACE.C:663 reads it to choose settlement/messages. |
| amount | 2/2 signed | Cash dollars, 0,10,25,50,100. King sets zero (:536), ordinary acceptance copies price (:613). RACE.C:664 copies it; :699 adds it on victory, :786,877 passes its negation on defeat. Zero for kicks and pink slips. |

Size 4, word aligned. Existing WagerAmount names all positive values; extend it
with WAGER_NO_CASH and type amount as that enum, preserving signed int size.
Do not change it to unsigned just because challenge's price is unsigned.
Both King stages have zero cash. BetKind encodes stake and race selection,
while amount is the dollar transfer; they are not interchangeable.

## 6. Sheet -> CarhopSheet

Purpose: geometry and picture ID for cutting carhop animation cells.
Storage: two initialized private g_carhopSheets records (CARHOP.C:20), indexed
by CarhopMode; cut_sheet reads them once per skate (CARHOP.C:40-42,138).

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| id -> pictureId | 0/2 signed | PIC_CARHOP_WALK_RIGHT for enter, PIC_CARHOP_WALK_LEFT for leave; loaded through di_bm at :43. |
| cols -> columns | 2/2 signed | Six cells per row; sheet width/columns determines width, wrap at columns, :41,44,50. |
| n -> cellCount | 4/2 signed | Eleven cells to extract, :42,49; not twelve (the bottom-right cell is omitted). |

Size 6, word aligned. Both sheets have two rows: half bitmap height gives cell
height. bm_grab produces high-stack-arena sub-bitmaps into imagePtrs[1..11];
g_carhopFrames selects them, while imagePtrs[0] is the foreground mask.
No record mutation or persistence; picture resources are loaded separately.
The picture ID becomes the existing signed-word enum PictureId (IDs through
1267); no new picture value set is needed.

## 7. ManualEntry -> ManualQuestion

Purpose: manual page and expected menu-color answer for copy protection.
Storage: 51 initialized private g_manual records, FAME.C:30-35. fa_protect_check
selects rn_int(MANUAL_ENTRY_COUNT) (:51). The first MANUAL_ICON_COUNT ask about
car icons; the rest about car keys (:52-66). No loading, saving or mutation.

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| page | 0/1 signed | Printed manual page, icons 2..23, keys 1..33 (selected pages only); formatted into the question, FAME.C:60,65. |
| color -> answerColor | 1/1 signed | Expected decoded menu answer 1..12, compared with mn_wait(KM_MANUAL_COLORS), FAME.C:80,83. |

Size 2, byte aligned. HOTSPOT.C:413-424 and MESSAGES.ASM:104-115 prove mapping:
grey,brown,red,pink,orange,yellow,dark green,light green,dark blue,light blue,
dark purple,light purple. These are ordinal menu choices, not EGA indices.
KEYMAPS.ASM:407-411 maps keys 1..0 to 1..10, F1/F2 to 11/12. Add ManualColor enum;
the signed char stays signed char and docs refer to it. Replace all question
initializers, those hotspot results and keyboard-map outputs with named colors.
The key-map EQU counterparts retain the established C/ASM constant convention.

## 8. Entry -> HallEntry

Purpose: one hall-of-fame driver's name and elapsed-hour result, lower is better.
Storage: communal g_hallFameTbl[10], FAME.C:28, 230 bytes. Unlike the preceding
records it is persisted: DISK.C:582-607 reads hall_dat, a two-byte little-endian
count followed by that many 23-byte records; di_save_hall (:614-654) writes the
same format. scramble_hall/unscramble_hall (:560-575) set/clear every byte's high
bit across the whole ten-record allocation, including unused slots.

| Original -> final member | Offset/size | Meaning and complete users |
| --- | --- | --- |
| name -> driverName | 0/16 char array | NUL-terminated driver name (15 editable chars). insert_entry copies g_driverName through the record's byte address, FAME.C:157-162; fa_show draws it (:231), copies to an edit buffer (:244), edits with limit15 (:245), copies back (:246). |
| score -> elapsedHoursText | 16/7 char array | NUL-terminated decimal game hours, written by itoa at record+16 (:163-164), parsed by score_beats_entry (:142), parsed/drawn by fa_show (:233,238). |

Size23, byte aligned, no record padding. elapsed_hours is ga_day()*24 + current
hour +12 (FAME.C:169-175). ga_day derives the game day count and ga_time the
current clock. fa_show finds the first strictly larger hour count; ties stay
after existing ties. insert_entry memmoves whole records down; a full table
drops its last record, or rejects a new worst result. Only a new entry is edited
and saved. Keep the raw record offset16 writer byte-identical.

Edges: disk count is trusted rather than bounded to HALL_FAME_COUNT; string
termination is trusted after loading; fa_show's digit alignment loop would not
terminate for a zero hour string. No validation/format changes in this task.

## Other misleading names and out-of-scope findings

- g_jailBars includes two wall edges, not just bars (BOBS.C:973-976).
- g_animCells is a pose remapping with an unused first entry, not animation
  steps. play_cell_anim animates the background character in PIC_CARHOP_IDLE,
  including at Bob's (BOBS.C:1006). The resource name is also misleading:
  decoded picture 1225 shows a trousered character leaning on a ledge; it is
  distinct from the red-skirted skating carhop in pictures 1142/1141.
- g_endCellsA/B are ending couple/lips crops; the old "intro picture" comment
  on Cell was wrong. Both coordinate conventions are documented above.
- ch_save_bg's old comment claims it clears a 200x68 rectangle; it copies a
  48x41 rectangle from (200,68) (CARHOP.C:180-192).
- score_beats_entry returns whether the stored hour value is greater than the
  new value, so the new result beats the entry, not vice versa (FAME.C:142).
- g_score is elapsed game hours, not points (FAME.C:210 and :175).
- g_protectCycle means the question menu is active (FAME.C:79-81), rather than
  being a numeric cycle count.
- Keep the Step overrun, pool/rival rank-index mismatch and unbounded load count
  visible as findings; this investigation does not fix game behavior.

## Planned replacements and validation

Tag/member renames above apply to every actual user in BOBS.C/H, CARPIC.C,
CARHOP.C/H, FAME.C/H and DISK.C. Wager remains named Wager; amount becomes
existing enum WagerAmount plus its no-cash enumerator. RivalCandidate.excluded
becomes BOOL with FALSE/TRUE initialization, test and selection stores.
ManualColor names replace all answer literals in g_manual, the twelve manual
hotspot results and KEYMAPS.ASM manual bindings. No unrelated symbols renamed.
The existing HALL_FAME_COUNT and MANUAL_ENTRY_COUNT replace matching table bounds.

Baseline layout captured through cdata.Types before editing: 14,2,6,6,4,6,2,23
bytes respectively; member offsets are listed above. Validate final sizes and
offsets with real MSC compilation, then the mandatory headers, formatting,
Doxygen, 572-function, clean packed-build, DGROUP and unittest gates. Merge
origin/main as requested and rerun those gates before reporting completion.

Resource cross-check: decoded original pictures 1142/1141 (240x82 each),
6/7 (couple 200x200 and lips 272x199), and 1225 (280x100). These confirm the
two-row sheet cuts, both ending crop sets, and the distinct idle character.
Original data dumps also confirm g_manual at 4b62, g_animSteps at 521e followed
immediately by g_animCells at 525a, and g_carhopSheets at 5a26. Addresses are
evidence in this note only, never source documentation.

Validation adjustment: the existing MANUAL_ENTRY_COUNT and HALL_FAME_COUNT
are preprocessor defines; cdata.Types does not resolve macros in array bounds.
The initial named-bound edit compiled and packed identically, but DGROUP failed
with ValueError: MANUAL_ENTRY_COUNT. Keep the original literal table bounds
51 and 10, rather than extending tooling outside this task. The color value set
is fully named independently of those table extents. This supersedes the planned
named-bound replacement above. Existing count macros remain at runtime uses.

Initial gates: headers/include ownership and formatting passed; Doxygen had
zero diagnostics; ./sr check --all matched 572/572 (551 MATCH, 21 MATCH*);
clean ./sr build ended Packed EXE vs orig/SR.EXE: IDENTICAL with 572/572
functions at original positions and 748/748 near-call encodings exact; all 109
unit tests passed. DGROUP source ownership and link order passed after restoring
literal bounds. A targeted real-MSC sizeof/offsetof probe checked all eight
records and 35 size/offset assertions with zero differences from the baseline.
The probe and PNGs remain disposable work output; the note contains their facts.
DOS-native MAKE also produced an IDENTICAL SR.EXE (matching SHA-1
3a5e28b795313028b7e77d147122f41cb29de334).

## Final integration and gates

Fetched origin and ran `git merge origin/main` after the implementation commit;
it reported Already up to date (origin/main 3848b38). No conflicts. Then reran
all gates against the committed sources:

- `./sr headers --check`: ownership/includes/dependencies current.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- Removed only work/layout and ran `./sr build`: 96/96 objects gated,
  572/572 functions at original positions, 748/748 exact near calls,
  Packed EXE vs orig/SR.EXE: IDENTICAL.
- `./sr dgroup check` and `./sr dgroup order`: passed.
- `python3 -m unittest discover -s tests` in shell.nix: 109 tests, OK.
- Targeted real-MSC audit: eight records, 35 size/offset checks, zero mismatches.
- `./dosbox.sh --make`: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL.

No unresolved investigation blockers; the documented original behavior defects
and naming findings remain unchanged and outside the editing scope.
