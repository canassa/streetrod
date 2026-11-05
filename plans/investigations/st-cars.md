# Car, dashboard, garage and pool types

Investigated in the requested order on 2026-11-05. Evidence locations below name
functions as well as line numbers from baseline commit 3848b38, so they remain
useful after edits.
This note was written before changing names. No storage, signedness, field order,
control flow or resource data is changed. These are independent type investigations;
other symbols discovered along the way are reported at the end.

## Band (CARPIC.H)

Purpose: describe a precomposited EGA car-frame strip in scratch video memory.
Storage: four communal records in `g_carSpriteStrips`; rebuilt by `make_bands`
(CARPIC.C:117–147), never saved. The scratch area starts below BACKDROP_ROWS in
page 2. Each strip is packed at its source bitmap's pitch, not the screen pitch.

| Member | Role and evidence |
| --- | --- |
| p → pixels | Far pointer to the packed, four-plane EGA strip. Assigned from the scratch cursor, filled from background colour or saved scene, then composited with the car image (`make_bands`:128–146). Read by stationary `cp_show_car`:548–560. |
| y → sourceY | First source-frame row, `height - bottomCut - stripHeight`; local bitmap coordinates, not a screen Y (`make_bands`:129,144). |
| h → height | Strip height in pixels, supplied by scene geometry (`cp_show_car`:441–445). |

`sp_setup` (SPRITE.C:101–113) passes each eight-byte record through Frame.data;
`es_step` (EGASPR.C:66–73) views it as Mask. `eb_sprite` (EGABLIT.C:123–131)
uses the far data and height, but does not read the middle Y word. Thus the
existing comment describing a saved screen-page band was misleading: the band
already includes the car, and sourceY is not a destination row. Keep the type
name, whose meaning is accurate with these contracts.

## Track (DASH.H) → ShifterTrack

Purpose: choose shifter gate artwork and the knob positions belonging to it.
Storage: three initialized, private `g_shifters` records; near pointers into the
initialized signed-byte `g_shiftPts` table (DASH.C:67–74). No disk loading or save.

| Member | Role and evidence |
| --- | --- |
| n → positionCount | Number of knob sites: automatic 2, three-speed 4, four-speed 5; controls background/knob cache creation (`da_init_shifter`:610). |
| id1 → gatePic | Resource ID of the gate/background, loaded before copying the 48×43 panel (`da_init_shifter`:596–603). |
| id2 → knobPic | Resource ID of the knob, masked into each cached position (`da_init_shifter`:604–622). |
| h → knobHeight | Copy height for old/new knob patches: 8 automatic, 19 manual (`da_draw_shifter`:641). |
| pt → positions | Near pointer to interleaved signed-byte X,Y coordinates relative to the gate's top left; all live coordinates positive (`da_init_shifter`:611–612; `da_draw_shifter`:645–658). |

Table selection is automatic first, otherwise top gear determines three/four-speed.
These counts/heights are geometry, not category codes. The old comment mistook a
near pointer for an inline coordinate table and described generic sprites.

## ShifterDraw (DASH.H)

Purpose: remember the active shifter layout and previously drawn knob location.
Private BSS `g_shifterDraw`, initialized per dashboard and used only by the two
shifter functions. No save/load.

| Member | Role and evidence |
| --- | --- |
| track | Borrowed pointer into `g_shifters`, set by `da_init_shifter`:596. |
| knobPosition | Previous cache/coordinate index, with -1 before the first draw (`da_init_shifter`:625). Manual uses DriveGear; automatic maps neutral to zero and other states to selector minus one (`da_draw_shifter`:635–640). Stored after drawing and used to erase the preceding knob. |

There is no single gear enum for knobPosition: automatic and manual indices have
different meanings. Name its unpainted sentinel without conflating these domains.

## WheelPoses (DASH.H)

Purpose: locate the three steering-wheel images for dashboard copies.
Private BSS `g_wheelPoses`; `da_load_wheel` initializes it for the driving scene.
The order is center, left, right (`g_wheelPicIds`, CARPIC.C:44). EGA retains
low-stack bitmaps. Other modes copy pixels into g_bufferSceneA and pop temporary
bitmaps. No save/load.

| Member | Role and evidence |
| --- | --- |
| y | Source row per pose, 148/174/73 for the live non-EGA path (`da_load_wheel`:474–484); unused for EGA, which starts copying at 0,0 (`da_draw_wheel`:537–541). |
| bitmaps | Far source bitmap pointer per pose; shared scene bitmap outside EGA, individually loaded images in EGA (`da_load_wheel`:477–489). |

