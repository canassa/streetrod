# Car paint schemes

## Purpose and storage

A paint scheme selects a dark/bright body-colour pair from GARAGE.C's six-row
`g_paint` table; it is independent of the palette registers receiving that pair.

Investigation recorded before changing identifiers. Evidence locations below refer
to the original source lines on branch `paint`.

| Index | Meaning | RGBI pair |
| --- | --- | --- |
| 0 | Red | 4, 12 |
| 1 | Cyan | 3, 11 |
| 2 | Green | 2, 10 |
| 3 | Magenta | 5, 13 |
| 4 | Blue | 1, 9 |
| 5 | White (light gray / bright white) | 7, 15 |

The pairs are literal data in src/GAME/GARAGE.C:269, confirmed by
`./sr data 50c2 12`. The colour interpretation agrees with RGB16 in
tools/sr/pics.py:11-16. No existing paint enum or RGBI colour constants exist in
headers/INC files (`git grep` PAINT, RGBI, EGA colour names). Reuse the existing
PaletteSlot enum from src/VIDEO/VIDEO.H:6, extending it for the rival highlight.
The table's two column indices also form a value set: base and highlight.

* Car.paintScheme (src/GAME/GAME.H:124): one char at offset 4 in the 40-byte
  owned-car record. Never widen or change signedness. write_cars/read_cars in
  SAVEGAME.C:208-267 serialize the full record. The POOLS.H Node2 storage is
  another view of the same allocation, not another paint index.
* Racer.paintScheme (src/GAME/BOBS.H:109): signed char at offset 10 in an
  18-byte record. DISK.C:69-82 and di_load_data:502-527 load the first 396 bytes
  of HOT_DATA straight into 22 records. Inspecting orig/freeware/HOT_DATA shows
  -1 for entries 0..20 and red for the King (entry 21). init_racers generates
  schemes for the 21 ordinary entries and leaves the King's assigned car alone
  (SAVEGAME.C:84-117). Save/load preserves all 396 bytes (339,466).
* CarDisplay.paintScheme (src/GAME/GARAGE.H:73): int at offset 10 in the
  30-byte display state, copied from the selected rival (BOBS.C:366) or the
  player's current car (GARAGE.C:324). This member can use the enum directly.

## Producers, copies, and callers

* gm_make_car (GAME.C:314-328) rolls rn_int(6), accepting any nonwhite result;
  for white it accepts on rn_int(2)==1, otherwise rolls again. Under uniform
  draws each nonwhite scheme has probability 2/11, white 1/11. The `i` local
  subsequently indexes engine bolts and manifold choices (351 onward), so it
  must remain int rather than become a paint-only enum.
* sg_init_racer (SAVEGAME.C:52-70) uses the identical rejection rule; its `i`
  subsequently indexes a specification list and must also remain int.
* buy_from_ads (GAME.C:427-551) uses a char `color` local, initially red, then
  reset to -1 for each listed item. Preview assigns rn_int(5) once; purchasing
  preserves that preview or independently draws from those same five nonwhite
  schemes. Its byte-sized local must stay char. Purchasing first calls
  gm_make_car, then overrides the generated scheme; do not alter random calls.
* ra_outcome (RACE.C:706) overwrites a won car's generated paint with the
  selected rival's paint. select_rival and ga_menu copy into CarDisplay as above.
* cr_drive (CRUISE.C:71-81) saves the displayed rival scheme in int `saved`,
  applies the player's scheme, then applies the saved scheme to rival slots
  during a race. `saved` can be enum-typed; its role is saved rival paint.
* sg_load (SAVEGAME.C:354) and buy_from_ads (GAME.C:548) restore the current
  car's palette after loading or leaving the adverts.
* cop_appears (RACE.C:1355) explicitly selects scheme 5 (white), independently
  of the cop record's paint byte.

## Palette operations and side effects

* ga_palette (GARAGE.C:166-176): parameter `a` is a scheme index, not an RGBI
  colour. On EGA/Tandy it sends both table columns to slots 6/7 if the display
  is enabled, and always updates the corresponding g_palette entries. Other
  video modes do nothing. vi_palette_reg (VIDEO.C:355-371) updates g_palette
  too, then programs BIOS palette registers; the explicit final stores also
  handle a disabled display. No range validation exists.
* ga_paint (GARAGE.C:393-451): bm1 is the saved popup bitmap; x/y are its
  position; key is the decoded menu action; idx4 is the candidate scheme.
  The candidate starts at the current car, cycles backward 0->5 or forward
  5->0, and previews the base column through slot 12. Commit requires $20,
  removes the sticker, updates the car's paint and player palette, restores
  slot 12 to RGBI magenta, charges TIME_PAINT and $20, and returns. Cancel
  restores the popup/slot without changing paint. No car and unsuitable video
  modes show their existing messages. Failed payment leaves paint unchanged.
