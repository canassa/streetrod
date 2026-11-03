---
name: sr-cli
description: Change or extend the ./sr CLI (tools/sr/) for the Street Rod project, i.e. the scanner, DB schema, DOSBox compile backend, OBJ/COD parsers, and the byte comparer. Use when a command is wrong or missing, or when the workflow needs a new tool. Keeps CLAUDE.md and the skills in sync with the CLI.
---

# sr-cli

## Map

| File | Job |
|---|---|
| `tools/sr/cli.py` | argparse commands; one `cmd_*` function each |
| `tools/sr/config.py` | paths, default flags, address ranges (CRT, emulator, DGROUP) |
| `tools/sr/exe.py` | the unpacked image, relocations, DGROUP strings |
| `tools/sr/disasm.py` | capstone wrapper; decodes the `INT 34h`-`3Dh` emulator ops as `emu_*` |
| `tools/sr/scan.py` | function finder (recursive descent plus prologue passes) and the C/asm/CRT classifier |
| `tools/sr/dosbox.py` | builds many jobs in one headless DOSBox-X boot in `work/<name>-<pid>` (safe in parallel): `cl` for `.C` (with an `.STB` sidecar prepended, `@NAME` lines expanded by `stb.py` from DB prototypes), `masm` for `.ASM` (`C:` work dir, `D:` MSC 5.1, `E:` src/include, `F:` MASM 5.1) |
| `tools/sr/omf.py` | OBJ segments, data, fixups with their targets, publics, externs (incl. MSC's LEXTDEF/LPUBDEF for statics) |
| `tools/sr/cod.py` | `/Fc` listing: procedure start/end and the locals layout; `from_publics` gives the extent of an assembled function |
| `tools/sr/match.py` | byte compare (fixups masked, near calls out of the function counted separately) and the two-column diff |
| `tools/sr/lib.py` | finds MSC library modules in the EXE (`crtnames`) |
| `tools/sr/refs.py` | resolves every fixup to its symbol and checks it against the original instruction |
| `tools/sr/headers.py` | generates `GLOBALS.H` / `PROTOS.H` from the DB |
| `tools/sr/layout.py` | the MSC 5.1 local-layout rule (`./sr layout`): bucket hash, block slot reuse, `/Ol` variant, rename suggestions |
| `tools/sr/lint.py` | the CLAUDE.md rule 4 lint, plus missing-prototype warnings from `/W3` |
| `tools/sr/db.py` | SQLite schema and helpers |

## Rules

- **`./sr scan` must keep human data.** It updates seg/off/size/far, and it never overwrites `name`, `status`, `src`, `flags`, `notes` or `kind` of existing rows. If the scanner stops finding a row, it warns and doesn't delete it.
- **The comparer must stay strict.** Only bytes covered by an OBJ fixup are masked, plus near-call displacements whose target is outside the function. Never add a mask to get a function to pass; if a real class of unverifiable bytes appears, report it as its own count (like `nearcalls`) and get the user's agreement.
- **Every DOSBox run is bounded** by `timeout` (rule in CLAUDE.md).
- After a change, run `./sr check --all` and at least one `./sr show`/`./sr diff` to confirm nothing regressed.
- Update the command list in `CLAUDE.md` and any skill that mentions a changed command, in the same change.

## Known limitations (fix when they bite)

- `sr scan` reaches functions through calls, far-pointer tables and prologue patterns. Functions only reached by computed jumps can be missed or split into fragments (a few `kind=c` rows without a prologue are fragments of bigger functions).
- `sr check` compares a procedure by name against the extent recorded by the scanner. A function with a jump table after its code may have its extent end early.
- Single-function compiles can't verify near calls to other functions (`MATCH*`). Module files (several functions in one `.C`, via `./sr src`) will verify them once the module's layout matches.
- An assembled function's extent runs to the next `PUBLIC` in the OBJ, so put one function per `.ASM` until modules are formed, or make every procedure in a module `PUBLIC`.
