# Investigation: show_rival_car

## Purpose

Animate the selected rival's arrival, departure or pass-by outside Bob's (also
seen from jail), keeping the rival's status-line banner, presence and carhop
activity consistent with that movement.

Written before any identifier changes. Evidence below uses the original source
line numbers in baseline commit eac854a; the later rename/documentation
inserts lines in BOBS.C. Per the user's revised instructions, the conclusions
are based on C/ASM source, resource data and state consumers, not disassembly.

## Inputs, output and contracts

The function is static, near and returns void. All five call sites are in
src/GAME/BOBS.C:355,398,1013,1106,1116. The first is in rival_arrives;
the others send an already selected rival away. No parameter is an output.

| Original | Proposed | Meaning and evidence |
| --- | --- | --- |
| mode | animation | CarAnimKind, forwarded to cp_show_car, then possibly replaced by departure for bookkeeping (BOBS.C:160-171). Actual values: 4, first arrival; 1, subsequent arrival; 2, departure. Call chains originate at BOBS.C:966,1022,1032,1033,1096,1103,1166,1226,1227. CARPIC.H:17-25 defines the values; SPRITE.C:241-258 proves the motion. |
| b | specIndex | Index into the car-spec/model/layout tables; must agree with the selected rival and selected spec. All callers pass g_opponentSpecIndex. select_rival sets all three together (BOBS.C:317-320); cp_build indexes the four tables (CARPIC.C:299-302). Disk storage has 26 entries (DISK.C:47-51). |
| c | showPortrait | Zero omits the occupant portrait; nonzero includes the selected portrait. Every caller passes 1. CARPIC.C:335-339 loads g_carDraw.overlayPic and underlays it into the body; BOBS.C:344 sets it from the racer. The ending substitutes the girlfriend's portrait before rendering, then restores the King's (BOBS.C:841-845). |
| d | carBottom | Inclusive bottom screen row of the whole car image; every caller passes 0xa3 = 163. CARPIC.C:435 derives the backdrop top, :493 computes top = bottom - height + 1, :538-554 uses that bottom for blits, and :558 passes it into sprite setup. |
| e | useGasMask | Zero omits the gas-station foreground mask; nonzero selects it. Every caller passes 0. CARPIC.C:489 passes either PIC_GAS_STATION_MASK or zero to cp_backdrop; CARPIC.C:193,230-238 loads the mask and establishes foreground rectangles. It is not a picture ID or a general animation mode. |
| f | tirePics | Borrowed pointer to four tire-frame resource IDs; callers pass the row selected by the car's tire variant (BOBS.C:341,355). GARAGE.C:42-44 defines the three rows. cp_show_car requests four frames for these movement kinds (CARPIC.C:438); cp_build reads one ID per frame and underlays it at both wheel positions (CARPIC.C:363-383). A null pointer is accepted by cp_build and omits tires. |

Only animation is a categorical enum: retain CarAnimKind. The two drawing
switches are truth tests accepting any nonzero int, not closed enumerated value
sets. specIndex and carBottom are indices/coordinates. tirePics is not written.

Implicit inputs: selected-racer and selected-spec pointers, their name offsets,
body modifications and sticker; selected portrait and paint prepared by
select_rival; current scene; pass-by phase; cash-text end position; demo state;
and the carhop/order/pass-count state. Valid scene graphics, picture libraries,
arenas and selected records must already be prepared by the caller.

## Locals

| Original | Proposed | Role and evidence |
| --- | --- | --- |
| x | atJail | Boolean snapshot of g_scene == SCENE_JAIL. Used for a +10 entry-chance adjustment and the post-animation carhop guard (BOBS.C:135,141,172-175). Not an x coordinate. |
| n | textOffset | Reused scratch integer: last-character index during punctuation removal, then horizontal gap between money text and rival banner (BOBS.C:144-151). It is neither a car count nor solely a string length. |
| old | wasPassBy | Snapshot of the incoming pass-by phase, retained across the renderer, which resets that global (BOBS.C:131,161-162; CARPIC.C:597-605). It is not an old animation kind. |

The existing declaration int x, n, old gives slots -2,-4,-6. The proposed
atJail, textOffset, wasPassBy order predicts the same slots with sr layout --ol.
Keep the reused scratch local rather than inventing separate lifetimes.

## Step-by-step behavior

1. Save the current pass-by phase. Build the banner as racer name, two spaces,
   car-spec description using offsets into g_desc (BOBS.C:131-134).
2. Snapshot whether the view is jail. For the first-arrival kind, disable the
   carhop. Before movement, presence is true only for a departing car: a car
   entering later has not arrived yet (BOBS.C:135-138). This intermediate value
   is deliberately opposite to the final presence for normal in/out movement.
