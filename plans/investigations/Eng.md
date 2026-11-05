# Eng investigation

## Purpose and scope

`Eng` holds one car's runtime driving state and generated drivetrain curves, including steering, tire drift, dashboard invalidation and race results. Rename the type to `DrivingState`: “engine” alone does not describe its consumers. This investigation concerns this type and the value sets of its members; other types, functions and globals retain their names.

This note was written before changing identifiers. Evidence line numbers below refer to the original source at the start of this investigation; the replacement inventory and verification section describe the resulting changes.

## Storage, construction and persistence

- Two near-data communal instances: `g_driveEng`, defined at `src/SYS/SYS.C:540`, and `g_oppEngState`, defined at `src/GAME/DRVTICK.C:18`. Original metadata records offsets 30944 and 31202 respectively. The compiler/data model gives 194 bytes, alignment 2, with the member offsets listed below. Preserve every member's size, signedness, order and all spare storage.
- No resource file loads this record. `pf_drivetrain` (`PERF.C:27-88`) constructs the curves from the installed engine, manifold, carburetor, transmission and tire variants, model rank, removed bodywork, timing and engine wear. It computes a speed rating capped at 130 mph, then RPM-per-mph ratios and three acceleration/deceleration bands for each of five slots (neutral and four forward gears).
- `pf_drivetrain_spec` (`PERF.C:92-118`) supplies a temporary car/parts record for an opponent specification. The player is configured when choosing the current car (`GAME.C:832`) and after repairs (`RACE.C:711`). Rival initialization/replacement invokes the spec constructor (`RACE.C:954,1352`).
- `ra_reset_eng` (`RACE.C:895-947`) clears runtime speed/gear/steering/results, initializes RPM/band caches, initializes automatic neutral, and sets tire drift/failure from the **player's current tires even for the rival instance**. It also resets shared input, rival pace and race state; it is not a pure record initializer. Called for racing (`RACE.C:987,956`) and cruising (`CRUISE.C:33`).
- These records are not serialized. `SAVEGAME.C:337-342,464-469` enumerates the disk-save payload (driver name, owned cars and spare parts, race counters, rival records, clock and road seed), excluding driving state. There are no aggregate copies or alternate aliases in game/ASM source; all accesses are through the instances or pointers passed to the constructor/reset/dashboard/gear-fit functions.

## Member map (all offsets decimal)

