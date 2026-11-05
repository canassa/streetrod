# Car investigation

## Purpose and scope

A mutable car instance holds its model identity, accounted value, installed
parts, assembly state, tuning, fuel, cosmetics and owned-car list link.
Keep the accurate type name `Car`. This note was written before source edits;
line pins below refer to the starting revision. Only Car members and their
immediate value/size contracts are changed. Other record investigations stay
outside this branch.

The one member rename is `rank` -> `stockRank`: it is a cached model ordinal,
not the car's computed performance with its current parts. Other names remain
accurate once their units, ownership and encodings are documented. Scoped
member edits are appropriate: sr rename supports globals/functions/files,
not record members (see the existing Spec investigation and sr-cli skill).

## Storage and persistence

- `POOLS.C:4,15-22` owns 16 fixed Node2 records, chains their free list, and
  marks the model-index word -1. Node2's a/b/raw carData/next view is exactly
  the Car layout: value at 0, model index at 2, opaque bytes 4..37, next at 38.
  `GAME.C:274-294` pops/pushes these nodes, updates carCount, and clears next
  on allocation. `gm_make_car` casts a popped node to Car (`GAME.C:320`).
- `SCREEN.H:16-30` supplies currentCar, otherCars and freeCars roots. The
  current car is kept separately from the otherCars chain. Its next can be
  stale/nonzero: switching assigns it before detaching (`GAME.C:630-639`),
  so it is not a reliable current-car sentinel.
- Purchases (`GAME.C:501`) and pink-slip wins (`RACE.C:696`) allocate cars.
  Sales/scrapping (`GAME.C:408-424,720-726`) return parts and car nodes to
  their pools. Rival hood previews build a stack Car and stack Node objects
  and pass them to gm_make_car for reuse (`BOBS.C:711-729`); these do not
  allocate pool entries or enter owned lists.
- PERF.C:89-118,124-166 builds partial stack cars from specifications for
  performance calculations. Such temporaries are not persistent owned cars.
- SAVEGAME.C:208-268 writes each 40-byte Car verbatim, followed by its
  transmission, five engine-rack slots, and tires as 8-byte Node chains.
  Reading allocates new nodes, replaces installed-part pointers, copies the
  raw Car, and reconstructs the otherCars links. Saved pointers are used as
  presence/continuation markers, not dereferenced as live addresses.
  The current-car flag stops traversal after one record even if next is nonzero.
- sg_save/sg_load write/read the current car, other cars, then spare parts
  (`SAVEGAME.C:337-340,464-467`). No Car instances are loaded from HOT_DATA:
  that file supplies immutable CarSpec/Model records (`DISK.C:68-82,502-527`).
- All pointers in the Car record are two-byte near data pointers. The fixed
  layout is 40 bytes, word aligned; every member's size, signedness and order
  must remain unchanged, including the unused byte.

## Members

