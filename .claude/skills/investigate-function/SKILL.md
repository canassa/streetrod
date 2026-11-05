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
