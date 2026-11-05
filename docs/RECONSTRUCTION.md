# Reconstruction choices

The rebuilt `SR.EXE` is byte-identical to `orig/SR.EXE`, but the EXE does not record
everything a source tree contains. This file lists what in `src/` is a choice made by the
reconstruction rather than a fact recovered from the binary. Each entry says what fixes it
and where it stops. Anything not listed here (code, data bytes, object boundaries, link
order, per-file compiler flags, the tools) follows from evidence in the EXE. It is checked
by `./sr build`, `./sr check --all` and `./sr dgroup check`.

## Names

- **Functions and globals.** `F_xxxxx` and `g_xxxx` are addresses. The other names
  (`ga_day`, `sy_wait_input`, `g_lib1Name`, ...) were given from what the code does
  (values read and written, callers, strings, ports, data), following CLAUDE.md's naming
  conventions; they describe behaviour and are not the original identifiers. Only the
  runtime's names (`./sr crtnames`) come from a library. Name lengths are not free: MSC's
  external-symbol budget (CLAUDE.md 4d) counts them, so GARAGE.C and VIEW.C keep
  placeholders for most of the symbols they use, and some public names are shorter than
  they would otherwise be (VIEW.C rather than ROADVIEW.C, `hide_spots`).
- **Communal storage** (DS:6E60..8F02, the far communals of `SLOTS.C`) and **file statics**
  (DS:6C22..6E2C). These names are made up, but chosen so that LINK's symbol hash
  (communals) and MSC's static-ordering key (statics) put every block at its original
  address (CLAUDE.md 4e/4f). The constraint fixes each name's hash bucket, not the name.
  Placeholders of the same length were kept where possible. Where a descriptive name
  was found that keeps the bucket or key order (`./sr fit`), it replaced the placeholder;
  its spelling (`g_drawnMinX`, `g_sprPageRing`) was picked to fit.
- **Unused global names.** Globals with no evidence-backed meaning use `g_unusedN`.
  Numbers 1..24 follow the former placeholders in ascending DGROUP address order;
  positions 2, 18 and 19 instead received `g_dashViewSaved`, `g_carPulledIn` and
  `g_tireFactors` from their writes and indexing. Missing numbers are intentional.
  Five unreferenced HOTSPOT communals use `g_unused596`, `g_unused696`,
  `g_unused1239`, `g_unused1449` and `g_unused34`, respectively in address order.
  Their suffixes satisfy LINK's bucket windows rather than encode addresses or
  sizes; the first two retain bucket ties, verified by the packed build.
  `g_unused24` preserves the POPUP static ordering and its full PROVISIONAL marker.
  Here unused includes storage written but never read; the names claim no original
  purpose. The tire factors {9, 8, 5} feed drivetrain slot 5, but its intended
  gameplay interpretation remains unproven. Identifier growth is checked by the
  full function gate, including GARAGE.C and VIEW.C.
- **Local variables.** MSC's slot order depends on a hash of the names (CLAUDE.md 3).
  Names with digit suffixes come from `./sr layout -w` and only reproduce the frame.
- **Initialized file-private data.** The 193 C initialized globals used by a single
  source are `static`; MSC emits the same data and code bytes, with private names
  recovered from the compiler listing for verification. Their linkage is a source
  reconstruction choice. The 21 scenery `ScenerySprite` tables retain public linkage: MSC
  rejects their static forward declarations followed by initializers (C2086).
  Declaring the initialized tables before the referring `SceneryLods` records preserves
  loaded bytes but changes relocation emission order (256 differing packed EXE
  bytes), so the exact packed-file gate requires the existing order and linkage.

- **Struct types and members** (shared types in their producer/API header;
  module-private records in their module header). Sizes and offsets come
  from the code; tags and member names are invented. Explicit unused or reset-only fields use `unused` / `unusedN`; legacy `pad` /
  `fNN` names also remain where meaning is uncertain. Explicit spare storage is distinct
  from the compiler's automatic alignment padding.
