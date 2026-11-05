# Model investigation

## Purpose

`Model` supplies a car specification's on-road opponent sprite set and the
side-view picture patches for chopping its roof, stripping its bumpers and
showing a large engine.

This note was written before changing identifiers. Source pins below refer to
the original lines on branch `inv-model`; the rename inventory records the new
spellings. The type name remains accurate in its car-model context.

## Storage, loading and saving

- `src/SYS/DISK.C:47` allocates `g_models[26]` as communal storage. It has no C
  initializer. `g_dataBlocks[2]` points at it and `g_dataSizes[2]` is 260
  (`DISK.C:69-82`). Thus each entry occupies ten bytes.
- `di_load_data` reads all thirteen blocks sequentially from `hot_data`, checks
  the exact number of bytes read, closes the handle, then loads the hall of
  fame (`DISK.C:502-526`). The first two blocks consume 396 + 260 bytes, so the
  model block is at file offsets 656..915 inclusive. This is the sole writer:
  no game code assigns any member of a model record.
- Decode `orig/freeware/HOT_DATA` with little-endian `<bB4H` from offset
  `656 + 10*i`. All 26 spare bytes are zero. The first byte is always 0..4;
  its signed-char declaration must remain signed and one byte wide.
- The model index is shared with `g_carData`, `g_layoutAutoSprArray` and
  `g_pointsModelService` (`CARPIC.C:297-300`). The description-text block begins
  at file offset 1904; the name offsets in the specification block identify
  the rows in the table below. The final row is the King's car.
- Models are immutable game resources, not save-game state. `sg_save` writes
  driver state, cars, parts, racer records, clocks and road layout
  (`SAVEGAME.C:464-469`); `sg_load` reads the same (`SAVEGAME.C:337-342`).
  Car serialization writes the 40-byte `Car` and its parts, not a `Model`
  (`SAVEGAME.C:198-250`). Saving preserves car specification indices and body
  modification flags, which refer back to the loaded model data.

## Fixed layout and per-member evidence

| Offset | Original member and type | Meaning, values and evidence |
| --- | --- | --- |
| 0 | `kind`, signed char | Row of `g_opponentPics`, read only by `vw_load` (`VIEW.C:253-266`). Rows 0..4 select King/Corvette, sedan, coupe, hot rod and sports car sets (`VIEW.C:61-66`), not engine types or model indices. |
| 1 | `unused`, char | Loaded spare byte; all zero in `HOT_DATA`. No member access anywhere in the source. Keep it, with no invented meaning. |
| 2 | `roofPic`, enum PictureId | Picture replacing the upper body with a chopped roof; zero means roof chopping is unavailable. `ga_body` checks it before offering chop/rebuild (`GARAGE.C:470-478`), and `cp_build` copies it at `(roofX, 0)` when body bit 4 is set (`CARPIC.C:315-320`). |
| 4 | `engineOverlayPic`, enum PictureId | Large-engine picture drawn at `(b5, b6)` when body bit 8 is set, except model 25 (`CARPIC.C:323-324`). The loaded ordinary-model values are 1182..1185; the King has zero. |
| 6 | `frontBumperPic`, enum PictureId | Replacement patch showing the front bumper removed; zero means it cannot be stripped. Availability check in `GARAGE.C:480-488`; copied at `(0, frontBumperY)` for body bit 2 (`CARPIC.C:311-312`). |
| 8 | `rearBumperPic`, enum PictureId | Replacement patch showing the rear bumper removed; zero means it cannot be stripped. Availability check in `GARAGE.C:490-498`; copied at `(rearBumperX, rearBumperY)` for body bit 1 (`CARPIC.C:313-314`). |

The four enum fields are already word-sized picture IDs. Keep their types,
order, signedness and total record size. In particular, the first member cannot
be changed to an int-sized enum. Its documentation will refer to the new enum;
the word-sized local carrying it in `vw_load` can use that enum.

## All readers and behavior

1. `vw_load` uses `kind` only while racing, loading four pictures from its
   selected row: passing, angled rear, rear, and distant rear. It derives
   smaller frames with `sr_ega`, `sr_tandy` or `sr_cga` (`VIEW.C:252-277`).
   The first three pictures of some sets are shared; the row identifies a
   whole set rather than a unique picture. The King's distinct set is loaded
   through the same path. No source assigns raw kind values: they arrive in
   the resource file.
2. `cp_build` uses a `Model *m2`, alongside the matching layout and service
   records, to build the side-view car bitmap (`CARPIC.C:281-324`). It starts
   from the stock body, conditionally applies front, rear and roof patches,
   computes the roof's occupied top row for portrait positioning, then draws
   the optional large-engine overlay. The rest of the function draws decals,
   the driver and tires and creates shifted animation frames. Its callers are
   `cp_show_car` (`CARPIC.C:474`) and the tire-changing view (`TIRES.C:195-205`).
