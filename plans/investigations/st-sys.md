# System and input type investigations

Investigated in the requested order: Mouse, Link, InputState, GameClock,
DemoControl, Vibrato, CursorDraw, TimerQueue, TimerPriorities, Cursor,
RandomState, Word, Pair, KeyMap and Tone. This note precedes the naming edits.
Source pins refer to the initial `st-sys` revision, `3848b38`; later include
formatting and member edits can shift lines. All paths below are under `src/`.

Only the requested records and their member value sets are changed. Other
misleading names and existing behavior defects are reported below, not fixed.
No field changes size, signedness, order, or spare storage.

## 1. Mouse (SYS/SYS.H)

Purpose: retain driver motion until it produces a cursor/control step, and
expose the latest button masks. `g_mouse` is initialized SYS data at DS:05E2,
18 bytes; SYS.C:68 supplies `{0,0,0,0,0,0,0,10,3}`. The final padding byte is
implicit. `sr data 5e2 18` checks those unusual initial right/present values.
Initialization supersedes present and shifts before normal polling; right is
overwritten by the first button poll. None is loaded from resources or saved.

| Offset | Member | Role and all accesses |
| --- | --- | --- |
| 0,2 | dx, dy | Signed steps from the latest poll. MOUSE.C:64-80 divides the absolute accumulated motion by right shift and reapplies the sign; SYS.C:467-469 uses pixel deltas, SYS.C:1101-1108 uses direction for driving. |
| 4,6 | accX, accY → mickeysX, mickeysY | Signed INT 33h relative motion accumulators (MOUSE.C:36-81). Cleared completely, with no remainder retained, only when an axis produces a nonzero step. SYS reset/init and drive start/stop clear both (SYS.C:138,291,657,683). |
| 8,10 | shiftX, shiftY | Log-two mickeys per output step; MOUSE.C:64,77 reads them. SYS init sets 1/1, driving sets 1/6, returning to menus sets 1/1 (SYS.C:292-293,658-659,684-685). They are scaling counts, not independent mode codes. |
| 12 | left | Truth-valued int retaining MOUSE_LEFT_BUTTON or zero (MOUSE.C:30). MOUSE.C:84 generates click/button state; DASH.C:332 and DRVTICK.C:93 use it for fire; CRUISE.C:24 and BOBS.C:1011 clear it. Use BOOL without normalizing mask writes. |
| 14 | right | Truth-valued int retaining MOUSE_RIGHT_BUTTON or zero (MOUSE.C:31); SYS.C:1110 tests it for acceleration. Use BOOL, retaining the historical initializer 10 and runtime mask 2. |
| 16 | present | Char flag set/cleared by detection (SYS.C:289-295), tested by reset, menu polling, drive transitions/ISR, DASH and DRVTICK. Remains char. |

`mo_motion` reads INT 33h function MOUSE_READ_MOTION, with signed CX/DX;
`mo_buttons` uses MOUSE_GET_POSITION's BL, not button transition counters.
Existing PC.H mask definitions are reused, not duplicated.

## 2. Link → TimerLink (SYS/SYS.H)

Purpose: an indexed timer node, 18 bytes, in initialized `g_timers[4]` at
DS:0572 (SYS.C:55-56). All four nodes initially contain zero, including spare
bytes. No load/save operation. The type name will identify this timer role. `sr rename` rejected the type
name because it only indexes functions/globals; the tag and its four source
references were therefore changed directly.

