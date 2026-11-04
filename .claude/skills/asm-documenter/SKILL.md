---
name: asm-documenter
description: Document or review Street Rod's MASM 5.1 src/**/*.ASM and .INC files, explaining game behavior, DOS and hardware, with measured byte-neutrality guidance.
---

# Street Rod assembly documentation

Use this skill when explicitly asked to document or review assembler sources.
Reading the skill does not authorize adding comments. CLAUDE.md's human review
rule still applies. This task's scope may be tooling only: do not turn it into a
source documentation sweep.

Adapted from bak2's asm-documenter. Street Rod uses Microsoft MASM 5.10 (1988),
not TASM. The measurements below supersede that project's assembler advice.
Naming is governed by [CLAUDE.md](../../../CLAUDE.md).

## Audience and scope

A reader fluent in assembly but unfamiliar with DOS, PC speaker, EGA, CGA,
Tandy, Hercules, the PIT timer or 8259 interrupt controller should understand
any file from top to bottom. Teach the needed system concepts in that file.
Explain each concept at first use, then taper. Don't explain MOV, stack-frame
boilerplate or routine conditional jumps.

Documentation explains the **game**: what the module does, its contracts, data
formats, algorithms and hardware effects. Never add decompilation addresses,
original offsets/sizes, byte dumps, hash ordering, compiler budgets, matching
status, reconstruction history or storage-placement banners. That reasoning
belongs in `docs/RECONSTRUCTION.md`. Runtime coordinates, resource-format byte
offsets and I/O port numbers are meaningful facts, not decompilation addresses.
Preserve every **PROVISIONAL** marker and its full text. Do not promote a guess
into a confident statement; leave unknown names and meanings unknown.

## Files and ABI

Street Rod links its assembler with MSC 5.1 C, medium model: far code, near
data in DGROUP. There are no bak2 standalone DUDE/OVL drivers here. Sources live
in uppercase subsystem folders under `src/`; `.INC` files include shared
`src/SYS/PC.INC` (hardware constants) and `MAIN.INC`.

Inspect each file before writing. Most use `.MODEL MEDIUM`, `.DATA`, and
`.CODE Sxxxxx_TEXT`; SPEAKER.ASM instead uses `.MODEL SMALL` and an explicit
`S2196_TEXT SEGMENT WORD PUBLIC 'CODE'`. Preserve the actual model, code-segment
name, ASSUME statements and data/code ordering. MASM does not search beside its
input file: SRENV.BAT supplies the project's INCLUDE path; check jobs stage
headers flat. Do not change include organization during a documentation pass.

`PUBLIC _xx_name` exports the C function `xx_name`; EXTRN spellings must agree
with the owner's `.H`. `/Ml` preserves symbol case. `PROC FAR` and `PROC NEAR`
are real distance declarations: bare `ret` depends on them. Public near helpers
can be called by another object sharing the same code segment. Don't infer
private scope from NEAR. A macro's LOCAL is different from a procedure local.

## File anatomy and banners

Keep the existing directives and declarations in place. Add, when authorized:

1. A short semicolon banner naming the module and its game purpose.
2. Enough DOS/hardware background to understand its operations.
3. Data descriptions beside the existing tables and state.
4. Concise function plates, followed by conceptual inline comments.

Example banner (a style example, not a source edit):

```asm
; ============================================================================
; SPEAKER.ASM -- music through the PC's internal speaker
;
; The game produces tones by switching the speaker signal through the PC's
; peripheral interface. Tune records describe the timing of each tone.
; ============================================================================
```

Explain only the hardware used by that file. For example:

- DOS/BIOS calls: name the service, register contract, carry/error behavior,
  and any interrupt-vector ownership and restoration.
- PC speaker/PIT: distinguish software toggling from timer-driven output;
  explain gate/data bits, divisor units and zero-divisor behavior if used.
- 8259: distinguish a device acknowledgement from end-of-interrupt signaling;
  explain why the handler chains or sends EOI in this path.
