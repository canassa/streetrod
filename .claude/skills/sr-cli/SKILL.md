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
| `tools/sr/dgroup.py` | `./sr dgroup`: original DGROUP ownership from positions (literal pools of the layout objects pin each unit; runs between pools go to referrers; `FORCE_OWNER`/`DATA_UNITS` for assembler data and data-only files), declaration slots, C/MASM definition text with symbolic pointers, source insertion, the DGROUP.ASM remainder, `link_order` |
| `tools/sr/datacheck.py` | a module's own `_DATA` against the original: base from the code's references, bytes, every data fixup resolved by symbol; `module.gate(..., con)` reports it (strict once the module defines data) |
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

## Module and scanner lessons (2026-11-03)

- `module create/add/show/dissolve` supports homogeneous `.C` and `.ASM` groups. MASM merging namespaces local labels and removes only EXTRNs supplied by actual members; no padding or instruction bytes are manufactured. Whole-span gates apply to both languages.
- STB entries are definitions, not lines: keep multiline placeholders together when removing a member's stub. A public symbol before the first member belongs to a placeholder, not the module span. External near EXTDEF fixups must be counted even when their raw displacement is zero.
- Near-call hulls are candidate grouping spans, not proof of an original C file. Language boundaries, linker alignment, flag-sensitive source and MSC heap limits can require smaller groups. All 572 game functions are now assigned to modules; some units contain one function.
- The scanner now discovers complete prologue-free routines in bounded gaps. It requires reachable instruction coverage, in-gap branches, returns, and only trailing alignment; it rejects the entire gap on ambiguous bytes. The shipped-image regression covers nine recovered game routines and the retired 0x0F8C2 false positive. Existing human metadata is preserved by `scan`.
- Run `nix-shell shell.nix --run 'python3 -m unittest discover -s tests -v'` for scanner/module regressions, then the full `./sr check --all` gate.

## Linked-build lessons (2026-11-03)

- `build.py` stages actual code without `.STB`, gives cross-object near functions
  public linkage and a shared `/NT` code segment, and maps F_0066F to `main`.
  Keep these adaptations separate from the original-byte comparison pipeline.
- MSC MAKE 4.07 takes `MAKE MAKEFILE`, not `/f`, and processes rules sequentially;
  put the link rule last. `ENV.BAT` sets the same four drives as the host wrapper.
- CL uses fixed intermediate filenames. A shared native-build directory must
  compile serially; the comparison backend avoids this with isolated job dirs.
- LINK 5.01.21 crashes under emu2 on the full game. Use bounded DOSBox-X for LINK.
  Report Lxxxx diagnostics as well as unresolved symbols; zero missing symbols
  does not imply a successful link. Delete partial EXEs on failure.
- MASM external far-data/code declarations belong outside segments. Near pointer
  initializers need `OFFSET DGROUP:label`. EQU aliases discard the PUBDEF group;
  export actual LABELs (ORG permits overlapping views) so C references use DGROUP.
- Original DS:629C..6C21 belongs to the runtime. The FF bytes at old BSS addresses
  are unpacker filler, not data. Use zero initialization and real runtime storage.
  Validate with `check_data_types.py` and `reconstruct_data.py --verify-built`.

## Whole-binary layout lessons (2026-11-03)

- `./sr build --layout` compiles/gates 103 reconstructed code units + 12 data
  modules + an empty runtime segment-order anchor. LAYOUT.JSON selects unchanged
  MSC archive members; preparation never reads/extracts original game bytes.
- `./sr linkcheck [EXE]` is the raw linked-file gate. It masks nothing, fails on
  extra/missing bytes, and treats MZ relocation-row order as part of identity.
  Mapped function/call comparisons are diagnostics, never a replacement verdict.
- Dropping STB bodies must retain their typed near declarations. Otherwise an
  implicit C declaration creates a far call to a real near RET routine.
- `#pragma loop_opt(on/off)` preserves per-function /Ol behavior in combined C.
  MSC 5.1 does not recognize modern `#pragma optimize` (C4068).
- Adjacent MASM fragments can need one unit: odd function starts and CS-local
  state preceding a function are evidence. The three blitter ORGs merely copied
  low paragraph offsets; remove them and let LINK supply those offsets.
- Masked library-code similarity does NOT establish aliases. `/=`, `%=` and
  unsigned wrappers share bodies but call different helpers. Verify external
  far/near targets, and DATA-table identities (sin versus sinh). Include small
  jump shims and code members with no publics; suppress incidental submatches
  contained in larger members. Re-identification invalidates stale aliases.