The nested EGA conditional inside the non-EGA branch is unreachable. Document
actual live behaviour, retain the code. Add a pose-index enum and replace bounds,
pose tests and cached pose value with it; coordinates stay numeric.

## Rack (GAME.H)

Purpose: group an engine, its manifold and up to three carburetor pointers.
Embedded at Car offset 8; temporary stack Cars use it too. Five two-byte near
pointers. `gm_make_car` (GAME.C:340–368) establishes the order: engine, manifold,
then carburetors. Null denotes a missing part; no ownership is transferred by
copying a Rack. `ga_install/remove_part` move nodes between installed storage and
the spare list; `gm_scrap_car` and `blow_engine` release them. PERF.C:43–59 and
168–184 take a pointer to the first member to compute performance.

The sole member `parts` holds those five slots. Add named indices using existing
ENGINE_RACK_SLOTS as the count, including each carb slot. Replace fixed indices,
carb-offset arithmetic, rack-sized local arrays and loops in GAME, GARAGE, GARGFX,
PERF, BOBS, RACE and SAVEGAME. Savegame writes the enclosing 40-byte Car then
separately serializes each nonnull part in this order (SAVEGAME.C:198–242); the
saved near pointers are presence/continuation markers, rebuilt on load.

## Look (GARAGE.H)

Purpose: the displayed car's two-byte packed sticker/body-modification word.
Embedded in CarDisplay, filled from Car.appearance in `ga_menu`
(GARAGE.C:331–338). Unlike the Car word, this cache is not saved.

| Member | Role and evidence |
| --- | --- |
| lo → unusedLow | Unused low eight bits; no member readers/writers. Preserve width and unsignedness. |
| sticker | Five-bit one-based sticker selection, zero for none. The garage renderer reads the cached value (`ga_menu`:344–348); the menu uses STICKER_CHOICE_COUNT choices. |
| roof → choppedRoof | One-bit cached roof-removal flag from APPEARANCE_ROOF. Written only. |
| rear → rearBumperRemoved | One-bit cached rear-bumper removal flag from APPEARANCE_REAR_BUMPER. Written only. |
| front → frontBumperRemoved | One-bit cached front-bumper removal flag from APPEARANCE_FRONT_BUMPER. Written only. |

Keep the unsigned bitfields, including the unused byte. These cannot become
BOOL or int-sized enums. Source docs refer to the existing appearance constants.

## CarDisplay (GARAGE.H)

Purpose: transient car-rendering caches plus the most recent wager input result.
Communal `g_carDraw`, filled by garage entry, Bob's rival display and `cp_build`.
Never serialized, and not a complete description of a single car at all times.

| Member | Role and evidence |
| --- | --- |
| stickerX/stickerY | Sticker background origin: initially aligned car-local X and local Y in `cp_build`:308–310, promoted to screen coordinates in `cp_show_car`:507–508. Garage sticker painting uses screen coordinates (GARAGE.C:593–615). ti_change saves/restores them because cp_build overwrites them. |
| stickerSubX | Remainder of local sticker X under the video alignment mask; added to the decal's X before drawing (CARPIC.C:308; GARAGE.C:593). |
| betType → wagerKey | Raw result from sy_wait_input, stored before its switch (BOBS.C:579); includes cancellation/timeout as well as KR_BET_* choices. Read only to distinguish FOR_KICKS after races (BOBS.C:1071,1097,1103). Use existing InputResult, not BetKind. |
| bodyPic | Cached body resource ID set at GARAGE.C:324 and BOBS.C:365; no readers. |
| paintScheme | Displayed car colour pair; garage/current car or selected rival, used by palette selection (GARAGE.C:325–328; BOBS.C:367–368; CRUISE.C:80). Existing PaintScheme retained. |
| tireVariant | Index selecting g_tirePics, from current installed tire or rival stock bits (GARAGE.C:326; BOBS.C:366,380). Use existing TireVariant. |
| portraitPic | Occupant resource loaded only when cp_build's showPortrait is true (CARPIC.C:336); zero when entering the garage (GARAGE.C:329). |
| specIndex | Current garage car specification used for compositing and hotspot placement (GARAGE.C:330,344,358). Rival display uses its own global spec index. |
| appearance | Cached Look described above. |
| transmissionPic | Transmission dialog resource, zero for no transmission; temporary incompatible choices are restored (GARAGE.C:812–891). |
| unused | Eight unreferenced bytes; preserved. |