- EGA: introduce planar storage, latches, write modes, set/reset and bit masks
  before relying on those terms. Explain the selected index/data register pair.
- CGA/Tandy/Hercules: describe pixel packing, scanline banks, stride and page
  selection for the mode actually used. Never transfer one adapter's model to
  another because they share an address or CRTC interface.

Use PC.INC names for established hardware facts. Explain what their values mean
rather than copying the entire constants table into prose. Cite a specific chip
manual section or RBIL interrupt/service for deeper reading; core comprehension
must not depend on a separate Markdown document. Verify hardware claims before
writing them. A small module needs no enormous glossary; define acronyms at
first use, and add a glossary for long hardware-heavy files.

## Function plates and inline comments

Start each PROC's documentation with the matching C signature where one exists.
Then describe the game's operation, argument units/ranges, outputs, clobbers,
segment assumptions and side effects that the caller must know. For register
interfaces describe domain values, not just register widths. Include flags if
callers consume them. Don't invent preservation guarantees from convention.

```asm
; ----------------------------------------------------------------------------
; void sk_delay(int n)
; Wait for a duration scaled by the game's delay calibration.
; Entry: n is a count of delay units, not milliseconds.
; Effects: occupies the CPU until the wait finishes.
; ----------------------------------------------------------------------------
```

Keep plates proportional: omit empty categories. Explain hardware transactions
and algorithm stages before groups of instructions. Selective end-of-line notes
can name the domain meaning of an arithmetic result. Avoid one comment per
instruction and fixed comment-density quotas. Use blank lines for logical
paragraphs, existing indentation, and roughly 80-column comment wrapping.

A non-symmetric save/restore, unreachable branch or surprising arithmetic needs
an honest behavioral explanation. Never justify it with "preserved for byte
identity" in source. Explain timing loops in terms of the device or calibrated
wait, not an unmeasured modern-host execution time.

## Data, macros and naming

Every documented table needs its index domain, entry shape, units, sentinel,
termination and consumers. State who updates mutable state and who reads it.
Describe parallel arrays as they stand; don't merge them into a STRUC, reorder
them, change alignment or manufacture padding declarations during documentation.
Public assembler names mirror CLAUDE.md's module prefixes after the ABI `_`;
internal labels should describe their purpose. Rename through `./sr rename`
when appropriate and run the full gates: names are not automatically free.

KEYMAPS.ASM is data, not a collection of procedures. Its `KEY sc, result` macro
emits a scan code and `KEY_VALUE_BASE + result`; KR_ names mean decoded results,
not encoded bytes. `ITEMS` uses macro `LOCAL` assignment symbols and `REPT` to
emit consecutive bindings. MASM splits macro arguments at spaces: keep
arithmetic inside DB or bracket a nested expression as the existing file does.
Document the input/output contract and sentinel once, then individual map roles.
The packed build and keymap tests cover this data; function checks alone do not.

## Measured MASM 5.1 behavior

Reproduce from the repo root:

```sh
nix-shell shell.nix --run 'python3 tools/scripts/masm_doc_probe.py'
```

The script writes small PROBE.ASM inputs under `work/masm-doc-inputs`, assembles
with `toolchain/masm51/BIN/MASM.EXE /Ml` via the bounded emu2 backend, and compares
**whole OBJ files**, not just disassembly. Every input has the same DOS filename
so THEADR differences don't obscure comparisons. JSON, listings and logs stay in
work/. Listings matter: some warnings are absent from the captured stdout.
Measurements on 2026-11-04 used the assembler's banner "Version 5.10".
These tiny cases establish behavior, not a proof about every possible program.

