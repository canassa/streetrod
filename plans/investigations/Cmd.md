# Cmd: animation instructions and script headers

## Purpose and scope

A six-byte record describes one instruction of the timer-driven bitmap animation
interpreter; the first record of each script also holds its active object slot.
Investigated 2026-11-05 on branch `inv-cmd`. Evidence line numbers in this note
refer to the source before the renames below.

The type belongs to VIDEO.H, which owns the interpreter. CARPIC.C owns the only
record array: `g_animScripts[44]` (CARPIC.C:43-86). VIDEO.C's private `g_animRun`
holds a near pointer to the current record (VIDEO.C:20,213). There are no local
record instances, file loaders, save-game entries, or external writers of its
fields. The initializer is compiled into DGROUP, not read from a resource file.
`./sr data 58f0 282` confirms the 264-byte array followed by the nine script
start words at DS:59f8. These addresses are investigation evidence only.

## Fixed layout and members

| Offset | Original declaration | Role and value set | Evidence |
| --- | --- | --- | --- |
| 0 | signed char op | Instruction selector, existing `enum AnimOp`; values 1..13 understood by the switch. The table has no opcode 12. | VIDEO.H:64-78; VIDEO.C:216-285; CARPIC.C:43-86 |
| 1 | signed char sub | Opcode-dependent mode: existing `enum BoxSet` for blits/set-box, existing `enum AnimWaitMode` for wait; ignored by all other instructions. | VIDEO.C:183-199,273 |
| 2 | int a | First operand: x, width, picture ID, jump index, wait ticks, or hook count. In a script's header it instead holds the running object slot (0..4), with -1 marking inactive after stop. | VIDEO.C:169-174,188-197,253-269,298,314,322,332-334 |
| 4 | int b | Second operand: y, height, or wait resume index (0 means next sequential index); ignored by other instructions and in headers. | VIDEO.C:189-198,264,267-268 |

The record is six bytes, word aligned. Preserve both explicit signed-char
fields, both signed two-byte integers, their order, and all initializer bytes.
Neither integer has one closed value set: typing either as a picture, coordinate,
slot, or boolean would misdescribe other uses. The byte fields cannot become
int-sized enums. Their documentation should link the existing enums instead.
There is no new enum, constant, boolean conversion, or raw-value replacement
needed: every opcode and every interpreted mode in the table already uses the
existing constants. Unused mode zeros are not box/wait mode uses.

## Complete instruction contract

| Opcode | Mode | First operand | Second operand | Effect |
| --- | --- | --- | --- | --- |
| ANIM_NOP_1 / ANIM_NOP_3 | ignored | ignored when executed | ignored | Advance without work. Separate use as script headers is described below. |
| ANIM_BLIT | BoxSet | x when mode sets coordinates | y when mode sets coordinates | Update box as selected; copy object image to back page. |
| ANIM_BLIT_MASKED | BoxSet | x when mode sets coordinates | y when mode sets coordinates | Same coordinate update, then masked copy to back page. |
| ANIM_HOOK | ignored | Hook-specific count | ignored by current hook | Invoke hook; a nonzero result falls through to STOP. |
| ANIM_STOP | ignored | ignored when executed | ignored | Release object slot and cancel its pending animation timers. |
| ANIM_JUMP | ignored | Absolute record index | ignored | Replace command pointer; the loop index still increments normally. |
| ANIM_LOAD_IMAGE | ignored | PictureId | ignored | Load a bitmap using ARENA_HIGH_STACK; advance pointer and loop index. |
| ANIM_SET_BOX | BoxSet | x | y | Set source, destination, both, or neither coordinate pair. |
| ANIM_SET_SIZE | ignored | Width in pixels | Height in pixels | Set the object's box dimensions. |
| ANIM_WAIT | AnimWaitMode | Delay in g_tick units | Absolute resume index, or 0 for next | Schedule timer, optionally copy back-page box to front, and return. |
| ANIM_COPY_TO_FRONT | ignored | ignored | ignored | Show cursor and copy back-page box to front if enabled. |
| ANIM_TILE_CAR | ignored | ignored | ignored | Mask the visible car overlap into each tile of the object image. |