## PartSelection (GARGFX.H)

Purpose: identify the engine-bay drawing position and one of its bolts, alongside
the ignition-wire artwork. Communal `g_slotSelec`; ga_hood sets it while installing,
removing and turning bolts. Drawing and hotspot builders save/restore selections
(GARGFX.C:520–545,561–588,630–666). No save/load.

| Member | Role and evidence |
| --- | --- |
| wirePic | Connected/disconnected wire resource or zero when absent (GARAGE.C:1041–1044,1081–1087,1229,1312). Values already have PIC_* names. |
| slot → drawSlot | Engine-bay location, NOT the five-slot rack index. Engine=0, manifold=1; carb positions 4..13 select V6/V8 two-barrel, four-barrel, three Tri-Power sites, two Dual-Quad sites and two racing sites. `g_cPos` (GARAGE.C:763), `g_carbPos`/`g_carbPosBase` (GARGFX.C:32–38), and `ga_hood`:1161–1189 prove each site. Slots 2/3 have no live location. Add EngineDrawSlot. |
| unused | Unreferenced int; preserve it. |
| subslot → boltSide | First/second bolt within the selected manifold or carb (g_boltSubslots, GARAGE.C:766; gx_bolt_pos, GARGFX.C:254–272). Engine has only first bolt. Add PartBoltSide, distinct from tightness or global bolt number. |

Replace the complete value sets in g_cPos, g_boltSlots, g_boltSubslots,
g_carbPosBase, slot assignments/switches/comparisons, and saved/iterator variables
and API parameters transporting these values. Array positions/geometry and
unrelated menu codes remain in their own domains. g_partSlotMap is a rack-index
map except for its unused entries; preserve their original 29/-1 payloads.

## TireWork (TIRES.H)

Purpose: own the working pointers and bitmap headers for one garage tire change.
Private BSS `g_tires`; only TIRES.C accesses it. `ti_change` releases/rebuilds the
arena and eventually releases its working allocation. The record itself persists;
several pointers become stale after their stack allocations are popped. No save.
All coordinates below are pixels, all bitmap pointers far, Sprite pointer near.

