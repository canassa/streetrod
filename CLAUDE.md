# Street Rod

Reverse engineering and decompilation of *Street Rod* (Logical Design Works / California Dreams, 1989), DOS version 1.0. The goal is the original source files (one per object, built with the original tools) that compile to the same bytes as the shipped `SR.EXE`. All 572 game functions match; `./sr build` reproduces `orig/SR.EXE` byte for byte.

## Facts about the binary

- **Compiler:** Microsoft C 5.1 (`toolchain/msc51`). Confirmed by library matches: the C runtime is `MLIBCR.LIB` and the 8087 emulator is `EM.LIB`.
- **Memory model:** medium (`/AM`): far code, near data. Floating point uses the emulator (`/FPi`).
- **Flags are per file:** `/AM /O... /Gs /FPi` with `/Oals` (26 files), `/Oalsi` (19), `/Oas` (7), `/Oasi`, `/Oa` and no `/O` (one each); MASM `/Ml` (25). Each file's command line is its block in `src/MAKEFILE` (`./sr flags FILE`); the data-only C files use `/AM /Oas /Gs /FPi`.
- **Packing:** Microsoft EXEPACK. `orig/SR_unpacked.EXE` is the unpacked image and is what every tool reads.
- **Addresses** are linear offsets into the load image (file offset minus the `0x22D0` header). `seg:off` is the segment value the callers use.
- **Size:** 719 real functions: 509 game C, 63 game asm, and 147 MSC runtime entries (kind `crt`, never reconstructed, named from the MSC libraries).
- **Hand-written asm:** line drawing, unrolled sprite blitters, video helpers, and small register-saving helpers. They are rebuilt with MASM 5.1 (`toolchain/masm51`, same 1988 release as the compiler) from the `.ASM` files of `src/`.

## The `sr` CLI

`./sr` drives everything (it enters `shell.nix` on its own if `dosbox-x` or capstone is missing). There is no database: every command reads names, addresses and declarations from the sources and headers (`tools/sr/source.py`, see rule 5). Run `./sr <command> -h` for options.

```
./sr build                     # run src/MAKEFILE (work/layout/SRPACKED.EXE), then verify; must end `Packed EXE vs orig/SR.EXE: IDENTICAL`
./sr build --no-verify         # only the build: no gates, no comparison (needs no orig/)
./sr build --prepare-only      # only copy src/ (MAKEFILE, SR.LNK, sources, headers) to work/layout
./sr build --objects-only      # run the MAKEFILE's compile blocks and gate every object, no link
./dosbox.sh --make             # the DOS-native build: MAKE MAKEFILE in DOSBox-X on a copy of src/, compared with orig/SR.EXE
./sr linkcheck [EXE]            # unmasked comparison of work/layout/SR.EXE (unpacked link) with the unpacked original
./sr scan                      # rediscover the functions in the EXE; exit 1 unless they are the sources' (address, size)
./sr crtnames [-v]             # the runtime functions' names: MSC 5.1 library modules located in the EXE (and their aliases)
./sr list [-k c] [--seg 0f38] [-r 10d6c-168f2] [--max 100] [--by-size]
./sr show ADDR|NAME [-b]       # annotated disassembly: callers, callees, strings, named globals
./sr check ADDR|NAME|FILE      # compile the function's whole file (its command line), compare with fixups masked, gate the file
./sr check --all               # regression gate: every file compiled once, every matched function must still match
./sr diff ADDR|NAME [--full]   # two-column instruction diff, ours vs original
./sr sweep ADDR|NAME           # compile the function's file under 46 /O flag combinations, score the function
./sr flags FILE|ADDR [FLAGS]   # a file's CL/MASM flags: its src/MAKEFILE line (--force when it has matched functions)
./sr rename OLD NEW            # rename a function, global or source file everywhere: sources, headers, MAKEFILE, SR.LNK, docs, tools; then ./sr headers (--file PAIRS, -n)
./sr fit OLD CAND...            # would a renamed communal/static stay at its address? (LINK bucket / MSC key window, 4e/4f)
./sr pic [ID...] [--list] [--png OUT.png] [--scale N] [--cells COLS N] [--mode ega|cga] [--palette game|intro|standard|cga-green]  # decode resource pictures; default output is inline kitty graphics
./sr data 4eee [N]             # hex dump of DGROUP bytes with known globals
./sr headers                   # rewrite every C source's include block and the MAKEFILE dependencies from the headers
./sr headers --check           # write nothing; exit 1 if that would change a file or a header declares a name its file does not define
./sr layout a b:long '{ c }'   # predict the [bp-N] layout of locals from their names (--ol for /Ol)
./sr layout -w x cnt i         # suggest renames + declaration order for this top-to-bottom order
./sr layout --bucket 5          # readable names in bucket 5 (pick one instead of a digit-suffixed rename)
./sr dgroup report [lo hi]     # original DGROUP: owner unit, declaration slot and C definition of every item
./sr dgroup problems           # named strings used as literals, aliases, untyped tables, fallbacks
./sr dgroup check              # every initialized global defined in the file that owned it, no alias views, no layout global the sources lack (exit 1 if not)
./sr dgroup order              # the link order recovered from DGROUP and ORDER_HINTS; exit 1 unless src/SR.LNK agrees
./sr module units [-v]         # candidate same-file spans from near-call evidence
```