| Offset | Original member | Role and observed values; readers/writers |
|---|---|---|
| 0 | speed | Integer mph; reset 0 (`RACE.C:908`), changed by race ISR (`SYS.C:1142-1143,1220-1230`), cruise (`DRVTICK.C:112-119`), rival gear fitting/acceleration (`RACE.C:1045,1059-1142`). Read by gauges (`DRIVE.C:89-141`), staging (`DRVTICK.C:42,57`), road/relative motion (`RACE.C:1206-1242,ROAD.C:1020-1037`), statistics, collision/repair/fine logic (`RACE.C:538,568,742-744,808-822,1455`) and the speed accumulator (`SYS.C:1136`). Not physical road-position units. |
| 2 | steeringInput | Signed player steering impulse, clamped to +/-24; reset by `SYS.C:661,991` and `RACE.C:908`, updated/cleared by input ISR (`SYS.C:1231-1241`). Road curve adjustment consumes it (`ROAD.C:1020-1022`); rival contact behavior tests its sign (`RACE.C:1438`). |
| 4 | steering | Signed wheel deflection, +/-24, decays toward zero in `SYS.C:1242-1244`; set from input at 1234,1239, from autopilot curve error at `ROAD.C:965-967`, cleared with input on reset/stop. Dashboard reads it (`DRIVE.C:135-146`); squeal hysteresis reads magnitude (`RACE.C:1378-1382`). |
| 6 | unused1[4] | No reader/writer; preserve four char bytes. |
| 10 | tireFailureIndex | Road index beyond which the tire fails; 0 means no scheduled failure. Reset calculates a random location in the second half of the course based on tire wear (`RACE.C:927-934`); `ra_tick` compares the player's road index and sets `g_failureTire` (`RACE.C:1376-1377`). Rename to tireFailureRoadIndex. |
| 12 | tireSlip | Nonnegative additive lateral-drift factor: table values 0,1,2,4,5,6,8,11,14 from tire variant and wear band (`RACE.C:632,925-926`). Only read in curve drift computation (`ROAD.C:1037`), not a boolean or tire kind. |
| 14 | rpm | Engine revolutions/minute; idle initialized at 1500; generated curves end at 5700. Updated by throttle/coast, shifts and braking (`SYS.C:1160-1230`), town cruise (`DRVTICK.C:109-119`), rival simulation (`RACE.C:959-971,1029-1045,1101-1133`). Also drives tachometer (`DRIVE.C:88-143`) and sound (`SYS.C:1021-1023`). |
| 16 | minRpm | Cached initial low-band lower bound; only assigned from neutral low band on reset (`RACE.C:920`), never read. Do not claim an actively maintained lower limit. |
| 18 | maxRpm | Cached band upper RPM limit; initialized at reset (`RACE.C:919`), initialized/refreshed for rival bands (`RACE.C:969,1104-1108,1130`). The player does not refresh it in ISR. |
| 20 | accelStep | RPM added per simulation update; reset from neutral low band (`RACE.C:917`), refreshed by player band selection (`SYS.C:1217`) and rival initialization/gear-fit/band changes (`RACE.C:970,1044,1108,1131`); read by throttle and auto-driving (`SYS.C:1120,1161`) and rival acceleration (`RACE.C:1101`). Cleared on stop (`SYS.C:991`). |
| 22 | decelStep | RPM removed per coasting update; reset (`RACE.C:918`), player band refresh (`SYS.C:1218`), coasting test/subtract (`SYS.C:1171-1173`), stop clear (`SYS.C:991`). |
| 24 | gearIndex | Neutral slot 0 or forward gear 1..4. Reset 0 (`RACE.C:908,957`), town begins at 1 and increments (`DRVTICK.C:79,117`), fit starts at 1 (`RACE.C:1029`), rival shifts at 1113, ISR shifts/downshifts/neutral at `SYS.C:1192-1207`. Used for curves/ratio indexing and shifter/status display (`DASH.C:635-640,RACE.C:1374-1375`). Give it enum DriveGear. |
| 26 | shifterDirty | Boolean: constructor, town setup/shifts and ISR shifts set 1 (`PERF.C:87,DRVTICK.C:78,116,SYS.C:1206`), dashboard clears 0 (`DASH.C:660`). Drive loop tests it for redraw (`DRIVE.C:136`); ISR also uses it to refresh acceleration caches (`SYS.C:1215`). Use BOOL, TRUE/FALSE. |
| 28 | gearCount | Actually highest forward gear index, either third or fourth (`PERF.C:20,86`). All bounds compare current gear against this value (`SYS.C:1125-1194,RACE.C:1038,1110-1115,DRVTICK.C:115`); shifter layout consumes it (`DRIVE.C:80,DASH.C:596`). Rename topGear; enum DriveGear. |
| 30 | shiftRpm | Engine-specific recommended shift threshold from existing enum EngineShiftRpm (`PERF.C:19,83`); reset adjusts automatic second-gear band boundary (`RACE.C:911`), rival shift target subtracts driver penalty (`RACE.C:971,1116`). Retype using that enum. |
| 32 | automatic | Boolean result of transmission-variant comparison (`PERF.C:78`). Read by reset/outcome and player transmission/limiter logic (`RACE.C:676,910,SYS.C:1132,1162,1178,1186,1196`). Use BOOL; retain comparisons as truth tests. |
| 34 | autoShiftRpm | Automatic upshift RPM, assigned from final gear middle-band upper bound only for automatic transmission (`PERF.C:81`); player and rival read it under automatic selector checks (`SYS.C:1179,RACE.C:1111`). Same proven values as enum EngineShiftRpm. |
| 36 | autoSelector | Existing enum AutoSelector: manual sentinel, automatic neutral or drive. Constructor's comparison yields manual/neutral (`PERF.C:78`); reset/outcome select neutral (`RACE.C:912,677`); town selects drive (`DRVTICK.C:80-81`); ISR toggles/selects neutral (`SYS.C:1198-1203`). Dashboard and status map it to indicator positions (`DASH.C:635-640,RACE.C:1371-1373`); rival uses nonzero as automatic (`RACE.C:971,1111,1114`). |
| 38 | unused2 | Written -1 by automatic constructor/reset (`PERF.C:80,RACE.C:914`), never read; preserve name and int size. |
| 40 | unused3 | Same as unused2. |
| 42 | initialRpmPerSpeed | Only read on reset into rpmPerSpeed (`RACE.C:916`); no writer anywhere in source. Communal startup leaves it zero. Rename resetRpmPerSpeed to describe the observed use without implying constructor initialization. |
| 44 | unused4[8] | No reader/writer; preserve eight char bytes. |
| 52 | gearRpmPerSpeed[6] | Entries neutral through fourth are RPM per mph (`PERF.C:60-64`); consumers index by gear in shifts and gear fitting (`SYS.C:1207,RACE.C:958,1030,1119`). **Sixth slot is not a gear ratio:** `PERF.C:84-85` writes a rank/tire calculation, and no reader uses that slot. Preserve six-word member size; name its five-gear prefix with existing DRIVE_GEAR_SLOTS and document the final unread entry. |
| 64 | rpmPerSpeed | Active RPM-per-mph conversion; reset copies unwritten reset field, rival overrides immediately, shifts/gear-fit select ratio (`RACE.C:916,958,1030,1119,SYS.C:1207`). Used in speed/RPM conversions and guarded before player division (`SYS.C:1229-1230`); rival divisions use selected nonzero ratio. |
| 66 | rpmBand | Low/middle/high curve index, 0..2. Reset 0; rival selects/increments bands (`RACE.C:961-970,1033-1040,1105,1121-1129`), ISR scans and assigns (`SYS.C:1211-1218`), low-band truth tests control automatic/manual shifts (`SYS.C:1181,1192-1194`). Give it enum RpmBand, and type rival's g_rpmBand accordingly; ISR's i also holds wear so stays int. |
| 68 | gear[5] | Five 24-byte Gear records, three eight-byte Range records each; constructed at `PERF.C:60-76`. Neutral curve is a free-rev curve. Reset reads low-neutral and modifies automatic second gear (`RACE.C:911,917-920`); ISR and rival select by gear/band (`SYS.C:1213,RACE.C:961,1029-1040,1064,1107,1121`). Retain embedded types; replace dimension/index literals with their proven counts/enumerators. |
| 188 | stopFlags | OR mask using existing STOP_* definitions in RACE.H, zero when clear. Reset clears (`RACE.C:909`); ISR sets engine/transmission/roadside loss (`SYS.C:1157,1167,1188,1246`); race tick/movement sets loss/police/collision (`RACE.C:1214-1241,1402-1432`); staging adds jump-start (`DRVTICK.C:44`); outcome adds tire flag (`RACE.C:679`). Drive step tests/returns combined masks, outcome reads exact reasons (`DRVTICK.C:40-63,RACE.C:660-843`). Keep int, document flag refs rather than pretending mutually exclusive enum values. |
| 190 | unused5 | Cleared on reset (`RACE.C:909`), receives elapsedTicks when stopping (`SYS.C:992`), never read. Preserve spare name, describe write-only stop-time snapshot. |
| 192 | elapsedTicks | Simulation-update count; reset 0 (`RACE.C:909`), ISR increments (`SYS.C:1137`). Statistics divide by RACE_TICKS_PER_SECOND (~6.069/s) and speed accumulator (`RACE.C:568-569`), rival catches up from delta (`RACE.C:1061-1065`), stop snapshots it (`SYS.C:992`). Not raw BIOS ticks. |

