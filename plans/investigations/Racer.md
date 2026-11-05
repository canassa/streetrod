# Racer investigation

## Purpose and scope

One persistent rival driver's identity, driving/wager behavior, assigned car,
appearance and encounter history, stored as an 18-byte record.

This note was written before source renaming. Evidence pins refer to the source
at the start of this investigation. The target is the record, so function
parameters/locals and unrelated globals remain outside scope.

Keep `Racer`: this describes a driver, not a complete owned `Car`. Keep the tag,
member order, widths and signedness. Rename `betToleranceClass` to
`stakeToleranceRow` (a table row, not the car class), `shiftPenalty` to
`earlyShiftPenalty` (an RPM deduction, not a shift delay), `overlayPic` to
`portraitPic` (the driver's in-car image), and `betWins` to `rivalBetWins`
(explicitly the rival's wins, not the player's). Other field names are accurate
with the contracts below. `./sr rename` supports functions/globals/files rather
than members (tools/sr/rename.py:49-59); use scoped member edits.

## Storage, inputs and outputs

BOBS.C:109-111 owns 22 records and the selected-record pointer. DISK.C:69-82
loads their 396-byte block first in HOT_DATA, followed by car specifications,
models, description text and behavior tables. di_load_data reads these 13 blocks
verbatim (DISK.C:502-526). GAME.C:78 and MAIN.C:355 call that loader.
SAVEGAME.C:339,466 reads/writes exactly 0x18c bytes of records: 22 * 18 = 396.
The format uses little-endian two-byte ints; byte fields must not become enum or
BOOL words. No record contains a pointer: the name is a text offset and the
car/picture members are indices/IDs. Array storage lives for the game; selection
borrows a pointer into it and save loading overwrites it in place.

### Each member, with byte offsets relative to the record

| Offset | Original member | Meaning, source and consumers |
| --- | --- | --- |
| 0 | signed char betToleranceClass | Index 0..2 into g_rivalStakeToleranceTable, selected before stake classification/rating comparison (BOBS.C:464-478). Loaded behavior, not changed during play. |
| 1 | char shiftPenalty | Early manual-upshift deduction in hundreds of RPM: shiftRpm - 100 * value (RACE.C:955,971,1114-1118). Police copies slot 10's value too (1351-1354). |
| 2 | signed char drivingPenalty | Driving handicap shared by bend braking and contact retaliation. At a terrain boundary target speed is vmax minus this value when ahead, half when behind (RACE.C:1066-1074). A contact counterattack has probability 1 / ((value >> 2) + 2) (1450-1461). Thus a smaller value makes the rival faster and more likely to ram; it is not just a speed penalty. |
| 3 | char unused1 | Loaded/saved byte; no member reads/writes. All resource values zero. Its original intended meaning is unknown; preserve it. |
| 4..5 | int overlayPic | Resource ID of the driver's portrait. Copied to g_carDraw by select_rival (BOBS.C:368), loaded and placed behind the car body by cp_build (CARPIC.C:335-338). Ending substitutes the King's girlfriend and restores the King (BOBS.C:865-869). |
| 6..7 | int nameOffset | Byte offset into g_desc, used for the rival/car banner (BOBS.C:154-156) and call-rival menu (394). Demo assigns DESC_THE_GEEK (SAVEGAME.C:115). |
| 8..9 | int specIndex | Index into g_carData and corresponding model tables. Ordinary resource records start at -1, then sg_init_racer chooses from a supplied nonempty list (SAVEGAME.C:66-79). King starts at 25 and is not randomized (110). Selection sets g_opponentSpecIndex/g_oppSpecPointer (BOBS.C:344). Also used for pool speed/rank comparisons (281-284). |
| 10 | signed char paintScheme | Palette scheme 0..5 after initialization (SAVEGAME.C:56-59); resource has -1 for ordinary unassigned cars, 0 for King. Passed to ga_palette through g_carDraw (BOBS.C:366-367), copied to a won car (RACE.C:706). GARAGE.C:166-175 and CRUISE.C:42-65 select paint table rows. |
| 11 | char status | RACER_EMPTY (-1): removed; RACER_NOT_MET (0): not callable; RACER_MET (1): callable. Ordinary selection marks met (BOBS.C:347); pool excludes empty (285); call menu admits met (392). King's status is recalculated from eligibility (387-390), not strictly whether he has appeared. |
| 12 | signed char betRaces | Wagered race count for the current assigned car, reset at initialization/replacement (SAVEGAME.C:61), incremented after outcomes unless for kicks (BOBS.C:1102-1105). Feeds acceptance adjustment (485) and appearance-change thresholds (349-361). |
| 13 | char betWins | Rival's wins among counted races: incremented only if g_resultWin == 0 (BOBS.C:1104-1105); RACE.C:671,687-688 establishes that g_resultWin means player victory. Reset with betRaces; acceptance uses (2*wins-races)*baseChance/4 (BOBS.C:485-486). |
| 14 | char bodyMods | Bit 0 rear bumper overlay, bit 1 front bumper overlay, bit 2 roof overlay, bit 3 large-engine overlay (GAME.H:37-42; CARPIC.C:311-324). Initialization sets large-engine bit from spec, randomizes available body overlays (SAVEGAME.C:74-79); encounters may add them after 8 wagered races (BOBS.C:354-361). These are rendering overlays; do not call them installed-part flags without examining each model image. Won cars copy body bits to appearance bits 13..15 (RACE.C:700-705). |
| 15 | signed char sticker | 0 means no sticker; 1..8 selects g_imageDecalLayout[value-1] (CARPIC.C:331-333). Init/encounter draws rn_int(9), so zero is also possible on a successful random choice (SAVEGAME.C:75; BOBS.C:349-353). Copied into won car's appearance (RACE.C:698-699). |
| 16 | char lostCarOnce | Byte boolean: after the first pink-slip loss a lower-ranked replacement is initialized and this flag becomes 1. A second loss removes the rival (RACE.C:713-726). sg_init_racer resets it to 0 before the caller sets 1 (SAVEGAME.C:65). King is removed immediately; demo rival skips the loss handling. |
| 17 | char unused2 | Loaded/saved byte, zero in resource, no member reads/writes; preserve without inventing a purpose. |

## Resource evidence

Decoded orig/freeware/HOT_DATA directly with Python struct format
`<bBbBhhh bBbBBbBB`, at offsets i*18 for i=0..21. This is the file loaded by
di_load_data, not a guessed original-image offset. Relevant loaded values:

| Slots | Tolerance rows | Early-shift penalties | Driving handicaps |
| --- | --- | --- | --- |
| 0..6 | 0,0,0,0,1,1,1 | 20,18,20,18,18,18,18 | 10,12,10,12,10,12,10 |
| 7..13 | 1,1,1,1,2,2,2 | 18,18,18,16,16,16,16 | 7,7,7,10,7,7,7 |
| 14..20 | 2,2,2,2,2,2,2 | 16,15,15,14,12,11,11 | 7,7,5,5,5,5,5 |
| 21 (King) | 2 | 10 | 4 |

Portrait IDs by slot: 1196,1194,1209,1197,1198,1210,1213,1195,1208,1211,
1203,1216,1202,1199,1205,1207,1206,1204,1212,1214,1215,1244.
Name offsets: 1427,1433,1440,1446,1452,1459,1466,1472,1479,1486,1492,1499,
1505,1510,1519,1527,1534,1539,1546,1553,1559,1567.
All unused bytes, statuses, race/win counters, stickers and loss flags are zero.
King has spec 25, paint 0, bodyMods 8; ordinary records have spec/paint -1 and
bodyMods 0. Tolerance rows, in free/cash/car order, are (60,45,12), (55,40,8),
(50,30,5). Larger row indices mean less tolerance, so do not invent a more
"tolerant" direction or an enum equating this to car class.

## Lifecycle, context and indirect effects

1. Resource loading supplies fixed identity and behavior plus default car state.
2. init_racers groups ordinary specs by rank into low (<3), medium (3..5), high
   (>=6), assigns seven drivers per group, and marks King not met
   (SAVEGAME.C:83-116). sg_init_racer consumes an unused entry of a count-first
   list, marks it -1 to avoid reuse, and resets mutable car/history fields.
   Fixed behavior and portrait/name survive replacement. Demo overrides its name.
3. build_rival_pool computes vmax/rank distances, excludes removed rivals;
   pick_rival chooses a pool member or King (BOBS.C:273-335). select_rival
   updates selection globals, appearance, palette and King state (338-369).
   rival_arrives calls that, then animate_rival_car renders body/sticker/portrait
   and the text banner (372-379,154-156,179).
4. list_met_rivals builds the callable menu; King readiness recalculates his
   status. king_status considers whether ordinary rivals remain and player's
   win totals/rate (BOBS.C:224-245,383-399).
5. accepts_bet reads the behavior row and opponent/player car/engine ratings,
   chooses a base probability by stake and rating relationship, adjusts it by
   rival's record, player's sticker and demo mode, then compares rn_int(100)
   with the score (434-489). It never writes the record.
6. Race engine initialization copies the early-shift penalty into race-local
   static state; acceleration reads driving handicap directly. cop_appears
   replaces the selected pointer with ordinary slot 10 and uses police spec 25
   independently of that record's assigned spec (RACE.C:1350-1360).
7. ra_outcome transfers appearance/paint on a pink-slip player win, resets a
   replacement rival or marks removed (RACE.C:695-726). bo_visit counts wagered
   outcomes after ra_outcome returns, unless an early exit such as jail or the
   final King victory intervenes (BOBS.C:1067-1105). Palette, money, owned car
   lists, global win totals and scene transitions belong to those consumers,
   not operations performed by the struct itself.
8. Save/load preserves fixed and mutable fields together, including unused bytes.

## Edge cases and open questions

- Counters are bytes, betRaces explicitly signed, and increments are uncapped.
  Do not promise unbounded counts or change signedness while documenting them.
- Replacement resets history inside ra_outcome before bo_visit increments the
  selected record for that result. Thus the first replacement may already have
  betRaces=1 despite not yet being raced. This is observed behavior to preserve.
- specIndex=-1 and paintScheme=-1 are resource defaults before initialization;
  later unchecked table readers expect initialized ordinary records. Save files
  are read raw without per-member validation.
- status is stored as char; an enum would double its width. lostCarOnce also
  stays char, even though boolean. All existing field types/layout are retained.
- The King keeps resource-assigned car state; only his status is reset by
  init_racers. His callability is eligibility, not historical contact.
- No code names or interprets unused1/unused2. Their original purpose cannot be
  recovered from zero resource values and block I/O. Keep that uncertainty.
- Historical identifier spellings are not recovered; names describe proven use.

## Other misleading identifiers outside scope

- RACER_COP_ENTRY (BOBS.H:160) names slot 10 as though it were a dedicated cop
  record, but SAVEGAME.C:106-107 gives it an ordinary medium-group rival car.
  Police borrows its behavior in RACE.C:1351-1354, not its car/identity.
- g_driverIdxSelVal is the selected rival's array index (BOBS.C:342), not a
  player driver setting; g_resultWin is player victory (RACE.C:671,687-688).
- g_carDraw.overlayPic is the in-car portrait source in CARPIC.C:335-338, even
  though the generic name can also describe other image overlays.
- RIVAL_STICKER_CHANCE et al. are random upper bounds, with rank-based values
  compared against them (SAVEGAME.C:73-79); they are not percentages themselves.

## Validation

All required gates passed on branch inv-racer:

- `./sr headers --check`: includes/dependencies current.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics; struct and enum/member XML generated.
- `./sr check --all`: 572/572, including 21 MATCH* external near-call cases.
- Clean `rm -rf work/layout && ./sr build`: 96/96 object gates, 572/572
  mapped game functions byte-exact at original positions, 748/748 near calls
  exact; packed executable IDENTICAL to orig/SR.EXE. The unpacked-link diagnostic
  reports container/header/relocation-order differences; the decisive packed
  executable comparison succeeds.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  104 tests, OK.
- Real MSC 5.1 `tools/scripts/check_data_types.py`: 154 types, 708 size/offset
  checks, zero mismatches, including the unchanged Racer layout.
- `./sr diff init_rival_eng`: RACE.C module matches.
- `./dosbox.sh --make`: DOS-native executable IDENTICAL, SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334.
- `git diff --check`: clean.

No algorithm, type width, data value, function/local/global name or struct layout
changed. No new tests were needed: existing byte, compiler-layout and unit gates
cover the risks of these member renames and documentation changes.
