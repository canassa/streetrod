# Collected naming follow-ups

Source revision: `98346d7ac7e39adba0e1ace046cb6949fb9cff66`.

Snapshot before the inv-names edits; links refer to the original investigations.


## Assessment against the current source

This is the collected follow-up backlog requested alongside the six concrete
inv-names fixes. The source excerpts below are historical findings, with this
assessment recording their present status. Unrelated functions remain with
their existing owners.

| Finding group | Present status / remaining work |
| --- | --- |
| CARPIC q/kk/ids/mode2 and other arguments | Fixed here; parameters, booleans, body-mod enum and locals now state their roles. |
| CarDisplay.overlayPic | Fixed here as portraitPic. Racer.portraitPic was already fixed. |
| g_carPassBy and g_passCount | Fixed here as typed g_carPassPhase and g_departureCount; threshold renamed too. |
| Selected rival and victory globals | Fixed here as g_racerActiveSlot and BOOL g_driverWonFlag; both pass sr fit. |
| RACER_COP_ENTRY | Fixed here as RACER_POLICE_BEHAVIOR_SLOT. |
| Win popup/list alias; Style.pal | Fixed here with PopupLayout, existing Hotspot and the full schemes array. |
| ch_skate car-body comment and b argument | Already fixed: CARHOP.H documents the attendant, definition uses BOOL carPresent. |
| SPEC_KING / manifold variants | Already fixed, as Spec.md itself records. |
| fit_gear rpm parameter | Definition already uses speed; its forward declaration still says rpm (RACE.C:67). |
| INPUTDAT name | The old placeholder was renamed; it intentionally owns BIOS helpers plus catalog data. A catalog-only name would also misstate its scope. |
| RIVAL_*_CHANCE | Still draw-bound names, despite the corrected header brief describing exclusive bounds; potential *_DRAW_BOUND follow-up. |
| MEM kind/allocation arguments; ar_bm flags; Slot.bitmapFlags | Still present. Bitmap allocation options and storage kind need distinct argument names/value sets. |
| Slot.pal / di_unpack_pic.colors16 | Still present RLE dictionary names (SLOTS.H:16, DISK.C:328-352). |
| eb_copy, tb_compose, bm_copy_box source/destination | Still reversed in prototypes and definitions; their actual source-first flow was rechecked. |
| ed_line mono | Still a dead kind comparison; no proven rendering role. |
| Node2 payload names / Rack / Node | Still present; allocation, installed engine assembly and owned-part meanings remain leads. |
| mu_play tones; voice globals; g_voiceNum | Still present; repeat-count and background-hotkey permission meanings were rechecked in MUSIC.C and SYS.C. |
| PIC_FUEL_GAUGE; gas-station label/pos/done | Still present; scenery/background, charge and fuel-delivered roles. |
| ra_speed_step rpm | Still present in RACE.C/H; argument is speed. |
| g_autoTrans / ra_reset_eng | Still present; dashboard/town flag and wider race-reset effects require contextual docs/naming. |
| Hotspot array names | Still broad storage aggregates; no homogeneous role is proved. |
| set_jail_spots id | Already enum HotKind, but parameter name id remains generic. |
| GARAGE engine compared with MANIFOLD_TWO_BARREL | Still present at GARAGE.C:1047; existing ENGINE_V6 has the proper domain. |
| RACE tire Node called car; GAME part-ad branch used | Still present locals. |
| Spr12.b5/b6 | Still present engine-overlay coordinates; reserved for layout-record work. |
| g_mainCarSprite / Frame.pos / Frame.data | Still present; general sprite, bitmap and backend-dependent band data meanings. |
| g_autoSprImagePtr | Still present; retained last car-frame bitmap. |
| cp_patch_bg source/unused parameter | Still present as dst/p8 in the definition; its write target is the saved car backdrop. |
| sc_status_text w | Still present; centering width only, not the erase span. |
| gx_trans_bolt / pf_vmax / bm_put_pic / mn_bbox / DASH shifter comments | Existing comments still need correction; no new rename inferred from them. |
| sp_car_clip / sp_reverse / grab_strip / ch_save_bg / ta_copy_page comments | Existing comments still misdescribe clipping, ring traversal, capture or display mode. |
| ANIM_NOP_* / PIC_FUEL_PUMP_DIGITS / car roof/bumper resource names | No additional semantic rename proved; keep the investigations' uncertainty. |

Repeated reports (notably copy-direction and car-renderer names) are one
follow-up each. Full original evidence is retained below for traceability.

## Bitmap

[Original investigation](Bitmap.md)

- `me_new_bitmap`'s c and `me_wrap_bitmap`/`me_set_bitmap`'s flags are the kind
  value, while the other flags argument controls allocation. `ar_bm`'s flags
  and `Slot.bitmapFlags` likewise describe mask/color allocation, not kind
  (`MEM.C:324-350`; `ARENA.C:85-93`; `DISK.C:324`).
- `di_unpack_pic`'s colors16 and `Slot.pal` are RLE dictionary tokens, not a
  display palette: they are passed to `rl_decode` as its token dictionary
  (`DISK.C:327-341`; `src/SYS/RLE.ASM`), with run counts/values in DISK.C.
