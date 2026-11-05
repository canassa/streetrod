# ch_skate investigation

## Purpose and naming decision

Animate the tray-carrying carhop skating between the left edge and her standing
position, preserving the scene's foreground and any parked rival car.

Keep `ch_skate`: the module prefix and action are accurate. The old definition
comment incorrectly says this animates a moving car body. Replace it with a
concise public contract in CARHOP.H. Keep the already-correct CarhopMode enum.
The second argument is a nonzero car-preservation flag, not a car index or a
second animation mode; call it `carPresent`.

Evidence below uses the source line numbers before this investigation's edits.
This note was written before renaming parameters or locals.

## Inputs, outputs, and lifetime

- `a` / `mode`: CARHOP_ENTER (0) or CARHOP_LEAVE (1), both used by the sole
  caller `maybe_carhop` (BOBS.C:71-106). Selects the right/left sprite sheet,
  start coordinate, motion sequence and final backdrop patch (CARHOP.C:102-174).
- `b` / `carPresent`: nonzero preserves a parked car overlapping the animation
  strip. BOBS.C:84,88,98 forwards `arg`, which always comes from
  `g_rivalHereState` (159-162,193-197). Before the rival animation that flag is
  `animation == CAR_DRIVE_OUT` (157); afterwards it is 0 for driving out and 1
  otherwise (191). Both 0 and 1 reach this function. It controls clipping and
  mask merging (CARHOP.C:109-123,135-136), so an int boolean is appropriate.
- Returns void; no caller consumes a result. Changes displayed scene pixels and
  the car-animation backdrop. Retains the terminal sprite rectangle for leave.
- Requires the persistent 48x41 background saved by `ch_save_bg` (181-194).
  CARPIC.C:461-469 initializes it during CAR_PULL_IN at Bob's or the jail.
  Leave also relies on the rectangle retained by a preceding enter.
- Temporary arena allocations live only until the final `ma_release` (178).
  Shared sprite pointers still point at that temporary storage afterwards;
  callers must set up another sprite before reusing the sprite engine.

## Caller contexts

Only `maybe_carhop` calls this function. It hides the cursor, checks carhop state,
pass count and random percentages, then calls enter (BOBS.C:77-85), optional
quick leave after `da_retrace(100)` (86-90), or ordinary leave (93-101). It then
updates the carhop's presence/order/pass state and restores the cursor (104-106).
`ch_skate` itself does not make those game-state decisions.

`animate_rival_car` tries these operations before a CAR_DRIVE_IN and after car
movement, including immediate pass-bys. CAR_PULL_IN disables the carhop while
preparing the scene and re-enables her afterwards (BOBS.C:155-162,179-197).
The code can use the jail rendering path too; this is not exclusively a Bob's
screen effect (BOBS.C:154,192; CARPIC.C:468).

## Step-by-step behavior and callees

1. Point `bm9` at stack header `hdrz`, discard temporary arena marks, and
   `grab_strip` into a scratch view (CARHOP.C:99-101). MARKS.C:12-16 resets both
   temporary ends to persistent limits. `grab_strip` (60-71) uses picture 1140's
   dimensions, 256x41, but captures scene pixels, not that picture's pixels.
   `me_set_bitmap` (MEM.C:377-390) wraps scratch memory below BACKDROP_ROWS in
   page 2 without allocating pixels. EGA's `ea_rect` reads front-page columns
   0..255 at row 68; the -0xaa0 pointer adjustment compensates its screen-row
   addressing (EGALATCH.ASM). Other modes call `pu_grab` to read the same
   rectangle from the back page (POPUP.C:28-52).
2. On leave, copy the saved 48x41 clean background to strip (200,0), removing the
   stationary carhop from the restoration image (CARHOP.C:102-108).
3. If a car is present, intersect screen rectangle (0,68,256,41) with the car's
   last image and visible height via `sp_car_clip` (SPRITE.C:36-48). Its rectangle
   helper aligns horizontal endpoints to the video byte grid (18-31). Grab the
   intersection from `g_autoSprImagePtr` relative to `g_picXDraw/g_sprDrawY`, then
   composite it at the strip's bottom (CARHOP.C:110-120). Width deliberately uses
   x2-x1, height y2-y1+1; do not silently normalize the asymmetry. No intersection
   leaves a null cutout (122). CARPIC.C:492-494 sets car coordinates/height, and
   567-571 retains the last rendered car frame in `g_autoSprImagePtr`.
4. `cp_backdrop` creates `tab2[0]`, a 41-row restoration bitmap, and `B7`, the
   mode-specific working image (124; CARPIC.C:182-242). Its y1=y0=41 makes the
   source-row offset zero. EGA uses a header viewing scratch video memory as the
   working image; CGA/Hercules allocate a linear image; Tandy returns null and
   draws on the back page. Picture 1140 supplies a foreground mask. Both output
   rectangle pointers intentionally target the same discarded local; the sprite
   later gets its own full-strip rectangle, not these narrow car-overlay rects.
5. On enter, allocate `bm27` (48x41, with mask), copy clean pixels from the saved
   background, and copy the strip's mask at x=200 into it (126-134). This occurs
   before merging the parked-car mask: it preserves the standing-position scene
   foreground for the final patch, independently of a transient rival.