| Construct | Measured result |
|---|---|
| Semicolon comments and blank lines | Whole OBJ identical; code `B8 02 00 CB`. |
| Numeric `VALUE EQU 2`, substituted in MOV | Whole OBJ identical to literal 2. |
| Forward EQU in this MOV | Whole OBJ identical; don't generalize to size-sensitive forward operands. |
| `arg EQU WORD PTR ss:[bp+6]` | Whole OBJ identical to raw operand; `8B 46 06 CB`. |
| STRUC definition and `es:[di].second` | Whole OBJ identical to `es:[di+2]`; `26 8B 45 02 CB`. |
| `es:[di+PAIR.second]` | Also accepted and whole OBJ identical (unlike bak2's TASM result). |
| RECORD `highbit:1,lowbits:7`, MASK/shift | Whole OBJ identical to literals 80h/7; `A8 80 B1 07 CB`. |
| Two macro expansions or REPT 2 | Whole OBJ identical to two INC AX instructions; `40 40 CB`. |
| Macro LOCAL jump labels | Whole OBJ identical to two explicit short jumps; `EB 00 EB 00 CB`. |
| LABEL WORD alias, TITLE/SUBTTL/PAGE | Whole OBJ identical to baseline; listing presentation may differ. |
| Plain PROC FAR | No generated frame; bare RET emits CB. |
| Plain PROC NEAR | No generated frame; bare RET emits C3, so changing distance changes bytes. |
| `.MODEL MEDIUM,C`, `PROC FAR arg:WORD` | Generates frame: `55 8B EC 8B 46 06 5D CB`. |
| `.MODEL MEDIUM,C`, `LOCAL temp:WORD` | Generates allocation/frame; `55 8B EC 83 EC 02 8B 46 FE 8B E5 5D CB`. |
| `.MODEL MEDIUM,C`, `PROC FAR USES si di` | Generates saves/restores; `56 57 B8 02 00 5F 5E CB`. |
| Plain-model `PROC FAR C arg:WORD` | A4001 extra-text warning, arg undefined when used. |
| Plain-model PROC USES / LOCAL temp:WORD | A4001 warning; ignored text is not valid documentation. |
| `LOCALS` directive | Rejected, A2105. Do not port TASM's LOCALS setup. |
| `@@done` ordinary label | Accepted; jump test emits `EB 00 CB`. This does not prove TASM-style scoped names. |
| Anonymous `@@:` and `@F` | Accepted; jump test emits `EB 00 CB`. Avoid in prose-oriented rewrites. |

Do not claim that all `@@` syntax is absent in MASM 5.1. Named `@@done` is not a
license to import TASM's scope-reset rules. Keep ordinary descriptive labels;
the probe does not establish repeated named-label scope.

STRUC/RECORD definitions supply offsets/masks without storage in these probes;
instances emit data. Field symbols share the module namespace, so choose names
that do not collide. EQU operand aliases are also module-wide. Macro LOCAL
symbols are per expansion; assignment `=` can advance an assembly-time counter.
Use these aids only when separately authorized to refactor source.

Avoid high-level PROC parameters, USES and LOCAL for hand-written frames.
Language/model settings control their parsing and automatic prologues. Never
carry over TASM ARG or MASM 6 syntax. Plain PROC distance must stay unchanged.

## Edits that still require gates

Preserve explicit SHORT/NEAR, operand PTR widths, segment overrides, ASSUME,
radix, CPU/model/segment/group directives and PUBLIC/EXTRN order. These can
change encodings, fixups or linked placement. Never add TASM JUMPS, SMART or
NOSMART; they are not this project's dialect. Do not infer multipass relaxation
from TASM `/m`: this project invokes MASM `/Ml`, as recorded in src/MAKEFILE.
A forward constant tested in MOV does not establish forward-branch sizing safety.

For an authorized documentation pass, make comments first, then any constants,
field references, macros or renames one at a time. Check each affected module
with `./sr check FILE.ASM`; revert a divergent step before continuing. Finish
with `./sr headers --check`, `./sr check --all`, a clean `./sr build` (packed EXE
IDENTICAL), `./sr dgroup check`, `./sr dgroup order` and the unittest suite.
`./sr docs` covers C documentation only; assembly plates require human review.
Do not claim a successful function check verifies data-only files or the link.