## Behavior and edge cases

1. Construct gear curves and transmission configuration; reset runtime fields before the trip.
2. Race ISR updates the player's input, RPM, selected gear, band acceleration/deceleration, speed and steering. Road-frame updates consume speed/steering/tire drift, while dashboard updates consume speed/RPM/deflection/dirty flag.
3. Rival simulation consumes the player's elapsed-tick delta, updates the rival's speed/RPM/gear/band, and reads cached upper RPM/acceleration. Town cruising uses separate gear-ratio and RPM-step tables rather than the generated ratios.
4. Failures, finishing and police/collision logic accumulate stop flags; stop clears player rates/steering and snapshots ticks. Race outcome examines flags and modifies persistent car parts/cash; it does not save the driving record.

- Neutral's generated ratio uses a 0.2-mph denominator fallback, but player reset starts with the unwritten reset ratio (zero). Do not invent a default ratio assignment.
- Top gear is third/fourth, while the curve/ratio tables always reserve all five gear slots; the unused fourth-gear entry for a three-speed transmission still gets generated.
- The rival's global band selector can lag its member after a band increment (`RACE.C:1105` only updates the member, whereas 1064 reads the global at entry); preserve behavior, do not silently synchronize it.
- Tire failure threshold uses strict roadIndex > threshold, not >=. Tire failure does not itself immediately set STOP_TIRES; outcome adds it later.
- Spare fields, cached lower bound, reset ratio, sixth ratio-array word and stop snapshot are not evidence of a live additional feature. Original intended roles remain unknown where no consumer exists.