- **Files.** Each object's file is named for what its code does (GARAGE.C, DISK.C,
  CGABLT.C, ...); SCENERY.C/SCENERY2.C and the `src/DATA/` names likewise describe their
  data. These are labels for recovered objects: a file boundary is evidence, its name is
  not (a source path only changes an object's THEADR record). INPUTDAT.ASM in
  `src/INPUT/` labels the shared joystick, keyboard and cursor state and tables. The
  same object also holds game part/fit/picture tables, win counters, display labels
  and list storage, plus four uncalled BIOS video-mode store/apply helpers. The name
  describes its shared input-data role without claiming an original filename or
  splitting the recovered object. The renamed object still produces an identical
  packed EXE; its THEADR filename does not reach the executable.
- **Code segments** `S<region>_TEXT` (`/NT` in the MAKEFILE, `.CODE` in assembler files).
  Which objects share a segment is recovered; the name is the segment's paragraph.
- **Assembler labels.** `PROC` names are the functions' names. Local labels are made up
  (`done`, `next`, `row`, ...) and get a number where one file uses the same name twice.
  Unnamed strings in assembler data are `S_<offset>`; unproven key-map lists
  remain `K_<offset>`, while proven screens have descriptive `K_` labels.
- **Folders.** SYS, UI, GAME, VIDEO, CGA, EGA, TANDY, HERC, INPUT, SOUND and DATA
  are the user-decided organization, not recovered original directories. Same-name headers
  live beside their sources; shared headers live in SYS. SRENV.BAT supplies INCLUDE for
  both compilers without adding long /I lists to DOS command lines.
- **Headers.** One hand-written header per source file (`src/<folder>/<FILE>.H`)
  with shared type definitions beside their producer/API and private types in their module, and a tiny
  `STRUCTS.H` for ownerless helpers, is a plausible organisation, not a recovered one.
  The ownerless helpers are `union Word` (a word/byte view) and `struct Pair`
  (two ints used for cursor scaling, shifter positions and jail gaps). `union Word`
  has no global instance or producer API:
  GARAGE reads paint bytes, SOUND forms a speaker divisor, and SYS forms timer
  bytes independently. Giving one of those modules ownership would add an
  unrelated API dependency to the other two.
  `./sr headers` maintains source includes and MAKEFILE dependencies only.
  Unused declarations cost nothing (rule 4d), so the include lists cannot be checked
  against the code.
- **Annotations.** Original addresses and function extents are kept in `tools/sr/symbols.json`, separate from the source. Reconstruction explanations live here, outside the sources; game-behaviour comments and unresolved PROVISIONAL markers remain in source.

## Source forms

- **`BACKDROP_ROWS`** is a choice among byte-identical literal, `#define`, macro, `sizeof`-record and constant member-offset forms.

- **Equivalent forms.** Wherever several C forms compile to the same bytes, the source uses
  one of them. Some arbitrary picks: the equal-arm ternaries
  `!x ? 1 : 1` in sy_drive_isr (the condition leaves no code), `case -2: case -1:` in gm_run
  (any run above -3 gives the same code), the empty increment of `for (; *str; str)` in
  eg_draw_text, and `i = 4` after swap_wheel's loops.
- **dr_bevel's `int bm = x_colors.c[4] == 2 ? 3 : 3;`** (DRAW.C). The original stores 3
  through `ax` and reloads `bm` from memory (including a `bm * 2` CSE in `si`), so the
  constant was not propagated; a plain `int bm = 3` (also as a statement, `volatile`, any
  flags) is folded. An equal-arm `?:` gives exactly that: the compare goes, the value stays
  in `ax` and blocks propagation (as in sy_drive_isr). `if (bm = 3)` also matches. It reads as a
  leftover of a style-dependent border width; the condition and the arm values are a
  guess, only "a ?: or assignment-condition with value 3" is evidence.
- **Block-layout forms.** MSC 5.1 emits basic blocks in source order (with cross-jumping),
  so a jump the original's layout shows has to be somewhere in the source. Where no
  structured form gives the layout, the source has a `goto` or a particular loop form.
  74 `goto`s in 11 files; the layout-driven ones: bo_visit's `while` whose body ends in a redundant `goto test;` (BOBS.C: the
  original's body-before-test layout, and it keeps `/Ol` from enregistering `n`),
  ra_outcome's `goto finishstop` from both arms, ra_reset_eng's `goto nx;` to the label right
  after its `else`, anim_run's `goto next;` (a `continue` there would pull the loop
  increment up), mn_draw_hot's `goto box;`, mn_hit's `goto next;` into a then-branch,
  mn_list's `goto restore;`, load_pic's `goto done;` (a `break` moves the after-loop block),
  fill_quad's goto loop (escapes `/Ol`, see `loop_opt` below), render_mirror/vw_draw's `goto next`,
  cp_show_car's `again`/`done`, sg_save's shared retry tail; and the loop forms
  `for (;; g_inputKey = 0)` in ed_line (the increment gives the `jmp test` entry) and
  av_open's accumulation inside the inner `while` test (keeps `cx` for the outer loop).
  That the original had a jump at each of these points is evidence; the exact statement
  (a `goto`, a `continue`, a differently nested `if`) is not.
- **`#pragma loop_opt`** in SYS.C (off before sy_demo_step, to the end of the file),
  GAME.C (off/on around sell_parts) and EGABLIT.C (off before eb_latch_pack). These five
  functions keep plain memory loops that the file's `/Ol` would otherwise optimize; the
  rest of each file needs `/Ol` (13 functions break without it). `volatile`, `register`,
  static or global counters, taken addresses, calls, intrinsics and `interrupt` do not stop
  MSC 5.1's loop optimizer (it ignores `volatile`); it only optimizes a loop entered at
  the target of its back edge. eb_latch_pack also matches without the pragma with its row loop
  entered at its test (`row = 0; goto test; next: row++; src += pitch; test: if (row < h)
  { ...; goto next; }`); for the other four the only matching goto forms are jumps to the
  next statement. The pragma is kept as the more plausible source everywhere: it is MSC's
  documented mechanism, the same files use it, and eb_latch_pack has a reason (as `rep movsw`
  its write-mode-1 latch copy would move words). Where exactly the pragmas sat is unknown: in SYS.C the `off`
  fits anywhere after sy_timer_cancel, and GAME.C matches equally as `/Oasi` with one
  `loop_opt(on)` after sell_parts.
- **`GCREG`/`SEQREG`** in EGASHIFT.C. ex_blit needs a block boundary after `GCREG(5, 0)`.
  Braced register-write macros give it. Their names, and the fact that every other port
  pair of the file uses them, are a reconstruction (braced and unbraced pairs compile
  the same everywhere else).
- **ga_transmission's `int flags[3]`** (GARAGE.C), indexed by key 1 and 2. The element that is
  never touched, `flags[0]`, fills the original's unused 2-byte frame slot. This is an
  interpretation (it matches `flags[k--]`) and is not proven.
- **`/Oi`, not `#pragma intrinsic`.** No compiled C code in the EXE calls a library routine
  that has an intrinsic form (only the runtime calls `strcpy`, `strlen`, `memcpy` and
  `memset`), and every C file matches with `/Oi` added to its flags. The 18 files whose
  inline `inp`/`outp`/`abs`/`_rotl`/`_rotr`/`memcpy`/`memset` came from a
  `#pragma intrinsic` now have `/Oi`; the 7 that had both lost the redundant pragma. The
  bytes do not show which of the two the original used. The other 20 C files without
  `/Oi` use none of these functions, so their `i` is unknown.