6. If a car cutout exists, `bm_and_mask` ANDs its transparency into the restoration
   mask at the previously established car-copy coordinates (135-136;
   BITMAP.C:272-292). This makes the parked car another foreground occluder.
7. Set `rcb1` to the whole restoration strip (137-139). Cut eleven 40x41 cells
   from the selected 240x82 sheet into slots 1..11 (140; cut_sheet:31-55).
   `di_bm` loads the sheet (DISK.C:377-387); `bm_grab` allocates each cell and
   copies pixels plus mask (BITMAP.C:304-321). Slot 0 is the backdrop, slot 12 is
   unused. The sheet bitmap itself is temporary too.
8. Allocate a masked 48x41 reusable shifted frame. `sp_setup` creates a one-frame
   sprite starting at x=-40 (enter) or x=200 (leave), bottom screen row 108
   (CARHOP.C:141-142). It converts that bottom to top row 68, initializes global
   frame/page rings and returns the shared sprite (SPRITE.C:86-131).
9. Copy all four motion records into writable stack storage; `sp_advance`
   decrements their counts, so the static templates must stay unchanged
   (CARHOP.C:143-146; SPRITE.C:303-320). Enter template is (78,+3), (3,+2),
   (3,+1), (-1,0); leave is (4,-1), (3,-2), (77,-3), (-1,0), with dy=0 throughout
   (CARHOP.C:25-26). The first count includes the initial draw before movement:
   77*3+3*2+3*1 = 240; 3*(-1)+3*(-2)+77*(-3) = -240. Thus both animations
   have 84 draws and end at x=200 / -40 respectively.
10. On leave, seed both page rectangles from `g_carhopRect` so the standing image
    gets erased correctly (147-150). On enter, sp_setup's initial offscreen clip
    suffices. Clear `g_spriteBottomCut` (151), which car drawing otherwise sets.
11. For 84 iterations, read current x, take its sub-byte phase with
    `x & ~r_vid.mask`, and shift the indexed cell into the reusable frame
    (152-155). VIDEO.C:112-136 establishes masks -8/-4/-2 for EGA/CGA/Tandy.
    `bm_shift_right` copies both pixels and mask and can adjust frame width
    within its allocation (BITMAP.C:59-119). `g_carhopFrames` has exactly 84
    entries, all in 1..11; its last entry 11 is the standing/turning pose (21-24).
    Set deadline to current tick+2, call the selected sprite step, then spin
    until that deadline (156-159). This is a minimum per-frame delay, not a
    promise of constant elapsed time; the returned active flag is unused.
12. EGA `es_step` rotates pages, restores exposed background, draws the sprite,
    reapplies the restoration mask over the full strip and advances the motion
    (EGASPR.C:26-99). CGA/Hercules `cs_step` composes into a linear working image,
    applies that foreground mask, synchronizes display copying and advances
    (CGASPR.C:30-128). Tandy `ts_step` composes directly into the back page
    (TANDYSPR.C:35-127). All three restrict jail rendering to gaps between bars.
    The ordinary masked-blit flag 1 selects `(dst & mask) | src`; flag 0 selects
    `(dst & mask) | (~mask & src)` (EGABLIT.C:175-227, CGABLT.C:77-127,
    MASKOR.ASM and MASKBLT.ASM). These operations affect pixel planes, not the
    destination bitmap's stored transparency plane.
13. Call spriteStep once more after the sentinel is reached. EGA's terminal
    path synchronizes the two pages and selects page 0 (EGASPR.C:38-46); CGA and
    Tandy simply return 0. Save the final page rectangle (CARHOP.C:161-162).
14. On leave, patch the car-animation backdrop with the clean saved background
    at screen (200,68) (163-164). On enter, build `t`: clean saved pixels plus
    final frame, then the saved foreground using flag 0 (166-174). Patch that
    standing image at the same position. `cp_patch_bg` clips the source patch
    into the persistent car backdrop relative to bottom row 163; it also copies
    jail-bar strips from the front page (CARPIC.C:612-640). Its final argument
    2 is unused, not a rendering mode.
15. Non-EGA copies the displayed front page to the back page, via
    `cg_copy_page` (CGA.C:380-385) or `ta_copy_page` (TANDY.C:130-132).
    Backend selections are the Hooks initializers in CGA.C:22-24, EGA.C:21-23
    and TANDY.C:17-19. Release temporary
    arena marks (CARHOP.C:176-178); persistent background and scene pixels remain.

## Parameter/local roles and selected names