| Offset | Declaration before editing | Meaning, value set, readers and writers |
| --- | --- | --- |
| 0..1 | int value | Accounted car value in dollars, initially the supplied purchase/base price (`GAME.C:322,501`, `RACE.C:696`). ga_install/remove_part add/subtract installed-part values (`GARAGE.C:650-666`); failures remove destroyed parts' values (`RACE.C:427,603,622`). Selling bases its offers and maximum price on this value (`GAME.C:649-701`), and junk compensation uses it (`RACE.C:873`). Scrapping subtracts transmission/rack values, discards tires separately, and clamps only the returned remainder (`GAME.C:408-424`). This is neither a recomputed market price nor the immutable model price; retain `value`. |
| 2..3 | int specIndex | Zero-based index into g_carData and parallel g_models/layout tables. gm_make_car writes its idx (`GAME.C:321`); PERF temporaries set n (`PERF.C:101,145`). Readers use model names, body pictures, modification availability, stock fit bits and rank (`GAME.C:614-618`, `GARAGE.C:323-337,465,842,1127-1150`, `PERF.C:47-54,85,171-178`, `BOBS.C:284,467`, `TIRES.C:195`). Normal indices 0..25; -1 in freed Node2 is an allocator marker, not an active Car value. Keep int, no duplicate model enum. |
| 4 | char paintScheme | Body-colour pair, existing PaintScheme. New/reused cars choose a valid scheme (`GAME.C:325-328`), advert purchase overrides it (`502`), painting changes it (`GARAGE.C:408-433`) and won cars inherit the rival's (`RACE.C:706`). ga_palette and garage/town drawing read it (`GAME.C:551`, `GARAGE.C:324`, `CRUISE.C:80`, `SAVEGAME.C:355`). Keep char; enum would widen it. |
| 5 | unsigned char rank | Cached g_carData[idx].rank written only by gm_make_car (`GAME.C:324`). Compared only when a pink-slip car is won: a higher ordinal becomes currentCar (`RACE.C:707-711`). PERF reads the spec's rank instead, proving this is not mutable upgrade performance. Numeric ordinal 0..8, higher better; retain unsigned byte and rename stockRank. Ordinal scale, not categorical CarClass. |
| 6..7 | struct Node *transmission | Installed transmission Node or null. Constructor selects stock variant and clears Node.next (`GAME.C:331-338`); ga_transmission installs/removes it (`GARAGE.C:806-898`). ga_ready requires it for driving (`924`), PERF/CRUISE read its variant (`PERF.C:39-46`, `CRUISE.C:32`); race wear/failure/replacement and scrapping read/remove it (`RACE.C:622-624,665-666`, `GAME.C:414`). Save/load follows it. |
| 8..17 | struct Rack rack | Five installed engine-assembly pointers: engine, manifold, then up to three carburetors. Constructor chooses stock equipment and nulls unused carb slots (`GAME.C:339-371`); ga_hood installs/removes and traverses them (`GARAGE.C:1038-1054,1125-1208,1312`); ready checks required carb count (`913-920`). PERF computes performance from slots 0,1,2 (`PERF.C:38-45,164-178`), gx_draw_parts uses manifold variant (`GARGFX.C:536`), car info/garage preview mirror the slots (`GARAGE.C:335,728`), and damage/scrap/save/load traverse all five (`RACE.C:602-605`, `GAME.C:415-417`, `SAVEGAME.C:216-219,246-249`). Keep embedded Rack rather than modifying its separate type investigation. |
| 18..19 | struct Node *tires | Installed tire-set Node or null after removal. Stock selection (`GAME.C:346-351`), garage replacement (`GARAGE.C:711-714`), race failure/replacement (`RACE.C:427-441`), wear (`697,1012-1020`), readiness (`GARAGE.C:926`), PERF handling (`PERF.C:40,46,85`) and car rendering read/write it. One Node describes the set, not one wheel. |
| 20..21 | char transmissionBolts[2] | Two assembly tightness states, existing BoltTightness. Constructor sets both tight (`GAME.C:374-375`); garage tightens 1->2->3 and loosens 3->2->absent, removing the transmission when both absent (`GARAGE.C:799-900`). Ready requires tight (`929`), drop_transmission clears both (`RACE.C:624-625`); gx_trans_draw renders each (`GARGFX.C:164-177`). Preserve bytes, use existing TRANSMISSION_BOLT_COUNT. |
| 22..30 | char engineBolts[9] | Assembly states in BoltTightness: first bolt secures engine, next two manifold, remaining pairs each carburetor. Constructor sets required bolts and clears unused carb pairs (`GAME.C:352-375`). The first five g_nBolt entries {1,2,2,2,2} and cumulative selection prove grouping (`GARAGE.C:753-760,1230-1269`); installation/removal/tightening updates state (`1197-1203,1239-1293`), ready requires exactly 3+2*carbCount tight bolts (`918-920`), engine failure clears all (`RACE.C:606-607`). Preserve bytes, use ENGINE_BOLT_COUNT. |
| 31 | char wireState | Existing WireState: disconnected -1, absent 0, connected +1. Constructor connects (`GAME.C:329`); garage toggles sign, disconnects newly fitted engines and clears removed/failed engines (`GARAGE.C:1035-1079,1130,1221,1299-1304`); blow_engine clears (`RACE.C:608`). ga_ready requires positive (`GARAGE.C:913`). Plain char is signed in this build; preserve exact declaration. |
| 32 | signed char timing | Signed ignition-timing setting, not degrees. Existing TIMING_MIN..TIMING_MAX (-8..4); random initial/assembly tuning uses TIMING_SETTING_COUNT (`GAME.C:330`, `GARAGE.C:1208,1320`), tuning clamps and advances/retards (`GARAGE.C:966-990`), race driving can worsen it (`RACE.C:1002-1010`). PERF rewards zero and penalizes absolute deviation (`PERF.C:54,178`); stock temporaries choose zero (`100,144`). Keep numeric signed-byte scale, not an enum of invented physical angles. |
| 33 | char unused | No explicit member reads/writes in Car users. Raw pool contents, whole-record assignment and save/load preserve it; do not remove it or promise zero initialization. |
| 34..35 | int fuel | Fuel remaining in tenths of a gallon: info divides by FUEL_UNITS_PER_GALLON (`GARAGE.C:774`). Constructor uses NEW_CAR_FUEL (`GAME.C:323`); town travel and race outcomes subtract saturating at zero (`CRUISE.C:25-28`, `RACE.C:681-685`). Gas station adds FUEL_FILL_AMOUNT and caps on departure to FUEL_TANK_CAPACITY (`RACE.C:226-227,275-291`). Admission thresholds in GAME.C:834-844 and BOBS.C:524-568. Temporarily can exceed the final tank cap while pumping; do not claim every stored value is capped. |
| 36..37 | unsigned appearance | Packed cosmetics. Sticker field uses APPEARANCE_STICKER_MASK; roof/front/rear flags mean roof chopped / bumper removed (GARAGE.C:475-516 prompt strings and XORs). Drawing maps them to body-overlay bits (`GAME.C:579-588`, `GARAGE.C:330-333`, `TIRES.C:197`); PERF uses them as speed bonuses (`PERF.C:47-54,171-178`), BOBS betting reads sticker bits (`480-483`). Constructor clears the whole word through masks (`GAME.C:376-380`); paint clears sticker (`GARAGE.C:432`), sticker selection writes it (`581-582`), won-car copying fills fields (`RACE.C:698-705`). Low byte has no meaning established; constructors clear it, masks preserve it. Keep unsigned word and document existing flag constants, not BodyMod's different encoding. |
| 38..39 | struct Car *next | Next other-owned car, or free-list link through Node2. Constructors insert into otherCars (`GAME.C:382-387`); car switching/selling splice links (`630-639,722-725`); won-car promotion splices (`RACE.C:708-710`). SAVEGAME reconstructs list links but leaves current-car next untouched when flag stops traversal (`SAVEGAME.C:253-268`). May be stale on the current car; no blanket promise that every active next is null or another active car. |