## Other misleading identifiers (outside scope)

- `fit_gear(int rpm, ...)` actually receives **mph** (`RACE.C:1031` multiplies it by RPM-per-mph; callers at 1073,1088,1099,1294,1302,1306,1354 pass speeds). The name of its parameter is misleading; retained outside this type investigation.
- `ra_speed_step(int rpm)` also receives mph (`RACE.C:1209,1229`) and returns road-distance step; parameter name is misleading.
- `g_autoTrans` is a separate dashboard/town flag derived from current transmission (`CRUISE.C:32`), not a replacement for the record's automatic flag.
- `ra_reset_eng` has global input/race/pace side effects and uses player tires for both instances; its name understates that scope.
- `DASH.C:580-582` calls the shifter artwork a car/wheel sprite; the code and its slot/knob updates show a gear-selector display. Retained for the separate type/function work.

## Planned replacements

- Struct tag Eng -> DrivingState, all declarations, definitions, prototypes and forward declarations.
- tireFailureIndex -> tireFailureRoadIndex; gearCount -> topGear; initialRpmPerSpeed -> resetRpmPerSpeed, all accesses to this type.
- BOOL fields automatic/shifterDirty; literal dirty writes become TRUE/FALSE, boolean comparison uses truth test.
- Existing EngineShiftRpm for shiftRpm and autoShiftRpm. New DriveGear and RpmBand enums, reusing existing DRIVE_GEAR_SLOTS and RPM_BAND_COUNT; apply every proven raw gear/band index, bound, assignment and table dimension in PERF, RACE, SYS, DASH and DRVTICK. Do not retype unrelated mixed-use locals or mixed display selector variables.
- Doxygen file/type/member docs in PERF.H and docs on its supporting selector/gear/band value sets. No unrelated function/global renames, logic changes, new storage or layout changes.

## Verification

Initial pre-edit status; completed results are appended under Final validation.

## Implemented replacement inventory