| Offset | Member | Role, values, evidence |
| --- | --- | --- |
| 0 | nxt → next | Index toward earlier deadlines in the active queue; also links free nodes. Reset writes i+1 (SYS.C:125-126); add traverses/rewires it (565,573,583-584,594); remove rewires/recycles it (151-168); cancel follows it (615). Negative terminates an active chain. |
| 2 | prv → prev | Index toward later deadlines; negative at active head. Written by add and neighbor removal, read by removal (SYS.C:153-166,589-594). Not meaningful while free. |
| 4 | deadline | Absolute game interrupt tick: g_tick + delay (SYS.C:560,581), sorted in add and cached during remove; dispatched in sy_wait_input (451-462). |
| 8 | priority | Integer tie breaker, ascending toward next on equal deadlines (SYS.C:571-582); therefore tail dispatch chooses larger priorities first, older first when equal. Only caller supplies animation priority 10. It is a rank, not a categorical enum. |
| 10 | action | Existing TimerAction; sy_timer_anim writes TIMER_RESUME_ANIM (605), cancel writes TIMER_CANCELLED (617), wait dispatches both (453-462). Replace the remaining comparison to raw 1 (616). |
| 12 | argument → animationId | Animation script ID passed from VIDEO.C:269 through sy_timer_anim, matched during cancellation, passed to vi_anim_resume after removing the node (SYS.C:459-461,606,616). |
| 14..17 | unused[4] | Zero-initialized spare bytes with no reader/writer; retain. |

`ANIM_WAIT` schedules continuation with command operand1 as delay and object
id as argument (VIDEO.C:264-276); stopping an object calls cancellation when
pendingTimers is nonzero (VIDEO.C:165-177). `vi_anim_resume` reenters its script.
No interrupt services these queued callbacks: sy_wait_input does.

Edges left intact: reset links node 3 to index 4, not -1; allocation only tests
negative free indices. The code does not safely bound pool exhaustion.
sy_timer_anim does not check a failed allocation. sy_timer_cancel starts at
the tail and follows next, so ordinarily examines only that node, despite its
general-sounding name. Zero-delay insertion bypasses sorting and prepends at
head. None of these observations authorizes a behavioral fix in this pass.

## 3. InputState (SYS/SYS.H)

Purpose: request raw positive keyboard events and retain the last character.
Initialized `g_input` at DS:05D0, four zero bytes (SYS.C:60); no persistence.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | any → captureKeys | Boolean int. GAME.C:92-95,540-542 enables it around title/message waits; EDIT.C:56-58 saves/enables it and restores it at 127. SYS.C:430-432 stores a positive kg_key result and returns KR_ANY_INPUT. SYS.C:490-492 permits clicks to return the same result. Retype BOOL; keep EDIT's saved local at its original declaration position. |
| 2 | key | Signed-char keyboard event promoted to int; positive raw character, or zero for a click/reset. Written in SYS.C:431,480,491 and EDIT.C:58,61; read for editing at EDIT.C:65. Negative menu results bypass raw capture, so this is not InputResult. |

kg_key atomically reads/clears g_lastKey (KBDGET.ASM:17-25) and has a char
return type (KBDGET.H:21). kb_isr and jy_demo_key populate it from g_scanMap.
Capture does not redirect negative biased menu events or the music command.
g_clickEnds also writes key=0 and returns KR_ANY_INPUT independently of capture.

## 4. GameClock (SYS/SYS.H)

Purpose: combine action time with elapsed BIOS-rate ticks for the calendar.
Initialized `g_gameClock` at DS:05FC, twelve zero bytes (SYS.C:71).

| Offset | Member | Evidence and units |
| --- | --- | --- |
| 0 | actionTicks | Extra game-time advancement, in BIOS-rate units. GAME.C:270,497-505,701,823-861, GARAGE.C:440,533,618,721,855-890,1007,1083-1313 and BOBS.C:538,619,1148 add action costs. GARAGE.C:137 adds zero. New game clears it (SAVEGAME.C:131); save/load writes/reads four bytes directly (468/341). |
| 4 | biosTicks | Process-lifetime count of calls to the previous timer ISR, once per four game interrupts (SYS.C:881,1061). Calendar readers add it; timing tuning samples it and adds up to three times elapsed ticks to actionTicks (GARAGE.C:960,1005-1007); driving wear checks its low bits (SYS.C:1155). Not saved verbatim. |
| 8 | timeBase | Offset subtracted from actionTicks+biosTicks. New game sets it to current biosTicks (SAVEGAME.C:132). Save writes biosTicks-timeBase as delta (462,468); load restores timeBase=biosTicks-delta (357). |

