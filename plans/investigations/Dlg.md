# Dlg: gas-station visit state

## Purpose and scope

`struct Dlg` holds one visit's car-rendering inputs, saved fuel-cap backgrounds,
held-tool cursor and refuelling state. Rename the type to `GasStationVisit`:
there is no general dialog interface here. Only this record and its member uses
are in scope; other questionable identifiers are reported below.

Evidence pins below refer to the pre-edit source on branch `inv-dlg`; the
RACE.C include addition shifts later source lines by one in the final version.

## Storage, construction and lifetime

- The only instance is automatic `dl` in `ra_gas_station`
  (`src/GAME/RACE.C:237-275`); two file-private helpers receive its address:
  `show_tank_cap` and `leave_station` (lines 177 and 202). A tree-wide search
  finds no other declarations, casts, readers, writers or serialized instances.
- The visit copies the current car's specification index and computed body
  overlays, then captures two screen regions using `pu_grab` (lines 254-264).
  The bitmap headers/pixels live in the high arena, not inside the record:
  `src/UI/POPUP.C:28-52` allocates at `ARENA_HIGH_STACK` when its last argument
  is zero. Both snapshots survive successive cap/nozzle redraws.
- Every named field is explicitly initialized; no disk/table loader supplies
  the record. It is never saved. Persistent effects are changes to the current
  car's fuel and the player's money, not serialization of this local state.
- Cap redraw allocates a picture and a work bitmap temporarily, copies the
  selected background, overlays the cap/nozzle, and pops those two allocations
  (`RACE.C:177-199`). Exit drives the car away, clears arena marks, and returns
  to the garage (`RACE.C:202-233`; `src/SYS/MARKS.C:6-10`). The local and both
  saved-background pointers must not outlive that visit.

## Fixed layout and every member

MSC medium-model layout: 28 bytes, word alignment, in this exact order. Offsets
are relative to the record, not original addresses. Both bitmap pointers are
four-byte far pointers. All other members occupy two bytes, including GasHand.
The real-compiler data-shape audit and byte gates verify this after editing.

| Offset | Existing member / final name | Role, values, all readers and writers |
| --- | --- | --- |
| 0 | `specIndex` (keep) | Copy of `g_driver.currentCar->specIndex` at RACE.C:254. Passed to `cp_show_car` on arrival (256) and departure (229). It selects the shared car-specification/model tables (`src/GAME/CARPIC.C:439`, 474); it is a table index, not an independent closed state set. |
| 2 | `bodyMods` (keep) | Bit mask returned by `gm_car_look` (RACE.C:255), consumed by the same arrival/departure rendering calls (256, 229). `src/GAME/GAME.C:574-587` proves the four existing `BodyMod` flag constants: roof, front bumper, rear bumper and large engine. Use the existing int-sized `enum BodyMod` for the combinations and reference its existing flag constants in the member docs; the size and signedness remain those of int under MSC 5.1. |
| 4 | `bm1` -> `capBgNozzleOnPump` | Snapshot of the cap rectangle with the nozzle on the pump (RACE.C:260); selected only by `show_tank_cap` when `nozzleOnPump` is true (182-183). The rectangle may overlap the pump, so its pump pixels matter too. |
| 8 | `bm2` -> `capBgNozzleOffPump` | Snapshot of that same rectangle after drawing `g_pumpNoNozzle` (RACE.C:262-263); selected when the nozzle is off the pump (184-185). Pump restored immediately after capture (264). |
| 12 | `hand` (keep enum GasHand) | Starts empty (252); switches to nozzle when picked up or removed from the tank (321, 355, 387), to rag when picked up (337), back to empty when put away/in tank (326, 345, 366, 397). Every assignment also chooses the cursor except initial assignment; empty cursor is selected immediately after it (253). Timeout tests rag (287); picking tools/cap tests empty (319, 336, 354, 380); exit restores empty cursor if needed (207). `src/UI/CURSOR.C:99-103` proves these are cursor-sheet indices as well as tool state. The decoded cursor sheet confirms empty hand, nozzle and rag in slots 1, 2, 3. Reuse the existing enum; no count is needed because nothing loops over this set. |
| 14 | `fillsLeft` (keep int) | `(FUEL_FILL_COUNT_BASE - fuel) / FUEL_FILL_AMOUNT` on entry (275); decremented once per pumping timeout (290), tested for exhaustion at 294 and 392 and positivity at 388. Each fill adds `FUEL_FILL_AMOUNT` (291). `GAME.H:67` defines ten fuel units per gallon, so these are three-gallon increments, not gallons or animation ticks. The normal entry-fuel range 0..FUEL_TANK_CAPACITY yields 0..6; this is an arithmetic count, not an enum. |
| 16 | `attendantAbsent` -> `selfService` (int -> BOOL) | TRUE initially (272), FALSE after full-service animation/message (315). Read at 299 to leave the player in control after pumping exhausts fuel/money, and at 220 to restore the attendant image before departure. The attendant is visibly seated on the initial station background, not absent. The flag selects self-service versus attendant takeover, including automatic departure at exhaustion. |
| 18 | `nozzleInTank` (keep; int -> BOOL) | FALSE initially (270), TRUE when the attendant/player inserts it (325, 396), FALSE when the player removes it (386). Only exit reads it (208), restoring the cap and possibly charging the nozzle penalty (209-213). |
| 20 | `nozzleOnPump` (keep; int -> BOOL) | TRUE initially (271) and on replacement (368), FALSE when picked up by attendant/player (322, 357). Selects the appropriate cap background (182) and whether exit restores the pump and may charge a penalty (215-218). It is independent of `nozzleInTank`: off-pump includes held and inserted states. |
| 22 | `unused` (keep int) | Written 0 at 273 and 1 on full-service takeover at 312. Never read, including by helpers. Preserve this spare member's name, size, order and writes; the historical intended use cannot be inferred. |
| 24 | `nozzlePenaltyDue` -> `chargeNozzlePenalty` (int -> BOOL) | TRUE initially (274). Cleared on a charged tank-exit penalty (212), full-service takeover (318), and the special leave-after-returning-nozzle path (363). Read only in exit (210, 217). Gates one `NOZZLE_PENALTY` charge when the nozzle was left in the tank/off the pump, clamped to available cash. Clearing after the first test prevents a double charge. It is permission to charge, not a record that a charge already happened. |
| 26 | `pumping` (keep; int -> BOOL) | FALSE initially (267), TRUE when attendant/player starts pumping (329, 404), FALSE at shared tail reached after pump handling or self-service exhaustion (375). Only timeout reads it (289); it gates fuel increments and payment. Rag-in-hand bypasses timeout handling before this test (287-288). |