| Member | Role and evidence (TIRES.C) |
| --- | --- |
| liftBuf | Car-lift sprite work bitmap: embedded EGA header or allocated CGA buffer; Tandy does not initialize/read it in its sprite backend (TIRES.C:273,274,283,297,299,302,305,327,393). |
| wheelX2 / wheelX1 | Second/first tire X relative to car left, copied from the model layout (TIRES.C:207,208,253,254,318,319,356,367,373,381,382), passed to grab_wheel_column and swap_wheel. |
| liftPics → jackFrames | Three jack images in up/column/down order (TIRES.C:200,201,203,204,205,244,245,248,249,250,265,268,271,293,294,308,311,350,351,387,388,414,415,419,422); frame oscillation during lifting/lowering, column slides vertically (TIRES.C:200,201,203,204,205,244,245,248,249,250,265,268,271,293,294,308,311,350,351,387,388,414,415,419,422). |
| underlayY | Tire shadow top within a wheel frame, height difference (TIRES.C:85,86,113,142). |
| overlayBounds | Inclusive foreground-restoration bounds for current wheel or car backdrop (TIRES.C:89,90,91,92,94,154,155,156,157,159,323,324,325,326,328,389,390,391,392,394). |
| wheelBitmap | Temporary loaded tire art used for shifting frames or compositing old wheels on car (TIRES.C:79,81,82,106,108,109,130,131,132,317,318,319). |
| liftHeader | Embedded view over EGA page-2 scratch memory, initialized by me_set_bitmap (TIRES.C:273). |
| rollCanvas | Ground-level rolling backdrop (TIRES.C:116,146,256,257,259,264,266,271,277,288,289,291,301,366,370); first-wheel area refreshed before the second swap (TIRES.C:116,146,256,257,259,264,266,271,277,288,289,291,301,366,370). |
| jackColumn | Saved vertical background for sliding the jack (TIRES.C:204,205,278,292,295,304,311,422). |
| backdrop | Active first/second wheel column from grab_wheel_column, selected before each swap (TIRES.C:91,92,93,156,157,158,355,357). |
| column → rollingTirePic | PIC_ROLLING_TIRE, shifted/composited into wheel-column capture (TIRES.C:44,45,251); freed after both columns are prepared. |
| groundStrip | Temporary garage strip composited with car shadows (TIRES.C:217,219,227,233,235,238), popped before animation. |
| underlay → tireShadow | PIC_TIRE_SHADOW used under wheel frames (TIRES.C:78,85,86,113,142). |
| sprite | Borrowed pointer to shared sprite state returned by sp_setup; replaced for every motion segment (TIRES.C:93,95,96,97,116,117,119,145,147,149,158,160,161,162,311,312,313,327,329,333,349,393,395,397,413,422,423,424). |
| phase | Pixel shift for removing the old tire, then the selected two-pixel frame index when fitting the new tire (TIRES.C:80,82,86,104,126,138,158,163). This is arithmetic alignment, not an animation-state enum. |
| rollHeader / jackHeader | Embedded scratch bitmap views; EGA maps page 3, CGA shares liftBuf pixel memory; Tandy sprite code uses its own work buffer (TIRES.C:32,33). |
| carX | Car left on screen, saved from and finally restored to g_xCoordSprite (TIRES.C:47,55,65,195,218,236,242,247,256,257,262,327,356,367,373,393,428). |
| rollShift | Computed per-frame pixel-alignment remainder (TIRES.C:104,105,109,113,128,129,132). |
| wheelHeader | Embedded scratch view for wheel lowering/raising; EGA page 2, CGA shared liftBuf (TIRES.C:31). |
| vehicleBottom | Saved bottom 28 rows of bare car, copied back before fitting new tires (TIRES.C:213,380). |
| groundOverlay | Temporary aligned car-shadow bitmap; two successive high-stack allocations are popped (TIRES.C:220,221,222,223,227,229,230,231,233). |
| basePic → shadowPic | PIC_CAR_SHADOW high-stack resource used for groundStrip, popped before motion (TIRES.C:215,217,218,220,221,229,230). |
| frames | Four shifted wheel frames, reverse order for roll-out and forward for roll-in (TIRES.C:81,82,83,84,85,86,98,107,109,112,113,116,117,125,131,132,135,139,140,142,146,147,158,163,375,377,378,381,382); later first element is reused for final installed tire art (TIRES.C:81,82,83,84,85,86,98,107,109,112,113,116,117,125,131,132,135,139,140,142,146,147,158,163,375,377,378,381,382). |
| copyBitmap | Mutable single-frame sprite image/clean wheel copy; header switched to show/remove shadow at y=125 (TIRES.C:83,84,93,98,139,140,163). |
| liftCar | Single bare-body car image built by cp_build, then fitted with old/new wheels for lifting/lowering (TIRES.C:60,63,67,68,206,209,212,213,216,217,218,240,241,242,256,257,258,261,264,274,275,280,282,316,318,319,327,374,376,378,380,381,382,393). |
| liftBackdrop | Masked garage backdrop under the lifted car, with jack frame composited into it (TIRES.C:241,242,243,249,250,279,283,297,325,326,327,350,351,387,388,391,392,393,414,415). |

No flags in this record are boolean ints. The three jack images/four rolling
frames are image sequences; their indices are arithmetic phases, not mode codes.


## Node (POOLS.H) → PartNode

Purpose: one installed or spare equipment item, or a free pool entry.
Storage: PART_NODE_COUNT communal records, plus stack stand-ins in PERF and BOBS.
pl_init links the pool; gm_new_part pops it; free_part resets the index to -1
and pushes it back (POOLS.C:17–26; GAME.C:154–172). No exhausted-pool guard is
present. Allocation does not initialize value/wear/index; callers must do so.

| Member | Role and evidence |
| --- | --- |
| value | Accounted dollar value, initialized from purchase/catalog value (GAME.C:308,337–368); affects car valuation/removal. V6 valuation is overridden by gm_part_value, so this is not necessarily the resale offer (GAME.C:175–183,235–245). |
| wear | Wear in hundredths of a percent: zero new, PART_WEAR_FULL fully worn. Engine performance decreases with it (PERF.C:59,184); tire wear/slip/failure use it (RACE.C:937–941,1024–1029). PART_WEAR_UNTRACKED for intake parts. Menu divides by 100 (GAME.C:211–215; GARAGE.C:645–648); outcome clamps installed wear to PART_WEAR_LIMIT (RACE.C:670–675). |
| idx → catalogIndex | Index into g_parts, with -1 for a free pool entry. Not a PartCategory/variant. Stock-car construction derives this from packed specification bits (GAME.C:334–368). |
| next | Free-list or spare-list link; null for installed equipment. ga_unlink leaves it temporarily attached until ga_install clears it (GARAGE.C:656–690). |