ga_day and ga_time (GARAGE.C:50-71) divide the combined count using
GAME_DAY_TICKS/GAME_HOUR_TICKS and cap the game end. Save format has two
four-byte clock values, not a dump of this twelve-byte record. ISR cadence is
set by SYSTEM_TIMER_RELOAD and g_tickDiv; biosTicks is not the four-times-faster
g_tick used by timers. Keep long widths and signedness.

## 5. DemoControl (SYS/SYS.H)

Purpose: coordinate simulated menu keys, cursor travel and driving progress.
Ten-byte zero-initialized private SYS data at DS:06A4 (SYS.C:374).
It is neither resource-loaded nor saved.

| Offset | Member | Complete access contexts |
| --- | --- | --- |
| 0 | quit → exitState | Reentry guard: initially 0, assigned 0x63 before sy_exit when g_demo is 0x63 (SYS.C:414-417). Exit displays a message and can reenter input handling (SYS.C:190-195). Not BOOL: name its two values with DemoExitState. |
| 2 | keyDown | Char: sy_demo_step sends SCAN_BREAK and clears it on the next visit (714-718), sets it after a make event (864-865); ISR uses it to choose 16- versus 128-tick spacing (888). |
| 3 | moving | Char: skip choosing a new action while moving to the existing target, set while target differs and clear on arrival (SYS.C:720,842-846). ISR moves one pixel per axis toward the target (889-893). |
| 4 | count → garageStep | Reverse garage-menu selection counter, incremented/wrapped by menu length (758-759), reset to 15 after automatic driving and set to 4 at the gas station (721-723,741); spare-parts behavior tests it against 11 (781). Not a generic timer/countdown. |
| 6 | lastGroup | Existing ScreenId for the preceding injected action; consulted for garage/engine/rival/car-list choices (757,766,791,795,849), assigned KM_NONE or current group (851-853). |
| 8 | autoShifts → autoUpshifts | Counter incremented only by high-RPM automatic-control upshift requests (SYS.C:1126-1129), used to postpone leaving Bob's until ten requests (847), then reset on garage return (721-722). Counts requests, not proven completed gear changes. |

The transmission demo script and target coordinates live outside this record.
Do not infer that all g_demo magic values have the same meaning as exitState.

## 6. Vibrato (SYS/SYS.H)

Purpose: scratch values for the selected music voice's pitch modulation.
Four-byte private zeroed SYS BSS at DS:6C52 (SYS.C:41). No loading or saving.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | gain → sampleScale | At a non-rest note, unsigned depth * base divisor / 1000 (SYS.C:936-937). Held for that note and multiplied by the waveform sample at 900-901. Integer truncation can make it zero. |
| 2 | divisor | Resulting base divisor plus scaled sample (SYS.C:899-902), output low then high to PIT_CH2 (903-904). Signed word carries the raw timer bits. |

Only sy_timer_isr reads/writes the fields. gain is not speaker volume.
The multiplication is unsigned word arithmetic: its low 16 bits are divided
by 1000, so sampleScale is 0..65, not an unbounded mathematical product.
The source has no long cast (SYS.C:937); the compiler also discards the high
product word before dividing. This wrapping is retained.

## 7. CursorDraw (SYS/SYS.H)

Purpose: snapshot pending cursor coordinates for main-loop drawing.
Private zeroed SYS BSS at DS:6C46, six bytes including one padding byte
(SYS.C:37). No loading or saving; distinct from the renderer's Cursor record.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0,2 | x, y | Init sets -1/-1 (SYS.C:296-297); input loop copies and clamps shared coordinates to 0..314/0..194 under disabled interrupts, then passes the snapshot to cu_draw (500-510). No ISR accesses x/y. |
| 4 | dirty | Char pending-update flag, also used to request hover refresh (83). Menu mouse movement ORs it (467); input loop clears it before draw (508). Timer demo/keyboard/joystick movement sets it (893,976), as do drive input paths (1085-1107). Remains char. |