## Visit behaviour and caller context

The only external entry is `src/GAME/GAME.C:844-857`: the garage rejects fuel at
or above `REFUEL_VISIT_LIMIT` and zero cash, then cruises to the station before
calling `ra_gas_station`. `src/GAME/CRUISE.C:24-28` consumes `TOWN_TRIP_FUEL`
with a floor of zero. Thus valid entry fuel is below the refuel threshold and
`fillsLeft` starts positive (normally 1..6), independently of the later exit
clamp. The preconditions explain why the zero-at-entry case below is unreachable
through normal routing.

1. `ra_gas_station` sets up the station, brings the current car in, saves the
   backgrounds and initializes the record. `cp_show_car` constructs the two
   pump strips before those captures (`CARPIC.C:463-466`, 576-592).
2. Idle hotspots permit taking a rag or nozzle. Rag mode changes the cursor,
   replaces the rag/bucket region with a background patch, and enables window
   hotspots; the two window cases themselves are empty (RACE.C:410-413).
3. Taking the nozzle removes it from the pump and enables pump/cap hotspots.
   Inserting it into the cap switches the hand to empty, starts animation 2 and
   pump sound, and enables timed fuel delivery. Removing it stops those effects
   and restores a held nozzle. Hotspots control which combinations are reachable
   (`RACE.C:111-174`, 333-405).
4. Pumping timeouts add one fill, debit the current price (`pos`, initially 1),
   and stop on exhaustion of count or funds. During self-service the nozzle
   remains for the player to return; during attendant service the visit ends.
5. After 40 idle timeouts, animation 3 makes the attendant stand, the full-service
   message appears and the price doubles. `CARPIC.C:55-67` plus decoded resource
   1233 confirms that animation. If any fuel was already delivered (`done`), the
   visit immediately ends; otherwise penalties are disabled and the attendant
   inserts the nozzle and starts pumping.
6. `leave_station` restores cap/pump, charges at most one nozzle penalty,
   restores the attendant if necessary, clamps fuel to tank capacity and uses
   the saved car-rendering inputs to drive away. No code mutates those inputs
   after entry.

## Side effects and edge cases

