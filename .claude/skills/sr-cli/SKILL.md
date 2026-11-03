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
| `tools/sr/dosbox.py` | `Job` and `compile_all`: lays jobs out in `work/<name>-<pid>/Jnnnn` (safe in parallel) and runs them with the `SR_EMU` backend: `emu2` (default, `emu2.py`) or `dosbox` (one headless DOSBox-X boot over RUN.BAT); `.STB` sidecars are prepended, `@NAME` lines expanded by `stb.py` (`C:` work dir, `D:` MSC 5.1, `E:` src/include, `F:` MASM 5.1) |
| `tools/sr/emu2.py` | the emu2 backend: one `timeout`-bounded emu2 process per job (`SR_JOBS` threads), same drives and current directory as DOSBox, DOS stdout to `CC.LOG`, own `TMP` per job. emu2 itself is `tools/nix/emu2.nix` (upstream pin + `emu2-flush-before-fork.patch`) |
| `tools/scripts/emucmp.py` | proves emu2 == DOSBox-X: snapshots sources and headers, compiles every source × flag sets, the 46 sweep combos, probes (FP, errors, MASM) and optionally `--streetrod DIR` under both, compares every output file |
| `tools/sr/omf.py` | OBJ segments, data, fixups with their targets, publics, externs (incl. MSC's LEXTDEF/LPUBDEF for statics) |
| `tools/sr/cod.py` | `/Fc` listing: procedure start/end and the locals layout; `from_publics` gives the extent of an assembled function |
| `tools/sr/match.py` | byte compare (fixups masked, near calls out of the function counted separately) and the two-column diff |
| `tools/sr/lib.py` | finds MSC library modules in the EXE (`crtnames`); `locate` returns every public at an address, since a module can publish two names for one offset (`__aFlmul` = `__aFulmul`) |
| `tools/sr/refs.py` | resolves every fixup to its symbol and checks it against the original instruction |
| `tools/sr/headers.py` | generates `GLOBALS.H` / `PROTOS.H` from the DB |
| `tools/sr/layout.py` | the MSC 5.1 local-layout rule (`./sr layout`): bucket hash, block slot reuse, `/Ol` variant, rename suggestions |
| `tools/sr/lint.py` | the CLAUDE.md rule 4 lint, plus missing-prototype warnings from `/W3` |
| `tools/sr/db.py` | SQLite schema and helpers; the `aliases` table (filled by `crtnames`) holds the extra library names of a function, and `refs` accepts any of them for a call |

- `tools/sr/module.py`: modules (`./sr module`). `units()` derives same-file spans from near / `push cs; call near` edges; `merge()`/`split()` move text between singletons and `src/mods/NAME.C`; `gate()` compares the whole span (resolves in-module EXTDEF calls and code-segment offsets per member frame, masks the rest). `cli.py` runs the gate from `_check_rows` once per module object and stores it on each member's `Result.module`.

## Rules

- **`./sr scan` must keep human data.** It updates seg/off/size/far, and it never overwrites `name`, `status`, `src`, `flags`, `notes` or `kind` of existing rows. If the scanner stops finding a row, it warns and doesn't delete it.
- **The comparer must stay strict.** Only bytes covered by an OBJ fixup are masked, plus near-call displacements whose target is outside the function. Never add a mask to get a function to pass; if a real class of unverifiable bytes appears, report it as its own count (like `nearcalls`) and get the user's agreement.
- **Every emulator run is bounded** by `timeout` (rule in CLAUDE.md); emu2 also by RLIMIT_FSIZE.
- **The backends must stay byte-identical.** After changing `dosbox.py`, `emu2.py`, `tools/nix/emu2.nix` or the job layout, run `nix-shell shell.nix --run 'python3 tools/scripts/emucmp.py '` and expect `IDENTICAL`. Never make a backend the default, or change its pin, without that. The only tolerated differences are MASM's LST page-header time and its "symbol space free" line (emu2 gives MASM the full 640K; DOSBox-X keeps COMMAND.COM resident), which `emucmp` reports separately.
- **No compile cache.** A compile is ~25 ms under emu2; a cache would add a way to get stale results for no measurable gain.
- After a change, run `./sr check --all` and at least one `./sr show`/`./sr diff` to confirm nothing regressed.
- Update the command list in `CLAUDE.md` and any skill that mentions a changed command, in the same change.

## Known limitations (fix when they bite)

- `sr scan` reaches functions through calls, far-pointer tables and prologue patterns. Functions only reached by computed jumps can be missed or split into fragments (a few `kind=c` rows without a prologue are fragments of bigger functions).
- `sr check` compares a procedure by name against the extent recorded by the scanner. A function with a jump table after its code may have its extent end early.
- Single-function compiles can't verify near calls to other functions (`MATCH*`). Module files (several functions in one `.C`, via `./sr src`) will verify them once the module's layout matches.
- An assembled function's extent runs to the next `PUBLIC` in the OBJ, so put one function per `.ASM` until modules are formed, or make every procedure in a module `PUBLIC`.