* cr_rival_paint (CRUISE.C:42-67): `mode` is a scheme or the special restore
  request -1. char a/b are the base/highlight RGBI output values, not schemes.
  EGA/Tandy map the pair to slots 12/3. The first non-restore call saves the
  previous g_palette entries into g_savedPal12/g_savedPal3; further recolours
  retain that first snapshot (including the police substitution). Restore
  consumes it and resets the cache to -1. BOBS.C:1069 requests restoration
  after cr_drive returns. Hardware writes depend on g_displayEnabled, cache
  writes do not. Other video modes do nothing. A restore without a preceding
  recolour would write the cache sentinel; no new validation is in scope.

## Replacement list

Planned replacements, to be verified in the final diff:

1. Define enum PaintScheme beside the owning g_paint/CarDisplay declarations:
   unassigned sentinel, six named colours, scheme count, nonwhite draw count.
2. Define enum PaintShade for base/highlight column indices and column count;
   replace every g_paint column subscript and both dimensions (including extern).
3. Type CarDisplay.paintScheme, ga_palette's parameter, cr_rival_paint's
   parameter, ga_paint's candidate, and cr_drive's saved rival scheme as
   enum PaintScheme. Document the unchanged char members with an enum reference.
   Keep mixed-purpose i locals and byte-sized colour storage at their sizes.
4. Replace paint-related rn_int(6), ==5, rn_int(5), -1, initial red, and the
   paint-menu wrap bounds across GAME.C, SAVEGAME.C, GARAGE.C, RACE.C.
   The independent rn_int(5) calls in GAME.C:136-137 (newspaper choices) and
   BOBS.C:622 (refusal messages) are unrelated and must remain as they are.
5. Keep the RIVAL_PAINT_RESTORE interface name: it is a command sentinel,
   numerically the same as an unassigned scheme, never a table row.
6. Reuse PaletteSlot for paint register accesses; extend it with the rival
   secondary slot. RGBI literals in table cells and slot-12 restoration are
   hardware colours, not paint scheme indices, and remain unchanged.
7. Retain the accurate ga_palette, ga_paint, cr_rival_paint and g_paint names.
   Rename only their paint-related parameters/locals to express their roles.

## Edge cases, limitations, and other identifiers

No new behaviour or validation: loaded bytes remain trusted, -1 must not index
`g_paint`, and all save-game layouts remain fixed. Colour names denote the RGBI
pairs, not undocumented in-game paint marketing names. No claim about original
1989 identifier spellings is possible.

Out-of-scope misleading identifiers: `used` in buy_from_ads actually marks the
parts section (GAME.C:467-469), not used cars; Node2's `a`, `b` and `carData`
are an allocator's overlaid car storage (POOLS.H:19-24 and GAME.C:277-293).
Neither needs renaming for the paint value set.

## Validation

All required gates passed:

* `./sr headers --check` and `./sr format --check`.
* `./sr docs`: zero diagnostics.
* `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
* Clean `rm -rf work/layout && ./sr build`: 96/96 object gates;
  572/572 functions at original positions, 748/748 near calls exact;
  `Packed EXE vs orig/SR.EXE: IDENTICAL`.
* `./dosbox.sh --make`: `DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL`.
* `./sr dgroup check` and `./sr dgroup order`.
* `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests, OK (including the new parser regressions).
* Real MSC data audit: 156 types, 710 sizeof/offsetof checks, zero mismatches.
* `./sr diff ga_paint`: no byte differences; 32 references verified.

The remaining palette-register literals 6/7 in SYS.C:423-424 animate the
copy-protection palette, not car paint; assigning car-role names there would
misdescribe the operation.

## Applied changes and tooling finding

All seven replacement groups above are applied. The paint-menu locals are now
`popup` (saved bitmap) and enum-typed `paint`; ga_palette and cr_rival_paint take
`scheme`; cr_rival_paint's char outputs are `primary`/`highlight`; cr_drive keeps
an enum-typed `savedPaint`. The ga_paint diff confirms the original frame slots:
popup=-4, x=-6, key=-8, y=-10, paint=-12, and all 431 bytes match.

`RIVAL_PAINT_RESTORE` now aliases `PAINT_UNASSIGNED`, retaining the command name
and value. This does not make the sentinel a valid table row. The unchanged
RGBI cache sentinels in CRUISE.C are not paint-scheme values.

The first DGROUP check exposed a pre-existing tooling limitation: cdata.number
accepted only literal arithmetic, so an enum-named dimension raised ValueError.
The data-shape reader now collects enum expressions from all indexed headers,
resolves names lazily (including implicit increments and unary signs), and
rejects unknown/cyclic constants and unsupported expressions. Regression tests
cover multidimensional bounds, aliases across headers, negative/implicit values,
field offsets, and errors. No game-source workaround or guessed size was used.
This finding is also recorded in CLAUDE.md and the sr-cli skill.
