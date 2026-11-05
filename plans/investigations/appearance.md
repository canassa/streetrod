# Car appearance and bumper naming investigation

## Purpose and scope

Name every packed-appearance shift/mask operation and make the bumper names
agree with the menu, saved bits and visible art, without changing game bytes.
This evidence and replacement plan were recorded before changing source names.
Source line pins in the evidence refer to the starting `fix-appearance` tree.

## Storage and data flow

`Car.appearance` is an unsigned word at offsets 36..37 of the 40-byte saved
car (`GAME.H:142-165`; serialization in `SAVEGAME.C:198-250`). The low byte
has no established gameplay meaning. `gm_make_car` clears it, the sticker
field and the three modifications in five separate AND operations
(`GAME.C:376-380`). Keep those operations and the unsigned type unchanged.

| Packed field | Position | Unshifted mask | Existing packed mask/flag |
| --- | --- | --- | --- |
| Sticker | 8 | `STICKER_INDEX_MASK` (31) | `APPEARANCE_STICKER_MASK` (0x1f00) |
| Chopped roof | 13 | 1 | `APPEARANCE_ROOF` (0x2000) |
| Removed rear bumper | 14 | 1 | 0x4000, incorrectly named `APPEARANCE_FRONT_BUMPER` |
| Removed front bumper | 15 | 1 | 0x8000, incorrectly named `APPEARANCE_REAR_BUMPER` |

- Seven reads shift the sticker field for car rendering: `GAME.C:709`,
  `GARAGE.C:300,330,523`, `RACE.C:229,257`, `TIRES.C:197`.
- `ga_menu` also extracts all three one-bit modifications into the matching
  `Look` bit fields (`GARAGE.C:331-333`; `GARAGE.H:85-91`). Those last two
  field names are reversed, just like the appearance flags.
- Painting clears the sticker field (`GARAGE.C:432`). Sticker purchase masks
  the selected index, shifts it and preserves every other bit (`581-582`).
- Winning a car copies a rival's sticker and three BodyMod flags into the
  packed fields, preserving unrelated bits at each insertion
  (`RACE.C:699-706`). Retain the existing `!= 0 & mask` expression shape.
- `ga_body` toggles one modification after confirmation, charges cash and
  elapsed time, and redraws the car (`GARAGE.C:453-534`).
- `gm_car_look` converts the three packed modification flags to BodyMod and
  derives the large-engine flag from the installed engine (`GAME.C:574-587`).
- Rivals hold BodyMod directly in a byte (`BOBS.H:113`); initialization and
  later upgrades set roof/bumper/engine flags (`SAVEGAME.C:63-80`,
  `BOBS.C:354-362`). `GasStationVisit.bodyMods` is already enum-sized (`RACE.H:83`).
- `cp_show_car` passes those flags to `cp_build` (`CARPIC.C:409,474`);
  `TIRES.C:196-205` calls `cp_build` with `gm_car_look`'s result directly.
  `cp_build` has four remaining raw BodyMod tests, for 2, 1, 4 and 8
  (`CARPIC.C:311-324`). The stock advert preview already uses the engine
  constant (`GAME.C:533-535`). No other raw nonzero BodyMod uses remain in
  the callers, writers or readers found across `src/`.
- `pf_drivetrain` and `pf_vmax` use the packed flags and model availability to
  add performance bonuses (`PERF.C:51-55,176-180`). Bob's wager calculation
  tests the packed sticker mask and its low bit (`BOBS.C:480-485`); reuse
  `BET_STICKER_LOW_BIT`, which has a distinct betting purpose.

## Bumper evidence chain

The menu is the reliable starting point, not the old member names:

1. `MESSAGES.ASM:79-80` contains the actual strings `Front bumper` and
   `Rear bumper`; `MESSAGES.H:77-78` names their text offsets correctly.
2. `g_gar` binds the rear text to decoded result 11 and front text to 12
   (`HOTSPOT.C:122-123`). `KEYMAPS.H:71-72` agrees, and `K_GARAGE` binds B to
   the rear result and F to the front result (`KEYMAPS.ASM:204-205`). These
   names and numeric results must stay as they are.
3. The rear action passes value 2 to `ga_body`, while the front action passes
   value 1 (`GAME.C:925-930`). The names of those BodyMod constants are wrong;
   changing the dispatch's numeric values would change the original game.