- **PROVISIONAL dummies** (each is marked in the source). They reproduce something in the
  EXE whose original text is unknown:
  - vi_anim_hook's unused inner-block `struct Colors save` (VIDEO.C): a 6-byte frame slot.
  - `long unused` in sc_picture (SCREEN.C), gx_draw_bolt, gx_draw_part and gx_erase_part (GARGFX.C): a
    4-byte slot above the `struct Blt` local.
  - `x_trk0`/`x_trk1` (DASH.C, labels in `src/DATA/NEEDLE.ASM`): two unreachable far
    accesses for the two SEG constants in CONST (DS:68E6, DS:68E8) that nothing reads.
    MSC 5.1 experiments (2026-11-04, `/AM /Oalsi /Gs /FPi`) did not recover a natural
    replacement. The live code uses only DS:68E4, with offsets 0/1 and 0x106/0x107.
    Two separate tip/hub arrays require another ES reload: both functions grow from
    155 to 159 bytes and emit only two needle SEG words, not three. Without `/Oa`
    they grow to 172 bytes; `/Gt1` is ignored in medium model; `/NDNEEDLE` changes
    the calling/data assumptions and gives 166 bytes. A `huge` table gives 257
    bytes and no needle CONST word. A far struct containing both arrays, or a far
    array of two-byte point structs, preserves live code but still emits just one
    word when the unreachable accesses are removed. Global, file-static and local-static
    far pointer initializers emit four-byte `_DATA` pointers, not spare CONST words;
    unused automatic far pointers retain a four-byte frame slot and immediate SEG
    stores. Unused scalar reads (also `register`) retain code/frame storage. A
    discarded read, `sizeof`, an overwritten output store and an overwritten
    self-subtraction emit no spare word; an empty `if (far_read) ;` retains segment
    loading code. Both `if (0) far_read` and `if (0 && far_read)` leave unused SEG
    words, like the existing post-return access. Thus unreachable control flow is
    a measured explanation, not proof that the original contained `return` there.
    The original expressions, alias offsets and symbol names remain unknown.
    A second pass (needle2, 2026-11-04; 112 compiler jobs) found that a
    constant `if/else` with the live arm first also preserves all segment bytes
    and the target-resolved fixup sequence; constant `?:` table/value selection
    removes the spare word. This is a compiler mechanism, not a recovered table
    selection: selecting separate speedo/tach tables in the two functions emits
    only two needle words, with a different live word in each function. Declaring
    both far arrays together shares one segment constant, also insufficient.
    Disabled feature/type-size branches and early-break macro forms can preserve
    the layout only when supplied the same unsupported alternate symbols.
    No such replacement was applied. The original relocation stream contains
    DS:68E6, DS:68E4, then DS:68E8 consecutively; it carries segment values, not
    symbol offsets or names, and does not identify which table an unused word
    named. `_x_trk0` at offset 0 and `_x_trk1` at 0x106 are reconstruction labels,
    not evidence that the unused words originally named the tip and hub. The full
    experiment matrix and comparison limits are in the needle2 diary entry.
  - `NeedlePoint`, `NEEDLE_TIP`/`NEEDLE_HUB` and gauge centre constants describe the
    observed table and coordinate transforms, not recovered original declarations.
    The tip reaches radius 18 and the hub radius 5; the latter includes axial
    `(0,5)` and `(5,0)` samples. The point struct preserves the complete object code
    and the existing CONST fixup order (offsets 2, 0, then 4).
  - `g_unused24` (POPUP.C): four unreferenced `_BSS` bytes after `g_popupLayout`.
  - The discarded `g_list[0]` reads in the default cases of your_cars (GAME.C)
    and sg_load (SAVEGAME.C) are reproduced by ordinary range checks ending in
    `break`: `if (item > 0 && item <= g_list[0]) break;` and
    `if (r >= 1 && r <= g_list[0]) break;`. Both paths leave the switch, so MSC
    drops the upper comparison and transfer but keeps the count load after the
    lower test. These replace the former PROVISIONAL self-assignments; each
    complete raw OBJ is byte-identical to its baseline under the exact file
    flags. The bounds checks are redundant and do not reject invalid keys;
    their historical intent and unique original spelling remain unknown.
    Replacing `> 0` with `>= 1` changes code generation here. See
    plans/investigations/glist-loads.md for the complete trial matrix, comparison
    scope and raw-object hashes.
- **The hook installers** cg_install (CGA.C), ta_install (TANDY.C) and eg_install
  (EGA.C): `g_videoOps = g_cgaHooks;`. Their bytes are
  MSC 5.1's struct assignment (`push di; push si`, no frame, `mov cx,1Eh` and `f2 a5`:
  MSC writes `rep movsw` with the F2 prefix, MASM's `rep` is F3; the `memcpy` intrinsic
  adds `shr cx,1 ... rep movsb`). So the original assigned one 60-byte struct: the hook
  block DS:78A2..78DE was a struct, and each driver's table probably one too. The
  reconstruction uses one `struct Hooks` object and typed driver tables; the object's
  name fits LINK's communal hash window (CLAUDE.md 4e). Each installer
  is the first function of the driver file whose table it copies (that file's table
  needs no forced owner then). The EXE does not decide this placement: none of the three
  has a segment relocation, so a one-function file of its own, or for ta_install and eg_install
  the end of the C file before (TANDYBLT.C, TANDYSPR.C), gives the same image.
- **Mouse state** (`struct Mouse g_mouse`, DS:05E2..05F3). Eight ints and a
  present byte are a natural record shared by SYS and MOUSE. Separate globals
  produce identical code too: neither operand order nor the compiler budget
  proves the original declaration. The unreferenced byte at 05F3 is represented
  by MSC's zero tail padding. The neighboring input flags, unused 05F4 words and
  clocks stay separate; folding g_tick while keeping the earlier deadlines plain
  reverses comparison operands in SYS and DRVTICK. The experiments and limits of
  this conclusion are summarized in plans/diary.md (2026-11-05).
- **Evidence-only structs** `RoadEdges`, `RoadView` and `RivalPosition`. MSC's operand order
  shows that these blocks are struct members, not plain variables (recipe book). Their
  extent and member types are a reconstruction.
- **Budget structs and message offsets.** `struct DriverState` (`g_driver`, DS:7EAA..7EC1) and
  `struct CarDisplay` (`g_carDraw`, DS:8272..828F) each fold 11 placeholder globals. GARAGE.C
  (one object, as the relocation table requires) loses ga_transmission's loop register variables
  once it references more external symbols than MSC 5.1's budget allows (CLAUDE.md 4d), and
  these two COMMON clusters as structs bring it under. For the same reason ga_body
  (GARAGE.C) addresses its four messages as offsets into the message block,
  `g_txt + 1283`, `+ 1299`, `+ 1318`, `+ 1337` ("Chop roof - $70" ...), not as named
  globals; the game addresses messages by offset into g_txt everywhere (`Hotspot.textOffset`, the
  argument of gm_message), so the original may have had named constants there. That the
  blocks are grouped is evidence; the member list (a wider CarDisplay changes ga_hood's
  loop temps, so the arrays from DS:8290 stay separate) and the spelling are not.
- **Per-module type views.** VIEW.C reads the char `g_runRacingFlag` (DS:8ACA) as a word, so it
  writes `*(int *)&g_runRacingFlag`. The original file probably had an `extern int` of its own
  there. `struct Win` (MENU.C, SAVEGAME.C) is a second view of the 18-byte hot-spot record
  `struct Hotspot`: `g_listLineBasePointerValue` holds the address of the first menu-line record as an `int`
  (`g_listLineBasePointerValue = (int)p`) and the code computes `(struct Win *)(0x12 * sel + g_listLineBasePointerValue)`.
  Declaring it `struct Hotspot *` and writing `g_listLineBasePointerValue + sel` makes mn_list 10 bytes shorter,
  so the int arithmetic is evidence; that there were two struct names is not.