## 8. TimerQueue (SYS/SYS.H)

Purpose: active/free timer list endpoints and cached earliest deadline.
Initialized private SYS data at DS:0566, twelve bytes (SYS.C:54).
No resources or save file contain it. sy_reset_queues resets every member.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | earliest | Tail index; -1 when empty (reset 121), updated by insertion at tail/removal (156,586), read by wait and cancel (452,615). |
| 2 | head | Latest-deadline end, -1 empty. Reset 121; add starts here and can replace it (558,592); removal replaces it (166). |
| 4 | free | First free-node index; reset 0 (122), pop during add (553,583), push during removal (167-168). Negative is treated as exhausted, but see the pool-bound defect above. |
| 6 | nextDeadline | Tail's absolute tick or TIMER_NO_DEADLINE. Set by reset/add/remove (123,158-160,587), tested by wait (451). |
| 10 | unused | Spare char, initialized/cleared alongside busy (54,124); no read. Retain. |
| 11 | busy | Char set during add, cleared on failure/success/reset (552,555,595,124). No reader: not an effective concurrency lock. Retain char. |

## 9. TimerPriorities (SYS/SYS.H)

Purpose: scheduler priority settings, of which only animation is consumed.
Eight-byte communal `g_sysTimerPriorities` at DS:8EF6 (SYS.C:52). Startup
writes every member (SYS.C:261-264); no resource or save-game serialization.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | animation | Set to 10; read only by sy_timer_anim (604) and passed to add's equal-deadline rank comparison (571). |
| 2,4,6 | unused2, unused4, unused6 | Set to 2,5,5 respectively (262-264), never read. Preserve names, widths and values; do not invent additional timer categories. |

## 10. Cursor (UI/CURSOR.H)

Purpose: keep cursor sheets, background storage and redraw extents.
Forty-byte communal `g_cursor2` at DS:7AEC (CURSOR.C:15), initialized by
cu_init (27-86). Its allocated sheets derive from PIC_CURSORS via di_bm,
then video copy/shift operations; the struct itself is not loaded or saved.
The allocation remains live for graphics operation; it is not a saved-game
pointer. Every field access is in CURSOR.C.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | save | Far background bitmap in the cursor allocation; EGA pixels use CURSOR_EGA_SAVE, others the tail of that allocation (45-49). Read during restoration and passed to cursor blit (136-140,159). |
| 4 | clip | Six-int restore strip, cleared as 12 bytes (50). w=0 means no saved background. cu_draw restores using dx/dy/w/h, then prepares the next strip; sx/sy remain zero (134-161). |
| 16,20 | shape, mask | Far bases of all pre-shifted color/mask sheets (67-68); EGA inverts masks at init (69-76). Drawing adds the same computed offset to each (164-165). |
| 24 | shapeOffset | Unsigned bytes to current shape's first row; cu_shape_off multiplies index by shapeSize (20-22,79,103). |
| 26 | shapeSize | Unsigned bytes per shape in one sheet plane: pitch * CURSOR_HEIGHT (77); used by cu_shape_off (22). Not the whole multi-plane allocation. |
| 28 | shapeIndex | Init and ordinary UI use GAS_HAND_EMPTY; gas station selects existing GasHand values (RACE.C:210-402). cu_ctl also uses it to select cursor pairs (101-103). Reuse enum GasHand; garage selection uses shape 0 (GARAGE.C:287). |
| 30 | state | Char CursorDrawState: hidden, draw buffered/now, erase buffered/now (51,107-122,133-173). Must stay char: negative state drives restoration and refresh. |
| 32 | drawnRect | Previous inclusive 16x16 visible extent; x2<0 means none (80,143-150). Used to refresh union of previous/new extents (165-169), then updated (170-173). It is not the aligned 24-pixel restore strip. |

