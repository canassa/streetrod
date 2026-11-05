# Misleading identifier follow-up

## Purpose and evidence (recorded before source edits)

Make the car renderer's arguments, encounter state and UI data views describe
what their producers and consumers actually do, preserving every byte.

### Car pictures: cp_build and cp_show_car

`cp_build` (CARPIC.C:282-407) loads the body picture indexed through g_carData,
adds removal/chop/large-engine pictures from g_models, saves the sticker
background, draws the selected sticker and optional occupant portrait, then
creates shifted frame copies and underlays tire pictures and optional shadows.
Its output is an array of arena-low-stack bitmap pointers; g_featurePos and
sticker coordinates are side effects, and high-stack pictures are popped.

Arguments, in source order: out = output frame pointers; idx = car specification
index (not a body-picture ID); flags = BODY_* modifications; kk = one-based
sticker choice, zero for none; q = include the occupant portrait; ids = tire
picture per frame, or zero; w = include shadow; n = output frame count.
Evidence: CARPIC.C:298-325 (spec/model/body), 330-340 (sticker/portrait),
342-365 (frame shifts), 367-391 (tires), 392-406 (shadow). TIRES.C:205 calls
with no portrait, tires or shadow and one output, for the lifted car.

`cp_show_car` (CARPIC.C:410-609) centers the car on the screen and renders it
stationary or drives it through a saved background. Its args are animation,
spec index, body modifications, sticker, include portrait, bottom screen row,
use gas-station foreground mask, tire pictures. The gas-mask boolean chooses
PIC_GAS_STATION_MASK for cp_backdrop (489), not a graphics/video mode.
CARPIC.C:475 is its only cp_build call; it always requests a shadow and chooses
one frame for stationary rendering or four for motion. Callers: GAME.C:535,709
(advert preview/sale); GARAGE.C:300,344,347,528 (swap, arrival, edits);
RACE.C:229,258 (gas departure/arrival, mask enabled); BOBS.C:182,868 (rivals,
portrait enabled). Other calls disable portraits and gas masking. The selected
sticker is a packed-car field or Racer.sticker, and tire lists come from g_tirePics.

Rendering sequence: derive dimensions, clipped band, centered X and travel;
capture/reuse the scene background and arena storage; build frames, masks and
bands; update feature coordinates; draw stationary views or sp_setup/plan/step
the animation; save last rectangle; handle the fuel-cap/pump foreground;
set dashboard view and pages; possibly reuse the frames for immediate departure;
release storage and restore the cursor. Globals include the backdrop, feature
coordinates, sticker positions, sprite state and pass phase. The departure
phase deliberately reuses locals initialized before the backward `again` jump.
Zero tire pointers are accepted; roof opacity scanning supplies portrait Y
adjustment; stationary animation uses one frame and never indexes other frames.

### Encounter state and rival slot

CarDisplay.overlayPic (GARAGE.H:127) receives Racer.portraitPic from select_rival
(BOBS.C:369), is cleared for the player's parked car (GARAGE.C:329), and is
loaded only by cp_build's portrait branch (CARPIC.C:337). Rename portraitPic.

The initialized CARPIC global g_carPassBy takes exactly three states: ordinary
movement (0), arrive and continue through (1), and the reused-frame departure
(2). BOBS.C:377 requests phase 1; its King novice path writes a comparison
result (379). CARPIC.C:448,507,566 skips rebuilding/restarting sound in phase 2;
483 selects a full-screen backdrop for phase 1; 570 keeps sound running after
phase 1; 598-606 transitions 1 -> 2 -> 0. Use an enum and named values at every
write/comparison. animate_rival_car's wasPassBy remains a boolean snapshot;
make the nonordinary test explicit rather than storing an enum in BOOL.
No existing phase enum exists (headers/INC search).

BOBS.C:68 g_passCount is incremented for every departure at 187, including a
pass-through converted to CAR_DRIVE_OUT at 183-185. It gates carhop arrival at
80 and resets only on carhop departure at 101, so it persists across visits.
Rename departureCount and its CARHOP_MIN_PASSES threshold accordingly.

BOBS.C:342 assigns g_driverIdxSelVal from the selected Racer table slot; RACE.C
uses that slot for demo selection and regenerating the defeated rival. It is
neither a player configuration nor a car-spec index. Candidate g_racerActiveSlot
passes sr fit at bucket 135, original window 134..135, tie decided by full link.

