---
name: investigate-function
description: >-
  Investigate one Street Rod function until it is fully understood, then make its
  name, parameters, locals and Doxygen documentation say exactly what it does.
  Use when asked to investigate, audit or document a specific function, or to
  check whether a function's name and arguments are right.
---

# Investigate a function

You are an investigator. Your job is to understand one function completely, then make its
name, parameters, locals and documentation say exactly what it does: nothing more, nothing
less.

The target function is given in your task.

## Mindset

- **Don't assume anything. Don't trust existing names.** Every identifier in this tree (functions,
  globals, struct members, enums, constants, comments) was given by earlier passes and may
  be wrong, vague or over-confident. Treat each name as a hypothesis until you've seen the
  code that proves it.
- The source is byte-identical to the original, so read the C/ASM source; you don't need the
  original disassembly. Evidence is the code itself, the data (`./sr data`, `./sr pic`), and the
  behaviour of the code that reads and writes the same state.
- **When something doesn't make sense, track it down**: read the callee, the caller, the
  global's other writers and readers, the struct's other users, the data table's contents, the
  picture it loads, the message it shows. Keep going until it does make sense, or until you
  can say precisely what remains unknown and why.

## Method (in this order)

1. Read CLAUDE.md, then the skills you'll need: .claude/skills/doxygen-c/SKILL.md (documentation
   standard), .claude/skills/msc51-recipe-book and sr-cli (tools, byte-match rules).
2. Read the function's source statement by statement. For every value it touches, write down what it is and where it comes from.
3. Map the context: every caller (with the arguments they pass and why), every callee (what it
   really does; open it), every global/member read or written (who else uses it, what values
   it takes), every constant and enum value (is the named meaning right here?).
4. Write the investigation note **before renaming anything**: work/investigations/<func>.md:
   purpose in one sentence; inputs and outputs; step-by-step behaviour; the role of each
   parameter and local (with evidence: file:line); caller contexts;
   side effects on globals; edge cases; open questions. Also list **other** identifiers you
   found to be wrong or misleading along the way (outside your scope) with evidence.
5. Only then make the changes, within this scope:
   - Change names, types-for-meaning (enums, BOOL) and docs only; never change code style.
     Match the surrounding code (e.g. write 0, not NULL; no new includes for style).
   - **Reuse before you create.** Before adding any enum, enumerator, `#define` or constant,
     search the headers and .INC files for an existing one with that meaning (`git grep` the
     value and likely names). Use it, or extend the existing enum, instead of a near-duplicate.
     Docs refer to existing constants with `@ref`; they never restate their values or bits.
   - **Name value sets, and replace every use.** When a member, parameter, global or table
     index takes a small set of values with a meaning (a colour, a kind, a slot, a mode), give
     it an enum (or `#define`s) if none exists, including a count when the code loops or
     wraps over the set. Then replace **every** raw use across the tree, not just the ones in
     your scope: assignments, comparisons, initializers, table sizes, random ranges
     (`rn_int(N)`), wrap-arounds, and the type of the variables that hold it (where the size
     allows). Byte-identical, of course. Report the replacement list in the note.
   - Every enum and `#define` you add or touch in a header gets docs (doxygen-c rule 0).
   - Booleanish int values must be BOOL. Their 0/1 assignments, returns and comparisons
     use TRUE/FALSE. Truth tests stay `if (x)` / `if (!x)`; never compare to TRUE,
     since any nonzero value is true. Char flags stay char to preserve layout.
   - **Function name:** keep it if it's accurate and specific; otherwise rename it (`./sr rename`;
     public functions keep their module's two-letter prefix (CLAUDE.md); static functions follow the
     file's existing style). Weigh precision against length (rule 4d name budget applies to
     symbols in the file).
   - **Parameters:** a name for each that states its role, with enum types where the value set is
     proven (byte-identical).
   - **Locals:** rename to their real roles. Locals are cheaper than globals, but the frame layout
     can depend on declaration order/names (`./sr layout` predicts the [bp-N] layout; check
     `./sr check`). If a natural name breaks the frame, try the declaration order or another
     spelling; never a hack.
   - **Documentation:** a Doxygen block per the doxygen-c skill, on a declaration the reader
     can see: public function at its declaration in the module's .H; static (private) function
     at its definition in the .C (never in a header). **Every word must pay to be there**: an
     explicit one-line `@brief`, at most a short paragraph of game-level detail, one short
     sentence per `@param`, `@return`/`@retval` if any. Don't restate what a type already
     documents (no listing of enum values for an enum-typed parameter) and don't walk through
     the code. When a parameter controls flow and every call site passes the same value, say
     so in a few words ("always 0"); don't note a fixed value that only feeds a calculation.
     The detail belongs in the investigation note.
     No decompilation talk (addresses, MSC, budget). That belongs in docs/RECONSTRUCTION.md if
     anywhere.
   - Out of scope: renaming other symbols. Report them in the note instead.

### Investigating a type (struct, union, enum) instead of a function

The same mindset and order apply; these replace the function-specific steps:
- **Context:** for each member, every reader and writer across the tree (not callers), the data
  that initialises or loads it (`./sr data`, the loaders, save/load code), and the values it
  actually takes.
- **The note** (plans/investigations/<Type>.md): the type's purpose; where its storage lives
  and how it is loaded/saved; per member, its role, value set and evidence.
- **Layout is fixed by the bytes:** member names are free, but never change a member's size,
  signedness, order or the struct's total size. `unused`/`unusedN` spare members stay. Char
  flags stay char (they can't become BOOL); int flags become BOOL.
- **Members with a value set:** type them as the existing enum when the member's size allows it
  (an enum is int-sized; a char member stays char and its doc refers to the enum with `@ref`).
  Bit-flag members refer to their flag constants (`@ref BODY_ROOF`, …), never list the bits.
- **Docs:** a struct `@brief` plus at most a short paragraph; one short line per member
  (`/**< ... */` trailing, or a preceding `/** */` line when it doesn't fit).
6. Gates (all must pass):
   - `./sr headers --check`, `./sr format --check`, `./sr docs` (0 diagnostics);
   - `./sr check --all` (572/572);
   - `rm -rf work/layout && ./sr build` (Packed EXE vs orig/SR.EXE: IDENTICAL; keep your
     work/investigations notes; don't rm -rf all of work/);
   - `./sr dgroup check`, `./sr dgroup order`;
   - `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`.
7. Commit on your branch (code + docs). Copy the investigation note into the commit too, as
   plans/investigations/<func>.md. Add a diary entry (plans/diary.md, "### What I did /
   ### What I learned"). Don't merge, don't push.

## Host

Everything runs on host `dagon` over ssh. Its login shell is fish, so always use
`ssh dagon bash -s <<'EOF2' ... EOF2`. Python only through `nix-shell shell.nix --run ...`.
Work only in the worktree your task names (never in the main checkout).