- `eb_copy` has dst/src parameter names reversed relative to its copy direction:
  it reads dst->bits and writes src->bits (`EGABLIT.C:42-44,80-89`), matching
  other copy hooks' source-first calling convention.
- `ed_line`'s mono is a dead kind comparison, not a monochrome-mode test
  (`EDIT.C:35`, no later reader); EGA/CGA/Tandy page kinds all satisfy it.
- `ta_copy_page`'s EGA/VGA wording is misleading: it copies TANDY_PAGE_BYTES
  (`TANDY.C:144-149`).

## Car

[Original investigation](Car.md)

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

## Cmd

[Original investigation](Cmd.md)

- `bm_copy_box`'s `dst` and `src` parameters are reversed relative to actual
  data direction (BITMAP.C:326-342 and EGABLIT.C:44-46,80-89).
- `eb_copy` also calls the source `dst` and destination `src` by the same
  `movedata` evidence. Its planar comment says VGA although it handles EGA.
- `sp_car_clip`'s comment says screen window; its bounds come from the car
  image position, width and visible height (SPRITE.C:34-48).
- `ANIM_NOP_1` and `ANIM_NOP_3` accurately describe interpreter behavior but do
  not explain the header role. Header records can have any of several opcodes;
  do not rename them as header kinds without further evidence.
- `PIC_FUEL_PUMP_DIGITS` is used by the timing-tuning animation (CARPIC.C:75;
  GARAGE.C:959-1004); this does not establish that its asset name is wrong.

## Dev

[Original investigation](Dev.md)

- MUSIC.C:19 `mu_play` parameter `tones` is assigned to repeatTable/repeatCursor
  (:41); it is a repeat-count table, not a tone-selection list. Its existing
  leading comment is correspondingly wrong. Leave these outside this type task.
- `g_secondaryMusicStateData` and `g_songBgState` are playback voices for an
  interrupting effect and the background song; their inconsistent suffixes hide
  this relationship (SYS.C:42-43, MUSIC.C:29-43). Global renames are out of scope.
- `g_voiceNum` is a background-music-toggle permission/selection marker, not a
  number of simultaneously active voices: MUSIC.C:30,39 write 1 or 2, and
  SYS.C:438 permits the music hotkey only for 1. GARAGE.C:962,1002 also writes
  2/1 around manual timing tones without changing g_voice. Thus it is not
  strictly a voice index either; only g_voice is advanced by the ISR.

## Dlg

[Original investigation](Dlg.md)

- `PIC_FUEL_GAUGE` (1139, `src/SYS/DISK.H:158`) is the scenery patch covering the
  rag/bucket pickup area, not a fuel gauge. `RACE.C:338` displays it at (80,170)
  when taking the rag, and line 346 restores the captured region when replacing
  it. Decoded 1090/1139 show the same pavement/faucet/bucket region.
- `label` in `ra_gas_station` (RACE.C:239, 259, 346) is the rag/bucket background,
  not text. `label` in `show_tank_cap` (179, 187-198) is a cap/nozzle image, also
  not a text label. `pos` (266, 293, 311) is dollars per fuel increment, not a
  position; `done` (269, 292, 316, 362) means some fuel was delivered, not that
  the visit is over. These function-local names are outside this type task.

## Eng

[Original investigation](Eng.md)

- `fit_gear(int rpm, ...)` actually receives **mph** (`RACE.C:1031` multiplies it by RPM-per-mph; callers at 1073,1088,1099,1294,1302,1306,1354 pass speeds). The name of its parameter is misleading; retained outside this type investigation.
- `ra_speed_step(int rpm)` also receives mph (`RACE.C:1209,1229`) and returns road-distance step; parameter name is misleading.
- `g_autoTrans` is a separate dashboard/town flag derived from current transmission (`CRUISE.C:32`), not a replacement for the record's automatic flag.
- `ra_reset_eng` has global input/race/pace side effects and uses player tires for both instances; its name understates that scope.
- `DASH.C:580-582` calls the shifter artwork a car/wheel sprite; the code and its slot/knob updates show a gear-selector display. Retained for the separate type/function work.

## Hot

[Original investigation](Hot.md)

- `Win.w/h/tx/ty/lines` describe popup dimensions/offsets/line count in
  POPUP.C:107-115, but in the list alias they mean left/top/right/bottom/style
  index (`MENU.C:433-459`, `SAVEGAME.C:436-445`). A single prose meaning for
  Win without distinguishing those views would be wrong.
- `Style.pal[7]` suggests seven banks or hardware palette entries, whereas
  indices through 15 select complete Colors records across the rest of Style
  (`VIDEO.H:45-55`, `HOTSPOT.C` initializers, `MENU.C:54-96`). `Colors.frame`
  is used as a condition for beveling here (`MENU.C:58`); its wider semantics
  belong to the Colors investigation.
- `g_hotCalendar`, `g_hotGasExit`, `g_bodyAsk` and `g_hotBobs` are not homogeneous
  calendar/exit/body/Bob's-only arrays: their contiguous storage includes many
  later menus (`HOTSPOT.C:137-203,228-360`). Keep global names/layout intact.
