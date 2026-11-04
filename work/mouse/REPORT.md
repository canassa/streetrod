# Mouse record experiment

## Conclusion

Undecidable from the executable whether the nine mouse variables were separate
symbols or one struct. The natural reconstruction is `struct Mouse g_mouse` at
DS:05E2: eight ints followed by `char present`, with compiler tail padding at
05F3. All nine values describe the same mouse polling state. The final record is
18 bytes; it keeps the initializer and the entire packed executable byte-identical.
This is a reconstruction choice, not proof of an original struct declaration.

No mouse reference forces separate globals. Conversely, no operand-order change
or compiler-budget effect proves that these nine values were members. Extending
the record into the clock variables is ruled out for the tested boundaries by
operand-order changes. The preceding unrelated input globals and the unused
05F4 words can be included without proving their membership; they remain outside
because there is no shared mouse role or positive code evidence.

## Complete source survey

The baseline source is commit 8c10946. All definitions occur consecutively in
SYS.C's initialized data, before sys_hover_reset, in address order. All references
are in six C translation units. Searching every C and ASM source finds no
assembly reference, offset alias, array index, pointer subtraction, or bulk copy
of the proposed mouse block. No assembly update is required.

| Address | Original symbol | Type / initializer | Uses |
| --- | --- | --- | --- |
| 05E2 | g_mouseDx | int / 0 | mouse_poll writes converted horizontal motion; SYS reads motion in sys_wait_input and sys_drive_isr |
| 05E4 | g_mouseDy | int / 0 | mouse_poll writes converted vertical motion; the same SYS readers |
| 05E6 | g_mouseAccX | int / 0 | mouse_poll accumulates and clears; SYS resets on initialization, queue reset, drive start/stop |
| 05E8 | g_mouseAccY | int / 0 | same vertical operations |
| 05EA | g_mouseShiftX | int / 0 | mouse_poll conversion; SYS sets 1 at initialization and drive start/stop |
| 05EC | g_mouseShiftY | int / 0 | mouse_poll conversion; SYS sets 1 normally and 6 while driving |
| 05EE | g_mouseLeft | int / 0 | mouse_buttons writes INT 33h button bit; mouse_poll, dash_wait_key, drvtick_step read; bobs_visit and cruise_go clear |
| 05F0 | g_mouseRight | int / 10 | mouse_buttons writes the other button bit; sys_drive_isr reads |
| 05F2 | g_mousePresent | char / 3 | sys_init writes 0 or 1 after mouse_reset; SYS, dash_wait_key and drvtick_step test |

The unusual initial right-button value 10 and present value 3 are retained
exactly; they are not replaced with plausible runtime defaults. mouse_reset and
mouse_motion use INT 33h and local REGS/pointers, and do not themselves reference
these globals. The only MOUSE.C consumers are mouse_buttons and mouse_poll.
SYS consumers are sys_reset_queues, sys_init, sys_wait_input, sys_drive_start,
sys_drive_stop and sys_drive_isr. The other consumers are exhaustively named in
the table.

## Neighbor survey

| Range | Baseline meaning / use | Decision |
| --- | --- | --- |
| 05D6 | g_05d6, zero; cleared by DRVTICK, RACE and SYS driving reset | independent, no demonstrated mouse connection |
| 05D8..05DA | g_05d8/g_05da, zero; RACE clears them | independent; identity not inferred |
| 05DC | g_msgTicks = 350, GAME/RACE message wait duration | outside mouse state |
| 05DE | g_clickEnds = 0, POPUP sets/clears, sys_wait_input tests | popup/input policy, outside mouse state |
| 05E0 | g_kbdHooked = 0, SYS hook/unhook and SCREEN use | keyboard lifecycle, outside mouse state |
| 05F3 | unreferenced zero char g_05f3 | becomes the compiler's zero tail padding; no independent symbol is needed |
| 05F4..05F7 | g_05f4[2] = {0,0}, no code references | leave independent; no evidence these are mouse position fields |
| 05F8 | g_tick = 0L, global interrupt clock, deadlines throughout the game | separate; direct comparison evidence below |
| 05FC..0604 | g_05fc/g_0600/g_0604 = 0L, elapsed/game-time accounting, GARAGE and SAVEGAME consumers | separate, not mouse state |
| 0608..060B | timer/drive divisors and unused g_060a | separate |

## Compiled experiments