3. Only before a later arrival (kind 1), attempt a carhop entry. Entry chance is
   10*atJail + 75*g_ordered + 8*g_demo + 5, with quick-departure threshold 10.
   If that attempt takes no action, attempt departure at threshold 20
   (BOBS.C:139-143; BOBS.H:11-18). No chance is clamped to 100.
4. Strip one final period from the combined string, if present
   (BOBS.C:144-145). No other punctuation or backtick separators are rewritten.
5. Measure the string in font pixels. Compute free space to the right of the
   money display; take half if positive, else zero; enforce a gap of at least
   15 pixels; add the money end x and round down to an 8-pixel boundary.
   If the banner extends beyond pixel-width 320, drop its last space-delimited
   word and repeat (BOBS.C:146-157; DASH.C:736-743; PC.H:31). drop_last_word deletes trailing nonspace bytes and
   one preceding space; backticks are not spaces (BOBS.C:110-123).
6. Clear the current centered status message, then draw the fitted rival banner
   (BOBS.C:158-159). sc_status_text saves/restores colors, hides/shows the cursor,
   draws on the back page and optionally copies to front. Its width argument
   controls centering, not the erased strip's width: the cleared strip extends
   to the screen edge (SCREEN.C:46-71).
7. Render with the requested animation, forwarded drawing options, and selected
   racer's bodyMods/sticker (BOBS.C:160). cp_show_car builds images, backgrounds,
   clipping/motion and updates shared sprite/feature positions. First arrival
   initializes backdrop/arenas and saves the carhop background. Movement uses
   four frames, starts/stops drive effects and steps the sprite until finished
   (CARPIC.C:435-605). If pass-by phase was 1, it reuses the images for a second,
   drive-out traversal under phase 2, then clears the phase to zero.
8. If the saved phase was nonzero, treat the result as departure even if the
   requested animation was arrival. Departures increment g_passCount, empty the
   retained banner buffer and erase the displayed banner (BOBS.C:161-168).
9. First arrival that did not become a pass-by re-enables the carhop as absent.
   Final presence is false for departure, true otherwise (BOBS.C:169-171).
10. Unless jail and a rival remains present, try carhop entry at the same formula.
    Quick departure is threshold 8 if no rival remains, else zero. If no entry
    action occurred, try departure at threshold 40 (BOBS.C:172-176).

## Callees and their real effects

- strcpy/strcat/strlen are standard string operations; this source includes
  string.h and performs no capacity validation (BOBS.C:1,132-145).
- maybe_carhop requires absent state and at least three counted departures for
  entry. It calls rn_int(100), which produces 0..99 for positive bounds
  (BOBS.C:66-103; RANDOM.C:10-63). Enter/leave animates the skating attendant,
  preserving the rival car's overlap when its argument is true
  (CARHOP.C:97-179, especially :109-142). It hides and restores the cursor.
  It returns whether an action occurred, including an enter-then-quick-leave.
- Its quick leave clears order but does not reset g_passCount; its standalone
  leave clears order and resets g_passCount (BOBS.C:81-96). Enter's quick leave
  waits 100 retraces. Thus the fallback is gated on action, not final presence.
- drop_last_word, da_text_width and sc_status_text are explained in steps 5-6.
- cp_show_car is explained in step 7. Its q is portrait inclusion; its mode2
  selects the gas-station mask; its ids are per-frame tire pictures. Those
  meanings come from opening cp_build and cp_backdrop, not their current names.

## Caller contexts

- rival_arrives selects a racer and sets the drawing state first. Jail requests
  RIVAL_PASS_BY, setting phase 1. A King challenge also passes by when king_status
  returns KING_NOVICE (BOBS.C:348-355). Other requested arrivals do not set phase.
- At Bob's, the first arrival uses kind 4 to initialize scene/backdrop state;
  subsequent arrivals use kind 1. Calling a met rival first sends the current
  rival away, then brings in the chosen rival (BOBS.C:385-407,966,1022-1033).
- Bob's timeout sends an existing rival away (BOBS.C:1006-1016).
- After challenge handling ends, the current rival leaves (BOBS.C:1040-1108).
- Looking under the hood again, or asking to inspect the King's hood, displays
  the chicken message and sends the rival away (BOBS.C:1111-1123).
- After winning a drag race against the King, first-arrival setup is repeated
  (BOBS.C:1096-1104).
- Jail uses Bob's background with jail foreground bars/window. Timeout calls
  rival_arrives with a pass-by and switches the next animation from 4 to 1
  (BOBS.C:1154-1230). The rival never remains present on that normal path.

## State inventory, producers and consumers