Interpreter evidence is VIDEO.C:203-289; coordinate selection is
VIDEO.C:181-200. BOX_SET_BOTH deliberately falls through from source to
destination assignment. Coordinates are signed pixel positions; dimensions feed
pixel rectangles and blits. Operand values are not universally nonnegative:
unused initializer operands commonly contain -1.

The command pointer advances independently of local `k`. ANIM_JUMP changes only
the pointer, not `k`; therefore a later WAIT with zero second operand resumes at
`++k`, not necessarily the instruction following the jumped-to pointer. The
shipped table uses JUMP only as a header. Do not claim a general conventional
program counter for that unexercised path.

## Storage lifetime and script contexts

`g_animScriptStart` contains `{8, 0, 2, 12, 10, 25, 27, 29, 31}`
(CARPIC.C:87). Start skips the header, stores the newly allocated object slot in
its first operand, initializes object state, then runs the next record
(VIDEO.C:292-306). Stop writes -1 into that same header operand; active checks
its sign; resume reads it to select the object (VIDEO.C:165-178,310-323). No
other record fields are written at runtime.

| Script ID | Header index | Header opcode | First executed record | Call-site evidence |
| --- | --- | --- | --- | --- |
| 0 | 8 | WAIT | 9, STOP | No caller found. |
| 1 | 0 | NOP_1 | 1, STOP | No caller found. |
| 2 | 2 | BLIT | 3, SET_SIZE | RACE.C:298-330,384-402; pump blinking while fueling. |
| 3 | 12 | NOP_3 | 13, LOAD_IMAGE | RACE.C:316 via GAME.C:736-741; attendant arrives after delay. |
| 4 | 10 | BLIT_MASKED | 11, STOP | No caller found. |
| 5 | 25 | HOOK | 26, STOP | No caller found. |
| 6 | 27 | STOP | 28, STOP | No caller found. |
| 7 | 29 | JUMP | 30, STOP | No caller found. |
| 8 | 31 | LOAD_IMAGE | 32, LOAD_IMAGE | GARAGE.C:959-1004; tuning display. |

Thus a header's opcode and mode do not define the meaning of its first operand.
For example script 0's header operand initially contains 10, so active(0) would
report true before any start. Start overwrites the header before interpreting
its body. All three called scripts' headers initially hold -1.

Script 2 sets an 8x1 box at (48,111), initializes the hook count to 250, waits
10 ticks, then repeats hook index 7 and a wait/copy until the hook stops it.
The hook only defines behavior for object ID 2 (VIDEO.C:326-356). Nonzero first
operand initializes the count and starts pump sound; zero decrements it, draws
the blinking lines, and eventually restores music and returns true. Its second
operand is never read. `so_pump` chooses pump sound or background song
(SOUND.C:331-339). The line color is changed without restoring the saved local.

Script 3 loads PIC_GAS_ATTENDANT, sets 48x64 cells at (200,81), composites the
visible car into its sheet, waits 16 ticks, then displays source x positions
0, 48, 96 with waits 16,16,20 before stopping. The LOAD_IMAGE second operand is
2 in the initializer but is ignored: allocation uses ARENA_HIGH_STACK directly
(VIDEO.C:255-258; DISK.C:377-389). `sp_car_clip` clips against the visible car,
not an arbitrary screen window (SPRITE.C:36-48).

Script 8 loads PIC_FUEL_PUMP_DIGITS, sets a 16x94 box at (160,52), waits four
ticks, then cycles source x positions 0,16,32, resuming at absolute index 36.
`tune_timing` reads its image while running and explicitly stops it on exit
(GARAGE.C:959,978,1004). This is a tuning-display use despite the picture name.