Visibility is nested separately in g_cVis. cu_ctl's -1/-2 requests produce
states 2/1; -3/-4 produce char states -2/-1. The odd arithmetic preserves
those char results; keep it. Do not replace char state with an int-sized enum.
The shape-index field reuses GasHand, extending that enum with the garage
pointer at 0 and a count at 4. `sr pic 2 --scale 4 --png ...` was inspected:
the four stacked cells are the red garage pointer, hand, nozzle and rag.
GARAGE.C:287 uses shape 0; it is not an unused/reserved cell. The cursor-pair
array and sheet height now use the same count.

## 11. RandomState (SYS/RANDOM.H)

Purpose: three recurrence states for rn_int. Private six-byte zeroed RANDOM
BSS `g_random` at DS:6C62 (RANDOM.C:4), never resource-loaded or saved.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | a → state30269 | First recurrence, multiplier 171, modulus 30269, split at 177; initialized from truncated time and advanced in positive-bound calls (RANDOM.C:16,36-38). |
| 2 | b → state30307 | Second recurrence, multiplier 172, modulus 30307, split at 176 (39-41). Seeding instead scrambles b with the first recurrence until <=30000 (16-24). |
| 4 | c → state30323 | Third recurrence, multiplier 170, modulus 30323, split at 178 (26-32,42-44). |

All three are read in the final per-bound modular sum (45). Only rn_int
accesses them. GAME.C:66 and SYS.C:713 seed with -1; n<=0 returns -1.
Do not promise strictly positive state: signed time truncation/remainder and
startup zeros permit zero or negative seed behavior. Keep signed int words.
The result uses integer modular sums, not a floating Wichmann-Hill fraction.

## 12. Word (SYS/STRUCTS.H)

Purpose: expose the little-endian bytes of a signed 16-bit word without
changing its bits. Two-byte union, no permanent instance or load/save.

| Offset | Member | All uses |
| --- | --- | --- |
| 0 | w | Whole PIT divisor: SOUND.C:104, GARAGE.C:973, SYS.C:1025,1034,1044-1046,1053. |
| 0 | b[2] | Low byte then high byte, written to PIT_CH2 in each of those callers. Both bytes alias w; not a two-character string. |

These are automatic locals in so_set_freq, tune_timing and sy_drive_isr.
Keep names: their generic word/byte meanings fit every caller. MusicVoice's
similar embedded union is outside this task and remains untouched.

## 13. Pair (SYS/STRUCTS.H)

Purpose: an intentionally generic pair of signed integer values (four bytes).
No standalone allocation/load/save. Three initialized table contexts exist:

| Storage | a (offset 0) | b (offset 2) | Evidence |
| --- | --- | --- | --- |
| CURSOR.C:17 private g_cursorPairs[4], DS:06EC | First auxiliary value | Second auxiliary value | cu_ctl copies them to g_unused14/g_unused15 (101-102); neither destination has a reader anywhere. Initial pairs 1/1,2/2,1/1,8/8. No supported semantic units. |
| DASH.C:73 private g_shiftSlots[5], DS:038C | Buffer x | Buffer y | Shifter backgrounds are stored/restored with these as blit coordinates (616-617,643-644,652-653); y+19 addresses the knob-composited copy. |
| BOBS.C:977 g_jailGaps[6], DS:539A | Horizontal pixel start | Pixel width | EGASPR.C:67, CGASPR.C:73, TANDYSPR.C:84 pass these as sp_clip_span's start/width, defining gaps between jail bars. |

sp_clip_span intersects the horizontal span with the sprite and produces a
blit strip. Pair cannot be renamed to Point or have x/y members without
misstating both the width and unknown cursor contexts. Keep a/b and document
caller-defined meanings honestly. Spare-looking cursor outputs are real writes.

