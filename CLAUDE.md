# Street Rod

Reverse engineering and decompilation of *Street Rod* (Logical Design Works / California Dreams, 1989), DOS version 1.0. The goal is the original source files (one per object, built with the original tools) that compile to the same bytes as the shipped `SR.EXE`. All 572 game functions match; `./sr build` reproduces `orig/SR.EXE` byte for byte.

## Facts about the binary

- **Compiler:** Microsoft C 5.1 (`toolchain/msc51`). Confirmed by library matches: the C runtime is `MLIBCR.LIB` and the 8087 emulator is `EM.LIB`.
- **Memory model:** medium (`/AM`): far code, near data. Floating point uses the emulator (`/FPi`).
- **Flags are per file:** `/AM /O... /Gs /FPi` with `/Oals` (26 files), `/Oalsi` (19), `/Oas` (7), `/Oasi`, `/Oa` and no `/O` (one each); MASM `/Ml` (27). Each file's line is recorded in LAYOUT.JSON (`./sr flags FILE`); data-only files use `/AM /Oas /Gs /FPi`.
- **Packing:** Microsoft EXEPACK. `orig/SR_unpacked.EXE` is the unpacked image and is what every tool reads.
- **Addresses** are linear offsets into the load image (file offset minus the `0x22D0` header). `seg:off` is the segment value the callers use.
- **Size:** 719 real functions: 506 game C, 66 game asm, and 147 MSC runtime entries (`kind = crt`, `status = skip`, never reconstructed).
- **Hand-written asm:** line drawing, unrolled sprite blitters, video helpers, and small register-saving helpers. They are rebuilt with MASM 5.1 (`toolchain/masm51`, same 1988 release as the compiler) from `src/mods/M_xxxxx.ASM`.

## The `sr` CLI

`./sr` drives everything (it enters `shell.nix` on its own if `dosbox-x` or capstone is missing). The DB is `db/sr.sqlite`. Run `./sr <command> -h` for options.

```
./sr build                     # build work/layout/SRPACKED.EXE; must end `Packed EXE vs orig/SR.EXE: IDENTICAL`
./sr build --prepare-only      # only write the DOS build tree: sources, MAKEFILE, BUILD.BAT, ENV.BAT, LINK.RSP
./sr build --objects-only      # compile and gate every object without linking
./sr linkcheck [EXE]            # unmasked comparison of work/layout/SR.EXE (unpacked link) with the unpacked original
./sr scan                      # rediscover functions in the EXE, update the DB (keeps names/status/src)
./sr crtnames                  # name CRT functions by locating MSC 5.1 library modules in the EXE (also records their aliases)
./sr list [-s todo] [-k c] [--seg 0f38] [-r 10d6c-168f2] [--max 100] [--by-size]
./sr next [--near ADDR]        # todo C functions, smallest first, or nearest to ADDR (none are left)
./sr show ADDR|NAME [-b]       # annotated disassembly: callers, callees, strings, named globals
./sr check ADDR|NAME|FILE      # compile the function's whole file (its command line), compare with fixups masked, gate the file, record
./sr check --all               # regression gate: every file compiled once, every matched function must still match
./sr diff ADDR|NAME [--full]   # two-column instruction diff, ours vs original
./sr sweep ADDR|NAME           # compile the function's file under 46 /O flag combinations, score the function
./sr flags FILE|ADDR [FLAGS]   # a file's CL/MASM flags in LAYOUT.JSON (--force when it has matched functions)
./sr name ADDR NAME            # name a function
./sr global 82ca g_table --decl "struct Entry @[]"   # declare a DGROUP global (no args: list them)
./sr proto 15236 "int @(int n)" # a far (or public near) function's prototype (goes into its file's header) ("" clears; --force replaces an existing one)
./sr data 4eee [N]             # hex dump of DGROUP bytes with known globals
./sr headers                   # regenerate the per-file headers and every source's include block
./sr layout a b:long '{ c }'   # predict the [bp-N] layout of locals from their names (--ol for /Ol)
./sr layout -w x cnt i         # suggest renames + declaration order for this top-to-bottom order
./sr layout --bucket 5          # readable names in bucket 5 (pick one instead of a digit-suffixed rename)
./sr mark ADDR blocked -m why  # set status: todo | wip | matched | partial | blocked | skip
./sr kind ADDR asm -m why      # correct the scanner's c/asm/crt guess
./sr note ADDR text...         # append a note to a function (--replace to overwrite)
./sr status                    # summary + regenerate docs/PROGRESS.md
./sr dgroup report [lo hi]     # original DGROUP: owner unit, declaration slot and C definition of every item
./sr dgroup problems           # named strings used as literals, aliases, untyped tables, fallbacks
./sr dgroup apply UNIT... --write  # write a unit's data definitions into its source file (marked /* DS:xxxx */)
./sr dgroup aliases --write    # rewrite overlapping DB views as base[i]; `register --write` adds recovered globals
./sr dgroup asm                # list initialized DGROUP data no source owns (none left; exit 1 if any)
./sr dgroup order --write      # the recovered link order into src/link/LAYOUT.JSON
./sr module units [-v]         # candidate same-file spans from near-call evidence
```