| Old | New | Role and evidence |
| --- | --- | --- |
| a | mode | Entry/departure choice, 102,125,140,142-149,163. |
| b | carPresent | Preserve/occlude with the parked car, 109-123,135-136. |
| t | patchedBg | Standing-pose backdrop patch, 166-174. |
| r2 | patchBlt | Copy/composition request for standing-position patch, 127-133,167-173. |
| r3 | carCopyBlt | Strip restoration and car capture/composition request, 103-120,136. |
| sp2 | carhopSprite | Shared one-frame sprite returned by setup, 142-162. |
| x | carhopX | Current horizontal position before each draw, 153-154. |
| prc3 | ignoredRect | Aliased, discarded cp_backdrop rectangle outputs, 124. |
| bm9 | captureStrip | Scene strip view and captured pixel source, 99-124. |
| hdrz | sceneHeader | Stack bitmap header for that view, 99-101. |
| okn | stepActive | Unused sprite-step active result, 157,161. |
| clip | carCutout | Masked parked-car fragment, or null, 115-122,135-136. |
| tm7 | nextTick | Frame delay deadline, 156-159. |
| trk7 | moves | Writable four-record motion sequence, 144-146. |
| fr | frame | Reused shifted frame, 141-155,166-172. |
| bm27 | foreground | Clean standing-area pixels with scene mask, 126-134,173. |
| rcb1 | overlayArea | Full-strip foreground repaint rectangle, 137-142. |
| rca2 | carClipRect | Screen-space overlap with parked car, 110-119. |
| i | stepNum | Sequence-copy index, then 84-frame index, 144-155. |
| B7 | canvas | Mode-specific sprite work bitmap/view, 124,142. |
| p9 | motionPtr | Read cursor through chosen motion template, 143-145. |
| tab2 | imagePtrs | Backdrop in slot 0, eleven cells in 1..11, spare slot 12, 124,140,155. |
| v3 | shift | Horizontal sub-byte phase, 154-155. |

Use natural names and declaration order that keep the observed stack offsets;
there is no reason to change the algorithm, data, allocation order or frame.
The renamed function passes `./sr check ch_skate`: 1156 bytes, all 51 references
verified, and the same 214-byte frame with every local at its original offset.
Declaration order breaks ties between equally hashed natural names. The pointer
assignment uses NULL, supplied by the standard stddef.h header; its bytes match.

## Global and indirect side effects

- Writes `g_carhopRect` (162), subsequently read only on leave (148-149).
- Reads `g_carhopBg` saved by ch_save_bg; never overwrites its pixels.
- Reads both sheet descriptors, frame indices and motion templates (20-26).
- Sets `g_spriteBottomCut=0`; all sprite backends consult it. CARPIC.C:440 and
  603 are its other game assignments.
- `sp_setup` replaces global sprite/frame/page rings; steps mutate page rectangles,
  frame state and writable motion counts, and update `g_xCoordSprite/g_spritePic`.
- cp_backdrop changes CARPIC's overlay rectangle heights and can initialize its
  Tandy backdrop width. cp_patch_bg updates its persistent backdrop pixels.
- Reads tick state advanced by timer handlers (SYS.C:873,1010); no timer writes.
- Captures/writes scratch page memory, display pages and final back-page pixels;
  EGA also changes display-page selection and video drawing registers.
- Resets temporary arena markers on entry and exit; leaves persistent limits.
- Does not change g_carhop, order state or rival presence. Does not access
  g_keepMask; that CARHOP-owned variable belongs to car construction elsewhere.

## Edge cases and remaining uncertainty

- Invalid mode is unchecked and indexes beyond the two sheet descriptors. All
  callers supply valid enum members. Leave-before-enter is unsupported.
- With carPresent=0 the uninitialized carCutout is protected by short circuiting.
  With no overlap it is null and mask merging is skipped. foreground is only
  initialized and consumed on enter; both modes are the proven enum choices.
- Allocation failure and stopped/overflowed tick clock are not handled here.
- The extra terminal sprite call is necessary for EGA synchronization even though
  its result is ignored. Do not remove it as a redundant frame.
- No unresolved question about this function's inputs, dataflow or rendering role.
  Original identifier spellings and author intent behind spare storage are not
  recoverable from behavior; no claim is made that the new names are historical.

## Other misleading identifiers/comments (outside scope)

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

## Validation and delivery

Rebased inv-skate with `git fetch origin` and
`git rebase --onto origin/main fb7d753 inv-skate` onto 78df6d2 before the final
gates. No removed song files were used or restored. All following gates were run
on that base with the final source changes:

- `./sr headers --check`: current includes/dependencies and unique ownership.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 match (551 MATCH, 21 MATCH*).
- `rm -rf work/layout && ./sr build`: 96/96 objects pass; packed executable
  IDENTICAL to orig/SR.EXE. All 572 mapped functions have exact raw bytes and
  original positions; all 748 near-call encodings are exact. The build's separate
  unpacked-container diagnostic reports header/relocation-order differences and
  14 missing load-image bytes; the required packed comparison passes.
- `./sr dgroup check`: every initialized global belongs to its owning source.
- `./sr dgroup order`: source link order agrees with recovered order.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`: 104 tests,
  all pass.
- `./dosbox.sh --make`: DOS-native executable IDENTICAL to orig/SR.EXE, SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334.
- `./sr diff ch_skate`: MATCH, 1156 bytes, all 51 references verified, unchanged
  local offsets. Whole CARHOP.C span: MATCH, 1631 bytes.

Logs and decoded picture evidence remain under work/investigations and work/;
this note is also committed as plans/investigations/ch_skate.md.