RACE.C:674 clears g_resultWin and :691 sets it only in the opponent STOP_LOST
branch; show_race_stats and BOBS settlement consume player victory. Use BOOL,
TRUE/FALSE and the fitted name g_driverWonFlag: Driver is the player's existing
record. sr fit accepts bucket 52 in the single-bucket window 52..52.

RACER_COP_ENTRY is slot 10, which SAVEGAME.C:104-107 initializes among seven
ordinary middle-group rivals. RACE.C:1365 borrows that record's race behavior
for police pursuit; it does not give the record a cop identity. Name it
RACER_POLICE_BEHAVIOR_SLOT and document the borrowing.

### Popup and list record views

POPUP.H Win has nine words. POPUP.C:20 initializes three actual speech bubbles;
107-115 uses picture, text line count, origin, width/height, text offsets.
Its last word has no reader: preserve and call unused. Rename this popup-only
type PopupLayout and give each field its actual role. Storage is an initialized
three-record array; g_popupLayout is a private pointer, no save format.

MENU.C:108-111 obtains lineBase from a HOT_LIST_ROW Hotspot. Every struct Win
cast in mn_list and sg_save is this Hotspot, whose nine-word layout already
matches exactly. Reuse Hotspot, not a new list alias: lines->styleIndex,
w/h->x1/y1, tx/ty->x2/y2 (MENU.C:433-459; SAVEGAME.C:437-444).
Preserve pointer arithmetic, integer lineBase and field offsets.

### Style

VIDEO.H:49-60 declares 80 bytes, sixteen five-byte Colors records, with only
seven covered by pal. VIDEO.C:34-65 initializes every record. HOTSPOT.C stores
indices 0..15 and MENU.C indexes directly; thus pal[7] is an incomplete view.
Replace with a single Colors array of sixteen entries and convert all named
aliases: c23=7, c28=8, c2d=9, colors=11, hilite=13, popup=14, bank=15.
The existing unused[5] is slot 12, used by save/load rows; preserve its values
and bytes as a Colors record. FAME.C temporarily substitutes slots 7/8;
MENU.C copies current list colors to slot 9. These are drawing schemes, not
hardware palette registers; reuse no PaletteSlot enum (that means hardware).
Name the drawing-style indices and replace every hotspot initializer, variable
holding that index, direct subscript and named alias. No new storage.

## Planned replacement inventory

- CARPIC public prototypes, definitions, parameter references and concise docs;
  boolean portrait/shadow/gas-mask types and all boolean call arguments.
- CarDisplay portrait member, declaration and all three accesses.
- Renderer PassByPhase enum, phase global, all reads/writes and BOOL snapshot.
- Departure counter and minimum-departures threshold, every use.
- Both fitted communal global names everywhere, metadata included; victory BOOL.
- Police behavior slot define and sole subscript, correct docs.
- PopupLayout tag/members and every popup use; list casts use existing Hotspot.
- Style's full array, initializers and every scheme consumer; documented index
  enum/count, all raw scheme values in hotspot records and code.
- Investigate remaining 'Other misleading' findings as an inventory below,
  preserving other workers' ownership instead of sweeping their functions.

## Other misleading findings collected from earlier notes

The companion backlog records every Other-misleading section at this branch's
starting revision. Its entries remain investigation leads, not permission to
rename unrelated modules without evidence. Already-resolved names are marked
in the assessment after comparing the current sources. Comment-only issues,
uncertain picture naming and suggestions that explicitly lacked evidence are
kept separate from proven names in that assessment.

## Validation

Completed results are recorded in the final gate section below.

## Local role inventory (recorded before local renames)

Names below are tied to the statements documented above. The declaration order
is selected to preserve the original allocation order; no expression changes.


### cp_build

- `row8` -> `roofCut`

- `blt` -> `blt`

- `p12` -> `carLayout`

- `h` -> `underY`

- `pp8` -> `decal`

- `left` -> `left`

- `off` -> `framePixelShift`

- `bm2n` -> `carShadow`

- `bm` -> `pic`

- `j5` -> `imageFrame`

- `p69` -> `servicePoints`

- `right1` -> `right`

- `m2` -> `bodyModel`


### cp_show_car

- `n2` -> `frameCnt`

- `blt` -> `blt`

- `x0` -> `centeredX`

- `spr3` -> `carSpritePtr`

- `hh8` -> `bandHeightPixels`

- `scratch` -> `capSave`

- `w1` -> `imageWidth`

