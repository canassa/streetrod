# Street Rod

Reverse engineering and decompilation of *Street Rod* (Logical Design Works / California Dreams, 1989), DOS version 1.0. The goal is C source that compiles, function by function, to the same bytes as the shipped `SR.EXE`.

## Facts about the binary

- **Compiler:** Microsoft C 5.1 (`toolchain/msc51`). Confirmed by library matches: the C runtime is `MLIBCR.LIB` and the 8087 emulator is `EM.LIB`.
- **Memory model:** medium (`/AM`): far code, near data. Floating point uses the emulator (`/FPi`).
- **Default flags:** `/AM /Oas /Gs /FPi`, used by most matched functions. Some modules differ: `/Oasi` (`0x032F4`) and `/Oals` (`0x14294`, `0x16532`). Per-function flags are recorded with `./sr flags`. Modules may differ; some show `/Oi` intrinsics (inline `strlen`) and `/Ol` (`loop`), so expect per-module flags.
- **Packing:** Microsoft EXEPACK. `orig/SR_unpacked.EXE` is the unpacked image and is what every tool reads.
- **Addresses** are linear offsets into the load image (file offset minus the `0x22D0` header). `seg:off` is the segment value the callers use.
- **Size:** 719 real functions: 506 game C, 66 game asm, and 147 MSC runtime entries (`kind = crt`, `status = skip`, never reconstructed).
- **Hand-written asm:** line drawing, unrolled sprite blitters, video helpers, and small register-saving helpers. They are rebuilt with MASM 5.1 (`toolchain/masm51`, same 1988 release as the compiler) from `src/mods/M_xxxxx.ASM` (singletons use `src/funcs/` while being reconstructed).

## The `sr` CLI

`./sr` drives everything (it enters `shell.nix` on its own if `dosbox-x` or capstone is missing). The DB is `db/sr.sqlite`. Run `./sr <command> -h` for options.

```
./sr build --layout            # recover original code positions/link order in work/layout
./sr linkcheck [EXE]            # fail unless the linked file matches, no byte masks
./sr build                     # compile/link the runnable game into work/game/SR.EXE
./sr build --prepare-only      # emit DOS-native MAKEFILE, ENV.BAT, BUILD.BAT and LINK.RSP
./sr build --objects-only      # compile actual objects without linking
./sr scan                      # rediscover functions in the EXE, update the DB (keeps names/status/src)
./sr crtnames                  # name CRT functions by locating MSC 5.1 library modules in the EXE (also records their aliases)
./sr list [-s todo] [-k c] [--seg 0f38] [-r 10d6c-168f2] [--max 100] [--by-size]
./sr next [--near ADDR]        # todo C functions, smallest first, or nearest to ADDR (same module)
./sr show ADDR|NAME [-b]       # annotated disassembly: callers, callees, strings, named globals
./sr new ADDR [--asm]          # create src/funcs/F_xxxxx.C (or .ASM for hand-written code) and mark wip
./sr check ADDR|NAME           # compile in DOSBox-X, compare with fixups masked, record the result
./sr check --all               # regression gate: every matched function must still match (partials aren't gated)
./sr diff ADDR|NAME [--full]   # two-column instruction diff, ours vs original
./sr sweep ADDR|NAME           # try 46 /O flag combinations in one DOSBox boot
./sr flags ADDR /AM /Os /FPi   # per-function flags (--reset for the default; --force on a matched function)
./sr name ADDR NAME            # name a function
./sr global 82ca g_table --decl "struct Entry @[]"   # declare a DGROUP global (no args: list them)
./sr proto 15236 "int @(int n)" # a far function's prototype (goes into its file's header) ("" clears; --force replaces an existing one)
./sr data 4eee [N]             # hex dump of DGROUP bytes with known globals
./sr headers                   # regenerate the per-file headers and every source's include block
./sr layout a b:long '{ c }'   # predict the [bp-N] layout of locals from their names (--ol for /Ol)
./sr layout -w x cnt i         # suggest renames + declaration order for this top-to-bottom order
./sr layout --bucket 5          # readable names in bucket 5 (pick one instead of a digit-suffixed rename)
./sr mark ADDR blocked -m why  # set status: todo | wip | matched | partial | blocked | skip
./sr kind ADDR asm -m why      # correct the scanner's c/asm/crt guess
./sr note ADDR text...         # append a note to a function (--replace to overwrite)
./sr src ADDR path             # point a function at another source file (a module)
./sr status                    # summary + regenerate docs/PROGRESS.md
./sr dgroup report [lo hi]     # original DGROUP: owner unit, declaration slot and C definition of every item
./sr dgroup problems           # named strings used as literals, aliases, untyped tables, fallbacks
./sr dgroup apply UNIT... --write  # write a unit's data definitions into its module sources (marked /* DS:xxxx */)
./sr dgroup aliases --write    # rewrite overlapping DB views as base[i]; `register --write` adds recovered globals
./sr dgroup asm                # regenerate src/data/DGROUP.ASM: the data no module owns yet
./sr dgroup order --write      # the recovered link order into src/link/LAYOUT.JSON
```