- **Return types the code does not decide.** `main` (MAIN.C) and sc_picture (SCREEN.C)
  are `int` but end without a `return` (C4035); sc_picture_at returns sc_picture's value. ra_outcome
  (RACE.C) returns 1 on one path, a value its caller stores, and `return;` on another
  (C4033). `void` and `int` give the same bytes for the first two; the source keeps `int`
  (`main` by convention, sc_picture because sc_picture_at passes its value on).

- **Hooks** (`g_videoOps`, DS:78A2..78DD): supported by the three MSC struct-copy installers.

- **RenderRows** (`g_renderRows`, DS:6E08..6E29): a natural, byte-identical reconstruction choice; separate globals also match.

- **TireWork** (`g_tires`, DS:6CD4..6DFD): a natural, byte-identical reconstruction choice; separate globals also match.

- **TextEnd** (`g_penPoint`, DS:824C..824F): a natural pair for the last drawn text endpoint, written by the drivers and read by ADS/MENU/EDIT.
- **StatusText** (`g_statusLine`, DS:8250..8259): a natural record for status-line placement, dimensions and display state; separate from the generic text endpoint.

- **BodyPoints** (`g_featurePos`, DS:8264..8271): a natural, byte-identical reconstruction choice; separate globals also match.

- **Right scenery cursor** (`g_hereRoadsideRightRowPtr`, DS:8634..8637): a separate right-side terrain/scenery cursor used by both forward and mirror views.
- **Mirror road Y projection** (`g_mirYProj[8]`, DS:8638..8647): a full eight-row array matching its writer; the array form is byte-identical.

- **MirrorEdges** (`g_mirGeom`, DS:864A..8669): a natural, byte-identical reconstruction choice; separate globals also match; both member arrays contain all eight rows written by the projection loop.

- **Forward building rows** (`g_yBuildingTopArray[15]`, DS:7AB0..7ACD): one full-length array matches its fifteen-entry writer and is byte-identical; the preceding backdrop reveal height remains separate.

- **ListDialog** (`g_menuList`, DS:8B9A..8BA3): a natural, byte-identical reconstruction choice; separate globals also match.

- **TimerPriorities** (`g_sysTimerPriorities`, DS:8EF6..8EFD; type in SYS.H): a natural, byte-identical reconstruction choice; separate globals also match.

- **PartSelection** (`g_slotSelec`, DS:82AC..82B3): a natural, byte-identical reconstruction choice; separate globals also match; the explicit unused two-byte word is an `int`, not automatic padding.

- **InputState** (`g_input`, DS:05D0..05D3): natural record of the any-input latch and input key only; the unused sentinel and drive-reset words at 05D4..05DB stay separate with neutral placeholder names.

- **Wager** (`g_raceWagerTerms`, DS:79A2..79A5): a natural, byte-identical reconstruction choice; separate globals also match.

- **Blt** (`g_adPacket`, DS:0522..0535): a natural, byte-identical reconstruction choice; separate globals also match.

- **TimerQueue** (`g_timerQueue`, DS:0566..0571): the timer insertion/removal/reset paths share the latest-deadline chain head, earliest entry, free-list head, next deadline and busy state. The reset-only byte is explicit unused storage; timer entries and later spare words stay separate. Separate globals also match; the control record is a natural reconstruction choice.

- **GameClock** (`g_gameClock`, DS:05FC..0607): ga_day and elapsed-time accounting compute actionTicks + biosTicks - timeBase. SAVEGAME resets and serializes that same clock, while SYS advances biosTicks. g_tick remains a separate scheduler clock. Separate globals also match; this grouping is a natural reconstruction choice.

- **GasInteraction** (`g_fuelStationActions`, DS:6CA4..6CA9): clear_gas_spots resets all three interaction flags together; the cap, nozzle and rag handlers set them and ra_gas_station tests them when enabling the matching hotspots. Separate globals also match; this grouping is a natural reconstruction choice.

- **PicturePlacement** (`g_picturePlacement`, DS:04BE..04C3): Screen picture drawing uses the x/y destination and maximum height together to clip and place one bitmap. Separate globals also match; this grouping is a natural reconstruction choice.

- **AnimRun** (`g_animRun`, DS:6C38..6C3D): anim_run and anim_set_box use command, object and box pointers as the current interpreter position. The pump hook countdown (`g_pumpBlinkCount`, DS:6C3E) persists across calls and stays separate. Separate globals also match; the interpreter record is a natural reconstruction choice.

- **CursorDraw** (`g_drawnCursor`, DS:6C46..6C4B): SYS saves the drawn cursor position and tracks whether it needs repainting as one cursor drawing state. MSC supplies the final byte of alignment padding. Separate globals also match; this grouping is a natural reconstruction choice.

- **Vibrato** (`g_vibrato`, DS:6C52..6C55): sy_timer_isr uses gain and divisor together to modulate PIT channel 2. Separate globals also match; this grouping is a natural reconstruction choice.

- **RandomState** (`g_random`, DS:6C62..6C67): rn_int seeds and updates all three Wichmann-Hill generator words together. Separate globals also match; this grouping is a natural reconstruction choice.

- **PicLibSelection** (`g_picLib`, DS:6C6A..6C71): open_lib selects the count, handle, library path and other handle together before picture reads. The initial DOS drive (`g_initialDrive`, DS:6C68) and hot_data path (`g_dataPath`, DS:6C72..6C81) are separate session and game-data state. Separate globals also match; the selection record is a natural reconstruction choice.

- **ViewPages** (`g_viewPages`, DS:5E3E..5E41): flip_page swaps draw and show page indices together and select_draw_page consumes the pair. The preceding unused word stays separate. Separate globals also match; this grouping is a natural reconstruction choice.

- **RivalPace** (`g_rivalPace`, DS:6CAC..6CB3): ra_reset_eng and cop_appears derive fast and slow speeds from the top speed; rival_accelerate selects those targets or the player speed plus speedMargin. Shift penalty (`g_shiftPenalty`, DS:6CB4), shift point, target lane and RPM band stay separate. Separate globals also match; the speed-target record is a natural reconstruction choice.