Every row was generated from an untouched source snapshot, preserving member
order and all initializer values, and compiled with the actual MAKEFILE flags
(MSC 5.1 and MASM 5.1, SR_JOBS=6). Every row ran `./sr check --all` and `./sr build`.
The check total incorporates whole-file gates, so a SYS data-layout failure
marks its 24 functions as failing even when all instruction bytes are identical.
The logs and reproducible generator are under work/mouse (ignored scratch).

| Variant / log prefix | Record range | check --all | Build / evidence |
| --- | --- | --- | --- |
| baseline | separate symbols | 572/572 | reference state |
| core | 05E2..05F2, retain separate g_05f3 | 548/572 | instruction bytes match; extra tail padding moves g_05f3 and later data; SYS data gate rejects |
| eight | 05E2..05F1, present remains separate | 572/572 | packed EXE IDENTICAL |
| padding | 05E2..05F3, explicit last char | 572/572 | packed EXE IDENTICAL |
| position | 05E2..05F7, includes unused words | 572/572 | packed EXE IDENTICAL; DGROUP clean |
| prefix | 05D6..05F2, retain separate g_05f3 | 548/572 | same tail-padding failure as core; does not independently reject the prefix members |
| time | 05E2..060B, includes clock/accounting | 547/572 | 6 SYS and 3 DRVTICK instruction-byte differences; 93/95 objects pass; DGROUP clean |
| wide | 05D6..060B | 547/572 | same instruction differences; DGROUP clean |
| prefixpad | 05D6..05F3 | 572/572 | packed EXE IDENTICAL; DGROUP clean |
| implicit (chosen) | 05E2..05F2 plus implicit pad at 05F3 | 572/572 | packed EXE IDENTICAL; DGROUP clean |

The first eight/padding experiments put the type in SYS.H, as the brief allowed.
Their builds are identical, but the DGROUP type reader only parses STRUCTS.H and
raises KeyError for a type defined in SYS.H. Subsequent experiments and the final
implementation put Mouse in STRUCTS.H. No tool was changed.

## Operand-order evidence

The nine fields have no comparison against a second nonconstant memory operand:
the tests compare with zero, and the sign conversion/shift uses locals. In
mouse_poll, `accX += mx` and `accY += my` use local deltas. In sys_wait_input,
`g_cursorX += g_mouseDx` and its Y counterpart are compound assignments. The
original at 01600 loads DS:05E2 into AX and adds AX to DS:472A; at 01607 it loads
DS:05E4 and adds to DS:472C. Both forms preserve these exact instructions. These
sites cannot distinguish the proposed flat member from a separate global.
The DRVTICK button OR also remains identical.

There is positive exclusion evidence for g_tick. With g_tick a member but
separate g_timerNext and g_waitUntil, MSC reverses the long comparison operands:

- sys_wait_input: `g_timerNext <= g_tick` changes branches at 01586/01588/0158E
  from JG/JL/JA to JL/JG/JB.
- sys_wait_input: `g_waitUntil < g_tick` changes branches at 016A5/016A7/016AD
  from JG/JL/JAE to JL/JG/JBE.
- drvtick_step: `g_waitUntil <= g_tick` changes branches at 08F62/08F64/08F6A
  from JG/JL/JA to JL/JG/JB.

The original loads the clock low/high words and compares against the other
plain long. The member version loads the other long and compares against the
clock. This is expression-shape/operand-order evidence, not a data relocation
or source operand-order issue. It rejects the tested records that include the
clock while leaving the earlier timers/deadlines independent. It does not claim
to exclude every conceivable larger record containing all timer state.

## Compiler budget and data layout

Every file and every later function was checked, including BOBS, CRUISE, DASH,
DRVTICK, MOUSE, SYS and the sensitive GARAGE/VIEW files. Reducing nine mouse
symbols to g_mouse produces no instruction changes anywhere with the chosen
layout. There is therefore no budget-based proof of membership, and no budget
regression. The wider clock experiments' failures are the explicit comparison
changes above, with unchanged function sizes.

MSC rounds this record to 18 bytes. The chosen initializer is
`{0, 0, 0, 0, 0, 0, 0, 10, 3}`. DS:05E2..05F3 is fourteen zero bytes,
0A 00, 03, 00 (the last zero is tail padding). Keeping a separate char after
this struct would emit an extra byte and alignment before 05F4. Removing the
unreferenced g_05f3 definition/declaration lets the padding supply exactly its
original byte. DS:05F4 and every following global retain their original address.
No artificial packing directive or initializer filler is used.

## Final verification

Before merging main: 572/572 functions and all file gates pass; the packed build
and DOS-native MAKE are IDENTICAL; all 53 unit tests pass; headers and DGROUP
checks are clean. Final gates will be repeated after merging main.
