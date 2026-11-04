---
name: decompile-function
description: Reconstruct or rework ONE Street Rod function in its source file so it byte-matches the original, as C under MSC 5.1 or, for hand-written routines, as MASM 5.1 asm, using the ./sr loop (show → edit in its file → check/diff). Use when asked to decompile, match, rewrite or fix a function by address or name.
---

# decompile-function

The per-function loop. `./sr check` is the oracle: `MATCH` (or `MATCH*`) is done, anything else isn't.

All 572 game functions are matched and live in their original object's file (`src/mods/M_xxxxx.C|ASM`, one per object of `src/MAKEFILE`). A function is always edited in that file and compiled with the whole file and the file's MAKEFILE command line: there are no per-function files, placeholders or flags any more. A function the scanner newly finds (`./sr scan` reports it) goes into the file whose span contains it, with its address comment above it (`/* name @ SSSS:OOOO (linear 0xAAAAA, N bytes) */`): that comment is what the tools know about the function (a file's extent is the span of its functions).

## 0. Pick

```
./sr show <addr>          # read it first: size, far/near, callers, callees, strings, its file and flags
```
 The scanner's `c`/`asm` guess for a new function is only a guess: if the disassembly looks hand-written (see the recipe book's fingerprints), take the asm path in step 2 (the kind is the file's extension).

## 1. Read the disassembly

`./sr show <addr>` annotates far/near call targets (by name once known), DGROUP strings, and named globals. Work out:

- **Return type.** Look at the callers (`./sr show <caller>`): if none reads `ax` after the call, it's `void`, whatever `ax` holds at the end.
- **Signature.** `far` (RETF) functions are plain `int f(...)` in the medium model. Near functions (RET) are `static int near f(...)`. Arguments start at `[bp+6]` for far and `[bp+4]` for near. Use `cbw`/`cwd` and how the result is used to work out types.
- **Locals.** `sub sp,N` gives the frame size, and the `[bp-N]` slots give the locals. Count them and their sizes (arrays are big slots, used via `lea ax,[bp-N]`).
- **Control flow.** Find the shapes in the recipe book (boolean results, switch dispatch, loops).
- **Callees.** Every far callee needs a prototype in its file's header, `int name(int n);   /* 0xAAAAA */` (look there first; another file may already rely on it). Use its current name, or `F_xxxxx` until you know what it does.
- **Far data.** `mov es,[0xNNNN]` with `./sr show` saying "segment constant" is a `far` array in its own segment: declare it in the defining file's header as `extern T far g_name[];   /* SSSS:OOOO */`, never as a DGROUP word.
- **Globals.** `[0xNNNN]` is a DGROUP global. `./sr show` names the known ones (the headers' `/* DS:XXXX */` lines, statics' definitions); declare a missing one in its file's header, `extern int g_NNNN;   /* DS:NNNN */`. Struct types go in `src/include/STRUCTS.H`, inside the include guard (before the final `#endif`); reuse an existing one when the offsets fit.

## 2. Write the C

Edit the function where it is in its file (`./sr show` prints the file and its command line). Write it the way a 1989 MSC programmer would have (CLAUDE.md rule 1): real string literals, no inline asm, no byte tricks. Run `./sr headers` after using another file's symbol; declare nothing in the file except `static ... near` forward declarations of the file's own static functions (rule 4).

**Hand-written functions** are `PROC`s in the file's `.ASM`: named local labels (unique in the file: MASM 5.1 labels are file-global without a `.MODEL` language), `EXTRN` in `.DATA` for DGROUP globals (`EXTRN _g_47ea:BYTE`) and for callees in other files, instructions rather than `db`. `ret` inside a `FAR` proc assembles to `retf`. The file's `.CODE S<region>_TEXT` names the shared code segment. `./sr check` finds the function by its `PUBLIC` (`_name`) and takes its extent from the `PROC` length in the listing. See the recipe book's MASM section for encoding gotchas.

**Callees.** `push cs; call near` in the original means the callee is in the same file, which it now really is. A call to a function of another file (far, or a public near function in the same code segment) needs that file's header prototype. A prototype inferred only from callers is a guess: say so in the diary.

The procedure name must equal the name in the function's address comment (rename with `./sr rename`, or keep `F_xxxxx`), because `./sr check` finds the procedure by that name in the listing.

## 3. Check, diff, iterate

```
./sr check <addr>         # compiles the whole file: MATCH / MATCH* / DIFF, the file gate, the locals layout
./sr diff <addr>          # where it diverges, instruction by instruction
```

- **Flags are the file's** (`./sr flags FILE`). `./sr sweep <addr>` compiles the whole file under other flag sets, but changing a file's flags changes all its functions: a function that only matches with other flags points at a source difference (or, for a whole loop shape, a real `#pragma loop_opt`), not at new flags.
- **After that, a DIFF is a source difference.** Look up the first divergent instruction in `msc51-recipe-book` and fix the C. Don't keep sweeping flags.
- **Wrong `[bp-N]` offsets only:** the locals have the right sizes but the names order them differently (CLAUDE.md rule 3). `./sr check` prints the layout. Read the slot order off the original, then `./sr layout -w NAMES...` (top of the frame first) gives renames and a declaration order that reproduce it while keeping your names where it can. `./sr layout NAMES` predicts a layout without compiling.
- **Correct instructions but stubborn stack-slot differences:** do not assume every stack store names a local. Reconstruct related CSEs together, including their pointer/address expression and the control flow that bounds them. For 0x04036, removing only wd2 reduced the diff to five bytes; removing q and wd too, using repeated indexed reads and ternary assignments through the following loop, gave the complete match. Explicit if/else statements changed that CSE region.
- **Frame too big by 2 per `register` variable:** if the original has no home for the `si`/`di` value, drop the variable and write the expression at each use (MSC's CSE register); if it has one, move the register variable into the block that uses it (recipe book).
- **`refs: ... WRONG`:** the bytes match but a reference doesn't: wrong global, wrong callee, or a different string. Fix the source or the declaration; it's never noise once the bytes line up.
- **`MATCH*`:** fine. The function near-calls a public near function of another object; `./sr build` verifies those encodings (`Near-call encodings: 748/748 exact`).
- **`MODULE DIFF`** (the file gate): something between or across functions moved: a function's size, the bytes between two procedures, an internal call. Fix it before anything else; `./sr build` must stay IDENTICAL.

## 4. Verify

On `MATCH` (nothing is recorded: the sources are the record):

```
./sr check --all          # nothing else regressed
./sr build                # must end with `Packed EXE vs orig/SR.EXE: IDENTICAL`
```

If the match needed something you can't justify as 1989 source (a dead variable, an odd cast), list it in `docs/RECONSTRUCTION.md` and the diary so it gets reviewed. If you can't get it to match, say why in the diary and tell the user.

## 5. Teach the next session

If this function taught you a new codegen tell, add a row to `msc51-recipe-book` now, not at the end. If a step here was wrong or missing, fix this skill. Note anything notable for the diary (`wrap-up`).