- **WheelPoses** (`g_wheelPoses`, DS:6C22..6C33): da_load_wheel caches three steering-wheel bitmaps and their source Y coordinates. da_draw_wheel chooses pose 0, 1 or 2 from the steering sign and blits that cached bitmap when the pose changes. Shifter state stays separate. Separate globals also match; the cached poses are a natural reconstruction choice.

- **ShifterDraw** (`g_shifterDraw`, DS:6C34..6C37): da_init_shifter selects a shifter track and resets its knob position; da_draw_shifter uses both to restore the old knob and draw the new one. Separate globals also match; this grouping is a natural reconstruction choice.

- **ViewClip** (`g_viewClip`, DS:5E4E..5E55): VIEW and the CGA/EGA/Tandy span assemblers use all four bounds as one clipping rectangle. Assembly references use a WORD external with the corresponding member displacement. Separate globals also match; this grouping is a natural reconstruction choice.

- **DemoControl** (`g_demoControl`, DS:06A4..06AD): sy_demo_step shares key-down, movement, count, previous group and auto-shift controls with SYS input and interrupt paths. The leading unused word stays separate. Target X/Y have the same demo-movement role, but folding them into this record reverses the cursor/target operands in sy_demo_step and sy_timer_isr (four wrong fixup references in each). Adding the script position too produces the same failures. The targets and script index therefore remain separate to preserve byte identity; this boundary is a compiler constraint rather than a semantic distinction. Separate globals also match; this grouping is a natural reconstruction choice.

## Compiler warnings

The build compiles with `/W3` (see Build). The C files give 109 warnings; at `/W1` only
the two C4047 remain, at `/W2` 99 (no C4100, C4101):

- C4051 "data conversion" (94, in 24 files): mostly an `int` value stored into or passed
  as a `char` (colours, flags, drive letters), a few `int`/`long` mixes. Believed original:
  casts would not change a byte, and the original need not have compiled at `/W2`.
- C4100 "unreferenced formal parameter" (5): ga_check_end `n`, gx_trans_clear `n` and `flag`, gx_erase_part
  `flag`, cp_patch_bg `p8`. Their callers push these arguments and the callees ignore them; the
  call sites decide the parameter count. Believed original.
- C4101 "unreferenced local variable" (5): the PROVISIONAL frame-filler locals above.
  Reconstruction.
- C4035 "no return value" (2) and C4033 "function must return a value" (1): the return
  types above. Believed original.
- C4047 "different levels of indirection" (2): mn_draw_hot's
  `g_numPageRow = g_listLineBasePointerValue = g_hotTextFld = 0;` assigns the `struct Hotspot *` through to the two ints
  (int-held pointer, see `struct Win`), and sp_setup's
  `g_frmTable[i].data = r_vid.mode == -2 ? cell++ : &g_bandRecordsSprArray[i];` mixes a `char (*)[8]` cell
  with a `struct Bitmap *` in one `Frame.data` pointer that restore_rect/restore_rect_back read as a
  Bitmap and restore_rect_latch as a Mask. Both need a cast or a different type in one place, and the
  code does not say which; they are left as written.

## Data

- **Which unit owns which data.** For most items the literal pools and initializer order
  decide (CLAUDE.md 4b). The `FORCE_OWNER` ranges in `tools/sr/dgroup.py` are judgements,
  one per range:
  - DS:4718..4AFE goes to INPUTDAT.ASM: joystick/keyboard/cursor state and tables
    shared with the assembler input handlers, BIOS mode pairs, and game tables,
    counters, labels and list storage. The FORCE_OWNER key remains the first code
    address, 0x02B9A; renaming the file does not change this ownership choice.
  - DS:5AEE..5B04 goes to VIDDET.ASM: after the bytes its video detection writes, one
    byte and three strings ("Tandy", "Tandy", "CGA") that no code references, before the
    hook table of CGA.C.
- **Data-only units** (`dgroup.DATA_UNITS`):
  - HOTSPOT.C (DS:09F4..239E) exists because the functions of the neighbouring code units
    match only without these tables (MSC's frame layout depends on the number of
    file-scope definitions).
  - MESSAGES.ASM (DS:239E..4718) holds odd-address ints and count-prefixed records that C
    cannot place.
  - KEYMAPS.ASM (DS:5580..57F6) holds the keymap lists and their odd-address pointer table.
    Assembler data is evidence; its separation from HERCDET.ASM is a reconstruction choice.
  - SCENERY.C and SCENERY2.C are two files because MSC emits a global before an earlier
    initializer that takes its address.
  The HOTSPOT, MESSAGES and SCENERY splits are evidence; the files' names and contents
  beyond the bytes are not.
- **HOTSPOT.C's indexed table.** DS:09F4..239E contains 365 18-byte
  `struct Hotspot` records. `g_groupHot[ScreenId]` selects a header;
  `drawNext` and link-box `hitCode` values address the complete `g_hots` array.
  `HOTS_*` names identify screen headers, and initializers are grouped by screen.
  Garage's 18 sortable regions include its calendar; the separately drawn car
  name follows. Gas station's six sortable regions include its exit. Engine's
  mutable run has 14 records followed by three fixed control templates.
  The old arrays started at first direct references and often split a screen
  or included several unrelated screens; they did not establish real bounds.
  A complete array reproduces all function bytes, including the unsigned
  address differences in garage/gas lookup and sorting. This cast-and-index
  form is not folded in the measured modules, so these expressions do not prove
  distinct original symbols. One array is the chosen readable representation;
  original names and declaration boundaries remain unknown. The table has
  no pointer fixups; the packed build also verifies relocation ordering in its
  consumers. See `plans/investigations/hottables.md` for the full screen map.