SAVEGAME.C:158–194 writes all eight bytes for each live linked item. On load the
saved next pointer is only an end marker; records are allocated anew and links
rebuilt. Stack stand-ins only initialize fields their consumer requires.

## Node2 (POOLS.H) → CarPoolNode

Purpose: fixed-size car storage with a free-list view, reused as Car while owned.
CAR_NODE_COUNT communal 40-byte records. POOLS.C:13–16 initializes each link and
free spec index; GAME.C:278–296 pops/pushes them, gm_make_car casts to Car at
line 321 and gm_scrap_car casts back at line 423. No pool exhaustion check.

| Member | Role and evidence |
| --- | --- |
| a → value | Car.value storage at offset 0; accessed through the Car view only. |
| b → specIndex | Car.specIndex storage at offset 2; set to -1 on free/init. |
| carData | 34-byte payload spanning Car.paintScheme through Car.appearance; opaque in the free-list view. Not a separately serialized blob. |
| next | Near free-list link, sharing offset 38 with Car.next. |

Save/load serializes the Car view (40 bytes plus installed parts), not the pool
array. Retain the opaque bytes and signed ints, without guessing a union or
changing the public allocation return type beyond the tag rename.

## Other misleading identifiers and observations (outside scope)

- Mask (EGABLIT.H; eb_sprite, EGABLIT.C:123–131) is also a view of a composited
  EGA strip, not necessarily a conventional transparency mask; its middle word
  is not read by that consumer. Do not merge/retype it here.
- g_partsPics is an ambiguous domain: ga_menu temporarily fills it with catalog
  indices (GARAGE.C:340), while ga_hood overwrites with picture IDs (:1049).
- ga_hood compares an engine variant against MANIFOLD_TWO_BARREL at GARAGE.C:1047;
  the intended zero-valued engine test appears to be ENGINE_V6. Reported only.
- g_shifters/da_init_shifter's old comment calls the gate a car and the knob a
  wheel (DASH.C:580–582); correct the comment with the type-member documentation.
- da_draw_wheel's first argument is used both for a three-way wheel pose and as
  an index into g_steerPoints (DASH.C:533,552); negative inputs would index before
  the latter table. No change to that function's contract/behaviour in this task.
- ra_reset_eng local `car` is a tire PartNode, not a Car (RACE.C:907,936–941).
- TireWork's phase is deliberately reused across alignment domains. Do not
  invent a state-machine enum from its old generic name.

## Verification

Before edits, real MSC 5.1 sizeof/offsetof audit: Band 8, Track 10, ShifterDraw 4,
WheelPoses 18, Rack 10, Look 2, CarDisplay 30, PartSelection 8, TireWork 298,
Node 8, Node2 40 bytes. Every non-bitfield member offset was captured; Look's
bitfield layout is additionally protected by the unchanged packed executable.
The generic check_data_types script cannot resolve existing macro array bounds
(known sr-cli skill limitation); use the direct MSC audit, not guessed dimensions.

Gate outcomes and final replacement details are appended after verification.

### Additional evidence and replacement inventory

Decoded original EGA resources with `./sr pic` and inspected the PNGs: 1001/1002/
1000 show center/left/right wheel poses; 1229/1232 show automatic gate/lever;
1227/1230 and 1228/1231 show manual gates/knobs; 1034/1035/1036 show jack handle
up/bare column/handle down; 1223 is tire artwork. This confirms the resource roles
independently of their existing names. MENU.C's wear_text (lines 352–364) renders
"(new)" at zero and "(xx% worn)" otherwise. INPUTDAT.ASM:275 initializes engine,
transmission and tire wear to zero and intake wear to the untracked sentinel.
SYS.C:1152–1160 increments engine/transmission wear and stops the engine at the
wear limit; these are additional PartNode readers/writers.

Final tag replacements: Track → ShifterTrack, Node → PartNode, Node2 →
CarPoolNode, including pointer forward declarations and all typed users. No
function/global names were changed. Member replacements are listed in the
per-type tables above; every corresponding source access was replaced.

Named sets and complete live replacements:

- EngineRackSlot: all fixed rack accesses, PERF's qn/nd/ndm accesses, g_partsPics
  and g_partPics row accesses, the rack output map's live values, carb-base offset
  arithmetic, and ga_hood's selected-slot assignments/tests. RACK_NONE names the
  absent selection; ga_hood.sel2 is enum-typed. ENGINE_RACK_SLOTS replaces Rack's
  bound, rack-sized local PartNode arrays, g_partsPics's bound and the full-rack
  loops in BOBS, GARAGE, RACE and SAVEGAME. Shared counters which also hold menu
  results, counts or other arithmetic remain ints.