- Persistent state: `g_driver.currentCar->fuel`, `g_driver.money` through
  `sc_add_money`; scene changes from station to garage and the cruise transition
  runs. Rendering also touches the video pages, cursor, arena marks, pump
  animation/sound, keyboard map stack and station hotspot/action state.
- `nozzlePenaltyDue` can be true while the nozzle is safely on the pump; it is
  tested only together with an unreturned nozzle. Conversely attendant service
  can have nozzle-in-tank true and penalty-due false. Do not collapse these flags
  into a single nozzle enum: their combinations carry different information.
- `fillsLeft == 0` is checked after decrement, not before timeout delivery. A
  malformed/full-tank entry followed by attendant takeover could therefore run
  the count below zero. Preserve the arithmetic and tests; normal game routing
  is a separate caller invariant, not an assumed clamp inside this type.
- Fuel can exceed tank capacity during a final increment and is clamped on exit.
  Payment/penalty debits never intentionally exceed available funds.
- The rag case bypasses the inactivity countdown, so holding it postpones the
  attendant. The leave hotspot only displays an insult; it is not an exit.
- No unresolved member meaning remains except the unread spare's original
  intended purpose. No claim is made about original 1989 identifier spellings.

## Other misleading names (outside scope; unchanged)

- `PIC_FUEL_GAUGE` (1139, `src/SYS/DISK.H:158`) is the scenery patch covering the
  rag/bucket pickup area, not a fuel gauge. `RACE.C:338` displays it at (80,170)
  when taking the rag, and line 346 restores the captured region when replacing
  it. Decoded 1090/1139 show the same pavement/faucet/bucket region.
- `label` in `ra_gas_station` (RACE.C:239, 259, 346) is the rag/bucket background,
  not text. `label` in `show_tank_cap` (179, 187-198) is a cap/nozzle image, also
  not a text label. `pos` (266, 293, 311) is dollars per fuel increment, not a
  position; `done` (269, 292, 316, 362) means some fuel was delivered, not that
  the visit is over. These function-local names are outside this type task.

## Planned replacement inventory (written before source edits)

- Type tag `Dlg` -> `GasStationVisit`: declaration, two helper forward
  declarations, their definitions and the one automatic instance, all in
  RACE.H/RACE.C. No global/function rename and no metadata change.
- The two bitmap members and their four uses become the explicit snapshot names
  in the member table above. The service-mode and penalty-enable flags become
  `selfService` and `chargeNozzlePenalty`, including every reader/writer listed
  above. Other accurate member names are retained.
- The body-overlay mask uses the existing `enum BodyMod`, with its defining
  GAME.H dependency; no new enum or duplicate constants are introduced.
- Five flag members become BOOL; their 0/1 stores and explicit zero comparisons
  use FALSE/TRUE. Truth tests and control-flow shape remain unchanged.
- Two cursor selections spelling the empty-hand state as `cu_ctl(1)` (RACE.C:206,
  253) reuse `GAS_HAND_EMPTY`. All assignments/comparisons of `hand` already use
  the enum. The unread `unused` writes remain numeric and its type stays int.
- Add focused Doxygen for the type/members and its existing GasHand enum, with
  a header file brief. Reuse BodyMod constants in docs; no new value set,
  duplicate constants, unrelated type changes or comments on functions.

## Verification

All gates below passed on the final source:

- `./sr headers --check`: include blocks and MAKEFILE dependencies up to date.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'`:
  158 types, 712 sizeof/offsetof checks, zero mismatches. This includes the
  renamed 28-byte record, both far pointers and the int-sized enum/BOOL members.
- `./sr check --all`: 572/572 match (551 MATCH, 21 MATCH*); unchanged function
  sizes and references, including the gas-station helpers and main visit loop.
- `rm -rf work/layout && ./sr build`: 96/96 object gates pass, 572/572 mapped
  game functions and 748/748 near-call encodings exact; packed EXE IDENTICAL.
- `./sr dgroup check` and `./sr dgroup order`: ownership and link order pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests, OK.
- `./dosbox.sh --make`: DOS-native EXE IDENTICAL.
- Original, clean packed build and DOS-native build all have SHA-1
  `3a5e28b795313028b7e77d147122f41cb29de334`.
- `git -c core.whitespace=cr-at-eol diff --check`: clean. The MAKEFILE keeps its
  required DOS CRLF endings and generated recursive header dependencies.

No code-generation workaround, behavior change, reordered storage, new value set,
global/function rename or unrelated type edit was needed. The unread spare
remains `int unused` with both original writes. Investigation notes are retained
both here and under the ignored work/investigations directory.