- **KEYMAPS.ASM's key maps.** 42 referenced lists of (scan code, value) pairs, each ended
  by a 0 byte (`g_keyMaps[1..42]`; slot 0 holds the count), for screens plus the
  shared music binding. They are local labels; KEYMAPS.H exports only the first
  as a data symbol, `g_keyMapData`, for the 543-byte area (no C code names it; the DGROUP model needs
  its extent). DS:5727 is a 0 byte between two lists that no pointer names, written as an
  unreferenced empty list.
  The Hercules code never references the maps, and the maps contain only near DGROUP
  pointers, with no MZ relocations tying them to that code. The 630-byte block, including
  its final alignment byte, lies between PERF.C's g_modelScore (DS:556E..5580) and MUSIC.C's
  g_toneShapes (DS:57F6). A data-only KEYMAPS.ASM immediately before code-only HERCDET.ASM
  reproduces the same executable with ordinary `.MODEL MEDIUM` and `.DATA` directives;
  its empty default code segment changes no layout. One file versus two is undecidable
  from the EXE. The split and the order of these two objects are reconstruction choices.
  The `SC_` hardware constants in PC.H/PC.INC, screen enum in KEYMAPS.H,
  decoded result aliases and KEY/ITEMS macros are readability
  reconstructions, not recovered original spelling or macro design. MASM 5.1
  assembles the LOCAL/REPT arithmetic unchanged into the original bytes.
  KEY takes a decoded result and emits `0xCE + result`; ITEMS advances both scan
  and result. Sticker choices use the same KEY encoding, with results -31..-38.
  Arithmetic is emitted directly by DB: an unbracketed expression passed to a
  nested MASM macro is split at spaces.
  PC.H/PC.INC were chosen instead of another standalone header because scan codes
  are hardware facts (CLAUDE.md). KEYMAPS.H owns ScreenId (the shared index into
  g_keyMaps and g_groupHot), InputResult (decoded KR_ values used in C switches),
  and KeyMapByte (only the two raw KV_ values used to patch the repair maps).
  Byte fields stay byte-sized; the menu/editor screen parameters, sy_keymap_set,
  g_keyGroup and the demo's lastGroup use the int-sized ScreenId enum. The header
  tool follows enum tags and
  members as well as macros when choosing source includes.
  Evidence for screen names is each menu caller, its associated Hotspot records and
  g_txt strings, and the decoded newspaper, BOBS and sticker pictures.
  Lists 17 (K_UNUSED_17, empty) and 40 (K_UNUSED_40) have no known callers; the
  names describe that evidence only. List 1 is RACE_OUTCOME, the base menu layer
  ra_outcome installs under its messages. List 42 is the music binding
  installed after every screen by sy_keymap_set, rather than a screen itself.
  kg_key returns char, so sy_wait_input sign-extends negative byte values and
  adds 0x32. Thus CF..DE select items 1..16; CB accepts the highlighted item
  (the caller obtains it through mn_sel), and CA cancels in the load/save/parts
  lists. These codes are context-dependent: CB is also Save Game, CC is Old Game
  or Sell it, and CD is New Game or Switch it, so screen aliases carry those
  meanings rather than assigning a universal action to CC/CD. The outcome map's
  CC/CD are written as decoded -2/-1 because their actions are unproven.
  B4 dismisses the general message, B5 is See it, B6 Offer, and A6 toggles music
  in sy_wait_input. B7/B8/B9 are -23/-22/-21: several screens call these
  different results "Forget it". BA (-20) is OK, Yeah, Go ahead or Quit depending
  on the screen; aliases state the screen's actual action. Engine result 13
  toggles car->wireState, so it is named ENGINE_WIRES.
  C4/C7 are scroll-down/up codes in mn_list, and advance/retard timing in the
  timing screen; timing aliases reuse the scroll result definitions. Sticker bytes
  AF..A8 decode to -31..-38 and select sticker
  numbers 1..8 through ga_stickers' `-30 - key` expression.
  The first spare-parts entry remains first for GAME.C's C/S Change/Sell patch;
  the first two repair entries remain first for RACE.C's Fix/Junk affordability
  patch. BOBS.C's jail setup changes its first G binding to Q and restores it.
  The newspaper's third item stays numeric: the map returns 3 for G/Esc, whereas
  the corresponding garage Hotspot record returns 4. No inferred equivalence is
  encoded in a name.
  C and MASM keep hand-written constants because MASM cannot read the C enum.
  tests/test_keymaps.py checks decoded result equality, the raw repair encoding,
  all screen IDs against their pointer-table slots (including the unused lists),
  and every SC_ hardware constant between PC.H and PC.INC. This prevents drift
  without generating or rewriting the historical-language source files.
- **HOTSPOT.C's filler communals** (`char g_unused596[2]` and the other unreferenced
  tentative definitions at the end of the file). No code references these blocks. Their types and sizes
  are invented to fill the gaps between referenced communals, and their names fit LINK's
  hash.
- **Assembler data form.** MESSAGES.ASM and the `.DATA` of the assembler files are written
  in MASM form: quoted strings, several values per line, `n DUP (v)` for runs, `EVEN` for
  alignment. The DUP and EVEN choices are presentation. DUP produces LIDATA records where
  the original may have had LEDATA, so the objects can differ from the originals in record
  types. The EXE is identical.
- **Far data assets** (`src/DATA/*.ASM`, SLOTS.C). Fonts, screens, gauge needle points, sound and
  roadside sprite spans are written as source records. Their original form is unknown (they may
  have been converter output or binary includes). The segment, file and label names are
  placeholders.

## Build

- **The MAKEFILE's form.** `src/MAKEFILE` is written for MSC MAKE 4.07 in the style of MSC
  5.1's own `STARTUP\MAKEFILE`: `CC`/`ASM` macros, one block per object, header
  dependencies. What it runs is recovered: each object's flags, the link order and options,
  the library. How it was written is not.
- **`/W3 /Fc`** in `CC`. These two options change no object bytes. `/W3` makes a missing
  prototype a warning (C4071) that the lint enforces. The `/Fc` listing is read by the
  verification (locals layout, `_BSS` statics, literal sizes). Both are there for checking,
  not because the original used them.
- **`/STACK:2560`.** The EXE header's SP (0A00h) requires this stack size, but it could have
  come from another source (for example a `STACK` segment, or EXEMOD).
- **The MLIBCE.LIB component order** `MLIBCR+LIBH+EM+MLIBFP` was measured: this order
  reproduces the runtime layout, and the other orders tried differ by 5381..12426 bytes.
  That MSC SETUP built the library the same way is assumed, not documented here.
- **SPEAKER.ASM's `.MODEL SMALL`.** The segment order shows an empty `_TEXT` SEGDEF before
  S2196_TEXT, which `.MODEL SMALL` produces. The exact directive form the programmer used
  cannot be recovered.
- **emu2.** The host build runs CL, MASM and LIB under emu2 instead of DOS. On every source
  emu2 gives byte-identical output to DOSBox-X (`tools/scripts/emucmp.py`), and
  `./dosbox.sh --make` runs the whole MAKEFILE natively in DOSBox-X (IDENTICAL).
- **`orig/SR.EXE`** is itself reconstructed: the freeware EXE with the crack reverted and
  the trailer removed (README). `orig/SR_unpacked.EXE` was made by `tools/unexepack.py`, and
  its header, relocation order and the 14 bytes of filler at the end of its load image
  come from the unpacker, so `./sr linkcheck` reports them as differences.