- `set_jail_spots`'s parameter `id` actually holds HotKind, not the record's
  returned ID (`BOBS.C:1162-1174`). `mn_bbox`'s comment says `f10` for the
  drawing-chain member (`MENU.C:188`), an obsolete reconstruction name.

## Item

[Original investigation](Item.md)

- `GARAGE.C:1040` compares the variant of `car->rack.parts[0]` (the engine)
  against `MANIFOLD_TWO_BARREL`. It actually distinguishes V6 from V8 while
  selecting the manifold sprite. `ENGINE_V6` has the right domain and same
  numeric value; leave this unrelated identifier use for a separate change.
- `RACE.C:925-926` local `car` is a `Node *` pointing to the owned tires, not
  a car. Its wear and variant determine traction. Leave local naming alone.
- `GAME.C:469-495` local `used` is true for part adverts and false for car
  adverts. It means a part purchase branch, not whether the merchandise is
  used; leave the function's locals for its own investigation.
- `Node` is an owned part with wear/value/catalog index, not a generic node;
  `INPUTDAT` also owns game catalog tables beyond input. Neither is renamed.

## Model

[Original investigation](Model.md)

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

## Racer

[Original investigation](Racer.md)

- RACER_COP_ENTRY (BOBS.H:160) names slot 10 as though it were a dedicated cop
  record, but SAVEGAME.C:106-107 gives it an ordinary medium-group rival car.
  Police borrows its behavior in RACE.C:1351-1354, not its car/identity.
- g_driverIdxSelVal is the selected rival's array index (BOBS.C:342), not a
  player driver setting; g_resultWin is player victory (RACE.C:671,687-688).
- g_carDraw.overlayPic is the in-car portrait source in CARPIC.C:335-338, even
  though the generic name can also describe other image overlays.
- RIVAL_STICKER_CHANCE et al. are random upper bounds, with rank-based values
  compared against them (SAVEGAME.C:73-79); they are not percentages themselves.

## Spec

[Original investigation](Spec.md)

- The spec-index and manifold naming issues have been resolved: `SPEC_KING`
  names the King's Corvette spec; `MANIFOLD_TRI_POWER` is variant 2 and
  `MANIFOLD_DUAL_QUAD` variant 3. Picture 1079 is Tri-Power and 1083 is
  Dual-Quad. See [misnames.md](misnames.md) for independently checked evidence,
  including the police code's reuse of the King's performance specification.
- `item` in build_rival_pool (`BOBS.C:276,281`) receives a specification pointer
  that is never read. No deletion or local rename is needed for this task.

## Sprite

[Original investigation](Sprite.md)

- `g_mainCarSprite` is also the carhop, tire, jack and slide Sprite, as proved by
  every sp_setup call above. Its car-only name overstates its scope.
- `Frame.pos` is the actual far frame bitmap, not a position; setup copies an
  image pointer and the backends read its dimensions/bits (`SPRITE.C:93`;
  `EGASPR.C:52-68`; `CGASPR.C:53-76`).
- `Frame.data` is backend-dependent band data: EGA casts it to Mask, while
  CGA/Tandy read it as a Bitmap (`SPRITE.C:94-98`; `EGASPR.C:64`;
  `CGASPR.C:52,57,69`; `TANDYSPR.C:55,60,79`). Its declared Bitmap type does
  not describe the EGA interpretation. Do not propagate that type into new docs.
- `tb_compose` parameters `dst` and `src` are opposite their actual roles:
  dst supplies frame bits and mask, src receives the writes (`TANDYBLT.C:76-120`).
- `sp_reverse`'s existing heading comment is misleading: it reverses frame-ring
  traversal and chooses a phase from screen X, rather than reading a heading
  (`SPRITE.C:180-197`).
- `bm_copy_box`'s parameter names dst/src are reversed relative to its copy
  calls (see its implementation before reusing those labels); callers in the
  sprite engine pass the source first (`SPRITE.C:291-295`).

## ch_skate

[Original investigation](ch_skate.md)

- `sp_car_clip`'s comment claims screen-window clipping (SPRITE.C:34), but lines
  40-48 intersect against the retained car image, position and visible height.
- `grab_strip`'s comment says it draws then copies to front (CARHOP.C:58); its
  implementation captures from front on EGA or back via pu_grab (64-71).
- `ch_save_bg`'s comment says it clears a 200x68 rectangle (181); it saves 48x41
  pixels from screen coordinate (200,68) instead (187-193).
- `cp_patch_bg`'s comment says seven frame cells go to front (CARPIC.C:609-610);
  lines 629,638 write the car backdrop, with seven jail-bar strips read from
  front only at the jail. Its parameter `dst` is the source image, and `p8` is
  unused (612-640).
- `eb_copy` calls its first parameter `dst` and second `src`, but its reads and
  writes show the first supplies pixels and the second receives them
  (EGABLIT.C:41-57). The video hook is source-first.
- `g_autoSprImagePtr` is vague: CARPIC.C:571 stores the retained last car frame
  in it, which sp_car_clip and this function use.

## show_rival_car

[Original investigation](show_rival_car.md)

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