`./sr check` is the gate. `MATCH` means every byte matches except fixups (relocated addresses), the sizes are equal, every fixup refers to the same thing as in the original (`refs:` globals by DGROUP offset, calls by function address, string literals by content), and the source passes the lint (rule 4). `MATCH*` means the same, but some near calls to other functions can't be verified because their displacements depend on the original module layout. A one-function compile can't reproduce those.

**Compile backend: `SR_EMU`.** `emu2` (default) runs `CL`/`MASM` under [emu2](https://github.com/dmsc/emu2), a user-space 8086 + DOS-call emulator with no boot: one process per job, run in parallel (`SR_JOBS`, default 16), ~25 ms per small compile. `SR_EMU=dosbox` is the original one-DOSBox-X-boot-per-batch backend. Both give byte-identical OBJ, COD and CC.LOG (proved on every source here and in ../msc51 with `tools/scripts/emucmp.py`; rerun it after touching either backend or `tools/nix/emu2.nix`). The `./sr` wrapper caches the nix-shell PATH in `work/.nix-path-<hash>` (refreshed when `shell.nix` changes), so a command no longer pays ~2 s to enter the shell. Ported from ../msc51, which has the measurements.

### Linked game build

`make` / `./sr build` compiles real modules without `.STB` placeholders, assembles
`src/data/` records, and links MSC's MLIBCR, MLIBFP, LIBH and EM libraries.
Shared near interfaces use four named code segments and public near definitions
in staging copies; the source used by the byte-match gate stays unchanged.
The entry point F_0066F becomes `main` in staging. `CRT.H` maps runtime-owned data.
LINK runs in DOSBox-X because emu2 crashes on this link workload. The generated
DOS Makefile works with MSC MAKE 4.07 (`ENV.BAT`, then `MAKE MAKEFILE`).
`./sr build --layout` uses `src/link/LAYOUT.JSON`, original region boundaries,
whole-source groups, and MSC's combined library: it builds `MLIBCE.LIB` with LIB 3.11
(`LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;`, the component order matters)
and links `/MAP /STACK:2560 /EXEPACK` with the objects' default library (no `/NOD`). All
116 source jobs are compiled and gated. It puts all 572 game functions at their
original addresses and reproduces all 748 E8 call encodings. Whole-file matching
is still red: initialized data/literals, uninitialized storage and MZ metadata
remain. Use `./sr linkcheck work/layout/SR.EXE`; its JSON includes raw differences,
relocation positions, mapped function positions and every near-call encoding.

The legacy runnable build is not proof of original whole-EXE layout. See README.md
and src/data/README.md for usage and source-data validation.

### Modules: `src/mods/`

A module groups a contiguous run of matched functions in one `.C` or `.ASM`, compiled as one object. It may reconstruct part of an original source file; address-based grouping does not establish the original file boundaries.

```
./sr module units [-v]           # candidate spans from near / push-cs calls, flags per span
./sr module create NAME.C FN...  # merge matched singletons into src/mods/NAME.C, then verify; restores everything on failure
./sr module add NAME.C FN...     # extend a module (members stay contiguous)
./sr module list | show NAME.C   # show rebuilds and gates it
./sr module dissolve NAME.C      # back to singletons (+ .STB placeholders)
```

- **Only byte-matched functions join a module** (status `matched`, 0 mismatches), and only as a contiguous run: no other function and no unexplained bytes between members. Matching happens on singletons; grouping is a re-verification step.
- **One flag set per file.** C `create` uses the union of the members' `/O` letters (or `--flags`). Not every union preserves code generation; failed merges restore the sources. Large C groups can exceed MSC 5.1's heap. Use smaller verified contiguous groups. MASM groups require the same assembler flags and namespace local labels without changing instructions.
- **Candidate spans are not file-boundary proof.** Near interfaces can cross objects; the first span includes both C and assembly. Keep languages separate and preserve observed alignment boundaries.
- **Single-function modules are valid.** They record standalone compilation units where combining neighbours changes flags, alignment or code generation; their names do not imply recovered historical filenames.
- **The module gate is stricter than per-function `MATCH*`.** `./sr check` on any member also compares the whole span against the image: bytes between functions, internal near-call displacements, and internal calls and jump-table offsets. References to other modules and DGROUP remain relocation-masked and checked by `refs:`. External near calls are counted explicitly, including EXTDEF calls to prepended placeholders; a module with such calls still has layout verification outstanding. Only a gate with zero external near calls clears `MATCH*`.
- **Retired scanner rows:** a skipped false positive wholly covered by a real member does not block grouping. Its history remains in the DB. This does not permit gaps or missing routines.
- Names are `M_<first addr>.C` until the real file name is known.

## Workflow

Use the `decompile-function` skill for the per-function loop. Consult `msc51-recipe-book` when a function diverges. Run `wrap-up` at the end of a session. Change the tooling with the `sr-cli` skill.

## Rules

### 1. Write the source the 1989 programmers wrote

The byte match is how we check a reconstruction. The goal is the original source. Only write constructs an MSC 5.1 C programmer in 1989 would have written:

- **String literals stay literals.** If the original pushes the address of a string, write the string. Never invent a named global to reproduce string bytes.
- **No inline asm.** MSC 5.1 has none. A function is either C (`.C`) or hand-written asm (`.ASM`); decide from the fingerprints in the recipe book, never because asm is easier to match.
- **Asm sources are real MASM source too:** named labels, `EXTRN` for globals and callees, instructions rather than `db` bytes. Only use `db` for data the original programmer would have written as data.
- **Never copy bytes you don't understand** into the source or tooling to force a match. If you don't know what some bytes are (a table? padding? a jump table?), find out, then emit them from a structure you understand.
- Data addresses are fixups and are masked. Getting a global's value or position right isn't needed for a code match, so don't contort the code to get it.

### 2. The compiler is settled; flags are per module

We use the exact compiler the developers used. Never blame a compiler version or a "patch level" for a divergence. Flags can differ between the original source files: run `./sr sweep` once when starting in a new area. If the best recipe still diverges, the difference is in the source. Keep working on the C.

### 3. Local variable names are load-bearing

MSC 5.1 assigns stack slots to locals in an order set by a hash of the identifier names (declaration order only breaks ties). Renaming a local can move it and break a match. `./sr check` prints the layout (`locals: a=-50, ...`), and `./sr diff` shows wrong `[bp-N]` offsets. The rule is known (recipe book, "Local layout"): `./sr layout NAMES` predicts the frame and `./sr layout -w NAMES` suggests names for a wanted order. Still recheck after every rename.

### 4. Declarations come from the defining file's header, never ad hoc

As in the original, each source file has its own header: `src/include/<FILE>.H` declares what that file defines (one header per link unit, named after its first source, e.g. `M_0914C.H` covers M_0914C.C..M_0B045.C; standalone data files get theirs, e.g. `SCREENS.H`, `MESSAGES.H`). A `.C` includes `STRUCTS.H`, its own header, and the headers of the files whose globals and functions it uses, and declares nothing itself. The headers and the include blocks are generated: declarations come from the DB, ownership from where the definition is. Run `./sr headers` after adding a reference to another file's symbol or moving a definition. Unused declarations do not count against the compiler budget (rule 4d), so the include lists cannot change code.

- **Globals:** `./sr global OFF NAME --decl "TYPE @"`. One name per DGROUP offset, project-wide.
- **Far functions:** `./sr proto ADDR "TYPE @(ARGS)"`. A call without a prototype fails the check (MSC's C4071 at `/W3`). A CRT function may have several library names (`aFlmul`/`aFulmul`); `crtnames` records them in the `aliases` table and `refs` accepts any.
- **Struct types:** `src/include/STRUCTS.H`, hand-written, one definition per type.
- **Allowed in a `.C`:** standard MSC headers, and `static ... near` forward declarations of module-mates.
- **Asm:** `EXTRN` names must be the DB names (the `refs` check enforces it).

`./sr check` lints for `extern`, local prototypes and local struct definitions. Name a global or function only once you know what it is; until then keep `g_XXXX` / `F_XXXXX`.

### 4b. Data belongs to the file that owned it

The original DGROUP is the modules' `_DATA` in link order: each translation unit's string literals (in parse order) and its initialized globals (flushed after the next function's literals). `./sr dgroup` recovers the owner and the declaration slot of every item from those positions and writes the definitions into the module sources with a `/* DS:xxxx */` marker; `src/data/DGROUP.ASM` only holds what no module owns yet. A module that defines data is gated on it (`datacheck`: base, bytes and every pointer). Overlapping DB names are views, rewritten as `base[i]`; a literal referenced twice at one address is a named array; a unit whose function stops matching once its tables are defined did not own them (MSC's frame layout depends on the number of file-scope definitions), so they go to a data-only unit.

### 4c. Objects and link order come from the relocation table

LINK lists relocations object by object, each data record's fixups in descending offset (MSC; MASM ascending). `orig/SR.EXE`'s packed relocation table therefore shows which units were one object and in which order objects were linked. A far call between two functions proves two files (MSC near-calls every function defined in the same file). `src/link/LAYOUT.JSON` holds the recovered units, `link_order` (written by `./sr dgroup order --write`), `order_hints` with the evidence for each, and the runtime member order. `./sr build --layout` ends with the packed EXE's comparison against `orig/SR.EXE`.

### 4d. The compiler budget: global symbols are load-bearing

MSC 5.1's code generator degrades (fewer loop register variables, other temps) once the external symbols referenced so far in a file exceed a budget measured in symbols plus name characters. A function can match alone and fail in its real file. External functions, external data and communals count; locals, statics, prototypes, literals and initialized data do not. Folding globals into the structs/arrays they were (with code evidence: a run-time `a - b` between two addresses or a direct `g[i]` index proves separate symbols) is how the original files stayed under it. `./sr build --layout` must stay IDENTICAL against `orig/SR.EXE`.

### 4e. Uninitialized storage is communal, and LINK's hash fixes the names

DS:6E60..8F02 (`c_common`) holds no labels: every block is a C tentative definition (`int x;`, `struct T a[N];`) in the file that first mentions it, under the comment `/* Uninitialized storage: communals ... */`. LINK 5.01 allocates communals by symbol-hash bucket (ascending; `tools/sr/linkhash.py`), and within a bucket the most recently created symbol first (created at its first EXTDEF/COMDEF/PUBDEF in link order). Blocks larger than one byte are word-aligned. A block's address therefore constrains its name: renaming a COMMON global, adding one, or moving its first mention to another object can move every later block. Check any such change with `./sr build --layout` (it must print `Packed EXE vs orig/SR.EXE: IDENTICAL`). The current names were chosen to satisfy this (same length as the old placeholder where possible), so they are still placeholders, not recovered names. There is no assembler COMMON storage left. Every array and struct has the size the original allocated, taken from the save-game table at DS:4EA4/4EBE (block addresses and byte counts) where one exists. Operand order in MSC comparisons and additions also tells whether an operand is a struct member or a plain variable (see the recipe book); several placeholder structs (`State70C0`, `State7D2E`, `State7E8A`) exist only because of that evidence. Unreferenced blocks are `char` communals in `src/mods/HOTSPOT.C` (kept across `./sr dgroup apply`).

### 4f. File-local storage is `static`, and MSC's hash fixes the order

DS:6C22..6E2C (`_BSS`) is file-local: every block is used by one original file, so it is an uninitialized `static` there (`static int g_6c46;   /* DS:6C46 */`), and LINK concatenates the objects' `_BSS` in link order. Within one object MSC 5.1 orders its uninitialized statics by `sum(byte & 0xDF) mod 256` of the name, ascending; equal keys put the later declaration first; ints and larger items are word-aligned. Renaming a static therefore moves the statics of its own file only: keep the keys of a file's statics increasing in address order (`./sr build --layout` must stay IDENTICAL). The names are placeholders chosen for that, like the COMMON ones. A unit split over several `M_*.C` sources repeats a shared static in each source that uses it; the merged unit (`module.merge`) hoists and dedupes those lines, so it has one definition at the top. Statics are not in any header; they stay in the DB for `./sr check`, which resolves a `_BSS` fixup by the names and offsets in the compiler listing. `g_6c42` (4 unreferenced bytes after `g_6c40`) is PROVISIONAL filler.

### 5. The DB comes from the EXE and the CLI

The EXE is the ground truth. `db/sr.sqlite` is derived from it by `./sr scan`, plus what we record through `sr` commands. The scanner also checks complete gaps for prologue-free routines, rejecting gaps that do not partition into bounded reachable code and trailing alignment; review new discoveries before matching them. Never edit the DB or the generated headers by hand, and never edit `docs/PROGRESS.md` (`./sr status` regenerates it).

### 6. Keep the gate green

Before finishing any change to sources, flags or tooling, run `./sr check --all`. A regression in a matched function blocks the change.

### 7. Surface structural blockers immediately

If something blocks progress (a tool limitation, a wrong assumption in this file, a scanner bug that hides functions, a missing toolchain piece), stop and tell the user before working around it.

## Operational

### Captain's log: `plans/diary.md`

Append an entry at the end of every session with real work: heading `## YYYY-MM-DD HH:MM TZ: <title>` (use `date`), then **What I did** and **What I learned**. Newest at the bottom. Never edit old entries; correct them in a new one. The `wrap-up` skill does this.

### Skills update themselves

Skills in `.claude/skills/` are living documents. When a session teaches something that would have saved time (a new codegen tell, a CLI gotcha, a better step order), add it to the relevant skill in the same session. The recipe book grows one row per lesson.

### Python

The shell has no `python3` of its own; `./sr` brings its own. For a one-off script use `nix-shell shell.nix --run 'python3 ...'`.

### DOSBox safety

Never redirect a DOS program's output to a file without a timeout and a size bound: a crashing program can fill the disk. `tools/sr/dosbox.py` bounds every run with `timeout`; keep that for any new DOS capture.

### Layout

| Path | What |
|---|---|
| `orig/` | `SR.EXE` (reconstructed retail EXE), `SR_unpacked.EXE`, the freeware files, extra original files |
| `dist/` | Downloads as fetched (freeware zip, archive.org, The Good Old Days image) |
| `toolchain/msc51/` | MSC 5.1 `BIN`, `INCLUDE`, `LIB`, plus the original disk images |
| `toolchain/masm51/` | MASM 5.1 `BIN`, `INCLUDE`, docs, plus the original disk images (`F:` in DOSBox) |
| `src/funcs/F_xxxxx.C` / `.ASM` | One reconstructed function per file (8.3 names; DOS builds them) |
| `src/funcs/F_xxxxx.STB` | Optional placeholders for same-module callees, prepended at build time so calls compile to `push cs; call near`: `@F_xxxxx` (generated from the DB prototype) for far ones, a trivial `static ... near` definition for near ones. Not source |
| `src/include/` | per-file headers (generated), `STRUCTS.H` (hand-written); on the compiler's `INCLUDE` path as `E:` |
| `tools/sr/` | The CLI |
| `db/sr.sqlite` | Functions, callees, named globals |
| `docs/PROGRESS.md` | Generated progress report |
| `plans/diary.md` | Captain's log |
| `work/` | Scratch output (ignored). `work/op/tryv.sh ADDR V.C...` checks variants and restores the source, `work/op/diffv.sh ADDR V.C [--full]` diffs one, `work/op/regions.py` prints a diff as address ranges; `work/bp/x2.py DIR FLAGS` lists small test functions (`T*.C`, 8.3 names) |