- g_selectedRacerPointer / g_oppSpecPointer: select_rival installs pointers to
  racer/spec records, marks racers met, evolves stickers/body modifications,
  extracts tire variant, sets palette and portrait (BOBS.C:317-344). The wrapper
  reads nameOffset from both and bodyMods/sticker from the racer; it does not
  mutate the records. Racer declarations are BOBS.H:90-108; Spec is
  DISK.H:292-299. Race results can replace or remove a rival's car
  (RACE.C:696-725). Initial records and description strings load from HOT_DATA
  via DISK.C:69-82,502-524. sg_init_racer assigns ordinary racers their spec,
  paint, sticker and body modifications (SAVEGAME.C:52-79); init_racers groups
  the model choices by rank and replaces the demo rival's name offset with
  DESC_THE_GEEK (SAVEGAME.C:83-116). Saved games restore/persist all 396 racer
  bytes (SAVEGAME.C:339,466). The King starts with spec 25 from HOT_DATA.
- g_carDraw: portrait/tire selection comes from select_rival, not wrapper
  parameters; cp_build also writes sticker coordinates. CARPIC.C:303-312,335-339,
  492-505 updates car feature and sticker positions. g_models, g_layoutAutoSprArray,
  g_pointsModelService and g_carData are read-only loaded per-model data here.
- g_desc: loaded description/name pool. g_strOpp is an 80-byte persistent buffer
  (SYS.C:45). The wrapper builds/truncates/clears it; sy_wait_input restores it
  after Bob's hover text and clears it for race-type selection
  (SYS.C:402-408,515-532). Bob's/jail exits and race transitions also clear it
  (BOBS.C:1000,1041,1228).
- g_wRivBanner and g_bannerX: measured width and snapped x; persist after an
  erase so other UI paths can erase/restore the banner (SYS.C:407,519,530).
  g_bannerX initially 100 (BOBS.C:62). Width storage is SYS.C:50.
- g_xMoneyRhsVal: written by sc_add_money from the text renderer's ending pen x
  (SCREEN.C:74-86). sc_label draws the cash label and initializes its value
  (SCREEN.C:89-109). It is a pixel coordinate, not a money amount.
- g_scene: initialized to Bob's or jail by scene entry (BOBS.C:985,1201), with
  values 2/6 (MAIN.H:10-17). The wrapper only reads it.
- g_demo: read numerically as eight times the demo state, not normalized truth.
  It can also be 98/99, as the input/drive code demonstrates
  (SYS.C:188,413; DRVTICK.C:98-99). This can make the entry threshold exceed 100.
- g_carPassBy: written by rival_arrives, stepped 1 -> 2 -> 0 by cp_show_car
  (BOBS.C:352-354; CARPIC.C:597-605). Snapshot is needed to preserve final-state
  bookkeeping after that reset. Normal entry phase 2 is internal to the renderer.
- g_rivalHereState: temporary pre-movement overlap flag, then final presence.
  Bob's interaction decisions and hotspot setup read final state
  (BOBS.C:396,988,1010,1029,1112). Scene entry initializes zero
  (BOBS.C:981,1196). No other module reads it.
- g_carhop: -1 disabled, 0 absent, 1 present (BOBS.H:185-189). Scene entry
  disables it; non-pass-by first arrival re-enables it; maybe_carhop transitions
  0/1. Normal jail first arrival is a pass-by, so never re-enables it. Jail's
  +10 chance term exists, but the disabled-state check prevents entry on the
  ordinary jail call path (BOBS.C:73-74,136-137,161-170,1197,1226).
- g_ordered: boolean Bob's order request, initialized zero at scene entry and
  set to 1 by the order action; cleared when the attendant leaves
  (BOBS.C:85,95,983,1126-1127,1198). Read as +75 entry threshold.
- g_passCount: increments on every effective departure, including pass-bys.
  Only a standalone carhop leave resets it; scene entry does not. It is an
  eligibility counter, not necessarily this visit's pass-by count
  (BOBS.C:63,75,96,163-164).
- Indirect effects: RNG state, cursor state, status-line shown flag/colors/pages,
  arena lifetimes, car imagery, feature/clipping coordinates, drive-effect and
  sprite state are modified by the opened callees (SCREEN.C:46-71;
  CARHOP.C:97-179; CARPIC.C:435-607; RANDOM.C:32-63). No disk save or race result
  is performed by the wrapper.

## Data checks

The three tire rows were checked with sr data 5030 24: IDs 1065..1068,
1028..1031,1106..1109, consistent with GARAGE.C:42-44 and DISK.H's picture enum.
Decoded pictures 1194,1217,1244 really are occupant portraits; 1142 is a skating
carhop sheet, corroborating DISK.H:158,210,233,260 and CARHOP.C:20. Resource PNGs
were inspected in work/rival-portraits.png. Parsing HOT_DATA according to
DISK.C:69-82 and BOBS.H:90-108 confirms 22 racer names and portrait IDs; car
names in the 26 Spec records include final periods and embedded backticks.
The King has portrait 1244 and spec 25, whose name is '1963 Corvette' with no
final period. Thus removing a period is conditional rather than universal.

