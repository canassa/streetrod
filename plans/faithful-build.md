# A faithful build: the original files, the original tools, no build-time edits

Goal: the tree holds the source files the developers had (one per original object), plus a
MAKEFILE and LINK response file of the kind they used, and running MSC 5.1 / MASM 5.1 /
LIB 3.11 / LINK 5.01 on them (under DOSBox-X, unmodified binaries) produces `orig/SR.EXE`.
Nothing between the files in git and the tools may rewrite text, extract library members or
choose link order outside the response file.

**Status (2026-11-04): done.** `src/` holds one source file per original object, the
checked-in `src/MAKEFILE` (MAKE 4.07) and `src/SR.LNK`. `MAKE MAKEFILE` in DOSBox-X
(`./dosbox.sh --make`) and the host build (`./sr build`, the same commands under emu2 and
DOSBox-X) both produce `orig/SR.EXE` byte for byte. Nothing rewrites a file between git and
the tools. Data definitions live in the sources (`./sr dgroup check` only verifies them),
and the headers are source (`./sr headers --check`). What the tree contains beyond the
recovered facts is listed in [docs/RECONSTRUCTION.md](../docs/RECONSTRUCTION.md).

Nothing in the build remains to be done. What is still open is the reconstruction itself:
real names, the record structure of HOTSPOT.C's flattened tables, the original text behind
the PROVISIONAL dummies, and the remaining compiler warnings that need struct retyping
(C4047). None of it is needed for the identical build.