- EngineDrawSlot: PartSelection.drawSlot, saved selections/iterators in GARGFX,
  g_cPos/g_boltSlots tables, every draw-slot assignment/switch/comparison, the
  char g_carbPosBase values, the coordinate-table extent and its index base.
  Unused table zeroes keep their payload using ENGINE_DRAW_ENGINE; unused map
  values 29/-1 remain untouched. These are not new live draw locations.
- PartBoltSide: PartSelection.boltSide, saved selection variables, the complete
  g_boltSubslots table, comparisons and the pointer types of gx_draw_bolts and
  gx_build_hots (both declarations and definitions). ga_hood's k remains int
  because it also holds catalog indices and menu results.
- WheelPose: WheelPoses array bounds, source-row and resource-table bounds,
  da_load_wheel's iterator/ranges, da_draw_wheel.sel and range test, and the
  cached g_wheelShown type/initializer. The boolean-sum pose calculation has no
  raw pose code to replace and is retained unchanged.
- Existing InputResult and TireVariant type CarDisplay's wager/tire fields;
  their writers already use named input results, catalog variants or packed
  specification masks. SHIFTER_NOT_DRAWN and POOL_ENTRY_FREE replace every
  initialization/free-list assignment of their sentinel values. The surviving
  equivalent raw wear sentinel in car_info now uses PART_WEAR_UNTRACKED.

New header constants/enumerators and every target member have Doxygen docs.
Explicit header dependencies expose enum types/counts to independent includes;
`sr headers` regenerates C includes and MAKEFILE dependencies.

### Preserved compiler layout

All 76 real-MSC sizeof/offsetof values matched before/after. Offsets in the
following table are in declaration order, using the final member names above.
The compiler audit includes all non-bitfield members, not only accessed ones.

| Type | Bytes | Member offsets |
| --- | ---: | --- |
| Band | 8 | 0, 4, 6 |
| ShifterTrack | 10 | 0, 2, 4, 6, 8 |
| ShifterDraw | 4 | 0, 2 |
| WheelPoses | 18 | 0, 6 |
| Rack | 10 | 0 |
| Look | 2 | Packed unsigned bitfields unchanged in width/order |
| CarDisplay | 30 | 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 |
| PartSelection | 8 | 0, 2, 4, 6 |
| TireWork | 298 | 0, 4, 6, 18, 20, 28, 32, 80, 84, 88, 92, 96, 100, 104, 106, 108, 156, 204, 206, 208, 256, 258, 262, 266, 270, 286, 290, 294 |
| PartNode | 8 | 0, 2, 4, 6 |
| CarPoolNode | 40 | 0, 2, 4, 38 |

The audit builds a temporary C array of sizeof/offsetof expressions with
`dosbox.Job(..., '/AM /Oas /Gs /FPi', ...)`, reads its initialized words through
`sr.omf.Obj`, and compares them after translating renamed tags/members. Unlike
the generic shape reader it lets MSC evaluate the existing macro array bounds.

### Gates before integration

Headers and formatting pass; Doxygen reports zero diagnostics. All 572 functions
match (551 MATCH and 21 MATCH*), all 96 object gates pass, all 748 near-call
encodings are exact, and a clean build produces an IDENTICAL packed executable.
DGROUP ownership/order pass, all 109 unit tests pass, and DOS-native MAKE also
produces an IDENTICAL executable (SHA-1 3a5e28b795313028b7e77d147122f41cb29de334).
Source comments were subsequently wrapped to the documentation width; formatting
and Doxygen were rerun. Full gates will run again after the required main merge.

### Integration and final gates

Initially fetched main and ran git merge origin/main against 3848b38;
Git reported already up to date, with no conflicts. Reran all gates after that
merge step: headers and format pass; Doxygen has zero diagnostics; all 572
functions match; the clean packed build and DOS-native MAKE are IDENTICAL;
DGROUP ownership and order pass; all 109 unit tests pass. Repeated the 76-value
MSC layout audit with identical results. Generated Doxygen XML confirms docs for
all 70 members across all eleven target structs.

The uncompressed-link diagnostic reports relocation/header differences from the
unpacked reference, while all 572 function bytes/positions and all 748 near calls
are exact. Both packed-build paths reproduce the actual retail executable byte
for byte, including its relocation representation. No game-behaviour or layout
change was needed, and the out-of-scope observations above remain reports only.