## Details moved out of source comments

- **Far storage order** (`src/DATA/SLOTS.C`). LINK allocates uninitialized far
  COMDEFs in FAR_BSS after the objects, in symbol-hash bucket order. The recovered
  order is the 277 Slot records followed by five separate 1120-word arrays:
  left roadside flags (`g_roadsideLeftBits`), right roadside flags
  (`g_roadsideRightBits`), horizontal displacement (`g_roadCurve`), shared road
  flags (`g_roadBitsArray`) and height (`g_roadHeight`). Their buckets are
  160,166,188,192,241, after `g_slot` (64); their historical names are unknown.
  Preserve separate definitions and separate extern declarations: `dt_step`
  reloads ES from a different cached SEG word between the two roadside reads,
  even though the arrays share a linked segment. A struct or combined extern
  declaration can change that code. `./sr fit` currently does not model far
  COMDEF placement; inspect the emitted records, calculate LINK buckets and
  require the packed build. Cursor spelling also has to preserve near communal
  placement and the contributing-symbol name budget (CLAUDE.md 4d/4e).
- **Video and cursor records** (`MAIN.H`, `CURSOR.H`). `Video` at DS:822A is supported by
  `/Ol` leaving `r_vid.mode` in memory at 0x004F4 and 0x10AFD rather than hoisting
  it as a plain global. `Cursor` at DS:7AEC has rectangle members whose comparison
  operand order in cu_draw requires nested members rather than separate globals.
  The list-menu `Pages` record starts at DS:04FE. RoadView at DS:7D2E..7D3F
  and RivalPosition at DS:7E8A..7E99 use the struct-member evidence described above.
- **EGA text loop** (`eg_draw_text`). A `for` with an increment that emits no code
  gives the entry jump to the test, then the body tail, then the test with the
  body head. This is the layout behind the equivalent empty-increment form above.
- **EGA block and latch details.** The GCREG(5, 0) braces end the block holding
  `r->sx / 8` in SI, preventing `/Ol` from allocating a stack slot absent from
  ex_blit's original frame. In eb_latch_pack, `/Ol` otherwise turns the inner
  copy into `rep movsw`; every tested for/while/do form acquires register counters
  and loop temporaries. A word write would copy the second byte's latches into
  both bytes, explaining why a byte loop is needed. The pragma/source-form
  uncertainty remains as described above.
- **Speaker segment ordering** (`SPEAKER.ASM`). `.MODEL SMALL` introduces an
  empty `_TEXT` before the other segments, placing the runtime's `_TEXT` between
  S1C11_TEXT and S2196_TEXT. Small model rejects `.CODE name` (A2063), so the
  far code uses an explicit SEGMENT. CGAFONT's segment offset four comes from
  linker alignment. MESSAGES' odd-address words and arrays require assembler
  definitions rather than C definitions.


## Unreferenced zero storage and alignment (padbytes)

MSC 5.1 does not align each string, literal pool, function's literals, or the end
of `_DATA`. Literal bytes are consecutive. Under the actual SYS/FAME/SAVEGAME
flags (`/AM /Oalsi /Gs /FPi`), initialized objects larger than one byte start on
word boundaries, including char arrays and all-char structs. A scalar char,
char[1], and a one-char struct do not require that alignment. Five-char structs
occupy five bytes; any byte before the next word-aligned item is separate from
the struct. LINK aligns the next module's WORD `_DATA` contribution after an odd
segment length. MASM 5.1 does not align a DW after a DB; EVEN is explicit.
These rules were measured with 20 compiler/assembler probes and OBJ/COD/LST
inspection, then tested by independent deletion variants of 13 real-module candidates.

Three declarations were removed, with their externs, symbol metadata and unnamed
rows. The complete packed EXE remains identical:

| Former symbol | Evidence and chosen source form |
|---|---|
| g_501d, DS:501D | SAVEGAME's preceding literals finish at 501D; the 18-byte g_buyCarMsg starts at 501E automatically. Removing the char leaves all `_DATA` bytes and public offsets unchanged. |
| g_4bed, DS:4BED | FAME's final Colors occupies 4BE8..4BEC. Deletion shortens its `_DATA` from 240 to 239 bytes without moving any remaining public. LINK supplies 4BED before the next module starts at 4BEE; this is module alignment, not literal-pool padding. |
| g_5af3, DS:5AF3 | VIDDET's five bytes end at local offset 5. EVEN emits the same zero before the first Tandy string at offset 6. The object data bytes remain identical. The explicit directive is a reconstruction choice; byte identity does not establish the historical spelling. |

Retained storage is not explained by these alignment rules:

| Candidates | Removal evidence / limit |
|---|---|
| g_unused11 | Deletion shifts g_keymapGroup 064E -> 064D and g_keymapSaved 0650 -> 064E; SYS `_DATA` shrinks 435 -> 433 bytes. Its meaning is unknown; the following char does not induce alignment. |
| g_unused17 | Local offset 216 is already even: EVEN emits nothing. Deletion moves the bankroll string and later data; `_DATA` shrinks 998 -> 996 bytes. Meaning unresolved. |
| g_unused16 | Contains 1D, 1E, 1F, not zeros. Deletion shifts the following saved interrupt words back three bytes. EVEN supplies only one byte, not three. |
| g_unused4 / g_unused9 | SYS shrinks by 16 / 4 bytes; already-aligned following data moves by that amount. |
| g_unused20 / g_unused21 | VIEW shrinks by 2 / 8 bytes; g_viewPages / g_sheetRects move back respectively. |
| g_unused24 | POPUP `_BSS` shrinks 6 -> 2 bytes, moving later modules' static storage. Four bytes cannot be a single WORD alignment pad. Preserve its existing PROVISIONAL marker. |
| g_unused1 / g_unused12 | Additional unused zero words found by auditing all source/header symbols. Deletion shrinks MAIN / SYS `_DATA` by two and moves following globals. |
| g_unused596 / g_unused696 / g_unused1239 / g_unused1449 / g_unused34 | HOTSPOT tentative arrays allocate 2 / 6 / 64 / 2 / 2 bytes through COMDEF. They are explicit communal blocks, not initialized-data or literal padding; purpose remains unknown. |