## 14. KeyMap (INPUT/KEYMAPS.H)

Purpose: a scan-code/replacement-byte binding, also reused for restoring
previous scan-map entries. Two chars, exactly two bytes; do not widen to enums.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | key → scanCode | PC make scan code, zero terminates a map. KEYMAPS.ASM's KEY/ITEMS macros initialize data; SYS.C:336-345 installs/saves entries and 361-367 restores them. Demo reads selected codes (810). GAME.C:219-221 and BOBS.C:1175 patch bindings at runtime. |
| 1 | val → mappedByte | Replacement g_scanMap byte. Menu tables encode KEY_VALUE_BASE + InputResult (KEYMAPS.ASM:104-106), copied by SYS.C:339-344. Saved entries instead hold previous ASCII/control/direction bytes (339,343), restored at 362/366. Demo decodes with +0x32 (811). RACE.C:861-868 patches KV_REPAIR_FIX/JUNK. |

Menu bytes occupy g_keyMapData[543] at DS:5580; g_keyMaps at DS:579F is
an assembler near-pointer table with a count word in its first slot, not a
usable map for KM_NONE. Each packed map ends with a single zero scan byte,
not a whole zero record (KEYMAPS.ASM:124 onward). Never read mappedByte at
the terminator. SYS private g_keymapSaved[30] at DS:0650 uses full zeroed
records, but only its scanCode is consulted at termination. SYS permits up to 29 normal
bindings, adds music, then writes the next scan byte as terminator. If that
limit is reached the terminator is outside the 30-record array; shipped maps
do not establish that as a safe general-purpose bound.
No saved-game persistence; this is live input configuration.

Signed char promotion matters: menu bytes compare negative and decode by
adding 0x100-KEY_VALUE_BASE (SYS.C:437); saved non-menu bytes are not all
biased InputResult. Keep char signedness and document the byte-level role.
No new enum is valid for mappedByte's full domain.

## 15. Tone (SOUND/MUSIC.H)

Purpose: mutable waveform preset, including its playback phase. Five initialized
ten-byte records in MUSIC.C:9-13 at DS:5826; no file load or save. Slot 0 is
programmed by mu_set_vibrato (49-58); others are phrase-selected presets.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0 | phase → sampleIndex | Next waveform sample; ISR reduces modulo length then postincrements (SYS.C:898-900). Initial presets zero; setter resets programmable phase (MUSIC.C:52). Can equal length between updates; phase is shared when a preset is selected again, not reset every note. |
| 2 | waveform | Existing Waveform row into g_toneShapes (SYS.C:900). Initialized TRIANGLE/PULSE; setter accepts all existing rows (MUSIC.C:53); sound effects use SAWTOOTH (SOUND.C:335,346,367). |
| 4 | depth → depthPermille | Per-sample scale in thousandths of the base divisor, not fixed divisor units (SYS.C:937). Built-ins 0,0,4,6,5; programmable callers use 10,5,30,20,14 (SOUND.C:313-367). |
| 6 | length → sampleCount | Prefix length used for phase modulo (SYS.C:898); presets 8 or 2; programmable callers all pass 8. Slot 0 starts at zero and must be programmed before modulation uses it. |
| 8 | delay → sampleTicks | Interrupts between sample updates. Copied to g_vibCount on a note and after an update (SYS.C:905,936). Presets 10 or 1; all programmable callers pass 1. Zero is unconfigured, not an immediate update guarantee. |

g_toneShapes has three eight-sample rows: triangle 0,1,2,1,0,-1,-2,-1;
sawtooth 0..7; pulse 0,1,0,0,0,0,0,0 (MUSIC.C:8). mu_play and
sy_music_start select the preset pointer; phrase boundaries advance it
(MUSIC.C:32, SYS.C:632,928). ISR skips modulation for rests. Sample count must
fit its row and be positive on an update; no validation enforces this.
Keep the signed int divisor path: these words ultimately represent PIT bits.