## Value sets, changes and replacement plan

- Reuse PaintScheme, WireState and BoltTightness. Char fields cannot become
  enums; no Car member is an int boolean. Timing/fuel/value/rank/index are
  numeric scales, not newly invented categories.
- Complete BoltTightness with the observed partly-tight state 2 and a state
  count for the bolt-picture lookup. Document existing values and WireState.
  The initializers in INPUTDAT.ASM already name the four pictures correctly.
- Replace raw bolt-state 1 comparisons in ga_transmission/ga_hood, both
  g_boltPics[1] reads, and race bolt clears with BOLT_INSERTED/BOLT_ABSENT.
  gx_trans_bolt's frame is directly a BoltTightness, so type it accordingly
  and replace its frame==1. Leave the independent UI direction flags
  (0 absent, 1 tightening, 2 loosening) unchanged: those are not bolt states.
  Reused key/picture/bolt locals cannot take a single bolt enum type.
- Replace Car array lengths and direct full-bolt loop bounds with existing
  counts, preserving relational forms. Bind g_boltPics' extern bound to the
  completed enum count. Use existing STICKER_INDEX_MASK in TIRES.C.
- Retain the separate Rack and Node2 types and names. No other struct members,
  function names, globals or locals are renamed.

## Edge cases and open questions

- Pool allocation assumes availability; save readers trust stored markers and
  do not validate member ranges or cycles. No behavioral repair is in scope.
- PERF's stock temporaries leave appearance uninitialized even though its
  performance formulas read it; this existing behavior stays unchanged.
- Car value updates are not a universal conservation law: V6 uses a special
  gm_part_value, failures use raw gm_take_part values, and scrap discards tires
  without subtracting them. Do not equate value with a simple sum of parts.
- Current-car next is not reliably null. The save current-car flag is essential.
- The low appearance byte and unused byte have no established gameplay meaning.
  Original intent cannot be determined from the available readers/writers.
- No other member representation or gameplay role remains unresolved.

## Other misleading identifiers outside scope

- Node2.a/b/carData (`POOLS.H:19-23`) are the allocation view of Car's value,
  specIndex and remaining payload; b==-1 means a free car, not an unrelated
  field (`POOLS.C:18`, `GAME.C:291`). Leave to its own investigation.
- Rack sounds like garage spare-part storage, but its five pointers are the
  installed engine/manifold/carburetor assembly (`GAME.C:339-371`). It is
  distinct from g_driver.spareParts (`GARAGE.C:650-666`). Leave type naming alone.