4. Value 2 checks Model offset 6 and toggles appearance 0x4000. Value 1 checks
   Model offset 8 and toggles appearance 0x8000 (`GARAGE.C:480-498,513-516`).
   Both use the same generic strip/restore bumper messages for $15, and both
   reject a zero patch with the already-lost-bumper message. The messages
   prove removal/restoration but do not distinguish front from rear.
5. `cp_build` draws Model offset 6 at `(0, Spr12 offset 7)` for flag 2 and
   Model offset 8 at `(Spr12 offset 8, Spr12 offset 9)` for flag 1
   (`CARPIC.C:311-314`). Those Model and Spr12 names are therefore hypotheses
   to check against the resource pixels.
6. Models are ten-byte immutable records loaded from `HOT_DATA` offsets
   656..915; layouts start at 916 (`DISK.C:47-50,68-82`, `di_load_data`).
   Decoding `<bB4H` confirms model 0's patches are 1037/1033 and model 3's
   are 1050/1049. Layout 0 gives left Y=40, right X=224/Y=32; layout 3 gives
   left Y=26, right X=192/Y=32.
7. Viewed the actual PNG output from `./sr pic`, not just the picture names:
   all the inspected car bodies face right (hood/grille on the right,
   trunk/tail lights on the left). 1037 replaces the coupe's left rear corner;
   1033 replaces its right front corner. 1050/1049 do the same for the 1956
   Corvette; 1058/1180 do the same for the Dodge Lancer. Thus Model offset 6
   and BodyMod 2 mean **rear**, while Model offset 8 and BodyMod 1 mean
   **front**. The patch picture constants are reversed too.

Reproducible first image command (body, rear patch, front patch for each car):

```sh
./sr pic 1039 1037 1033 1048 1050 1049 1060 1058 1180 \
  --scale 4 --png work/appearance/bumper-patches.png
```

Also viewed every remaining nonzero bumper patch beside its body with
`./sr pic` in three sheets (`work/appearance/patches-{a,b,c}.png`). The
complete resource inventory below records the inspected ID pairs, so the
conclusion does not depend on the misleading PictureId spellings or on one car.
The 1932 Fords and the King have zero bumper patches, so no picture renames.

| Model index | Car | Rear patch (Model +6) | Front patch (Model +8) |
| --- | --- | --- | --- |
| 0 | Chevrolet 1940 coupe | 1037 | 1033 |
| 1 | Chevrolet 1940 roadster | 1041 | 1042 |
| 2 | Chevrolet 1938 Master | 1045 | 1044 |
| 3 | Corvette 1956 | 1050 | 1049 |
| 4 | Corvette 1961 | 1052 | 1051 |
| 5 | Corvette 1963 | 1055 | 1056 |
| 6 | Dodge 1955 Lancer | 1058 | 1180 |
| 8 | Dodge 1962 Polara | 1063 | 1064 |
| 9 | Plymouth 1962 Savoy | 1114 | 1115 |
| 11 | Ford 1940 Deluxe | 1120 | 1121 |
| 12 | Ford 1955 Fairlane | 1123 | 1124 |
| 13 | Ford 1957 Fairlane | 1128 | 1129 |
| 14 | Mercury 1954 Monterey | 1132 | 1133 |
| 15 | Mercury 1956 Custom | 1136 | 1143 |
| 16 | Chevrolet 1949 Styleline | 1146 | 1147 |
| 17 | Chevrolet 1958 Impala | 1150 | 1151 |
| 18 | Oldsmobile 1949 88 | 1153 | 1154 |
| 19 | Oldsmobile 1952 88 | 1158 | 1159 |
| 20 | Pontiac 1950 Silver Streak | 1161 | 1162 |
| 21 | Thunderbird 1957 | 1167 | 1166 |
| 22 | Valiant 1961 V200 | 1168 | 1169 |
| 23 | Valiant 1963 V100 | 1173 | 1174 |
| 24 | Chevrolet 1955 Bel Air | 1176 | 1177 |

## Reuse search and planned replacement inventory

Searched all source headers/INC files for appearance, sticker, BodyMod,
bumper names and numeric masks before creating constants. Reuse the existing
packed masks, clearing masks, `STICKER_INDEX_MASK` and BodyMod enum. Add only
missing field shifts and the three field-specific unshifted one-bit masks,
following `SPEC_*_SHIFT` / `SPEC_*_MASK` in `DISK.H:305-322`.