## Changes and replacement inventory

- Rename Link to TimerLink directly (the CLI does not support type tags);
  change only the requested
  members listed above and all their references. Keep Word/Pair generic.
- InputState.captureKeys, Mouse.left/right use BOOL; rewrite their 0/1 stores
  as FALSE/TRUE, preserving raw mouse masks and the historical initializer.
  EDIT's saved capture flag uses BOOL at the same declaration position.
- DemoControl.exitState uses new DemoExitState, with READY=0 and ENTERED=0x63;
  replace both guard-member comparison/assignment and its initializer. The
  unrelated global g_demo value remains outside scope.
- Cursor.shapeIndex uses existing GasHand. Replace every cu_ctl(1) in GAME,
  BOBS and RACE plus cu_init's shape/offset literals with GAS_HAND_EMPTY.
  Extend GasHand with GAS_HAND_GARAGE=0 for GARAGE.C:287 and GAS_HAND_COUNT=4
  for the cursor-pair bound and sheet height. No duplicate shape enum is added.
- Reuse TimerAction for the cancellation comparison and every node action
  initializer (TIMER_CANCELLED).
- Name ToneSlot/Waveform table counts and the fixed waveform sample capacity,
  replacing both g_tones/g_toneShapes definitions and header dimensions,
  the three full-length preset initializers and all five programmable callers.
- Document each requested type with a brief and every member with one concise
  role/units comment. Char flags and all unused storage remain unchanged.

## Other misleading names / unresolved behavior (outside scope)

- ToneSlot's LIGHT/WIDE/TRILL comments say "divisor units"; actual scale is
  depth*base/1000, then multiplied by the waveform (SYS.C:899-902,937).
  These comments predate this investigation; record the discrepancy here.
- sy_timer_cancel sounds like an all-matching cancellation but traverses from
  earliest via next, normally only the tail (SYS.C:615). Queue exhaustion and
  zero-delay ordering also remain as described above.
- g_timerQueue.busy is write-only, not a checked lock; g_sysTimerPriorities'
  unused slots are write-only, not established service categories.
- g_unused14/g_unused15 are assigned cursor auxiliary values, with no reader
  (CURSOR.C:101-102, INPUTDAT.ASM:84-87). Their purpose is not recoverable from
  current use; do not label the Pair fields cursor speeds or hotspots.
- K_UNUSED_40 is not empty: KEYMAPS.ASM:410-415 binds N/L/Q/S. The ScreenId
  KM_UNUSED_40 comment calls it an empty group. g_keyMaps[0] is a count, despite
  its pointer type. Both are outside these record definitions.
- CURSOR.C:25's scratch-strip/pop-up comment does not describe the actual
  shape-sheet initialization; only cursor state/storage is built there.
- DemoControl.autoUpshifts counts requests before shift resolution, including
  the g_autoDrive path (SYS.C:1119-1129); it cannot prove completed shifts.
- RandomState has no positive-only invariant from its signed time seed.
  In particular the cast to int precedes %30000 (RANDOM.C:16).

## Validation

Before integration: headers and format passed, Doxygen reported zero
diagnostics, all 572 functions matched, the clean packed EXE was IDENTICAL,
DGROUP ownership/order passed and all 109 unittests passed. A separate real
MSC audit checked all 15 records: 81 size/offset checks across seven headers
compiled independently, with zero mismatches. The fixed layouts are the
offsets and sizes tabulated above.

The required origin/main merge and complete post-merge gate rerun follow.

The first DGROUP run rejected TONE_WAVE_SAMPLES as a macro array bound.
The capacity now uses the documented ToneWaveSize enum, which the analyzer
can resolve; no tooling changes are needed. The clean build already produced
an IDENTICAL packed EXE, with 572 raw-exact mapped functions and 748 exact
near calls. The unpacked-file diagnostic reports different header/relocation
ordering and fourteen omitted tail bytes; the packed identity is the required
whole-EXE gate.