The full audit also found unused nonzero tables, message headers and constants;
being unreferenced is insufficient reason to remove them. Named data in retained
rows may eventually prove to be part of another object or aggregate; no such
interpretation is claimed here. `work/LEDGER.md` records each of the 18 candidates.
DGROUP now recognizes zero bytes explicitly emitted by EVEN in a fresh MASM
`.DATA` listing, relocated by a consistent public-symbol base. It still checks
explicit source globals and does not infer assembler padding from a DW type.


### Could the gaps instead mark file splits?

The code side supports one SYS object at 0D78..2B9A (24 functions). Original
instructions at 0D78 and 2639 both write DS:6C4A, the dirty byte of file-static
g_drawnCursor at DS:6C46; the last function sy_drive_isr also accesses that byte
at 25D2, 25E9, 2601, 2618 and 2655. DS:6C4C is the same file-static g_keyGroup
in sy_keymap_set (12FA), sy_wait_input (142E, 177B, 17F4) and sy_demo_step
(1BB6 through 1FA5). Under the established `_BSS` file-static reconstruction,
these shared private objects and contiguous code rule out a SYS code split at
064D's associated functions. Near-call hulls alone are weaker: `sr module units`
also joins separate modules through explicit near interfaces.

An independent compiler trial placed sy_hover_reset through sy_init in SYSHEAD
and sy_keymap_set through sy_drive_isr in SYSKEY, after SYS in SR.LNK. It produces
a 277-byte first `_DATA`, so LINK could indeed supply 064D, and the suffix's data
has the expected 157 bytes. But the six original `push cs; call near` calls from
sy_wait_input to sy_exit (twice), sy_music_pause, sy_music_resume and
sy_timer_remove (twice) become five-byte external far calls (9A) instead of the
original four-byte sequences (0E E8). Sharing the original private statics would
also require an unsupported interface. This trial was rejected before linking.
Cuts before sy_crit_error (105B) or sy_init (1111) additionally violate the WORD
code-contribution alignment. SYS's two literal pools surround the keymap data:
0614..064C precedes it, and sy_wait_input's six empty literals at 068C..0691
follow it. Moving only the keymap tables to an object after SYS cannot interleave
that new contribution between SYS's own pools. Retain g_unused11 as category (c),
required explicit storage of unknown meaning, rather than claiming a proven
independent historical variable or a compiler pad.

The classification has two unavoidable ambiguities, verified by full trial links
in isolated work directories, without changing src/MAKEFILE or src/SR.LNK:

| Gap | Classification and split trial |
|---|---|
| 4BED | (b), existing module boundary. FAME code is 2FC8..3613; POOLS follows at 3614 with no initialized data; MEM's first literal starts at DS:4BEE. FAME's final data ends at 4BED. Removing the dummy uses the existing link order, without inventing an object. |
| 501D | (a) in the retained reconstruction; (b) is byte-equivalent. Moving only g_buyCarMsg to MSGONLY.C after SAVEGAME.OBJ and before GARAGE.OBJ makes SAVEGAME end at 501D and LINK supply the zero; the packed EXE is IDENTICAL. No functions move, the message has no code users, and there are no message fixups to establish which file held it. No split implemented. |
| 5AF3 | Explicit assembly EVEN in the retained reconstruction; (b) is byte-equivalent. Keep both video functions and five live state bytes in VIDDET; put its three unreferenced strings in VDSTRS.ASM immediately after VIDDET.OBJ and before CGA.OBJ. Without EVEN, LINK pads VIDDET's five bytes. The packed EXE is IDENTICAL. No string code references or relocations establish a separate object. No split implemented. |

For the other initialized candidates (004A, 05BC, 05F4, 06A2, 47F0, 5E3C,
5E94), the proposed gap begins at an even address and occupies explicit storage;
a WORD object-boundary gap can only be one byte beginning at an odd address.
47D1 contains nonzero data. 6C42 is an already-even four-byte `_BSS` allocation,
not WORD inter-object alignment. The five HOTSPOT arrays are separate COMDEF
allocations. All are category (c) for this audit; their exact historical grouping
and meanings remain unproved. The per-candidate ledger distinguishes storage
necessity from recovered variable identity.


## POPUP tail and VIDDET strings follow-up (prov-data)

Neither data marker is settled; both remain verbatim. The complete evidence
and repeatable trial transformations are in
`plans/investigations/prov-data.md`.

POPUP's DS:6C40 word is a near pointer into the initialized three-entry Win
table at DS:04C4, not the Win value itself. All seven original direct accesses
name 6C40; none names the four bytes at 6C42..6C45. No indexed access or
escaped address establishes a larger BSS object. Whole-module trials using
`struct Win *g_popupLayout[3]` with every live use changed to `[0]`, or a
six-byte record with a live pointer and two spare words, both preserve the
code, ordered target-resolved fixups, six-byte BSS and IDENTICAL packed EXE.
Deleting only g_unused24 instead shrinks BSS to two bytes and changes 1426
packed bytes. Thus explicit storage is required, but the bytes cannot decide
between a separate unused object and an unused aggregate tail. Do not replace
the marker with an unsupported array or record claim.

MAIN's actual setup menu owns complete long CGA/Tandy rows and the six-pointer
menu table. Its ROM detector separately passes its own "Tandy" and "TANDY"
literals to ms_find. VIDDET's short "Tandy", "Tandy", "CGA" strings have no
surviving code references or data pointers; their second string also differs
in case from the second ROM signature. The current menu and ROM search do
not explain their historical consumer. Fresh full-link trials confirm that
EVEN, an explicit zero DB byte, and a separate WORD-aligned string-only object
immediately after the five-byte VIDDET contribution all give IDENTICAL packed
EXEs. Deleting EVEN alone changes sixteen packed bytes; deleting it and the
strings changes 9623. The existing alignment choice remains provisional.

### Provisional automatic frames (2026-11-05)

The four `long unused` slots above blit packets and vi_anim_hook's inner
`struct Colors save` remain PROVISIONAL after 157 real-MSC compiler trials.
A hypothetical 24-byte record with a Blt prefix, int[12], or Clip[2] reproduces
all four blit frames, but no use supports the additional fields/elements.
Changing an unused long to an unused far pointer or array also matches and
recovers no meaning. Ordinary dead assignments retain code; a color
save/restore fails, and even a constant-false Colors copy leaves SI/DI saves
and restores. No tested natural, evidence-backed replacement was found.
See `plans/investigations/prov-frames.md` for
source context, passing alternatives, rejected hypotheses and final gates.
