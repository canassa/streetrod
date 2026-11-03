---
name: decompile-function
description: Reconstruct ONE Street Rod function so it byte-matches the original, as C under MSC 5.1 or, for hand-written routines, as MASM 5.1 asm, using the ./sr loop (pick → show → new → write source → check/diff → record). Use when asked to decompile, match or migrate a function by address or name, or to "do the next one".
---

# decompile-function

The per-function loop. `./sr check` is the oracle: `MATCH` (or `MATCH*`) is done, anything else isn't.

## 0. Pick

```
./sr next                 # smallest todo C functions
./sr next --near 2fc8     # neighbours of a matched function: same module, likely same flags
./sr show <addr>          # read it first: size, far/near, callers, callees, strings
```

Prefer neighbours of matched functions: they share the module's flags and conventions. The scanner's `kind` is a guess: if the disassembly looks hand-written (see the recipe book's fingerprints), correct it with `./sr kind <addr> asm -m "<the tell>"` and take the asm path in step 2.

## 1. Read the disassembly

`./sr show <addr>` annotates far/near call targets (by name once known), DGROUP strings, and named globals. Work out:

- **Return type.** Look at the callers (`./sr show <caller>`): if none reads `ax` after the call, it's `void`, whatever `ax` holds at the end.
- **Signature.** `far` (RETF) functions are plain `int f(...)` in the medium model. Near functions (RET) are `static int near f(...)`. Arguments start at `[bp+6]` for far and `[bp+4]` for near. Use `cbw`/`cwd` and how the result is used to work out types.
- **Locals.** `sub sp,N` gives the frame size, and the `[bp-N]` slots give the locals. Count them and their sizes (arrays are big slots, used via `lea ax,[bp-N]`).
- **Control flow.** Find the shapes in the recipe book (boolean results, switch dispatch, loops).
- **Callees.** Every far callee needs a prototype in the DB: `./sr proto <addr> "int @(int n)"` (check `./sr proto <addr>` first; another file may already have set it). Use the DB name, or `F_xxxxx` until you know what it does.
- **Far data.** `mov es,[0xNNNN]` with `./sr show` saying "segment constant" is a `far` array in its own segment: declare it with `./sr global SEG:OFF g_name --decl "T far @[]"`, never as a DGROUP word.
- **Globals.** `[0xNNNN]` is a DGROUP global. `./sr global` lists the known ones; add a missing one with `./sr global NNNN g_NNNN --decl "int @"`. Struct types go in `src/include/STRUCTS.H`, inside the include guard (before the final `#endif`); reuse an existing one when the offsets fit.

## 2. Write the C

```
./sr new <addr>           # creates src/funcs/F_xxxxx.C, marks wip
```

Write the function the way a 1989 MSC programmer would have (CLAUDE.md rule 1): real string literals, no inline asm, no byte tricks. Include `"GLOBALS.H"` and `"PROTOS.H"`, plus any standard MSC headers; declare nothing in the file (rule 4).

**Hand-written functions:** `./sr new <addr> --asm` creates a MASM 5.1 skeleton (`.MODEL MEDIUM`, `.CODE`, a `PUBLIC` far or near `PROC`). Write it as the original programmer would: named local labels, `EXTRN` in `.DATA` for DGROUP globals (`EXTRN _g_47ea:BYTE`) and in `.CODE` for callees, instructions rather than `db`. `ret` inside a `FAR` proc assembles to `retf`. `./sr check` finds the function by its `PUBLIC` (`_name`) and measures to the next `PUBLIC`. See the recipe book's MASM section for encoding gotchas.

**Same-module callees.** If the original calls a function with `push cs; call near`, it was in the same source file. List it in `src/funcs/F_xxxxx.STB`, which `sr` prepends at build time: a far callee is one line `@F_xxxxx` (the placeholder is generated from its DB prototype, so set that first with `./sr proto`); a near (`static`) one is a hand-written trivial definition `static T near X(...) { }`. Keep the `.C` itself free of placeholders: a far callee's prototype comes from `PROTOS.H`; a near one is forward-declared in the `.C` as `static T near X(...);`. A prototype inferred only from callers is a guess: say so with `./sr note` on the callee.

The procedure name must equal the DB name (`./sr name` it first, or keep `F_xxxxx`), because `./sr check` finds the procedure by that name in the listing.

## 3. Check, diff, iterate

```
./sr check <addr>         # MATCH / MATCH* / DIFF, records result, prints the locals layout
./sr diff <addr>          # where it diverges, instruction by instruction
```

- **First DIFF in a new area:** once the control flow is right (sweep can't find `/Ol` before the loops exist), run `./sr sweep <addr>` once. If a recipe gives 0, record it with `./sr flags <addr> ...`, and say so in the diary: it probably applies to the whole module.
- **After that, a DIFF is a source difference.** Look up the first divergent instruction in `msc51-recipe-book` and fix the C. Don't keep sweeping flags.
- **Wrong `[bp-N]` offsets only:** the locals have the right sizes but the names order them differently (CLAUDE.md rule 3). `./sr check` prints the layout. Read the slot order off the original, then `./sr layout -w NAMES...` (top of the frame first) gives renames and a declaration order that reproduce it while keeping your names where it can. `./sr layout NAMES` predicts a layout without compiling.
- **Frame too big by 2 per `register` variable:** if the original has no home for the `si`/`di` value, drop the variable and write the expression at each use (MSC's CSE register); if it has one, move the register variable into the block that uses it (recipe book).
- **`refs: ... WRONG`:** the bytes match but a reference doesn't: wrong global, wrong callee, or a different string. Fix the source or the DB name; it's never noise once the bytes line up.
- **`MATCH*`:** fine. The near-call displacements will be verified once the function is compiled together with its module.

## 4. Record

On `MATCH`, `./sr check` sets `status=matched`. Then:

```
./sr check --all          # nothing else regressed
./sr status               # refresh docs/PROGRESS.md
```

If the match needed something you can't justify as 1989 source (a dead variable, an odd cast), say so in `./sr note` so it gets reviewed. `./sr note` appends (`--replace` overwrites). If you can't get it to match, record why and move on:

```
./sr mark <addr> partial -m "control flow matches; frame differs by 2 bytes, unknown local"
./sr mark <addr> blocked -m "needs MASM" 
```

## 5. Teach the next session

If this function taught you a new codegen tell, add a row to `msc51-recipe-book` now, not at the end. If a step here was wrong or missing, fix this skill. Note anything notable for the diary (`wrap-up`).