- `ang2` -> `spriteXPixels`

- `bms6` -> `carFrames`

- `w2` -> `pumpImageWidth`

- `pr16` -> `leftForeground`

- `h2` -> `pumpPicHeight`

- `pr26` -> `rightForeground`

- `top7` -> `backdropTop`

- `pb8` -> `workBg`

- `x2` -> `pumpBitmapFlags`

- `bm48` -> `canvas`

- `right` -> `driveSpan`

- `pan` -> `savedBg`

- `x7` -> `carPicFlags`

- `mask3` -> `bandBackgroundData`

- `h7` -> `carHeight`

- `tmpn` -> `backgroundColor`

## Implementation refinements

- Reused the existing BodyMod enum for both car-rendering arguments. Portrait,
  shadow and gas-mask booleans use BOOL and every literal caller uses TRUE/FALSE.
- The phase snapshot in animate_rival_car must retain the enum word, not
  normalize it with != PASS_BY_NONE: that inserts five bytes. Its existing
  truth test accepts either active phase. The King novice transition uses a
  named-value ternary; all three renderer phases now have no raw assignments
  or comparisons. A phase count is unnecessary: no loop or random range visits it.
- mn_list's style-index enum declaration remains between curl and n9, preserving
  insertion order among locals with the same hash. Moving it after wx8 changes
  nineteen stack-offset bytes. The original declaration order is preserved by
  splitting the declaration at the enum.
- Local names and declaration order in both CARPIC functions preserve the
  original allocation sequence. cp_build remains 1506 bytes (57 references),
  cp_show_car 2323 bytes (183 references), all exact in their whole-file context.
- PopupLayout remains nine int words (18 bytes); its last stored word has no
  reader. List views now use the actual Hotspot fields at matching offsets.
- Style's 80 bytes are sixteen Colors records, including the formerly
  misleading unused[5] at index 12. No record, initialization or storage byte
  is removed. StyleIndex names every slot, Hotspot.styleIndex and both MENU
  index locals; all hotspot records' style initializers use that enum.
- The earlier investigations' additional findings are assessed individually in
  naming-backlog.md. They are collected for their respective workers; source
  edits in this task stay with the six named follow-ups and their complete uses.

## Remaining uncertainty

STYLE_INVERSE is the inverse of the panel's text/background colors but has no
current selector; the name deliberately makes no claim about an unseen screen.
PopupLayout's last word is preserved with its original values; no current code
reads it, so its historical purpose remains unknown. The additional historical
findings are collection items in naming-backlog.md, not unverified code changes.

The note was started under work/investigations before any rename and copied to
plans/investigations for the commit. Source line pins above refer to the starting
revision; the local replacement inventory and implementation section describe
the final source.

## Final gate results

- `./sr fit`: g_racerActiveSlot stays in bucket 135 (allowed 134..135),
  g_driverWonFlag stays in bucket 52 (allowed 52..52). Both communal ties are
  resolved successfully by the identical packed/native builds. The phase and
  departure counter are initialized data, whose placement is definition-based.
- `./sr headers --check`: current includes/dependencies and valid owners.
- `./sr format --check`: all 157 C/header files pass.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572, comprising 551 MATCH and 21 MATCH*.
- Clean `rm -rf work/layout` then `./sr build`: 96/96 object gates;
  572/572 mapped functions raw-byte exact and at original positions;
  748/748 near calls exact; zero unresolved symbols;
  **Packed EXE vs orig/SR.EXE: IDENTICAL**.
- The build's auxiliary comparison to the separately unpacked original still
  reports header/relocation-order differences and a 14-byte load-image tail
  difference. This note does not claim that auxiliary comparison is identical;
  the final packed retail executable is identical, without masking.
- `./sr dgroup check`: every initialized global belongs to its original owner;
  `./sr dgroup order`: SR.LNK matches the recovered order.
- A real MSC 5.1 probe checks 13 sizeof/offsetof values: PopupLayout 18 bytes,
  Colors 5, Style 80, Hotspot 18 and every selected field offset unchanged.
- No old targeted identifier or incomplete Style palette view remains in src.

The post-format phase snapshot is named passPhaseAtEntry, reflecting its enum
value rather than implying a normalized boolean. Its truth test is unchanged.
- Final `./dosbox.sh --make`: **DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL**;
  both SHA-1 values are 3a5e28b795313028b7e77d147122f41cb29de334.
- Final `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests, OK.