`./sr check` is the gate. `MATCH` means every byte matches except fixups (relocated addresses), the sizes are equal, every fixup refers to the same thing as in the original (`refs:` globals by DGROUP offset, calls by function address, string literals by content), and the source passes the lint (rule 4). `MATCH*` means the same, but the function near-calls a function of another object (a public near function in the same code segment): the object leaves that displacement to LINK, so only the linked build verifies it (21 functions, all in segment 0000). Near calls inside the file are resolved and checked by the file gate.

**Compile backend: `SR_EMU`.** `emu2` (default) runs `CL`/`MASM` under [emu2](https://github.com/dmsc/emu2), a user-space 8086 + DOS-call emulator with no boot: one process per job, run in parallel (`SR_JOBS`, default 16), ~25 ms per small compile. `SR_EMU=dosbox` is the original one-DOSBox-X-boot-per-batch backend. Both give byte-identical OBJ, COD and CC.LOG (proved on every source here and in ../msc51 with `tools/scripts/emucmp.py`; rerun it after touching either backend or `tools/nix/emu2.nix`). The `./sr` wrapper caches the nix-shell PATH in `work/.nix-path-<hash>` (refreshed when `shell.nix` changes), so a command no longer pays ~2 s to enter the shell. Ported from ../msc51, which has the measurements.

### The build

`./sr build` (alias `--layout`; `make`) is the faithful build in `work/layout`: LIB 3.11
combines MSC's untouched libraries into `MLIBCE.LIB` (`LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;`,
the component order matters), CL/MASM compile every object of `src/link/LAYOUT.JSON` once (under
emu2, each code object gated against the original), and LINK 5.01 (in DOSBox-X: emu2 crashes on
this link) links `/MAP /STACK:2560 /EXEPACK` with the objects' default library. The packed result
`SRPACKED.EXE` is byte-identical to `orig/SR.EXE`; `SR.EXE` is the same link unpacked, for
`./sr linkcheck` and its `BYTE-MATCH.JSON` (positions of all 572 functions, all 748 near calls).
The generated `MAKEFILE` (MSC MAKE 4.07) and `BUILD.BAT` build the same file natively in DOSBox-X
(`./dosbox.sh`; README.md).

### Source files: `src/mods/`

Every original object is one source file in `src/mods/`, the unit of `src/link/LAYOUT.JSON`
(`units[]`: `start`, `end`, `sources` (one file), `kind`, `flags`; `data_units` for the data-only
files HOTSPOT.C, MESSAGES.ASM, CARMASKS.C, CARMASK2.C). The 55 C files and 27 MASM files hold
their functions in address order: includes, the file-scope statics and `static ... near`
forward declarations, then each function with its comment, initialized data before the
function whose literals precede it. Names are `M_<first addr>` until the real file name is known.

- **One command line per file.** `flags` in LAYOUT.JSON is the file's CL (or MASM) line; a C
  file is also given `/NT S<region>_TEXT` (the code segment shared by near interfaces, a
  placeholder name). An assembler file names that segment in its own `.CODE` directive
  (M_21962.ASM is a `.MODEL SMALL` file with an explicit `SEGMENT`, see its comment).
  `./sr flags FILE` prints or sets it. Code that must not be loop-optimized under the file's
  `/Ol` carries the real `#pragma loop_opt(off)` (M_00D58.C, M_039C0.C, M_23262.C).
- **What is compiled is the file in git.** `./sr check` and `./sr build` copy it unchanged; no
  placeholders, no text rewriting.
