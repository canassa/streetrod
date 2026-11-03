# Street Rod

Reverse engineering and decompilation of *Street Rod* (Logical Design Works / California Dreams, 1989), DOS version 1.0. The goal is C source that compiles, function by function, to the same bytes as the shipped `SR.EXE`.

## Facts about the binary

- **Compiler:** Microsoft C 5.1 (`toolchain/msc51`). Confirmed by library matches: the C runtime is `MLIBCR.LIB` and the 8087 emulator is `EM.LIB`.
- **Memory model:** medium (`/AM`): far code, near data. Floating point uses the emulator (`/FPi`).
- **Default flags:** `/AM /Oas /Gs /FPi`, used by most matched functions. Some modules differ: `/Oasi` (`0x032F4`) and `/Oals` (`0x14294`, `0x16532`). Per-function flags are recorded with `./sr flags`. Modules may differ; some show `/Oi` intrinsics (inline `strlen`) and `/Ol` (`loop`), so expect per-module flags.
- **Packing:** Microsoft EXEPACK. `orig/SR_unpacked.EXE` is the unpacked image and is what every tool reads.
- **Addresses** are linear offsets into the load image (file offset minus the `0x22D0` header). `seg:off` is the segment value the callers use.
- **Size:** about 709 functions. Roughly 480 are game C, 83 are game asm, and 145 are the MSC runtime (`kind = crt`, `status = skip`, never reconstructed).
- **Hand-written asm:** line drawing, unrolled sprite blitters, video helpers, and small register-saving helpers. They are rebuilt with MASM 5.1 (`toolchain/masm51`, same 1988 release as the compiler) from `src/funcs/F_xxxxx.ASM`.

## The `sr` CLI

`./sr` drives everything (it enters `shell.nix` on its own if `dosbox-x` or capstone is missing). The DB is `db/sr.sqlite`. Run `./sr <command> -h` for options.

```
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
./sr proto 15236 "int @(int n)" # a far function's prototype for PROTOS.H ("" clears; --force replaces an existing one)
./sr data 4eee [N]             # hex dump of DGROUP bytes with known globals
./sr headers                   # regenerate GLOBALS.H / PROTOS.H (check does this itself)
./sr layout a b:long '{ c }'   # predict the [bp-N] layout of locals from their names (--ol for /Ol)
./sr layout -w x cnt i         # suggest renames + declaration order for this top-to-bottom order
./sr layout --bucket 5          # readable names in bucket 5 (pick one instead of a digit-suffixed rename)
./sr mark ADDR blocked -m why  # set status: todo | wip | matched | partial | blocked | skip
./sr kind ADDR asm -m why      # correct the scanner's c/asm/crt guess
./sr note ADDR text...         # append a note to a function (--replace to overwrite)
./sr src ADDR path             # point a function at another source file (a module)
./sr status                    # summary + regenerate docs/PROGRESS.md
```

`./sr check` is the gate. `MATCH` means every byte matches except fixups (relocated addresses), the sizes are equal, every fixup refers to the same thing as in the original (`refs:` globals by DGROUP offset, calls by function address, string literals by content), and the source passes the lint (rule 4). `MATCH*` means the same, but some near calls to other functions can't be verified because their displacements depend on the original module layout. A one-function compile can't reproduce those.

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

### 4. Declarations come from the shared headers, never ad hoc

Every `.C` includes `GLOBALS.H` and `PROTOS.H` (in `src/include/`, generated from the DB) and declares nothing itself:

- **Globals:** `./sr global OFF NAME --decl "TYPE @"`. One name per DGROUP offset, project-wide.
- **Far functions:** `./sr proto ADDR "TYPE @(ARGS)"`. A call without a prototype fails the check (MSC's C4071 at `/W3`). A CRT function may have several library names (`aFlmul`/`aFulmul`); `crtnames` records them in the `aliases` table and `refs` accepts any.
- **Struct types:** `src/include/STRUCTS.H`, hand-written, one definition per type.
- **Allowed in a `.C`:** standard MSC headers, and `static ... near` forward declarations of module-mates.
- **Asm:** `EXTRN` names must be the DB names (the `refs` check enforces it).

`./sr check` lints for `extern`, local prototypes and local struct definitions. Name a global or function only once you know what it is; until then keep `g_XXXX` / `F_XXXXX`.

### 5. The DB comes from the EXE and the CLI

The EXE is the ground truth. `db/sr.sqlite` is derived from it by `./sr scan`, plus what we record through `sr` commands. Never edit the DB or the generated headers by hand, and never edit `docs/PROGRESS.md` (`./sr status` regenerates it).

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
| `src/include/` | `GLOBALS.H`, `PROTOS.H` (generated) and `STRUCTS.H` (hand-written); on the compiler's `INCLUDE` path as `E:` |
| `tools/sr/` | The CLI |
| `db/sr.sqlite` | Functions, callees, named globals |
| `docs/PROGRESS.md` | Generated progress report |
| `plans/diary.md` | Captain's log |
| `work/` | Scratch output (ignored). `work/op/tryv.sh ADDR V.C...` checks variants and restores the source, `work/op/diffv.sh ADDR V.C [--full]` diffs one, `work/op/regions.py` prints a diff as address ranges; `work/bp/x2.py DIR FLAGS` lists small test functions (`T*.C`, 8.3 names) |