- `PERF.H`: renamed the type and three members listed above, documented every member, used BOOL for both integer flags, reused EngineShiftRpm for both thresholds, typed current/top gear as DriveGear and current band as RpmBand. Kept Range/Gear names, all spare names and every member's footprint. Gear/band array sizes use the existing count names.
- `PERF.C`: constructor pointer tags/member accesses; gear/band index constants and loop variables; first-gear tire checks; deceleration denominator uses the gear-slot count; sixth ratio-array write uses the gear-slot count as its index. Gear-indexed table dimensions use the count. g_transGears holds DriveGear; g_shiftRpm holds EngineShiftRpm. Shifter dirty assignment uses TRUE. The comparison producing automatic/selector remains a comparison, preserving its simultaneous 0/1 result.
- `RACE.C`: reset/rival/gear-fit pointer tags and member names; g_rpmBand uses RpmBand; all explicit low/middle/high and neutral/first/second gear constants in reset, rival init, band/gear fitting and shifts are named. The reset's mixed speed/gear/band/steering chained zero assignment remains intact; its one zero initializes scalar state as well as the two zero-valued enums. No extra assignments or ordering changes were introduced.
- `SYS.C`: instance tag, topGear references, neutral/first gear tests and assignments, low/high band comparisons and band-scan initial sentinel (RPM_BAND_LOW - 1), TRUE dirty assignment and a truth test for automatic. Reset-ratio member name follows the one reader in RACE.C.
- `DRVTICK.C`: instance tag, topGear, first-gear assignment, TRUE dirty writes and gear-indexed cruise-table dimensions/neutral index.
- `DASH.C/H`: DrivingState pointer tag, FALSE dirty clear, neutral comparison, third-gear layout test. da_init_shifter's existing parameters now carry AutoSelector and DriveGear because its sole caller directly passes these members; parameter/function names and behavior are unchanged. DASH.H includes PERF.H for those enum definitions.
- `DRIVE.C`, `DRVTICK.H`, `SYS.H`, `RACE.H`: direct member/tag users and forward declarations updated. `sr headers` regenerated C includes and recursive MAKEFILE dependencies.
- `RACE.H`: documented the eight **existing** STOP_* flags so the target's references resolve; no values or flag definitions added. PERF.H and RACE.H have Doxygen file blocks. The recipe book's single current `struct Eng g_driveEng` reference now uses DrivingState.

The existing RPM_BAND_COUNT and DRIVE_GEAR_SLOTS names were initially macros. The data-shape parser resolves enum constants but not preprocessor macros, so retaining them as macro array bounds failed the layout audit with `ValueError: DRIVE_GEAR_SLOTS + 1`. They are now the count enumerators of RpmBand and DriveGear, with the same names and values and no duplicate definitions. This also fulfills the named-value-set count requirement without a tooling change.

## Final validation

- `./sr headers --check`: includes/dependencies current; all declarations owned.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 still match (551 MATCH, 21 MATCH*).
- Shape comparison against the saved pre-edit type: all 29 member offsets, sizes, alignment and array counts preserved; 194-byte total, alignment 2.
- `nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'`: 160 types, 714 real-compiler sizeof/offsetof checks, zero mismatches.
- Clean `rm -rf work/layout && ./sr build`: 96/96 objects pass, 572/572 mapped functions at original positions and raw-byte exact, 748/748 near-call encodings exact; packed EXE IDENTICAL to orig/SR.EXE. The separate unpacked-file diagnostic reports header/relocation-order and trailing-length differences; it does not change the exact packed-EXE result.
- `./sr dgroup check`: every initialized global defined by its original owning unit.
- `./sr dgroup order`: src/SR.LNK agrees with recovered object order.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`: 106 tests pass.
- `./dosbox.sh --make`: DOS-native SR.EXE IDENTICAL to orig/SR.EXE (SHA-1 3a5e28b795313028b7e77d147122f41cb29de334).

No runtime behavior was changed. Remaining semantic unknowns are the intended historical purposes of the unwritten reset-ratio word, unread sixth ratio-array word, unused fields and write-only caches; observed behavior is fully mapped above.