## Timers and other side effects

WAIT stores the resume index in the object and increments its pending timer
count (VIDEO.C:266-276). `sy_timer_anim` schedules `g_tick + delay`, records the
script ID, and requests TIMER_RESUME_ANIM (SYS.C:559,599-605). `g_tick` is
incremented in both timer handlers (SYS.C:873,1010). The input loop removes an
expired timer and calls `vi_anim_resume`, which decrements pendingTimers before
running again (SYS.C:450-460; VIDEO.C:310-317). Stop nulls bitmap references,
marks the object free, and calls `sy_timer_cancel` when pendingTimers is nonzero;
it does not free bitmap arena storage directly.

Blits use the image as source and back page as destination. The EGA copy body
proves direction via `m = dst->bits.p`, `a = src->bits.p`, then
`movedata(...m..., ...a...)` (EGABLIT.C:44-46,80-89), despite misleading parameter
names. `bm_copy_box` passes its first bitmap as that copy source
(BITMAP.C:326-342), so back-to-front names describe actual behavior. TILE_CAR
uses clipping, then masked copies from the car bitmap into the object image.
Cursor visibility, back/front bitmap pixels, hook line color, pump counter and
music can all change. The command table has only the header-slot mutation.

## Edge cases and limits

- No instruction-array, jump-target, or wait-target bounds check exists.
- Unknown opcodes are skipped; unknown box modes do not update coordinates.
  Unknown wait modes still wait but do not copy to front.
- ANIM_WAIT's nonzero target is used verbatim; only zero means sequential.
- No object slot available returns 5; current callers assume success.
- Stop/resume assume the header contains a valid active slot. Starting an
  already active script is not guarded.
- `vi_anim_hook` has no return for IDs other than 2. The shipped called scripts
  only execute HOOK for ID 2. No behavior is inferred for the other IDs.
- The apparent historical purpose of the unused header opcodes and unused
  LOAD_IMAGE second operand is unknown; current source ignores them.

## Changes selected before editing

Rename `struct Cmd` to `struct AnimCommand`: this is specifically an animation
instruction rather than a generic command. Rename `op` to `opcode`, `sub` to
`mode`, and `a`/`b` to `operand1`/`operand2`. All users are confined to
VIDEO.H/VIDEO.C and CARPIC.H/CARPIC.C. Keep the original investigation filename
for discoverability. Add concise Doxygen documentation for the record and its
existing selector/mode enums, without changing their values or storage types.
No unrelated identifier renames.

## Other misleading identifiers (outside scope)

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

## Validation

- Real MSC 5.1 sizeof/offsetof probe before and after: size 6, offsets 0,1,2,4.
- All old type references and all four member accesses replaced in VIDEO.H,
  VIDEO.C, CARPIC.H and CARPIC.C; no old Cmd references remain in src/.
- sr headers --check and sr format --check: pass (157 C/header files).
- sr docs: zero diagnostics; all four members and enum references verified in XML.
- sr check VIDEO.C and sr check --all: 572/572 functions match.
- Clean sr build: 96/96 objects pass; packed EXE IDENTICAL to orig/SR.EXE;
  572/572 mapped functions and 748/748 near-call encodings exact.
- sr dgroup check and sr dgroup order: pass.
- dosbox.sh --make: native DOS SR.EXE IDENTICAL; SHA-1
  3a5e28b795313028b7e77d147122f41cb29de334 matches orig/SR.EXE.
- python3 -m unittest discover -s tests through nix-shell: 106 tests, OK.
- git diff --check: pass.

The build also emits an unpacked-file diagnostic: its header and relocation
order differ from orig/SR_unpacked.EXE, with 14 trailing load-image bytes absent
and zero differing load-image bytes. tools/sr/linklayout.py treats this as an
additional diagnostic; the packed retail EXE and the independent native build
are byte-identical. No linker/tool changes are part of this investigation.