## Edge cases and limits

- No pointer/index/string bounds checks. Combined names must fit in 80 bytes
  before shortening. The inserted two spaces guarantee a nonempty initial
  banner even with empty names; invalid offsets and unterminated strings are
  not defended against.
- Word removal can leave an empty banner. With a corrupt/extreme money-end x
  that cannot fit even an empty string, the fit loop cannot terminate. Normal
  cash-label positioning supplies the necessary geometry (SCREEN.C:74-109).
- Rounding the gap down to 8 pixels means the final visible gap can be below 15.
- Passing an unsupported CarAnimKind is not rejected. This wrapper's meaningful
  call-site contract is 1/2/4; other renderer modes do not establish sensible
  presence/carhop initialization for this wrapper.
- Pass-by overrides the bookkeeping animation only after drawing. It prevents
  the first-arrival re-enable of the carhop, including on the jail path.
- Chance thresholds are raw comparisons against 0..99, with no clamp. Entry
  requires the minimum counter and absent state even at a threshold above 100.
- Clearing a banner does not zero its remembered width/x. That is intentional
  for the later status UI consumers.

## Other misleading identifiers/documentation (outside scope)

- CARPIC.C:281,335-339: cp_build's q is portrait inclusion; kk is a sticker
  selection (CARPIC.C:331-334), ids is a tire-frame list (:363-383). cp_show_car's
  matching q/kk/ids and mode2 are equally opaque (:409,474,489). Report only.
- CarDisplay.overlayPic and Racer.overlayPic conceal the specific occupant-
  portrait role in this path (BOBS.C:344,841-845; CARPIC.C:335-339). No broader
  use proves a generic vehicle overlay, and GARAGE.C:328 clears it for the
  player's unoccupied parked-car rendering. Report only; not renamed here.
- g_carPassBy sounds boolean but is a renderer phase with values 0,1,2
  (CARPIC.C:597-605). g_passCount counts departures, not just cars that pass by,
  and persists across visits (BOBS.C:96,163-164,981-985). Report only.
- CARHOP.C:81-82 says ch_skate builds a moving car-body animation, but it cuts
  and animates the skating attendant sheets (CARHOP.C:20,141-160). Its b means
  a rival car is present for clipping, not a generic argument (:109-142).
- sc_status_text's w sounds like a drawing/erase bound; it only affects
  centering when x is negative, while the erase/copy width reaches screen right
  (SCREEN.C:46-71). Do not describe the wrapper's banner clear as a width-limited
  erase.

## Open questions

None that prevent stating this function's behavior and contracts. Original
identifier spellings and the game designers' reasons for chance/spacing choices
are not recovered claims. No behavior changes or other-symbol renames proposed.

## Change decision

Rename show_rival_car to animate_rival_car: all actual callers request car motion,
and its wrapper responsibilities are documented rather than hidden in the name.
Keep it static and follow the file's snake_case style. Rename the six parameters
and three locals as above; retain CarAnimKind and int truth switches. Add the
private Doxygen block at its definition and a BOBS.C file block. Verify the full
file and all required gates, including the clean packed build and native MAKE.

## Implemented result and verification

Implemented animate_rival_car with the six parameter names and three local names
above. Only the source, symbol metadata, this note and the diary are changed.
The original CarAnimKind type remains; no other identifiers were renamed.

- sr layout and the compiler agree: atJail=-2, textOffset=-4, wasPassBy=-6;
  the function remains 640 bytes and the entire BOBS.C span remains 8829 bytes.
- sr headers --check and sr format --check pass.
- sr docs reports 0 diagnostics and includes the private function.
- sr check --all: 572/572 (551 MATCH, 21 MATCH*).
- Clean rm -rf work/layout followed by sr build: 96/96 objects pass;
  572/572 mapped functions raw-byte exact, 748/748 near calls exact;
  Packed EXE vs orig/SR.EXE: IDENTICAL.
- sr dgroup check and sr dgroup order pass.
- nix-shell shell.nix --run "python3 -m unittest discover -s tests":
  155 tests, OK.

The build's unpacked-file diagnostic reports header/relocation ordering and a
14-byte load-image length difference against the supplied unpacked reference;
the final packed retail EXE is byte-identical. No comparison rules were changed.
Native DOS MAKE also passes: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL;
SHA-1 3a5e28b795313028b7e77d147122f41cb29de334 for both files.