`./sr check` is the gate. `MATCH` means every byte matches except fixups (relocated addresses), the sizes are equal, every fixup refers to the same thing as in the original (`refs:` globals by DGROUP offset, calls by function address, string literals by content), and the source passes the lint (rule 4). `MATCH*` means the same, but the function near-calls a function of another object (a public near function in the same code segment): the object leaves that displacement to LINK, so only the linked build verifies it (21 functions, all in segment 0000). Near calls inside the file are resolved and checked by the file gate.

**Compile backend: `SR_EMU`.** `emu2` (default) runs `CL`/`MASM` under [emu2](https://github.com/dmsc/emu2), a user-space 8086 + DOS-call emulator with no boot: one process per job, run in parallel (`SR_JOBS`, default 16), ~25 ms per small compile. `SR_EMU=dosbox` is the original one-DOSBox-X-boot-per-batch backend. Both give byte-identical OBJ, COD and CC.LOG (proved on every source here and in ../msc51 with `tools/scripts/emucmp.py`; rerun it after touching either backend or `tools/nix/emu2.nix`). The `./sr` wrapper caches the nix-shell PATH in `work/.nix-path-<hash>` (refreshed when `shell.nix` changes), so a command no longer pays ~2 s to enter the shell. Ported from ../msc51, which has the measurements.

### The build

The build definition is checked in, as the developers had it: `src/MAKEFILE` (MSC MAKE 4.07,
written like MSC 5.1's own `STARTUP\MAKEFILE`: macros `CC=CL /c /W3 /Fc` and
`ASM=MASM`, one block per object with its source and the headers it includes) and the LINK
response file `src/SR.LNK` (the 96 objects in link order). MAKE runs the blocks in file order:
LIB 3.11 combines MSC's untouched libraries into `MLIBCE.LIB`
(`LIB MLIBCE.LIB +MLIBCR.LIB+LIBH.LIB+EM.LIB+MLIBFP.LIB;`, the component order matters), each
object's `$(CC) <flags> /NT<segment> SYS\<file>` or `$(ASM) /Ml ...` line, then
`LINK /MAP /STACK:2560 /EXEPACK @SR.LNK` with the objects' default library. The build directory
is `src/` itself (C:), with MSC 5.1 on D: and MASM 5.1 on F: and MSC SETUP's environment
(`PATH`, `INCLUDE`, `LIB`, `TMP`; `build.DOS_ENV`). Run `SRENV.BAT` before MAKE; it supplies the project folder list for INCLUDE. Headers stay beside their sources; shared STRUCTS.H, PC.H, PC.INC and MAIN.INC live in SYS. MASM does not search its source folder. The host reads the same batch file for its environment, and check jobs stage all headers flat on E:.

`./sr build` (`make`) copies `src/` to `work/layout` and runs every block exactly as written:
CL, MASM and LIB under emu2 (in parallel, each in its own copy under `work/layout/BUILD/<stem>`),
LINK in DOSBox-X (emu2 crashes on this link). This part does not read `orig/`
(`--no-verify` stops there). The verification follows: each code object is gated against the
original (its functions are those its source defines), the link is repeated without
`/EXEPACK` into `SR.EXE` for `./sr linkcheck` and its `BYTE-MATCH.JSON` (positions of all 572
functions, all 748 near calls), and the packed `SRPACKED.EXE` must be byte-identical to
`orig/SR.EXE`. `./dosbox.sh --make` runs `MAKE MAKEFILE` natively in DOSBox-X on a fresh copy of
`src/` (no host tool involved) and compares its `SR.EXE` too. Neither `./sr build` nor
`./sr check` writes `src/`.

### Source files: subsystem folders under `src/`

Every original object is one source file in its subsystem folder under `src/` with one block in `src/MAKEFILE`
(80 code files, plus the data-only HOTSPOT.C, MESSAGES.ASM, KEYMAPS.ASM, SCENERY.C, SCENERY2.C; their DGROUP
ranges are `dgroup.DATA_UNITS`). A code file's extent in the image is the span of the
functions it defines (`module.files()`). The 55 C files and 25 MASM files hold
their functions in address order: includes, the file-scope statics and `static ... near`
forward declarations, then each function with its comment, initialized data before the
function whose literals precede it. Each file is named for what the module is (GARAGE.C, DISK.C); M_02B9A.ASM keeps its address name until its identity is known.

- **One command line per file:** its MAKEFILE block. A C file's line ends in
  `/NT S<region>_TEXT` (the code segment shared by near interfaces, a placeholder name: the
  segment's first address / 16). An assembler file names that segment in its own `.CODE` directive
  (SPEAKER.ASM is a `.MODEL SMALL` file with an explicit `SEGMENT`, see its comment).
  `./sr flags FILE` prints or sets it. Code that must not be loop-optimized under the file's
  `/Ol` carries the real `#pragma loop_opt(off)` (SYS.C, GAME.C, EGABLIT.C).
- **What is compiled is the file in git.** `./sr check` and `./sr build` copy it unchanged; no
  placeholders, no text rewriting.
- **The file gate.** `./sr check` on any function compiles its whole file and also compares
  the whole span against the image: bytes between functions (including code-segment data such
  as CGALINE.ASM's `dbl` flag byte), internal near-call displacements, internal calls and
  jump-table offsets. References to other objects and DGROUP remain relocation-masked and
  checked by `refs:`. A near call to another object is counted (`MATCH*`) and verified by the
  linked build (`Near-call encodings: 748/748 exact`).
- `./sr module units [-v]` lists candidate same-file spans from near / `push cs; call near` evidence (how the units were found; not file-boundary proof on its own: 4c).

## Workflow

Every function is matched. To change a function, edit it in its file and `./sr check` it (the whole file is compiled and gated); the `decompile-function` skill has the loop. Consult `msc51-recipe-book` when a function diverges. Run `wrap-up` at the end of a session. Change the tooling with the `sr-cli` skill.

## Naming conventions

The tree is full of names a machine invented (`F_xxxxx`, `g_xxxx`, `M_xxxxx.C`, hash-tuned placeholders such as `ai70e4t`, struct members `m<addr>`/`fNN`). They are placeholders, not 1989 house style, and they go — one evidence-backed rename at a time. This is the standard from now on:

| Kind | Convention | Example |
|---|---|---|
| Trivial variable | one letter (if unclear, make it a local) | `i`, `n`, `c` |
| Local variable | `camelCase` | `count`, `curSlot` |
| Global variable | `g_` + `camelCase` (always, even pointers) | `g_trackPoints`, `g_videoDriver` |
| Const / macro | `SCREAMING_SNAKE_CASE` | `MAX_RACERS` |
| Struct / enum | bare `PascalCase`, no module prefix | `Car`, `Bitmap` |
| Struct member | `camelCase` | `kind`, `lodCount` |
| Enum value | `SCREAMING_SNAKE_CASE` | `GEAR_REVERSE` |
| Public function | `xx_snake_case`, using the module's two-letter prefix, declared in its `.H` | `ga_buy_part` |
| Private function | `static`, `snake_case` | `static build_list` |
| Source file | DOS 8.3, upper case, named for what the module is; its header has the same base name | `GARAGE.C`, `GARAGE.H` |

A *module* is one original source file (one object). Named public functions use `xx_snake_case`, where `xx` is the module's unique two-letter prefix from the table below (`GARAGE.C` → `ga_...`); assembler routines use the same prefix after the ABI underscore (`_ga_...` in MASM). Keep the semantic suffix and the file name unchanged. Private functions remain `static snake_case`. Existing address placeholders (`F_XXXXX`, `M_XXXXX`) stay unchanged until evidence supports a name; `main` and functions the C runtime owns keep their runtime names.

### Module prefixes

Prefixes are unique ignoring case. Data-only modules need none. A prefix is reserved even when a module currently has only unnamed public functions. `mb_` is provisional for M_02B9A.ASM (M + hex B), without asserting the module's identity; its already-named functions use this prefix.

#### CGA

| Module | Prefix |
|---|---|
| CGA.C | `cg_` |
| CGABLT.C | `cb_` |
| CGADRV.ASM | `cd_` |
| CGALINE.ASM | `cl_` |
| CGASHIFT.C | `cx_` |
| CGASPR.C | `cs_` |

#### EGA

| Module | Prefix |
|---|---|
| EGA.C | `eg_` |
| EGABLIT.C | `eb_` |
| EGACOPY.C | `ec_` |
| EGALATCH.ASM | `ea_` |
| EGALINE.ASM | `el_` |
| EGAPAGE.C | `ep_` |
| EGARECOL.C | `er_` |
| EGASHIFT.C | `ex_` |
| EGASLIDE.C | `ev_` |
| EGASPAN.ASM | `en_` |
| EGASPR.C | `es_` |

#### GAME

| Module | Prefix |
|---|---|
| ADS.C | `av_` |
| BOBS.C | `bo_` |
| CARHOP.C | `ch_` |
| CARPIC.C | `cp_` |
| CRUISE.C | `cr_` |
| DASH.C | `da_` |
| DRIVE.C | `dv_` |
| DRVTICK.C | `dt_` |
| FAME.C | `fa_` |
| GAME.C | `gm_` |
| GARAGE.C | `ga_` |
| GARGFX.C | `gx_` |
| PERF.C | `pf_` |
| RACE.C | `ra_` |
| ROAD.C | `rd_` |
| SAVEGAME.C | `sg_` |
| SHRINK.C | `sr_` |
| TIRES.C | `ti_` |
| VIEW.C | `vw_` |

#### HERC

| Module | Prefix |
|---|---|
| HERCDET.ASM | `hd_` |
| HERCGFX.ASM | `hg_` |

#### INPUT

| Module | Prefix |
|---|---|
| JOY.ASM | `jy_` |
| KBD.ASM | `kb_` |
| KBDGET.ASM | `kg_` |
| KBDOFF.ASM | `ko_` |
| KEYCLR.C | `kc_` |
| MOUSE.C | `mo_` |

#### SOUND

| Module | Prefix |
|---|---|
| MUSIC.C | `mu_` |
| SOUND.C | `so_` |
| SPEAKER.ASM | `sk_` |

#### SYS

| Module | Prefix |
|---|---|
| ARENA.C | `ar_` |
| DISK.C | `di_` |
| MAIN.C | `mi_` |
| MARKS.C | `ma_` |
| MEM.C | `me_` |
| MEMSTR.ASM | `ms_` |
| M_02B9A.ASM | `mb_` |
| POOLS.C | `pl_` |
| RANDOM.C | `rn_` |
| RLE.ASM | `rl_` |
| SYS.C | `sy_` |

#### TANDY

| Module | Prefix |
|---|---|
| TANDY.C | `ta_` |
| TANDYBLT.C | `tb_` |
| TANDYCPY.ASM | `ty_` |
| TANDYLIN.ASM | `tl_` |
| TANDYPIC.ASM | `tp_` |
| TANDYSPR.C | `ts_` |
| TANDYTXT.ASM | `tt_` |

#### UI

| Module | Prefix |
|---|---|
| CURSOR.C | `cu_` |
| EDIT.C | `ed_` |
| MENU.C | `mn_` |
| POPUP.C | `pu_` |
| SCREEN.C | `sc_` |

#### VIDEO

| Module | Prefix |
|---|---|
| BITMAP.C | `bm_` |
| CRTC.ASM | `ct_` |
| CURSBLT.ASM | `uc_` |
| DRAW.C | `dr_` |
| MASKBG.ASM | `mg_` |
| MASKBLT.ASM | `mk_` |
| MASKOR.ASM | `mr_` |
| SPRITE.C | `sp_` |
| VIDDET.ASM | `vd_` |
| VIDEO.C | `vi_` |

Rejected `ad_`: private `ad_picture` already occupies that prefix; ADS uses `av_` (adVertisements).

Rejected `sh_`: MSC sharing macros (`SH_*`) occupy that prefix; SHRINK uses `sr_` (ShRink).

Rejected `tc_`: MSC termination-code macros (`TC_*`) occupy that prefix; TANDYCPY uses `ty_` (copY).

**Name only from evidence.** Read the function, its callers, what it writes and who reads that back, the strings and data it uses. Only then name it. A wrong name is worse than a placeholder: it is a confident lie the next reader builds on. If you cannot tell what something is, leave the placeholder. Byte identity proves the data flow is right, never that a name is right.

**A rename is a change to verify, not a free edit.** In this project names are often load-bearing:
- local names decide the stack frame layout (rule 3);
- uninitialized globals are placed by LINK's communal hash (rule 4e) and file-local statics by MSC's name hash (rule 4f): renaming one can move others;
- external symbols' name lengths count toward MSC's per-file budget (rule 4d), and so do a file's static function names: renaming a function or global can change the code of a later function in any file that references it. GARAGE.C (F_07AA4) and VIEW.C (F_2969D) have almost no slack left.
Every rename must keep `./sr check --all`, `./sr build` and `./dosbox.sh --make` IDENTICAL. Rename through the tools so the sources, headers, MAKEFILE and SR.LNK stay consistent (`./sr rename OLD NEW`, or `./sr rename M_xxxxx.C NAME.C` for a file and its header; it runs `./sr headers`).

- **Never add comments.** Do not add comments to the code unless instructed to; all documentation is human-reviewed.
- **`NULL` for pointers.** Use `NULL` for a null pointer, never bare `0`; reserve `0` for integers (in the medium model `NULL` is `0` for data pointers, so it is byte-free — verify).
- **Enum or `#define`.** A closed set of values held in `int`-sized things (a function's return codes, an `int` variable or parameter, an `int` struct member) is an `enum`, and the function, variable or member is typed with it: MSC 5.1 enums are `int`, so the bytes stay identical. A closed set held in `char`-sized fields uses enum constants, and the field stays `char` (an enum type would make it 2 bytes and change the layout); a `typedef signed char XxxCode;` may document which enum it holds. A lone value that is not part of a set (a marker byte, a sentinel, a size) is a `#define`. Constants used by one file go at the top of that file; constants of a module's interface go in its header; PC hardware facts go in PC.H / PC.INC.

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

As in the original, each source file has its own header: `src/<folder>/<FILE>.H` declares what that file defines (one header per source file, e.g. `BOBS.H` for BOBS.C; standalone data files get theirs, e.g. `SCREENS.H`, `MESSAGES.H`). A `.C` includes `STRUCTS.H`, its own header, and the headers of the files whose globals and functions it uses, and declares nothing itself. Module headers may also define constants; these are owned by the header and need no duplicate source definition. The headers are hand-written source and are what the build compiles; each declaration line ends in its address comment, which the tools read (rule 5). `./sr headers` rewrites every C source's include block from them and the C and MASM include dependencies in `src/MAKEFILE`; run it after adding a reference to another file's symbol or a declaration. `./sr headers --check` (and the test suite) fails when those have drifted, or when a header declares a name its file does not define or two headers declare one name. Unused declarations do not count against the compiler budget (rule 4d), so the include lists cannot change code.

- **Globals:** `extern TYPE NAME;   /* DS:XXXX */` in the defining file's header. One name per DGROUP offset, project-wide.
- **Far data:** `extern TYPE far NAME[];   /* SSSS:OOOO */` in the defining file's header.
- **Far functions:** `TYPE NAME(ARGS);   /* 0xAAAAA */` in the defining file's header. A call without a prototype fails the check (MSC's C4071 at `/W3`). A CRT function may have several library names (`aFlmul`/`aFulmul`); they come from the library (`./sr crtnames`) and `refs` accepts any.
- **Near functions other files call:** about 29 near functions are called (`call near`, no `push cs`) from other objects sharing the code segment. They are public `T near F(...)` definitions (not `static`), prototyped `T near F(ARGS);   /* 0xAAAAA */` in the owner's header; their callers include that header and do not forward-declare them. Only file-local near functions stay `static`.
- **Struct types:** shared types in `src/SYS/STRUCTS.H`; types used by only one module in that module's header. Hand-written, one definition per type.
- **Allowed in a `.C`:** standard MSC headers, and `static ... near` forward declarations of its own static functions.
- **Asm:** `EXTRN` names must be the declared names (the `refs` check enforces it).

`./sr check` lints for `extern`, local prototypes and local struct definitions. Name a global or function only once you know what it is; until then keep `g_XXXX` / `F_XXXXX`.

### 4b. Data belongs to the file that owned it

The original DGROUP is the objects' `_DATA` in link order: each translation unit's string literals (in parse order) and its initialized globals (flushed after the next function's literals). `./sr dgroup` recovers the owner and the declaration slot of every item from those positions. The sources are the truth for the definitions themselves (C marks each with `/* DS:xxxx */`; MASM data carries the address in its comment): `./sr dgroup check` verifies that every named item is defined in the file that owned it, and the build checks the bytes. A file that defines data is gated on it (`datacheck`: base, bytes and every pointer). Overlapping names are views, rewritten as `base[i]`; a literal referenced twice at one address is a named array; a unit whose function stops matching once its tables are defined did not own them (MSC's frame layout depends on the number of file-scope definitions), so they go to a data-only unit.

### 4c. Objects and link order come from the relocation table

LINK lists relocations object by object, each data record's fixups in descending offset (MSC; MASM ascending). `orig/SR.EXE`'s packed relocation table therefore shows which units were one object and in which order objects were linked. A far call between two functions proves two files (MSC near-calls every function defined in the same file). `src/SR.LNK` is the recovered link order. `./sr dgroup order` re-derives it (the literal pools pin the objects that have data; `dgroup.ORDER_HINTS` holds the relocation-table evidence for the others) and fails if SR.LNK disagrees. The fonts (FONTDATA, CGAFONT) are linked just before the first file of code segment S0F38, the far data after the game objects, SLOTS last. `./sr build` ends with the packed EXE's comparison against `orig/SR.EXE`.

### 4d. The compiler budget: global symbols are load-bearing

MSC 5.1's code generator degrades (fewer loop register variables, other temps) once the external symbols referenced so far in a file exceed a budget measured in symbols plus name characters. A function can match alone and fail in its real file. External functions, external data, communals and the file's own statics (static functions and the static variables it references, names included) count; locals, prototypes, literals and initialized data do not. Folding globals into the structs/arrays they were (with code evidence: a run-time `a - b` between two addresses or a direct `g[i]` index proves separate symbols) is how the original files stayed under it. `./sr build` must stay IDENTICAL against `orig/SR.EXE`.

### 4e. Uninitialized storage is communal, and LINK's hash fixes the names

DS:6E60..8F02 (`c_common`) holds no labels: every block is a C tentative definition (`int x;`, `struct T a[N];`), placed in the file that first mentions it or in any later file that uses it (the slot is fixed by the first mention, an `extern` counts; e.g. the racer table is defined in BOBS.C though DISK.C mentions it first), under the comment `/* Uninitialized storage: communals ... */`. LINK 5.01 allocates communals by symbol-hash bucket (ascending; `tools/sr/linkhash.py`), and within a bucket the most recently created symbol first (created at its first EXTDEF/COMDEF/PUBDEF in link order). Blocks larger than one byte are word-aligned. A block's address therefore constrains its name: renaming a COMMON global, adding one, or moving its first mention to another object can move every later block. Check any such change with `./sr build` (it must print `Packed EXE vs orig/SR.EXE: IDENTICAL`). The current names were chosen to satisfy this (same length as the old placeholder where possible), so they are still placeholders, not recovered names. There is no assembler COMMON storage left. Every array and struct has the size the original allocated, taken from the save-game table at DS:4EA4/4EBE (block addresses and byte counts) where one exists. Operand order in MSC comparisons and additions also tells whether an operand is a struct member or a plain variable (see the recipe book); several placeholder structs (`State70C0`, `State7D2E`, `State7E8A`) exist only because of that evidence. Unreferenced blocks are `char` communals in `src/UI/HOTSPOT.C`.

### 4f. File-local storage is `static`, and MSC's hash fixes the order

DS:6C22..6E2C (`_BSS`) is file-local: every block is used by one original file, so it is an uninitialized `static` there (`static int g_cursorDrawX;   /* DS:6C46 */`), and LINK concatenates the objects' `_BSS` in link order. Within one object MSC 5.1 orders its uninitialized statics by `sum(byte & 0xDF) mod 256` of the name, ascending; equal keys put the later declaration first; ints and larger items are word-aligned. Renaming a static therefore moves the statics of its own file only: keep the keys of a file's statics increasing in address order (`./sr build` must stay IDENTICAL). The names are placeholders chosen for that, like the COMMON ones. Each static is defined once, at the top of its file, with its `/* DS:XXXX */`. Statics are not in any header; `./sr check` takes their addresses from those comments and resolves a `_BSS` fixup by the names and offsets in the compiler listing. `g_6c42` (4 unreferenced bytes after `g_popupLayout`) is PROVISIONAL filler.

### 5. The sources are the truth

The EXE is the ground truth for what must match; the sources and headers are the only record of what we know about it. There is no database. The tools derive every table from the files (`tools/sr/source.py`):

- **Game functions:** the comment above each definition, `/* name @ SSSS:OOOO (linear 0xAAAAA, N bytes) */` (`; name @ ...` in MASM): name, address, size; the file is its source, the extension its kind, `near` / `PROC NEAR` its distance.
- **Globals and far data:** the headers' `extern` lines and their address comments; file statics from the `/* DS:XXXX */` of their definitions.
- **Runtime:** the scanner's functions in the runtime range, named by the MSC 5.1 library modules located in the EXE; the library's DGROUP data publics (`errno`, `_doserrno`, `_amblksiz`) from the same match. Cached in `work/cache/`.

`./sr scan` reruns the scanner (which also checks complete gaps for prologue-free routines) and fails when its functions differ from the sources'. Every function in the sources is matched; `./sr check` reports, nothing is recorded.

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
| `src/{SYS,UI,GAME,VIDEO,CGA,EGA,TANDY,HERC,INPUT,SOUND}/` | One source file and same-name header per original object (8.3 names for what each module is, e.g. GARAGE.C, CGALINE.ASM), plus the data-only HOTSPOT.C, MESSAGES.ASM, KEYMAPS.ASM, SCENERY.C, SCENERY2.C |
| `src/MAKEFILE`, `src/SR.LNK` | The build: one block per object with its command line (MSC MAKE 4.07), the LINK response file (link order) |
| `src/SRENV.BAT` | INCLUDE search path: subsystem folders, DATA, then compiler headers; run before MAKE |
| `tools/sr/` | The CLI |
| `docs/RECONSTRUCTION.md` | What in the tree is a reconstruction choice, not recovered fact; update it when adding one |
| `plans/diary.md` | Captain's log |
| `work/` | Scratch output (ignored). Nothing in it is needed to reproduce a documented result; `work/...` paths in the recipe book name experiments that were not kept. To try a source variant, edit the file, `./sr check` / `./sr diff` the function, then `git checkout` the file; small test functions are compiled with `tools/sr/dosbox.py` jobs (`Job(path, flags, name)`, `compile_all`) |