- Add `APPEARANCE_STICKER_SHIFT`, `APPEARANCE_ROOF_SHIFT`,
  `APPEARANCE_REAR_BUMPER_SHIFT`, `APPEARANCE_FRONT_BUMPER_SHIFT` (8/13/14/15)
  and matching roof/rear/front `_MASK` constants (1 each) in `GAME.H`.
- Replace all seven raw sticker shifts, both sticker insertions, all three
  modification extractions and all three modification insertions with the
  corresponding shift/mask names (`GAME.C`, `GARAGE.C`, `RACE.C`, `TIRES.C`).
  Packed/clearing masks are already named; preserve and reuse them.
- Swap the names of `BODY_FRONT_BUMPER` / `BODY_REAR_BUMPER` everywhere they
  are defined or used, keeping values 1/2 at their original sites. Replace
  `cp_build`'s four raw BodyMod tests with the corrected enum constants.
- Swap the appearance bumper flag and clearing-mask names at all definitions
  and uses, preserving the original mask values. Result: rear=0x4000,
  front=0x8000, keep-rear=0xbfff, keep-front=0x7fff.
- Swap `Model.frontBumperRemovalPic` / `rearBumperRemovalPic` in `DISK.H`,
  `CARPIC.C`, `GARAGE.C`, `BOBS.C`, `SAVEGAME.C` and both PERF calculations.
- Swap `Spr12.frontBumperY` / `rearBumperY`; rename the old `rearBumperX` to
  `frontBumperX` (the rear patch always uses X=0). Keep all byte offsets.
- Swap `Look.front` / `rear` in `GARAGE.H` and the two extraction assignments.
- Swap `FRONT_BUMPER_SPEED_BONUS` / `REAR_BUMPER_SPEED_BONUS` in `PERF.H` and
  both readers: the existing 0.05 bonus belongs to the front, 0.01 to the rear.
- Swap FRONT/REAR in all 46 side-view bumper patch PictureIds in `DISK.H`
  and their preload entries in `MEM.C`. Retain each spelling's existing
  `_BUMPER` suffix, where present; do not rename unrelated rear-view sprites.
- Correct adjacent bumper documentation. Add a correction to the earlier
  Model/Car notes; their historical line pins and starting-name inventories
  are not current semantic evidence. Do not rewrite past diary entries.

## Edge cases and limits

No gameplay changes, layout/type-size changes, or new validation are intended.
Unsupported patch IDs stay zero; existing capability checks and null-check
ordering stay unchanged. Save-game bits, key results, menu strings, costs,
performance amounts, resource IDs, and draw coordinates all stay byte-identical.
The low appearance byte and historical identifier spellings remain unknown.
No other misleading identifier needs to be renamed to establish this chain.

## Validation

All gates passed after the planned replacements:

- `./sr headers --check`: include blocks and MAKEFILE dependencies current.
- `./sr format --check`: all 157 C/header files checked.
- `./sr docs`: zero Doxygen diagnostics.
- `./sr check --all`: 572/572 functions match (551 MATCH, 21 MATCH*).
- `./sr diff cp_build`: 1506/1506 bytes match, 57 references verified;
  the local frame layout is unchanged.
- `rm -rf work/layout` followed by `./sr build`: 96/96 object gates pass,
  572/572 mapped functions raw-byte exact at original positions, 748/748
  near-call encodings exact, no unresolved symbols, and packed EXE identical
  to `orig/SR.EXE`. As in the earlier Model gate, the separate unpacked-file
  diagnostic reports header/relocation ordering differences; the packed
  executable comparison is IDENTICAL.
- `./sr dgroup check`: every initialized global remains in its owning unit.
- `./sr dgroup order`: `src/SR.LNK` agrees with recovered object order.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  all 106 tests pass.
- `./dosbox.sh --make`: DOS-native EXE identical to `orig/SR.EXE`, SHA-1
  `3a5e28b795313028b7e77d147122f41cb29de334` for both.
- Final source search finds no raw numeric shifts in appearance reads/writes,
  no raw masks at modification extraction/insertion sites, and no raw
  BodyMod bit tests. All clearing masks and packed flags remain named.

No behavioral changes or blockers remain. The stored bits, resource values,
record layouts, numerical bonuses and rendered pixels are unchanged.
