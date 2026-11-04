# A faithful build: the original files, the original tools, no build-time edits

Goal: the tree holds the source files the developers had (one per original object), plus a
MAKEFILE and LINK response file of the kind they used, and running MSC 5.1 / MASM 5.1 /
LIB 3.11 / LINK 5.01 on them (under DOSBox-X, unmodified binaries) produces `orig/SR.EXE`.
Nothing between the files in git and the tools may rewrite text, extract library members or
choose link order outside the response file.

Assessment of 2026-11-04 (after aa49a8d). `./sr build --layout` is IDENTICAL today, but
it gets there through the steps below.

## What the current build does that the developers did not

| # | Step in `tools/sr/linklayout.py` | Status |
|---|---|---|
| 1 | Extracts 98 MLIBCR/LIBH and 2 EM.LIB members and links them as objects in a recovered order (`runtime`, `emulator` in LAYOUT.JSON), with `/NOD` and no libraries | **Solved by experiment**: `LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;` (the combined library MSC SETUP builds), then a plain `LINK /STACK:2560 /EXEPACK` with no `/NOD`, using the objects' default library → packed EXE IDENTICAL. Other component orders differ (5381..12426 bytes); the extracted-member order was the order LINK pulls from this library. |
| 2 | `CRTORDER.ASM`: an empty `_TEXT` anchor before region 2196 | Still required (57336 bytes differ without it). Something in the original object at that point declared `_TEXT`. First suspect: M_21962.ASM, the region's first object (e.g. `.MODEL SMALL`-style or explicit segment declarations in the original). |
| 3 | Concatenates 116 `M_*.C` into 55 generated `U_*.C` (`module.merge`: hoists includes, near forward declarations, statics) and 31 `.ASM` into 27 (`merge_asm`: renames labels, drops `.MODEL`/`END`, rewrites `.CODE` to the region segment) | The split is our scaffolding. Merge the sources once into real files, one per unit, and compile those. |
| 4 | ~~Inserts `#pragma loop_opt(on/off)` before every member source~~ | **Done** (branch faithful-loopopt): the injection is gone; the unit text is the concatenated sources. Five functions lose their match under the unit's `/Ol`: F_01BAE, F_01FE2, F_0238C (U_00D78), F_03C8D (U_039C0), F_2382D (U_23262). In each, the first divergence is a `for`/`while`/`do` loop that the original leaves as a plain memory loop (F_2382D's byte copy becomes `rep movsw`). `/Ol` takes every structured loop of these sizes: `while` and `unsigned char` forms of F_2382D, and `for`/`while`/`do` forms of F_0238C's gear search, all still get registers and loop temps. Only `goto` loops escape it (0x29170), and rewriting every loop of five functions that way is less plausible than a pragma, which in U_00D78 is a single line covering its last three functions. So the sources now carry the real MSC 5.1 pragma, with a comment: `loop_opt(off)` before F_01BAE (in effect to the end of U_00D78), off/on around F_03C8D, off before F_2382D (a word move would also break its write-mode-1 latch copy). Recorded per-function flags are unchanged (no `l` for these). Every other member that lacks `l` (M_039C0, M_03FB1, M_04940) matches under `/Ol` as it is. |
| 5 | Strips `static` from `near` functions | 29 static near functions are called from other objects. In the original they were non-static near functions in a shared `/NT` segment, declared in the owner's header. Fix in the sources. |
| 6 | Prepends `#include "CRT.H"` and near prototypes taken from `.STB` stubs | CRT.H is empty now; delete. The near prototypes go into the owning headers (item 5). |
| 7 | `#define F_0066F main` | Name the function `main` in M_00659.C. |
| 8 | One CL flag set per unit = union of per-function DB flags (`module.unit_flags`); `/NT S<region>_TEXT` | Write each file's real command line in the MAKEFILE (6 distinct flag sets). The segment names are not in the EXE: placeholders, like other names. |
| 9 | Places FONTDATA/CGAFONT/CRTORDER by a region rule in code; the link order comes from LAYOUT.JSON | Just lines in a checked-in LINK response file. |

## Scaffolding that goes away once the files are real

- `.STB` stubs (41), the per-source repetition of shared statics, `module.merge` / `split` / `merge_asm`: all exist because `./sr check` compiles a split `M_*.C` alone. With one file per unit, check compiles the unit (the layout build already gates every unit) and compares the function asked for.
- The legacy `./sr build` (work/game: four renamed code segments, public near definitions in staging copies, 55 unresolved symbols) and the README section that presents it as the build. `src/data/SLOTS.ASM` (unused; SLOTS.C is linked).
- `LAYOUT.JSON` shrinks to nothing once the MAKEFILE and LINK response file are the sources of truth; the Python side only runs MAKE in DOSBox-X and compares.

## Not hacks, but not original either (document, cannot be recovered)

- Names: globals, statics, far storage, struct members, file names, segment names. Many are constrained by LINK's and MSC's hash orders; they are placeholders that reproduce the layout.
- PROVISIONAL source: the two unreachable far accesses (`x_trk0`/`x_trk1`, M_0F38A.C), the filler `g_6c42`, the evidence-only structs (`State70C0`, `State7D2E`, `State7E8A`).
- Data files (`src/data/*.ASM`, MESSAGES.ASM, CARMASK*.C, HOTSPOT.C): the assets as source records. Their original form is unknown (could have been converter output); the records are a fair form.
- emu2 runs the compiler and assembler instead of DOS. It is proven byte-identical to DOSBox-X on every source; the final build should run in DOSBox-X end to end anyway.

## Order of work

1. MLIBCE: build it with LIB in the build, link with the default library, delete the extracted-member machinery and the `runtime`/`emulator` plan. (Proven.)
2. CRTORDER: find the original's `_TEXT` declaration (M_21962.ASM first), delete the anchor.
3. Source fixes that do not need the merge: `main`, CRT.H, non-static near functions with header prototypes; loop pragmas are done (explicit `#pragma loop_opt` in M_01BAE.C, M_03C8D.C, M_2382D.C).
4. Merge each unit into one real file (C and asm), with its own segment directive and pragmas; switch `./sr check` to unit compiles; delete STB, merge/split. (GLOBALS.H/PROTOS.H are gone since 2026-11-04.)
5. Check in MAKEFILE + LINK.RSP (+ the LIB step); the build is `MAKE` under DOSBox-X; retire the legacy build, LAYOUT.JSON, SLOTS.ASM; update README/CLAUDE.md.
6. Repair `work/dg/run.sh` / `./sr dgroup apply` or retire it once the data lives in final files (it currently moves `g_4fa6` to the wrong file).