3. `ga_body` uses a `Model *p9` to refuse unsupported chop/strip operations,
   then toggles the player's appearance bits after confirmation
   (`GARAGE.C:465-498, 509-516`). It does not modify the model resource.
4. `sg_init_racer` allows initial random bumper modifications only when both
   bumper pictures are nonzero, and a roof modification only when its picture
   exists (`SAVEGAME.C:77-80`).
5. `select_rival` applies the same capability tests before granting a rival
   later modifications after enough wagered races (`BOBS.C:354-361`).
6. `pf_drivetrain` (`PERF.C:47-51`) and `pf_vmax` (`PERF.C:171-175`) use the
   roof/front/rear fields only as capability tests. Each speed contribution
   applies when the car has that modification OR the model has no such patch.
   A zero picture is therefore also a performance input, not just absent art.

`gm_car_look` maps player appearance bits to body flags and sets the large-engine
flag for `ENGINE_V8_LARGE` (`GAME.C:574-587`). Racer initialization sets that
flag from the stock engine (`SAVEGAME.C:74-75`). `bm_put_pic` loads a picture,
copies it through `bm_put` using copy mode 3, remembers it in `g_lastPic` and
pops its temporary arena allocation (`BITMAP.C:229-250`). `bm_draw_pic` instead
uses the masked drawing path (`BITMAP.C:255-267`). Resource inspection with
`./sr pic 1039 1038 1037 1033 1061 1062 1185 1184` confirms lower-roof and
bumper-deletion patches versus the original bodies, and the exposed-engine art.
Viewing rear pictures `1057 1011 1248 1250 1252` also confirms the five
sprite-set silhouettes and the King's distinct Corvette treatment.
Thus naming these fields as ordinary roof/bumper component pictures is
misleading: the pictures depict the modified body, not detachable stock parts.

## Loaded values

Picture values below are resource IDs; zero denotes an unavailable patch.
Kind meanings: 0 King, 1 sedan, 2 coupe, 3 hot rod, 4 sports car.
Every unused byte is zero.

| Index | Model | Kind | Roof | Engine | Front | Rear |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | 1940 Chevrolet coupe | 2 | 1038 | 1184 | 1037 | 1033 |
| 1 | 1940 Chevrolet roadster | 2 | 1043 | 1184 | 1041 | 1042 |
| 2 | 1938 Chevrolet Master Deluxe | 2 | 1046 | 1182 | 1045 | 1044 |
| 3 | 1956 Corvette | 4 | 0 | 1183 | 1050 | 1049 |
| 4 | 1961 Corvette | 4 | 0 | 1183 | 1052 | 1051 |
| 5 | 1963 Corvette | 4 | 0 | 1184 | 1055 | 1056 |
| 6 | 1955 Dodge Lancer | 2 | 1059 | 1184 | 1058 | 1180 |
| 7 | 1932 Ford deuce coupe | 3 | 1062 | 1185 | 0 | 0 |
| 8 | 1962 Dodge Polara | 1 | 1111 | 1183 | 1063 | 1064 |
| 9 | 1962 Plymouth Savoy | 1 | 1116 | 1184 | 1114 | 1115 |
| 10 | 1932 Ford sedan | 3 | 1117 | 1182 | 0 | 0 |
| 11 | 1940 Ford Deluxe | 2 | 1122 | 1185 | 1120 | 1121 |
| 12 | 1955 Ford Fairlane Victoria | 2 | 1125 | 1182 | 1123 | 1124 |
| 13 | 1957 Ford Fairlane 500 | 2 | 1130 | 1184 | 1128 | 1129 |
| 14 | 1954 Mercury Monterey | 2 | 1134 | 1184 | 1132 | 1133 |
| 15 | 1956 Mercury Custom | 2 | 1144 | 1183 | 1136 | 1143 |
| 16 | 1949 Chevrolet Styleline | 1 | 1148 | 1183 | 1146 | 1147 |
| 17 | 1958 Chevrolet Impala | 1 | 1152 | 1184 | 1150 | 1151 |
| 18 | 1949 Olds 88 | 2 | 1155 | 1184 | 1153 | 1154 |
| 19 | 1952 Olds 88 | 2 | 1160 | 1182 | 1158 | 1159 |
| 20 | 1950 Pontiac Silver Streak | 1 | 1163 | 1182 | 1161 | 1162 |
| 21 | 1957 T-Bird | 2 | 1235 | 1184 | 1167 | 1166 |
| 22 | 1961 Valiant V-200 | 1 | 1170 | 1183 | 1168 | 1169 |
| 23 | 1963 Valiant V-100 | 1 | 1175 | 1183 | 1173 | 1174 |
| 24 | 1955 Chevrolet Bel-Air | 2 | 1178 | 1185 | 1176 | 1177 |
| 25 | King's 1963 Corvette | 0 | 0 | 0 | 0 | 0 |