- **The file gate.** `./sr check` on any function compiles its whole file and also compares
  the whole span against the image: bytes between functions (including code-segment data such
  as M_1DD22.ASM's `dbl` flag byte), internal near-call displacements, internal calls and
  jump-table offsets. References to other objects and DGROUP remain relocation-masked and
  checked by `refs:`. A near call to another object is counted (`MATCH*`) and verified by the
  linked build (`Near-call encodings: 748/748 exact`).
- **Retired scanner rows:** a skipped false positive wholly covered by a real function (0x0F8C2) has no `src`; its history remains in the DB.
- `./sr module units [-v]` lists candidate same-file spans from near / `push cs; call near` evidence (how the units were found; not file-boundary proof on its own: 4c).

## Workflow

Every function is matched. To change a function, edit it in its file and `./sr check` it (the whole file is compiled and gated); the `decompile-function` skill has the loop. Consult `msc51-recipe-book` when a function diverges. Run `wrap-up` at the end of a session. Change the tooling with the `sr-cli` skill.

## Rules

### 1. Write the source the 1989 programmers wrote

The byte match is how we check a reconstruction. The goal is the original source. Only write constructs an MSC 5.1 C programmer in 1989 would have written:

- **String literals stay literals.** If the original pushes the address of a string, write the string. Never invent a named global to reproduce string bytes.
- **No inline asm.** MSC 5.1 has none. A function is either C (`.C`) or hand-written asm (`.ASM`); decide from the fingerprints in the recipe book, never because asm is easier to match.
- **Asm sources are real MASM source too:** named labels, `EXTRN` for globals and callees, instructions rather than `db` bytes. Only use `db` for data the original programmer would have written as data.
- **Never copy bytes you don't understand** into the source or tooling to force a match. If you don't know what some bytes are (a table? padding? a jump table?), find out, then emit them from a structure you understand.
- Data addresses are fixups and are masked. Getting a global's value or position right isn't needed for a code match, so don't contort the code to get it.

### 2. The compiler is settled; flags are per file

We use the exact compiler the developers used. Never blame a compiler version or a "patch level" for a divergence. Flags differ between the original source files, never within one: a file has one command line (`./sr flags FILE`). `./sr sweep` compiles the whole file under other flags. If the best recipe still diverges, the difference is in the source. Keep working on the C.

### 3. Local variable names are load-bearing

MSC 5.1 assigns stack slots to locals in an order set by a hash of the identifier names (declaration order only breaks ties). Renaming a local can move it and break a match. `./sr check` prints the layout (`locals: a=-50, ...`), and `./sr diff` shows wrong `[bp-N]` offsets. The rule is known (recipe book, "Local layout"): `./sr layout NAMES` predicts the frame and `./sr layout -w NAMES` suggests names for a wanted order. Still recheck after every rename.

### 4. Declarations come from the defining file's header, never ad hoc

As in the original, each source file has its own header: `src/include/<FILE>.H` declares what that file defines (one header per source file, e.g. `M_0914C.H` for M_0914C.C; standalone data files get theirs, e.g. `SCREENS.H`, `MESSAGES.H`). A `.C` includes `STRUCTS.H`, its own header, and the headers of the files whose globals and functions it uses, and declares nothing itself. The headers and the include blocks are generated: declarations come from the DB, ownership from where the definition is. Run `./sr headers` after adding a reference to another file's symbol or moving a definition. Unused declarations do not count against the compiler budget (rule 4d), so the include lists cannot change code.

- **Globals:** `./sr global OFF NAME --decl "TYPE @"`. One name per DGROUP offset, project-wide.
- **Far functions:** `./sr proto ADDR "TYPE @(ARGS)"`. A call without a prototype fails the check (MSC's C4071 at `/W3`). A CRT function may have several library names (`aFlmul`/`aFulmul`); `crtnames` records them in the `aliases` table and `refs` accepts any.
- **Near functions other files call:** about 29 near functions are called (`call near`, no `push cs`) from other objects sharing the code segment. They are public `T near F(...)` definitions (not `static`), with `./sr proto ADDR "T near @(ARGS)"` putting the prototype into the owner's header; their callers include that header and do not forward-declare them. Only file-local near functions stay `static`.
- **Struct types:** `src/include/STRUCTS.H`, hand-written, one definition per type.
- **Allowed in a `.C`:** standard MSC headers, and `static ... near` forward declarations of its own static functions.
- **Asm:** `EXTRN` names must be the DB names (the `refs` check enforces it).

`./sr check` lints for `extern`, local prototypes and local struct definitions. Name a global or function only once you know what it is; until then keep `g_XXXX` / `F_XXXXX`.

### 4b. Data belongs to the file that owned it

The original DGROUP is the objects' `_DATA` in link order: each translation unit's string literals (in parse order) and its initialized globals (flushed after the next function's literals). `./sr dgroup` recovers the owner and the declaration slot of every item from those positions and writes the definitions into the unit's source file with a `/* DS:xxxx */` marker; every item has an owner (`./sr dgroup asm` lists any that does not). A file that defines data is gated on it (`datacheck`: base, bytes and every pointer). Overlapping DB names are views, rewritten as `base[i]`; a literal referenced twice at one address is a named array; a unit whose function stops matching once its tables are defined did not own them (MSC's frame layout depends on the number of file-scope definitions), so they go to a data-only unit.

### 4c. Objects and link order come from the relocation table

LINK lists relocations object by object, each data record's fixups in descending offset (MSC; MASM ascending). `orig/SR.EXE`'s packed relocation table therefore shows which units were one object and in which order objects were linked. A far call between two functions proves two files (MSC near-calls every function defined in the same file). `src/link/LAYOUT.JSON` holds the recovered units, `link_order` (written by `./sr dgroup order --write`), `order_hints` with the evidence for each, and the runtime member order. `./sr build` ends with the packed EXE's comparison against `orig/SR.EXE`.

### 4d. The compiler budget: global symbols are load-bearing

MSC 5.1's code generator degrades (fewer loop register variables, other temps) once the external symbols referenced so far in a file exceed a budget measured in symbols plus name characters. A function can match alone and fail in its real file. External functions, external data and communals count; locals, statics, prototypes, literals and initialized data do not. Folding globals into the structs/arrays they were (with code evidence: a run-time `a - b` between two addresses or a direct `g[i]` index proves separate symbols) is how the original files stayed under it. `./sr build --layout` must stay IDENTICAL against `orig/SR.EXE`.

### 4e. Uninitialized storage is communal, and LINK's hash fixes the names

DS:6E60..8F02 (`c_common`) holds no labels: every block is a C tentative definition (`int x;`, `struct T a[N];`) in the file that first mentions it, under the comment `/* Uninitialized storage: communals ... */`. LINK 5.01 allocates communals by symbol-hash bucket (ascending; `tools/sr/linkhash.py`), and within a bucket the most recently created symbol first (created at its first EXTDEF/COMDEF/PUBDEF in link order). Blocks larger than one byte are word-aligned. A block's address therefore constrains its name: renaming a COMMON global, adding one, or moving its first mention to another object can move every later block. Check any such change with `./sr build --layout` (it must print `Packed EXE vs orig/SR.EXE: IDENTICAL`). The current names were chosen to satisfy this (same length as the old placeholder where possible), so they are still placeholders, not recovered names. There is no assembler COMMON storage left. Every array and struct has the size the original allocated, taken from the save-game table at DS:4EA4/4EBE (block addresses and byte counts) where one exists. Operand order in MSC comparisons and additions also tells whether an operand is a struct member or a plain variable (see the recipe book); several placeholder structs (`State70C0`, `State7D2E`, `State7E8A`) exist only because of that evidence. Unreferenced blocks are `char` communals in `src/mods/HOTSPOT.C` (kept across `./sr dgroup apply`).

### 4f. File-local storage is `static`, and MSC's hash fixes the order

DS:6C22..6E2C (`_BSS`) is file-local: every block is used by one original file, so it is an uninitialized `static` there (`static int g_6c46;   /* DS:6C46 */`), and LINK concatenates the objects' `_BSS` in link order. Within one object MSC 5.1 orders its uninitialized statics by `sum(byte & 0xDF) mod 256` of the name, ascending; equal keys put the later declaration first; ints and larger items are word-aligned. Renaming a static therefore moves the statics of its own file only: keep the keys of a file's statics increasing in address order (`./sr build --layout` must stay IDENTICAL). The names are placeholders chosen for that, like the COMMON ones. Each static is defined once, at the top of its file. Statics are not in any header; they stay in the DB for `./sr check`, which resolves a `_BSS` fixup by the names and offsets in the compiler listing. `g_6c42` (4 unreferenced bytes after `g_6c40`) is PROVISIONAL filler.

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
| `src/mods/` | One source file per original object (`M_xxxxx.C` / `.ASM`, 8.3 names), plus the data-only HOTSPOT.C, MESSAGES.ASM, CARMASKS.C, CARMASK2.C |
| `src/link/LAYOUT.JSON` | The objects: span, source file, flags; link order and its evidence |
| `src/include/` | per-file headers (generated), `STRUCTS.H` (hand-written); on the compiler's `INCLUDE` path as `E:` |
| `tools/sr/` | The CLI |
| `db/sr.sqlite` | Functions, callees, named globals |
| `docs/PROGRESS.md` | Generated progress report |
| `plans/diary.md` | Captain's log |
| `work/` | Scratch output (ignored). `work/op/tryv.sh ADDR V.C...` checks variants and restores the source, `work/op/diffv.sh ADDR V.C [--full]` diffs one, `work/op/regions.py` prints a diff as address ranges; `work/bp/x2.py DIR FLAGS` lists small test functions (`T*.C`, 8.3 names) |