- gx_trans_bolt's old comment calls its source a highlighted bar/frame
  (`GARGFX.C:40-41`), but frame is the actual stored bolt tightness state
  (`GARAGE.C:852-898`). Function/comment rewriting is outside this type task.
- pf_vmax's comment describes a generic rating, but its result is clamped
  estimated MPH (`PERF.C:126,182-188`) and ga_info prints it as mph
  (`GARAGE.C:773`). No function rename here.

## Validation

Final replacement inventory:

- Car.rank -> stockRank at its declaration, gm_make_car's assignment, and
  both sides of ra_outcome's won-car comparison. All other rank fields remain
  unchanged.
- Documented all fifteen Car members, the record, BoltTightness/WireState,
  the four appearance masks/flags referenced by the member, and fuel units.
- Added BOLT_PARTLY_TIGHT and BOLT_TIGHTNESS_COUNT to the existing enum.
  INPUTDAT.H's g_boltPics bound uses the count; its ASM initializer was already
  four named picture entries and required no edit.
- ga_transmission and ga_hood use BOLT_INSERTED for both stored-state
  comparisons and both initial bolt-picture subscripts. Race damage clears
  use BOLT_ABSENT. gx_trans_bolt's parameter/prototype use BoltTightness and
  its inserted-state comparison uses BOLT_INSERTED. Mixed-purpose locals
  remain int; UI direction flags keep their distinct numeric meanings.
- All raw wire-state comparisons against zero now use WIRES_ABSENT
  (ga_ready and both ga_hood sign tests). Negation still toggles connection.
- Both Car bolt-array bounds, ga_transmission's full loops, ga_ready's
  transmission loop, ga_hood's opening bolt loop, both race damage loops,
  and gx_trans_draw's loop use the existing size constants. No new duplicate
  size constants or enums were introduced.
- TIRES.C's last raw packed-sticker mask now uses STICKER_INDEX_MASK.
- GARGFX.H includes the enum's owner GAME.H; sr headers updated recursive
  MAKEFILE dependencies. The MAKEFILE retains its required CRLF endings.

Required gates on the final source:

- ./sr headers --check and ./sr format --check: clean.
- ./sr docs: zero diagnostics.
- ./sr check --all: 572/572 (551 MATCH, 21 MATCH*).
- rm -rf work/layout && ./sr build: 96/96 object gates, all 572 functions
  raw-byte exact at original positions, 748/748 exact near calls, and
  Packed EXE vs orig/SR.EXE: IDENTICAL.
- nix-shell shell.nix --run "python3 -m unittest discover -s tests":
  106 tests passed.
- ./sr dgroup check and ./sr dgroup order: passed.
- ./dosbox.sh --make: DOS-native SR.EXE IDENTICAL; SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334, matching orig/SR.EXE.

The build also prints its known unpacked-reference header/relocation-order
and 14-byte image-length diagnostics (see the earlier Spec investigation).
It reports zero differing load-image bytes; the packed-file comparison is
IDENTICAL. No weaker comparison was substituted.

### Direct layout audit and tool limitation

The generic tools/scripts/check_data_types.py audit fails on
TRANSMISSION_BOLT_COUNT: cdata.Types collects enum constants but not
preprocessor #define values. This is a tooling limitation rather than a
compiler/layout failure. The required build, check and DGROUP workflows do
not need to evaluate Car's member arrays through that parser.

A direct MSC 5.1 probe compiled with /AM /Oas /Gs /FPi through dosbox.Job
instead emitted 33 unsigned results: sizeof Car (40), sizeof Node2 (40),
plain-char signedness, and sizeof plus offsetof for each of the fifteen
members. Parsed the OBJ _DATA words with tools.sr.omf.Obj and compared with
the member table above: 33/33 correct, zero mismatches. It did not use the
unsupported parser or change the game declarations to get a pass. Each
probe expression has this C form (all fields in the table were checked):

```c
#include "GAME.H"
#define offsetof(t, m) ((unsigned)&(((t *)0)->m))
unsigned carShape[] = {
    sizeof(struct Car), sizeof(struct Node2), (char)-1 < 0,
    offsetof(struct Car, value), sizeof(((struct Car *)0)->value),
    /* repeat offsetof/sizeof for each remaining member in declaration order */
    offsetof(struct Car, next), sizeof(((struct Car *)0)->next)
};
```

Expected offset/size pairs are
(0,2), (2,2), (4,1), (5,1), (6,2), (8,10), (18,2), (20,2), (22,9),
(31,1), (32,1), (33,1), (34,2), (36,2), (38,2). Char signedness is true.
All types, signedness, offsets, widths and the spare byte are preserved.