## Edge cases, side effects and open questions

- The three ordinary Corvettes cannot have their roofs chopped; the two 1932
  Fords have no strippable bumpers. The King has none of these patches, and
  `cp_build` explicitly suppresses its large-engine overlay.
- `cp_build` trusts its flags: it does not guard a zero picture ID before
  loading a roof or bumper patch. The player/rival availability tests normally
  prevent this. It also does not validate the specification index or sprite
  selector against the table bounds. No new guards belong in this investigation.
- `ga_body` forms its model pointer before checking for a null current car;
  that existing ordering remains unchanged.
- The record itself has no side effects after loading. Consumers allocate
  bitmaps, update render state, set appearance flags or compute performance.
- The spare byte has no proven meaning beyond unused loaded storage. Its
  being zero does not prove padding or a boolean; keep the named member.
- Why the artists chose specific shared engine pictures for each model, and
  the historical spelling of the type/members, are not recovered. Their runtime
  roles and every member's reader/writer are established.

## Rename and value-set replacement inventory

No existing header or INC enum/constant names the five sprite-set rows (searched
for race/opponent/sprite kinds and the `[5][4]` shape before adding anything).

- `Model.kind` -> `raceSpriteSet`, in `DISK.H` and its sole reader `VIEW.C`.
- Add `enum RaceSpriteSet` in the owning `DISK.H`: `RACE_SPRITES_KING`,
  `RACE_SPRITES_SEDAN`, `RACE_SPRITES_COUPE`, `RACE_SPRITES_HOT_ROD`,
  `RACE_SPRITES_SPORTS_CAR`, and `RACE_SPRITE_SET_COUNT`.
- Replace `g_opponentPics[5][4]` with the named sprite-set count. Table rows
  remain in resource-defined enum order; no runtime C assignments,
  comparisons, random ranges or wrap-arounds use raw sprite-set values.
- Type the existing `vw_load` local `idx` as `enum RaceSpriteSet`. Preserve its
  name and declaration position to keep the stack frame unchanged.
- `Model.roofPic` -> `choppedRoofPic`; `frontBumperPic` ->
  `frontBumperRemovalPic`; `rearBumperPic` -> `rearBumperRemovalPic`. Replace
  every use in `DISK.H`, `CARPIC.C`, `GARAGE.C`, `BOBS.C`, `SAVEGAME.C`, and
  both performance computations in `PERF.C`.
- Keep `engineOverlayPic`, documenting that it is for the large engine; keep
  `unused`. Add a brief and one short Doxygen description per member.
- Picture fields already use `enum PictureId`; do not create another picture
  enum or widen the byte-sized sprite selector. The zero capability sentinel
  is documented on each applicable member.

## Other misleading identifiers/comments outside this type's scope

- `Spr12.b5`/`b6` are the large-engine overlay's X/Y coordinates
  (`CARPIC.C:324`), not unexplained generic bytes. Leave their renaming for
  the layout-record investigation.
- `bm_put_pic`'s existing comment says "Blit to a bitmap fetched by index"
  (`BITMAP.C:241`), but its index selects the source, while `dst` is passed in.
- Existing per-car roof/front/rear `PictureId` names describe stock parts even
  though these resources replace body regions with chopped/stripped versions
  (e.g. `PIC_CHEVY_40_COUPE_ROOF`, `PIC_CHEVY_40_COUPE_FRONT_BUMPER`,
  `PIC_CHEVY_40_COUPE_REAR_BUMPER`; `DISK.H:78-83`, image inspection above).
  No resource-constant renames are needed to establish the member contracts.

## Validation

All checks passed after the source changes:

- `./sr headers --check`: include blocks and MAKEFILE dependencies current.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 functions match (551 MATCH, 21 MATCH*).
- `./sr diff vw_load`: 1369-byte function matches, all 130 references verified;
  the entire 12349-byte VIEW.C span matches. Local `idx` remains at `[bp-32]`.
- `nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'`:
  158 types, 712 sizeof/offsetof checks, zero mismatches. Model remains ten
  bytes, at member offsets 0, 1, 2, 4, 6 and 8.
- `rm -rf work/layout && ./sr build`: 96/96 object gates pass, 572/572 mapped
  functions raw-byte exact, 748/748 near-call encodings exact; packed EXE
  identical to `orig/SR.EXE`. The separate unpacked-file comparison reports
  header/relocation-order differences; it does not change the packed identity.
- `./sr dgroup check` and `./sr dgroup order`: ownership and link order pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests pass.
- `./dosbox.sh --make`: DOS-native packed EXE identical, SHA-1
  `3a5e28b795313028b7e77d147122f41cb29de334` (same as `orig/SR.EXE`).

No behavior, stored resource data or record layout changed. No blockers remain.