The rest of this file records how the build got there (the assessment of 2026-11-04,
after aa49a8d, with each step's resolution).

## What the current build does that the developers did not

| # | Step in `tools/sr/linklayout.py` | Status |
|---|---|---|
| 1 | Extracted 98 MLIBCR/LIBH and 2 EM.LIB members and linked them as objects in a recovered order (`runtime`, `emulator` in LAYOUT.JSON), with `/NOD` and no libraries | **Done** (2026-11-04): the layout build runs `LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;` (the combined library MSC SETUP builds), as a MAKEFILE rule, then `LINK /MAP /STACK:2560 /EXEPACK` with no `/NOD`, using the objects' default library → packed EXE IDENTICAL. `/STACK:2560` is an ordinary LINK option (header SP=0A00h). Other component orders differ (5381..12426 bytes); the extracted-member order was the order LINK pulls from this library. |
| 2 | `CRTORDER.ASM`: an empty `_TEXT` anchor before region 2196 | **Solved**: M_21962.ASM is a `.MODEL SMALL` file (far PROCs, code in its own `S2196_TEXT SEGMENT`, since small model rejects `.CODE name`). MASM's `.MODEL SMALL` emits the `_TEXT` SEGDEF first, exactly where the anchor was; the anchor is gone. |
| 3 | Concatenates 116 `M_*.C` into 55 generated `U_*.C` (`module.merge`: hoists includes, near forward declarations, statics) and 31 `.ASM` into 27 (`merge_asm`: renames labels, drops `END` and all but the first source's `.MODEL`, rewrites `.CODE` to the region segment) | **Done** (2026-11-04, branch faithful-merge): each unit is one file in `src/mods` (named after its first source; 151 sources → 86 files: 55 C, 27 asm, 4 data). The C files are the text the layout build compiled (includes, then the statics and `static near` forward declarations in their original order, functions in address order, the comments and loop pragmas where they were); the assembler files name their segment in `.CODE S<region>_TEXT` and keep only labels that clashed prefixed. `./sr check` and the build compile the file as it is in git, under its own name: C objects byte-identical to the old `U_*` objects, MASM objects differ only in the empty `.MODEL` segment's name (`M_xxxxx_TEXT`). |
| 4 | ~~Inserts `#pragma loop_opt(on/off)` before every member source~~ | **Done** (branch faithful-loopopt): the injection is gone; the unit text is the concatenated sources. Five functions lose their match under the unit's `/Ol`: F_01BAE, F_01FE2, F_0238C (U_00D78), F_03C8D (U_039C0), F_2382D (U_23262). In each, the first divergence is a `for`/`while`/`do` loop that the original leaves as a plain memory loop (F_2382D's byte copy becomes `rep movsw`). `/Ol` takes every structured loop of these sizes: `while` and `unsigned char` forms of F_2382D, and `for`/`while`/`do` forms of F_0238C's gear search, all still get registers and loop temps. Only `goto` loops escape it (0x29170), and rewriting every loop of five functions that way is less plausible than a pragma, which in U_00D78 is a single line covering its last three functions. So the sources now carry the real MSC 5.1 pragma, with a comment: `loop_opt(off)` before F_01BAE (in effect to the end of U_00D78), off/on around F_03C8D, off before F_2382D (a word move would also break its write-mode-1 latch copy). Recorded per-function flags are unchanged (no `l` for these). Every other member that lacks `l` (M_039C0, M_03FB1, M_04940) matches under `/Ol` as it is. |
| 5 | Strips `static` from `near` functions | **Done** (2026-11-04): the 29 near functions called from other link units are public `T near F(...)` in the sources, prototyped in their owner's header (`./sr proto ... "T near @(...)"`); the strip is gone. |
| 6 | Prepends `#include "CRT.H"` and near prototypes taken from `.STB` stubs | **Done** (2026-11-04): CRT.H deleted; the near prototypes come from the owning headers. |
| 7 | `#define F_0066F main` | **Done** (2026-11-04): the function is `main` in M_00659.C (now M_00000.C) and the DB. |
| 8 | One CL flag set per unit = union of per-function DB flags (`module.unit_flags`); `/NT S<region>_TEXT` | **Recorded per file** (2026-11-04): LAYOUT.JSON `units[].flags` is the file's one line (7 distinct: 6 CL sets, `/Ml`); C adds `/NT S<region>_TEXT`. `functions.flags` is gone; `./sr flags FILE` edits the unit. **Done** (2026-11-04, branch faithful-makefile): each file's line is its block in the checked-in `src/MAKEFILE`, `/NT` written out. The segment names are not in the EXE: placeholders, like other names. |
| 9 | Places FONTDATA/CGAFONT by a region rule in code; the link order comes from LAYOUT.JSON | **Done** (2026-11-04): lines of the checked-in `src/SR.LNK`; `./sr dgroup order` only checks it against the DGROUP evidence. |

## Scaffolding that goes away once the files are real

- ~~`.STB` stubs (41), the per-source repetition of shared statics, `module.merge` / `split` / `merge_asm`~~: **gone** (2026-11-04) with `stb.py`, `module create/add/dissolve/list/show`, `./sr new`, `./sr src` and `functions.module`. `./sr check` compiles the function's whole file and runs its gate; 551 functions are `MATCH`, 21 `MATCH*` (near calls to public near functions of other objects in segment 0000, verified by the linked build).
- ~~The legacy `./sr build` (work/game: four renamed code segments, public near definitions in staging copies, 55 unresolved symbols) and the README section that presents it as the build. `src/data/SLOTS.ASM` (unused; SLOTS.C is linked).~~ **Gone** (2026-11-04): `./sr build` runs src/MAKEFILE; SLOTS.ASM is deleted.
- ~~`LAYOUT.JSON`~~: **gone** (2026-11-04). The units' spans are the DB functions' `src`, flags and segments are MAKEFILE lines, `code_starts` follows from the segment names, the link order is SR.LNK, the data-only files' DGROUP ranges and the order evidence are `dgroup.DATA_UNITS` / `ORDER_HINTS` (analysis, not build input). The host build runs the MAKEFILE's commands as written (emu2 for CL/MASM/LIB, DOSBox-X for LINK); `./dosbox.sh --make` runs MAKE itself in DOSBox-X.

## Not hacks, but not original either

Moved to [docs/RECONSTRUCTION.md](../docs/RECONSTRUCTION.md), which lists every
reconstruction choice with its evidence and limits.

## Order of work

1. ~~MLIBCE: build it with LIB in the build, link with the default library, delete the extracted-member machinery and the `runtime`/`emulator` plan.~~ Done.
2. CRTORDER: done (M_21962.ASM is small model; see item 2).
3. Source fixes that do not need the merge: `main`, CRT.H, non-static near functions with header prototypes (done); loop pragmas are done (explicit `#pragma loop_opt`, now in M_00D58.C, M_039C0.C, M_23262.C).
4. ~~Merge each unit into one real file (C and asm), with its own segment directive and pragmas; switch `./sr check` to unit compiles; delete STB, merge/split.~~ Done (2026-11-04).
5. ~~Check in MAKEFILE + LINK.RSP (+ the LIB step); the build is `MAKE` under DOSBox-X; retire the legacy build, LAYOUT.JSON, SLOTS.ASM; update README/CLAUDE.md.~~ Done (2026-11-04): `src/MAKEFILE` + `src/SR.LNK`; `./sr build` runs their commands and needs neither the DB nor orig/ for the build part; headers in git are source (`./sr headers --check`).
6. ~~Repair or retire `./sr dgroup apply`.~~ Done (2026-11-04): the sources are the truth for data; `apply`, `asm`, `register --write` and `aliases --write` are gone, and `./sr dgroup check` verifies ownership, alias views and DB coverage without writing.
7. ~~Final cleanup: unused structs, stale DB notes, dead tool code, scratch-script references in the docs, the reconstruction list.~~ Done (2026-11-04).