- OMF COMDEF consumes external indices. Keep all DATA publics, and select the
  nonempty named code segment if MASM .MODEL first declares an empty default.
- CGA glyphs start at 0ED2:0004, WORD aligned after the complete EGA font. The
  old CgaFont header/overlap was a frame-offset artifact. PARTS2 ends at 3884,
  followed by EM.LIB's 0x170-byte EMULATOR_DATA, then SLOTS at 389B.
- LINK 5.01.21 COMMON allocation walks 256 symbol-hash buckets. Hash recovered
  from LINK's NE segment 2:4060: length + sum((byte|0x20) << (1-based-index&3)),
  low byte selects bucket. Bucket chains insert at head. Disassembly and 45-name
  forward/reverse declaration probes are in work/binary-match; helper linkhash.py.
  Historical global names are unknown; do not confuse reconstructed placeholders
  with recovered names. Verify allocation with actual LINK output.
- Raw data forensics: `tools/scripts/inspect_layout_data.py` derives compiler
  DATA/CONST ownership from original instructions and OBJ fixups. It also checks
  numeric FP operand values; all 94 currently match. Multiple pool bases can
  indicate missing named definitions within a TU, not merely a wrong TU split.

## DGROUP lessons (2026-11-04)

- The analysis reads the freshest unit objects (), the  listing for literal sizes ( is two bytes) and the source definitions for array bounds; run Prepared 119 source jobs and 100 unchanged MSC runtime objects in work/layout
119/119 compiled and gated source jobs
Link: 0 unresolved symbols (0 game data definitions)
Full diagnostics: /home/canassa/src/github.com/canassa/streetrod/work/layout/LINK.LOG; data inventory: /home/canassa/src/github.com/canassa/streetrod/work/layout/LINK.JSON
file: DIFF; 251267 differing, 0 missing, 400 extra bytes
header: DIFF; 8757 differing, 0 missing, 304 extra bytes
load_image: DIFF; 34833 differing, 0 missing, 96 extra bytes
MZ relocations: 2170 vs 2218; order different
Mapped game functions: 138/572 raw-byte exact, 572/572 at original positions, 0 missing
Near-call encodings: 748/748 exact
Unmasked comparison report: /home/canassa/src/github.com/canassa/streetrod/work/layout/BYTE-MATCH.JSON before . Definitions carry ;  strips and rewrites them, so it is idempotent.
-  deletes the alias rows: run it once; a later decl change that creates new overlaps needs a fresh run, and it also removes  lines for the aliases in  files (add the base's EXTRN before the first use when it only exists in a later  block).
- The DB is WAL-backed: restoring  from a backup needs / removed first.
- An assembler unit's data goes into its  section (//); EXTRNs for symbols it now defines are removed.

## DGROUP lessons (2026-11-04)

- The analysis reads the freshest unit objects (`work/layout/BUILD/U_*/`), the `/Fc` listing for literal sizes (`"\0"` is two bytes) and the source definitions for array bounds: run `./sr build --layout` before `./sr dgroup`. Definitions carry `/* DS:xxxx */`; `apply` strips and rewrites them, so it is idempotent. `work/dg/run.sh` runs apply, asm, order, the gate and the layout build in one go.
- `aliases --write` deletes the alias rows: run it once. A later decl change that creates new overlaps needs a fresh run, and the run also removes `EXTRN` lines for the aliases in `.ASM` files (put the base's EXTRN before the first use when it only exists in a later `.DATA` block).
- The DB is WAL-backed: restoring `db/sr.sqlite` from a backup needs `-wal`/`-shm` removed first.
- An assembler unit's data goes into its `.DATA` section (`PUBLIC`/`LABEL`/`DB`); EXTRNs for symbols it now defines are removed.

## Link-structure lessons (2026-11-04)

- `./sr dgroup apply all --write` strips every `/* DS:xxxx */` definition from `src/mods/*.C` before rewriting; after changing units, run the pipeline twice (the second pass reads the fresh objects of the new units). `work/dg/run.sh` is the pipeline.
- `order_hints` in LAYOUT.JSON move units (data-only units too, key `10xxxx`) after another unit; afterwards each code segment's units are re-sorted into code order within their slots.
- Storage objects need `ASSUME DS:DGROUP`, or MASM writes their PUBDEFs without the group and every reference resolves segment-relative.
- Runtime members: declared `_DATA` length can exceed the data records (`DUP (?)`); read SEGDEF lengths when tiling runtime data.
