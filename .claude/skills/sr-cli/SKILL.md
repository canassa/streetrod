---
name: sr-cli
description: Change or extend the ./sr CLI (tools/sr/) for the Street Rod project, i.e. the scanner, the source loader, the DOSBox compile backend, OBJ/COD parsers, and the byte comparer. Use when a command is wrong or missing, or when the workflow needs a new tool. Keeps CLAUDE.md and the skills in sync with the CLI.
---

# sr-cli

## Map

| File | Job |
|---|---|
| `tools/sr/format.py` | `./sr format [files]` / `--check`: explicit pinned style in `clang-format.yaml`, C/H only; headers and rename format changed C text |
| `tools/sr/docs.py` | `./sr docs`: Doxygen HTML/XML build, diagnostic gate, XML parameter lint and source-inventory coverage; config in Doxyfile, regression fixtures in tests/test_docs.py |
| `tools/sr/cli.py` | argparse commands; one `cmd_*` function each |
| `tools/sr/pics.py` | `./sr pic`: resource directory/RLE decoder, EGA planar and CGA packed pixels, stdlib PNG and kitty graphics output; `--list`, `--png`, `--scale`, `--cells COLS N`, `--mode ega|cga`, `--palette game|intro|standard|cga-green` |
| `tools/sr/config.py` | paths, address ranges, recursive source/header index, header staging and the INCLUDE environment read from SRENV.BAT |
| `tools/sr/source.py` | joins original function extents and data addresses in `tools/sr/symbols.json` by name to current source definitions and header declarations; derives source ownership, prototypes and near/far distance. Runtime functions, aliases, DGROUP publics and callees come from the EXE and MSC libraries, cached in `work/cache/runtime.json`. `reload()` after editing sources or metadata |
| `tools/sr/exe.py` | the unpacked image, relocations, DGROUP strings |
| `tools/sr/disasm.py` | capstone wrapper; decodes the `INT 34h`-`3Dh` emulator ops as `emu_*` |
| `tools/sr/scan.py` | function finder (recursive descent plus prologue passes) and the C/asm/CRT classifier |
| `tools/sr/dosbox.py` | `Job` and `compile_all`: lays jobs out in `work/<name>-<pid>/Jnnnn` (safe in parallel) and runs them with the `SR_EMU` backend: `emu2` (default, `emu2.py`) or `dosbox` (one headless DOSBox-X boot over RUN.BAT); the source is copied unchanged (`C:` work dir, `D:` MSC 5.1, `E:` a flat snapshot of all project headers, `F:` MASM 5.1) |
| `tools/sr/emu2.py` | the emu2 backend: one `timeout`-bounded emu2 process per job (`SR_JOBS` threads), same drives and current directory as DOSBox, DOS stdout to `CC.LOG`, own `TMP` per job. emu2 itself is `tools/nix/emu2.nix` (upstream pin + `emu2-flush-before-fork.patch`) |
| `tools/scripts/emucmp.py` | proves emu2 == DOSBox-X: snapshots sources and headers, compiles every source × flag sets, the 46 sweep combos, probes (FP, errors, MASM) and compiles under both, compares every output file |
| `tools/sr/omf.py` | OBJ segments, data, fixups with their targets, publics, externs (incl. MSC's LEXTDEF/LPUBDEF for statics) |
| `tools/sr/cod.py` | `/Fc` listing: procedure start/end, the locals layout, `_BSS` statics; `from_publics` + `asm_lengths` give an assembled function's extent (its PUBLIC, its PROC length from the MASM listing) |
| `tools/sr/match.py` | byte compare (fixups masked, near calls out of the function counted separately with their targets) and the two-column diff |
| `tools/sr/lib.py` | finds MSC library modules in the EXE (`crtnames`); `locate` returns every public at an address, since a module can publish two names for one offset (`__aFlmul` = `__aFulmul`); `locate_all` also places the located modules' data publics in DGROUP from their code's fixups |
| `tools/sr/refs.py` | resolves every fixup to its symbol and checks it against the original instruction |
| `tools/sr/headers.py` | `./sr headers`: calls `fileheaders.py` (include blocks) and `makefile.sync_deps`; `check()` is the dry run of `--check` (every write goes through `headers._write`, which records instead of writing) and returns the headers' problems too. Headers and sources are LF, MAKEFILE/SR.LNK CRLF. Never called by `build` or `check`. The headers themselves are hand-written |
| `tools/sr/fileheaders.py` | the include blocks of the C sources: declarations, struct/union tags, enums and macros with replacement text identify the header for each used name; include guards are excluded. Struct/union tags use their own namespace. `STRUCTS.H` (only the ownerless Word helper) and `PC.H` are standalone headers without owning sources; neither is included unconditionally. Module-header macros need no duplicate definition in the source; function and data declarations must match their source's definitions (`owners_layout`: code files, data-only files outside DATA in link order, then src/DATA); duplicate declarations are errors |
| `tools/sr/layout.py` | the MSC 5.1 local-layout rule (`./sr layout`): bucket hash, block slot reuse, `/Ol` variant, rename suggestions |
| `tools/sr/lint.py` | the CLAUDE.md rule 4 lint, plus missing-prototype warnings from `/W3` |
| `tools/sr/dgroup.py` | `./sr dgroup`: original DGROUP ownership from positions (literal pools of the layout objects pin each unit; runs between pools go to referrers; `FORCE_OWNER`/`DATA_UNITS` for assembler data and data-only files), declaration slots, the C definition text of each item (`report`), `check_sources` (`./sr dgroup check`, the only consumer of ownership), `link_order` |
| `tools/sr/datacheck.py` | a file's own `_DATA` against the original: base from the code's references, bytes, every data fixup resolved by symbol; `module.gate(..., data=True)` reports it (strict once the file defines data) |
| `tools/sr/rename.py` | `./sr rename OLD NEW` / `--file PAIRS`: one whole-word pass (`name` and MASM `_name`) over the sources, headers, docs, skills, tools and tests (not plans/diary.md); a file rename `git mv`s the source and its header (include guard renamed), updates the MAKEFILE and SR.LNK (link block rewrapped), then runs `./sr headers`. A new name must be unused, ignoring case (function, global and far data names, the runtime's names and aliases, every identifier of the sources). Runtime functions keep their library names |

- `tools/sr/makefile.py`: src/MAKEFILE and src/SR.LNK. `parse` handles the MAKE 4.07 subset the file uses (`#` comments, `NAME=value` macros, `target: deps` with `\` continuations, tab-indented commands); `compile_rules()` (blocks whose command starts `$(CC)`/`$(ASM)`: `Rule.flags`, `.segment` from `/NT`, `.source` DOS path, `.path` host file), `rule('LIB'|'LINK')`, `link_order()`. `set_flags` (`./sr flags`) and `sync_deps` (`./sr headers`, C `.H` and MASM `.INC` dependencies) are the only writers.
- `tools/sr/module.py`: the source files. `sources()` is every MAKEFILE object, `files()` the code files with their extent (span of the functions they define), `code_starts()` the first file of each segment; `File.command` is the per-file part of the line (`flags` + `/NT<segment>` for C). `file_of(row)` / `find_file(name)`. `gate()` compares a file's whole span (resolves in-file EXTDEF calls and code-segment offsets per function frame, masks the rest, counts near calls out of the file). `units()` derives candidate same-file spans from near / `push cs; call near` edges (`./sr module units`). `cli._check_rows` compiles each file once (`_file_jobs`), compares the functions asked for, runs `_file_gates` and stores each gate on its functions' `Result.module`; a function's `nearcalls` is then the number of its near calls to other objects.
- `tools/sr/linklayout.py`: `./sr build`: copies src/ to work/layout, runs each MAKEFILE block as written (`build.run_dos` under emu2, one `BUILD/<stem>` tree each; `build.link` runs the LINK line in DOSBox-X), then (unless `--no-verify`) `gate_objects`, the unpacked link and the comparison. The build part must not read orig/.

## Rules

- **Original locations are stored.** `tools/sr/symbols.json` holds only function segment/offset/size and data addresses, keyed by name. Register new symbols there; `./sr rename` maintains names. Declarations and ownership are derived from source and headers; a cache under `work/` must be keyed by everything it is derived from. `./sr scan` only compares the scanner with the sources (exit 1 on a difference); flags are the file's, in src/MAKEFILE.
- **The comparer must stay strict.** Only bytes covered by an OBJ fixup are masked, plus near-call displacements whose target is outside the function. Never add a mask to get a function to pass; if a real class of unverifiable bytes appears, report it as its own count (like `nearcalls`) and get the user's agreement.
- **Every emulator run is bounded** by `timeout` (rule in CLAUDE.md); emu2 also by RLIMIT_FSIZE.
- **The backends must stay byte-identical.** After changing `dosbox.py`, `emu2.py`, `tools/nix/emu2.nix` or the job layout, run `nix-shell shell.nix --run 'python3 tools/scripts/emucmp.py '` and expect `IDENTICAL`. Never make a backend the default, or change its pin, without that. The only tolerated differences are MASM's LST page-header time and its "symbol space free" line (emu2 gives MASM the full 640K; DOSBox-X keeps COMMAND.COM resident), which `emucmp` reports separately.
- **No compile cache.** A compile is ~25 ms under emu2; a cache would add a way to get stale results for no measurable gain.
- After a change, run `./sr check --all` and at least one `./sr show`/`./sr diff` to confirm nothing regressed.
- Update the command list in `CLAUDE.md` and any skill that mentions a changed command, in the same change.

## Known limitations (fix when they bite)

- `sr scan` reaches functions through calls, far-pointer tables and prologue patterns. Functions only reached by computed jumps can be missed or split into fragments (a few `kind=c` rows without a prologue are fragments of bigger functions).
- `sr check` compares a procedure by name against the extent in `tools/sr/symbols.json`. A function with a jump table after its code may have its extent end early.
- A near call to a function of another object can't be verified from one object (`MATCH*`, 21 functions); `./sr build` verifies all 748 near-call encodings.
- An assembled function's extent is its `PROC` length in the MASM listing; without a listing, it runs to the next `PUBLIC` in the OBJ. Every procedure of an `.ASM` is `PUBLIC`.

## Source-file and scanner lessons

- (2026-11-04) The split `M_*` sources, `.STB` placeholders, `module create/add/dissolve`, `./sr new`/`./sr src` and per-function flags are gone: one file per object. The merge kept the members' declaration order: moving `static ... near` forward declarations relative to the statics, or the communal definitions, or the includes, changes the order of MSC's EXTDEF/LEXTDEF/COMDEF records, so the object differs (the regenerated include blocks did this and the EXE stayed IDENTICAL; recheck with `./sr build` after such a move).
- MASM 5.1 labels are file-global (no `.MODEL` language): a local label name used by two procedures of one file is numbered (`done1`, `done2`).
- External near EXTDEF fixups must be counted even when their raw displacement is zero.
- Near-call hulls are candidate grouping spans, not proof of an original C file; the units come from the relocation table (CLAUDE.md 4c).
- The scanner now discovers complete prologue-free routines in bounded gaps. It requires reachable instruction coverage, in-gap branches, returns, and only trailing alignment; it rejects the entire gap on ambiguous bytes. The shipped-image regression covers nine recovered game routines and the retired 0x0F8C2 false positive.
- Run `nix-shell shell.nix --run 'python3 -m unittest discover -s tests -v'` for scanner/gate regressions, then the full `./sr check --all` gate.

## Linked-build lessons (2026-11-03)

- The sources name `main` and declare the cross-object near functions public
  (`T near F`, prototype `T near F(...);   /* 0xAAAAA */` in the owner's header);
  nothing rewrites the text before CL or MASM sees it.
- MSC MAKE 4.07 takes `MAKE MAKEFILE`, not `/f`, and processes rules sequentially;
  the link rule is last. It accepts macros, `\` continuations whose next line starts
  with a tab (as in MSC's STARTUP\MAKEFILE) and paths like `SYS\X.C`. CL writes the
  OBJ and `/Fc` listing into the current directory; a source path only changes the
  object's THEADR record (`SYS\X.C`), not code or segment names.
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
  Validate struct shapes with `tools/scripts/check_data_types.py`.

## Whole-binary layout lessons (2026-11-03)

- `./sr build` compiles/gates the 80 code files + the data-only files and
  src/DATA records, one object each. The runtime comes from
  MLIBCE.LIB, built by LIB 3.11 from the untouched MLIBCR+LIBH+EM+MLIBFP and
  linked as the default library; preparation never reads original game bytes.
- `./sr linkcheck [EXE]` is the raw linked-file gate. It masks nothing, fails on
  extra/missing bytes, and treats MZ relocation-row order as part of identity.
  Mapped function/call comparisons are diagnostics, never a replacement verdict.
- A near function called from another object needs its `T near` prototype in its
  header: an implicit declaration creates a far call to a near RET routine.
- `#pragma loop_opt(on/off)` keeps a function's loops out of the file's /Ol.
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
  old CgaFont header/overlap was a frame-offset artifact. SPANS2 ends at 3884,
  followed by EM.LIB's 0x170-byte EMULATOR_DATA, then SLOTS at 389B.
- LINK 5.01.21 COMMON allocation walks 256 symbol-hash buckets. Hash recovered
  from LINK's NE segment 2:4060: length + sum((byte|0x20) << (1-based-index&3)),
  low byte selects bucket. Bucket chains insert at head. Within a bucket the most
  recently created symbol (first EXTDEF/COMDEF/PUBDEF in link order) is allocated
  first; blocks over one byte are word-aligned. COMMON is C tentative definitions
  only; renaming any of them can move the layout. The
  hash is `tools/sr/linkhash.py`.
  Historical global names are unknown; do not confuse reconstructed placeholders
  with recovered names. Verify allocation with actual LINK output.
- Multiple literal-pool bases in one object can indicate missing named
  definitions within a TU, not merely a wrong TU split.

## DGROUP lessons (2026-11-04)

- The analysis reads the freshest objects (`work/layout/BUILD/<file stem>/`), the `/Fc` listing for literal sizes (`"\0"` is two bytes) and the source definitions for array bounds: run `./sr build` before `./sr dgroup`. Definitions carry no address annotations; original addresses live in `tools/sr/symbols.json`. The sources are the truth for data definitions: `./sr dgroup check` only verifies that each named item is defined in the file that owned it (since 2026-11-04 there is no writer; the old `apply --write` regenerated every definition from the model and reverted hand edits).
- An overlapping name (an alias) is a view of its base: write it as `base[i]` (or a member) in the sources and drop its declaration; `./sr dgroup check` fails while one exists, and also when the layout holds a global the sources lack (declare it in the owner's header and register its address in `tools/sr/symbols.json`). There is no writer (the old `aliases --write` / `register --write` are gone).
- An assembler unit's data goes into its one `.DATA` section, after its EXTRNs: `PUBLIC  _name` then `_name DW ...` (the label typed by its first element), several values per line, text as quoted strings, `n DUP (v)` for runs, `EVEN` for an alignment byte (MASM 5.1 pads with 00h in data, 90h in code). DUP becomes LIDATA, which omf.py expands. `./sr dgroup check` recognises a MASM definition as `PUBLIC _x` plus a `_x LABEL|DB|DW|DD|DQ` line.

## Link-structure lessons (2026-11-04)

- After moving a definition or changing units, rebuild and run `./sr dgroup check` (the analysis reads the fresh objects of the new units).
- `dgroup.ORDER_HINTS` move units (data-only units too, key `10xxxx`) after another unit; afterwards each code segment's units are re-sorted into code order within their slots. `./sr dgroup order` compares the result with src/SR.LNK (the other objects keep their lines).
- Storage objects need `ASSUME DS:DGROUP`, or MASM writes their PUBDEFs without the group and every reference resolves segment-relative.
- Runtime members: declared `_DATA` length can exceed the data records (`DUP (?)`); read SEGDEF lengths when tiling runtime data.

- MASM uses `src/SYS/PC.INC` unchanged. The check backends find it through `INCLUDE=E:\`; linked and native builds read INCLUDE from `src/SRENV.BAT`. `sr headers` maintains recursive MASM include dependencies as well as C headers.
- MASM 5.1 accepts a 32-bit far-pointer literal in `EQU`/`DD`, but `EGA_VRAM + 2000h` overflows its expression evaluator (A2029). Define the complete `A000:2000` pointer literal with its own `EQU`.

## Picture resources

- `./sr pic --list` lists EGA ids, dimensions, packed sizes, flags, and library paths; `--mode cga` selects LIB1C/LIB2C (the second is under orig/extra). `./sr pic 1 --png work/title.png` writes a PNG; without `--png` it sends base64 PNG through kitty graphics in 4096-byte chunks. Multiple PNG inputs stack vertically; inline output shows each picture separately.
- `./sr pic 1142 1141 --cells 6 11 --scale 3` shows both carhop sheets as rows of eleven cells, using CARHOP.C's cut_sheet geometry. Cells use integer width/COLS and height/2; leftover edge pixels are discarded as in the game. Scaling is nearest-neighbour. Masked sprites have alpha; mask-only resources display grayscale masks. Tandy packed output is not implemented.
- DISK.C reads a count word and 30-byte entries: width/height at 0/2, flags at 6, dictionary tokens at 7..22, relative data offset at 24, packed span at 28. The data base is 2 + count*30. Each entry reads span+1 bytes with BC sentinels; adjacent entries share the boundary sentinel. Header word +4 is unused by the loader and must not constrain decompressed size (mask-only EGA entries retain four-plane sizes there).
- RLE.ASM expands 00/FF followed by count to count+1 copies; other dictionary tokens select the corresponding g_rleCounts/g_rleValues pair, otherwise they are literals. The directory dictionary is not a display palette. The decoder reads those arrays, the CGA id map, and display palettes from the C initializers. EGA has four contiguous planes, least-significant color bit first, each ceil(width/8)*height bytes; mask-only entries contain one plane. CGA has ceil(width/4)*height packed 2bpp bytes, followed by a same-size mask when flagged. EGA sprite masks are generated by complementing the OR of the planes (MASKBLT.ASM).
- IDs 1..9 map to indices 0..8 and 1000..1267 to indices 9..276; CGA then applies g_cgaPicMap. Default EGA colors follow VIDEO.C's g_palette; --palette intro selects DASH.C's g_introPalette, --palette standard selects unremapped RGBI colors. CGA uses bright cyan/magenta/white by default, or --palette cga-green for bright green/red/yellow. These are palette choices, not automatic scene-script emulation.


## Struct grouping and data-shape checks (2026-11-04)

- Named callback fields put the identifier before the closing parenthesis of the function declarator. `cdata.Types.record` accepts that position; far callbacks occupy four bytes and near callbacks two.
- MSC enum members occupy two bytes with word alignment. Keep the real-compiler sizeof/offsetof audit passing when adding enum-backed records.
- Recovered DGROUP fallback regions must not create synthetic aliases inside a fully covering source aggregate. Explicit overlapping source declarations still require the normal alias diagnostics.
- Grouping statics lowers the symbol count without removing storage. Ownership tests compare actual declarations and uniqueness, not a fixed minimum number of symbols.
- `cdata.Types` and `check_data_types.py` read all indexed headers in deterministic source-index order; they do not require a central type header. The real-compiler audit includes private records, enums and typedefs from those headers; no separate private-type probe is needed.

## Subsystem layout (2026-11-04)

- Project sources and headers share uppercase subsystem folders; DATA holds asset sources and headers. Use `config.source_index()` / `source_path(name)` rather than assuming a folder. Duplicate filenames are rejected.
- Check jobs stage every header unchanged in a flat HEADERS directory mounted on E: under both backends. Linked builds preserve the subsystem folders and read INCLUDE from SRENV.BAT.
- Native DOSBox command sequences must use `call SRENV.BAT` so execution continues to MAKE. The 87-character INCLUDE line was proved with native MAKE before moving the files; keep it below the DOS command-line limit.

## Key-map readability lessons (2026-11-04)

- Header include discovery follows enum tags and members as well as macros;
  enum constants belong to their defining header and need no source definition.
- MASM 5.1 macro arguments split at spaces: emit arithmetic directly in DB
  inside KEY, or bracket a nested argument. LOCAL assignment symbols
  and REPT implement sequential item runs; verify data with the packed build,
  since function checks do not cover a data-only key-map object.

- Key-map bindings take decoded results; KEY alone encodes `0xCE + result`.
  C KR_ constants are decoded input results, while KV_ constants are raw bytes
  restricted to runtime patches. tests/test_keymaps.py keeps C/ASM constants,
  pointer-table screen slots and PC scan-code definitions in agreement.

- When folding split communal array tails, remove the tail declarations before asking `sr fit` for the new name window: it derives neighbours from current header symbols, not the intended aggregate size. Producers' pointer increments and consumers' descending current/previous-row pointers establish the full extent. A shared initialized width table can cross an apparent scalar boundary; trace both render directions before choosing its base and bounds, then verify with the packed build.

- An odd C `_DATA` segment length can leave one LINK alignment byte before the next unit's byte data or literal pool. DGROUP derives the segment end from consistent public-symbol bases and the fresh object's data extent; that single byte is alignment, not a synthetic global. A real source declaration or a larger gap must still be checked normally.

### Merging annotated branches

Before removing surviving address comments, run `nix-shell shell.nix --run 'python3 tools/strip_address_comments.py --extract'`. This rebuilds/merges `tools/sr/symbols.json` from the annotations, retains unannotated records for names still present in code and removes stale names absent from code. It includes annotated private MASM data labels. Then run `nix-shell shell.nix --run 'python3 tools/strip_address_comments.py'`. The stripper validates every annotation before writing any source. Missing metadata, mismatched addresses, conflicting annotations or an annotation that cannot be attached to a declaration/label fail closed. Resolve disagreements from binary evidence; a new prototype alone cannot recover a function's size. Re-running extract and strip is idempotent.


## Source comments

Keep reconstruction reasoning in `docs/RECONSTRUCTION.md`, including storage
hashes, local-name constraints, symbol budgets and compiler layout choices. Do not
add storage banners or decompilation explanations to `src/`. Preserve existing
game-behaviour comments and every PROVISIONAL marker with its full text.
`./sr headers` maintains includes and dependencies only; it must not add storage
comments or change declaration/function tokens; it formats C text when rewriting includes.

## C formatting (2026-11-04)

- Run `./sr format --check` with the other gates. The nix shell supplies clang-format
  and lefthook; `lefthook install` enables staged-C formatting and re-staging.
- `headers._write` formats changed C/H bytes before writing or recording dry-run
  drift. Rename formats each changed C/H before its include/dependency sync. Keep
  formatting out of build/check so those compile the checked-in bytes.
- Header declarations can wrap across lines. `source.headers` reads semicolon-ended
  declarations before normalizing whitespace; never parse prototypes line by line.
- When comparing C tokens around macro formatting, remove backslash-newline splices
  first (C translation phase two). Added continuation lines do not add C tokens.
- Tests that read `#define` constants must accept variable horizontal whitespace.
  EQUIP_VIDEO_COLOR80 is C-only; shared PC.H/PC.INC constants still agree by value.
comments or rewrite the declarations or function bodies.

## Documentation tooling lessons (2026-11-04)

- `./sr docs` needs Doxygen and Graphviz even when DOS tools and capstone are
  already available. The wrapper checks those tools for this subcommand before
  accepting the current shell or cached Nix PATH.
- Doxygen 1.17 does not warn when all parameter docs are absent in the measured
  fixture. The XML check in docs.py closes that gap; do not remove it based on
  WARN_NO_PARAMDOC alone. Bare symbol mentions still require human review.
- Keep coverage denominators in the source inventory: EXTRACT_ALL=NO omits
  undocumented entities from XML. Clear old XML before each run and deduplicate
  declarations/definitions. Missing documentation and invalid documentation are
  separate measures; the former is a baseline, the latter fails.

## Naming recovered objects

- INPUTDAT.ASM in INPUT holds shared input state and game tables with four BIOS
  mode helpers (`id_`). File names describe recovered objects, not recovered original
  spellings. Rename with `sr rename`; a subsystem move also needs `git mv` of source
  and header and the MAKEFILE source path updated before `sr headers`. Preserve the
  numeric FORCE_OWNER code-address key. Gate the renamed THEADR/object filename
  with a clean packed build and DOS-native MAKE; function matches alone do not
  establish that object metadata is absent from the EXE.


## Type ownership and header dependencies

- Struct/union definitions belong to their primary global owner header; producer/API
  modules own types without global storage. Related enums and embedded helpers follow
  their parent. Keep only independent ownerless helpers in the tiny STRUCTS.H.
- Includes for types are selected from explicit struct/union tags, using the tag
  namespace. A type-only header still counts as an owner; do not force STRUCTS.H into
  every source. cdata collects all indexed headers before evaluating layouts.
- A pointer forward declaration can suffice for a declaration but leave clients
  unable to traverse members. Include the dependency when that traversal is part
  of the API. Check embedded-record dependencies, independent header compilation
  and include cycles after moving definitions, then run all byte/build gates.
