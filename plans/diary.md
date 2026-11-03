# Captain's log

Append-only log of work on this project. Each session adds an entry at the bottom: `## YYYY-MM-DD HH:MM TZ: <title>`, then **What I did** and **What I learned**. Never edit an old entry; correct it in a new one.

---

## 2026-11-03 03:50 CET: project setup, first two matches, sr CLI

### What I did

- Identified the toolchain: Microsoft C 5.1, medium model, `/FPi`. The C runtime matches `MLIBCR.LIB` (93.6% of bytes, the rest unmasked fixups) and the emulator matches `EM.LIB` (96.8%).
- Unpacked EXEPACK. Found the crack in the freeware EXE (6 NOPs at file offset `0x334E`) and rebuilt the retail EXE (`orig/SR.EXE`, SHA-1 `3a5e28b7...`) using the uncracked EXE on The Good Old Days disk image as a reference.
- Matched `name_too_long` (`0x032D0`, 36 bytes) and `protect_check` (`0x02FC8`, 403 bytes, the manual color check) with `/AM /Oas /Gs /FPi`.
- Built the `sr` CLI, the SQLite DB (708 functions from `sr scan`, 108 CRT functions named by `sr crtnames`), CLAUDE.md, and the skills `decompile-function`, `msc51-recipe-book`, `wrap-up`, `sr-cli`.

### What I learned

- MSC 5.1 orders locals on the stack by something derived from their names. Renames break matches. Open question in the recipe book.
- `/Ox` duplicates epilogues per return path; the game was compiled for size (`/Oas /Gs`), at least in segment `0000`.
- About 92% of the game code is compiled C. The hand-written asm (line drawing, blitters) needs MASM 5.1, which we don't have.

## 2026-11-03 03:58 CET: first run of the decompile-function loop

### What I did

- Followed `decompile-function` on the smallest todo functions. `0x11E9C` (12 bytes, `unsigned * global`) matched on the first try with the default flags, the first match in segment `0F38`.
- Found that `0x02B9A` and `0x02BAD` are hand-written (they save `ax`) and reclassified them as asm. Added `./sr kind` for that.

### What I learned

- The classifier marks small asm routines that use a bp frame as C. Expect more of these among the small functions; the `push ax` tell is in the recipe book.

## 2026-11-03 04:05 CET: MASM 5.1, asm path in sr

### What I did

- Installed MASM 5.1 (the user downloaded the WinWorld 5.25" set) into `toolchain/masm51`, mounted as `F:` in DOSBox.
- `sr` now builds `.ASM` sources with `masm /Ml`, finds the function by its `PUBLIC`, and `sr new --asm` writes a MASM skeleton.
- Matched `0x02B9A` (19 bytes, stores two byte args into globals, saves `ax`) as asm on the first try.

### What I learned

- MASM 5.1's `.MODEL MEDIUM` / `.CODE` / `EXTRN ...:BYTE` in `.DATA` reproduces the original encodings directly, including the short `mov [mem],al` form. The game's hand-written routines may well have been written with this exact assembler.

## 2026-11-03 04:30 CET: first parallel run, 4 Sonnet agents, 15 functions

### What I did

- Four agents worked on disjoint sets: protect_check's callees, segment `0000` neighbours, small `0F38`/`2645` functions, asm candidates. 13 matched (3 of them `MATCH*`), 2 partial (`0x14294` frame 2 bytes bigger; `0x158DE` far-pointer construction). 17 matched in total now.
- Notable reconstructions: `random_int` (`0x15236`, Wichmann-Hill RNG), `insert_entry` (`0x032F4`, high-score insert; shows the table is `{char name[16]; char score[7];}` at `0x82CA`), `fill_span_clipped` and `blit_rect` (asm graphics primitives).
- Tooling: per-process build dirs (parallel-safe), `.STB` sidecars for same-module callee placeholders (agents had put them inside the `.C`), `check --all` gates only `matched`, `sr name ADDR` prints the current name.
- Merged the agents' findings into the recipe book and the skill.

### What I learned

- Same-module calls (`push cs; call near`) are common; until module files exist, `.STB` placeholders are the way to compile them.
- `register` variables get stack homes; block scope matters for the frame size.
- The "single letters are alphabetical" locals heuristic is not a rule.
- To review: `out_of_memory` (`0x036E9`) only matches with a dead `char *` local, which may be invented.

## 2026-11-03 04:45 CET: honesty review of the agents' sources

### What I did

- Reviewed all 19 sources against the disassembly. Nothing invented to force bytes. `out_of_memory` was rewritten to the simpler `char *msg = "..."; F_1174E(msg);`, which also matches; the agent's version had a duplicated literal and a dead local. `fill_span_clipped` now uses the clip-window names; fixed a wrong address in `F_02BAD.ASM`'s header.

### What I learned

- MSC 5.1 propagates a constant assigned to a local into later uses and keeps the store. Since fixups are masked, `./sr check` can't tell which string a push refers to: when two sources match, choose the simpler one.
- `0x14294`'s odd `p = cur;` mirrors a real read of an uninitialized local in the original (`mov si,[bp-2]`), not an invention.

## 2026-11-03 05:20 CET: shared headers, reference check

### What I did

- Globals and prototypes now live in the DB and are generated into `GLOBALS.H` / `PROTOS.H`; struct types in `STRUCTS.H`. All 15 C sources rewritten to include them; `sr check` lints out `extern`, local prototypes and local structs, and builds with `/W3` so a call without a prototype (C4071) fails.
- `sr check` verifies every fixup against the original (`refs:`): globals by DGROUP offset, calls by target address, string literals by content. All 17 matches pass it (35 references in `protect_check`).

### What I learned

- The byte check alone had let wrong references through: `random_int`'s seeding chain stored the three states in the reverse order; my callee names in `protect_check` were wrong (`pick_color` is `F_14294`, an allocator); `name_too_long` was really `atoi(g_table[i].score) > g_score`, now `entry_beats_score` (`0x1FA90` is the CRT `atoi` thunk; 3-byte CRT modules are too small for `crtnames`).
- MSC emits LEXTDEF/LPUBDEF (`0xB4`/`0xB6`) for statics; they share the extern index space.
- `DS:0692` is a 16-byte colour table, not a string.

## 2026-11-03 05:55 CET: second parallel run, 2 Opus + 5 Sonnet

### What I did

- 5 Sonnets took 29 fresh functions; 2 Opus took the two partials. All 31 now match: 47 functions matched in total.
- Tooling: far data (`./sr global SEG:OFF`, a `fardata` table emitted into `GLOBALS.H`, `refs` checks segment-constant words and `SEG` immediates), `sr show` labels segment constants and prints linear branch targets, atomic header writes (a parallel race broke one build), `refs` canonicalises CRT helper names (`__aFldiv`), `refs` stays quiet until the bytes line up, `sr global OFF` shows an entry.
- Merged the findings into the recipe book and skills; corrected the old "repeated `shl reg,1`" fingerprint, which came from asm.

### What I learned

- `0x158DE`: `DS:691C` is MSC's segment constant for a far array (relocated in the EXE), not a variable. With far data declared, the source is `if (*(pp = &g_slots[n].ptr))`.
- `0x14294`: the brief was wrong. `sr show` printed a backwards `je` as a segment offset, hiding a loop; with `/Ol` the loop variable lives in `si`. Linear targets now.
- Several modules use `/Ol` or `/Oi`; flags really are per module.

## 2026-11-03 06:10 CET: review of round-2 sources

### What I did

- Reviewed all 30 new sources and STRUCTS.H against the disassembly. Two invented constructs fixed: `F_03C3E` and `F_03FD4` returned the node, but no caller reads `ax`; now `void`, still matching. Added header comments to 13 files.

### What I learned

- Grounded oddities, kept: a dead loop counter in `0x16532` (`inc di`, stored at exit), `g_051a = 0; g_051c = g_051e = 0;` (`0x00D58`), `tbl[g = K] = v` in three functions (likely a macro; noted on the rows).
- A value in `ax` at the end is not a return value; check the callers.

## 2026-11-03 05:50 CET: the local-layout rule, and `./sr layout`

### What I did

- Measured MSC 5.1's stack-slot order with generated test functions (scratch drivers in `work/lay/`, hundreds of compiles per DOSBox boot) and solved it. The rule is in the recipe book ("Local layout"), replacing the open question.
- New `tools/sr/layout.py` and `./sr layout`: predicts the frame from names, types, blocks, labels (`|`) and `/Ol` loop temps (`~`), and with `-w` proposes renames plus a declaration order for a wanted top-to-bottom order. Documented in CLAUDE.md (rule 3, command list), the recipe book row, the decompile-function and sr-cli skills.
- Validation: default flags 1,620/1,620 random functions (names, every basic type, arrays, nested and sibling blocks, labels, register locals; `/Oas` and `/Oasi`). `/Ol` without nested register locals 550/550. Matched sources: 94/98 predicted exactly; the 4 misses are my scratch C parser (a function-pointer local), two wip files and one `/Ol` loop temp, which `~` now models.

### What I learned

- bucket = byte sum of the name (first 31 chars) & 15; buckets 0..15 from the top of the frame; inside a bucket the later-declared local is higher. Character sum mod 16 had been tried before, but without the declaration-order tie it looked wrong.
- The old open-question data point "`a b aa ba ab bb` lays out as `a, b, aa, ba, ab, bb`" doesn't reproduce: the compiler gives `a, aa, b, ab, ba, bb`.
- Sibling blocks reuse freed slots deepest first. Register locals always get a home slot. `/Ol` restricts reuse (by nesting depth, and the block after the first one at a level can't reuse it) and adds hidden temps for strength-reduced loops.
- Contradiction to check: recipe row "unreferenced ones at function scope get none" (`0x11213`). A plain `int a, b, c;` with `b` never mentioned gave `b` a slot under `/Oas`.
- Still open: `/Ol` with a `register` local inside a nested block (which locals get `si`/`di`, when its slots free).

## 2026-11-03 06:00 CET: merged duplicate struct types (Screen into Bitmap, Pkt into Blt)

### What I did

- `struct Screen` was `struct Bitmap` under another name: same offsets (plane size at 4, pixels at 0x24, pitch at 0x2c, mode/flags byte at 0x2e, stride 0x30 in `g_screens`), and the same pointer is passed both ways (`F_1A734` hands a screen to `F_1A2D0` as its blit source, `F_11A72` and `F_191DD` pass `g_back` and bitmaps to the same `g_78a2` hook). Removed `Screen`; `g_front`, `g_back`, `g_screens`, `g_screen1`, `g_78ae` and the protos of `F_10BD9`, `F_13682`, `F_1377F`, `F_1A734`, `F_1A92A` now use `Bitmap`. Field renames: `pixels` to `bits.p`, `size` to `planesize`, `mode` to `flags`.
- `struct Pkt` was `struct Blt`: every `Pkt` user fills width, height, source x/y, dest x/y in that order, and `F_1A2D0` passes one buffer to both `g_78aa` and `F_10A00`. Pkt users renamed (x,y,w,z,c,d,e to w,h,sx,sy,dx,dy,fc); Blt gained `fc` at 0xc (meaning unknown, often equals h). `g_78a2`, `g_78aa`, `g_7af0`, `F_1A89E` now use `Blt`.
- Dropped the casts in `F_1A734` (also `bm` is now a `Bitmap` with `->width/->height`) and `F_1A2D0`. `F_1A2D0.STB` now repeats the refined `F_1A03F` prototype (it no longer compiled after another worker changed it); `F_1A2D0` stays partial, 92 bytes off instead of 113.
- `struct Pkt` stays in STRUCTS.H, marked deprecated, because the wip `F_0C1D8.C` still uses it.

### What I learned

- Tone/Rec5 and Slot/Bitmap look alike by size but have no shared pointer or global, so they stay separate.
- Pointer-to-long casts on the hook's second argument (`(long)g_back`) come from the hook typedef `(void far *, long, ...)`; probably two `Bitmap far *` originally, not touched here.

## 2026-11-03 06:07 CET: blit hooks take two bitmaps; struct Pkt removed

### What I did

- Every call of `g_78a2`/`g_78aa` passes a bitmap or screen as both the first and the second argument, and a far pointer pushes the same as a long. I retyped both hooks to `(struct Bitmap far *, struct Bitmap far *, struct Blt *, int)` and dropped every `(long)` cast. `g_78ce` now takes a `struct Bitmap far *` too, since all its callers pass a bitmap; that removed the casts in `F_1264E` and `F_12B7C`.
- Knock-on changes: `F_1A69F`/`F_1A6E0` take `struct Bitmap far *dst` instead of `long n` (the long was the destination bitmap), `g_70a8` is a `Bitmap far *`, `F_1A89E` returns a `Bitmap far *`, and the `char far *`/`int far *` locals in `F_118D4`, `F_11EA8`, `F_123F4`, `F_12525` became bitmaps.
- `F_0C1D8` is matched now, so I moved it to `struct Blt` and deleted `struct Pkt`.

### What I learned

- The wip `F_12A17.C` still passes `(char far *)saved` to `g_78ce`, which now only gives a warning; the cast can go when that function is finished.

## 2026-11-03 06:22 CET: values in si/di with no home; six partials matched

### What I did

- Looked for the source behind a value kept in `si`/`di` with no 2-byte stack home. I compiled small test functions many at a time in one DOSBox boot (`work/reg/x.py` for frames and listings, `work/reg/h.py` to score source variants against the original).
- Matched `0x226F4` and `0x1264E`: dropped the `register` temps and wrote the expression at each use (`r->x1`; `f - 8`, `d + 8`).
- Matched `0x0B49C` with `p->x2 = (p->x1 = x) + w - 1;` and `0x019BC` with `if ((p = g_0572 + i)->fa == 1 && ...)`.
- Matched `0x01111`. The joystick loop is `for (j = 1; j; j++) if (...) break;`, the shift temps are CSEs, and `g_79b2`/`g_79b6`/`g_79ba` are 4-byte structs (`struct Pair` stands in, real type unknown).
- Matched `0x0105B`: `if (disk) { if (deverr & 0x1000) action = 1; else action = 2; } else action = 1;`.
- `0x23C3B` stays partial (see below). Updated the recipe book (the `register` home row, a CSE row, five new C-shape rows, a pragma fingerprint, open questions) and one line in decompile-function.

### What I learned

- A `register` local always gets a home slot, under `/Oas` and `/Oals` alike, in a block or not, with or without an initializer. A `register` parameter never gets one: it is loaded from `[bp+N]` into `si`. So a value in `si` with no home comes from MSC's CSE.
- Under `/Ol` a CSE is spilled to a stack temp at the bottom of the frame when its basic block falls straight into a loop. Any `if` or label in between keeps it in `si`, even one with no code (`if (0) ;`, `lab:`). `0x23C3B` matches byte for byte with such a statement before its outer `for`, so the original probably had one. I don't know what it was, so I left the function partial.
- The `/Ol` "out-of-line blocks" weren't caused by `/Ol`. They come from a `break` inside the loop body (the increment moves out) and from cross-jumping a duplicated assignment in a nested `if`.
- `#pragma loop_opt(on|off)` and `check_stack` exist in 5.1. A `loop_opt` inside a function body acts on the whole function. `/On`, `/Op`, `/Or` and `/Ow` changed nothing in these cases.
- `./sr flags ADDR` with no flags resets them to the default. It doesn't print them. I reset `0x23C3B` by accident and restored `/AM /FPi /Oals /Gs`.

## 2026-11-03 06:40 CET: Blt and Clip rect types

**What I did**
- Measured every frame that holds a blit rect local and every offset touched through the rect pointers. `struct Blt` stays 20 bytes; its tail `pad[3]` is now `fe, mx, my` (F_21C0B reads `r->mx`, `r->my` at 0x10/0x12 as the origin in the mask bitmap; F_22762 writes them). `struct Clip` stays 12 bytes.
- F_10A00 only reads and writes offsets 0..0xB of both rects, and its callers give it both sizes: 20-byte outputs in F_1D262, F_21D58, F_23262, F_234E9, F_236D5, F_1A79B, F_1A2D0; 12-byte outputs in F_1D3C8, F_1D195, F_21EAB, F_21C0B; F_1D3C8 also gets a 12-byte input from F_1D8C4 and 20-byte ones from F_1D925 and the g_78a2 callers. No single parameter type fits, so the prototype takes `void *in, void *out`. The C4049/C4024 warnings are gone.
- F_1D262: `int pad4[4]` removed, `ol` is a `struct Blt` (the 8 bytes were its untouched tail). F_1D262 and F_1D195 take `struct Blt *r` (they are fed 20-byte rects through g_78aa and by F_1D925).
- Recipe book: one row on holes at the tail of a struct local.

**What I learned**
- The `long unused` in F_118D4, F_12A17, F_12DD8 and F_12F99 is a real unreferenced 4-byte local, not part of the struct. F_12B7C, from the same module and using the same `move.h = move.fc = ...` pattern, has its 20-byte Blt right under `pict` with no hole, so a 24-byte type can't be the explanation.
- `int f();` (no parameter list) still gives C4071 at /W3, so a K&R declaration can't be used to accept both rect sizes.
- At the end, `check --all` showed 2 build failures (F_13F21, F_14294: PROTOS.H redefinition of F_13DE6/F_13F21). Those come from another worker's in-progress STB/proto change and have nothing to do with these structs.

## 2026-11-03 07:16 CET: /Ol loop-optimizer rules; ten partials matched

### What I did

- Worked through the ten `/Ol` partials whose loop registers or exit stores differed. All ten match now: `0x2B6DC`, `0x1D683`, `0x03614`, `0x1CF40`, `0x0D1DE`, `0x18ADB`, `0x21B7C`, `0x174A6`, `0x133CB`, and `0x0C52C` (MATCH*). Variants were scored with `work/reg/h.py`. Small register-priority tests went through `work/reg/x.py` and `work/lo/sum.sh`, with scratch files in `work/lo/`.
- Added a "`/Ol` loop optimizer" section to the recipe book, plus a note on hidden temp slots in layout rule 6 and one line in the fingerprints.

### What I learned

- Most of these blockers were compiler temps that we had written as named locals. If the loop index is only counted, `for (i = 0; i < K; i++)` turns it into a hidden down-counter. That counter is never stored. It takes an untouched slot at the bottom of the frame, and `i`'s exit value is stored where `i = 0` sits. A `for (i = K; i; i--)` or `do ... while (--n)` keeps the user variable as the counter and stores it at exit. The `[bp-N]` slots that only get advanced, the "unused" frame slots and the unstored loaded register were induction pointers, trip counts and CSE temps.
- Exit stores don't depend on liveness. Every register variable the loop modifies is stored at exit, even a dead parameter.
- `for (; n > 3; n -= 4)` computes its trip count up front with `neg; cwd; idiv -4`, which was the odd `nbn = -n / -4` in `0x1D683`. A byte-copy loop becomes `rep movsw` unless the two pointers differ in signedness.
- An assignment inside an `if` test makes the variable load into its register before the loop (`0x0D1DE`).
- Register priority: candidates rank by how many times the loop references them, and ties go to the first reference. A call inside the loop leaves only `si` and `di` free. Two cases still don't fit the count: `if (++d == 4)` (`0x21B7C`) and increments moved into the `for` clause (`0x174A6`).
- `F_1D741`, a matched function I didn't touch, still forward-declares `F_1D683` with `unsigned char far *dst`. The definition now takes `char far *`, which is consistent with the caller's `char far *c`.

## 2026-11-03 08:12 CET: seven stubborn partials (/Ol, CSE, switch tails)

**What I did**
- Matched `0x15C4A`, `0x012DE`, `0x19925` and `0x192AC` with plain source. For `0x192AC`, `g_8b02` is now `unsigned @[]`, and its four other users still match.
- `0x10AFD` is now a `for (i = 0; i < n; i++)` under `/Oals`. The old `sum` was really the `/Ol` exit value of `i`. The one byte-level divergence left is that `/Ol` hoists `g_videoMode` into `di`.
- `0x004F4` and `0x10AFD` both match once `g_videoMode` is a struct member (measured with a stand-in struct). I left that change undone because it touches 48 files. It needs a decision: `0x8236` probably sits in a video-state struct next to `g_front` and `g_back`.
- `0x008B4`: rewrote the inner loop as a plain `while` with no `register` locals, fixed the names, the store order and the `g_style->hilite` offset. When `g_04fe`, `g_0516`, `g_0518` and `g_051a` are one struct, the inner loop matches. The one divergence left is that `/Ol` keeps `pos` in `cx` across the outer loop.
- Added nine recipe-book rows.
- `./sr check --all`: 488/489. The one regression is `F_0DCBE`, a float-constant ref that another worker edited at 08:02. That file isn't one of mine.

**What I learned**
- If ours hoists a global into a register under `/Ol` and the original re-reads it every pass, the global is a struct member. Arrays indexed by a global counter in the loop behave the same way: when both are members of one struct, `/Ol` makes no induction pointer.
- If `tbl[i]` is reloaded right after `p += tbl[i]` on a far pointer, `tbl` is `unsigned`.
- An assignment used as a subscript (`tbl[q->key = c = p->key]`) stops the address CSE with the next statement.
- `for (; A; ...) { if (!B) break; ... }` keeps both tests at the bottom under `/Ol`. `while (A && B)` moves `B` to the top of the body.
- A shared tail store in a `switch` stays in the first body when every case ends in `break`. When the earlier cases `return`, it stays in the last body.

## 2026-11-03 08:30 CET: type review of shared globals and allocators

**What I did**
- `g_0294`/`g_0296` are one `struct Bitmap far *g_0294` (`g_0296` deleted); casts gone in `0x06F6B`, `0x07367`; `0x1923C`/`0x193D4` write `g_0294 = 0;`.
- `g_8df2` became `struct Model g_8df0[]` (10 bytes: `signed char kind`, then ints at 2, 4, 6, 8; replaces `struct Rec5`). The `- 2` and `* 10 - 2` casts in `0x0711A`/`0x28879` are gone; field names shifted in `0x0542E`, `0x098BA`, `0x0DCBE`, `0x0E218`.
- `g_7526`/`g_8ace` are `struct Bitmap far *[13]` (13 entries, up to `g_755a`/`g_8b02`); `struct Block48` removed. `0x0D992` copies only the first 12.
- `F_15BBE`, `F_1934E`, `F_0376B` (and the near `F_009B7`) return `struct Bitmap far *`, `F_0376B` takes one too; `g_755c`, `g_825a` are `struct Bitmap far *`. About 70 casts removed.
- `F_10A00(dst, src, struct Blt *in, struct Blt *out)` and `F_1D3C8(..., struct Blt *r, ...)`: the 11 casts inside `F_10A00` and 3 in `0x1D925` are gone; the four callers whose result local is a 12-byte `struct Clip` now cast. `F_21AAC` takes `struct Clip *`, as its caller's forward declaration says.
- `F_03AB8(int n)`: n is a message number (an offset into the message bank `g_2c01`, like `F_11C24`/`F_11DE2`). The 19 fake globals (`g_0efa`, `g_1914`, ...) and the literal `"\251"` became the numbers, and the globals were deleted.
- The voice structs `struct Dev g_79aa`/`g_79c6`: `0x01111` now writes their fields, and the 11 overlapping aliases (`g_79b0`..`g_79e0`) are deleted. `g_2e6b_0000` is `int far []`; `g_5858`, `g_5882`, `g_58cc` are `int []` (tone lists ending in -1).
- `do { ... break; } while (0)` in `0x2A84D`/`0x2AFDC` became `goto next;` with a label: same bytes, ordinary 1989 C.
- `Car.f24` is `unsigned`, which drops the `(unsigned)` casts before `>> 8`.
- Kept, each with a note: `*(int *)&g_8aca` in the five `0x28879`..`0x2AFDC` functions (byte everywhere else, so modules disagreed); the plain `g_78ea`/`g_78ec` inside `struct Eng g_78e0` (operand order, see below); `struct Blt16` in `0x10872`/`0x23FD9`; `long unused` in `0x118D4`/`0x12A17`/`0x12DD8`/`0x12F99`; `g_05fc += 0L` in `0x065AF` (likely a time macro called with 0 hours); `g_47e8` BYTE/WORD in asm (each asm module has its own EXTRN, and the byte table is indexed from its end by negative key codes); `g_4607` (a real array shared with `0x1438E`).
- `./sr check --all`: 502/502.

**What I learned**
- `./sr note` replaces the notes, it doesn't append. I overwrote five notes by mistake and restored them from `docs/PROGRESS.md`. Read `./sr show` first.
- Writing a field as `s.f` instead of a plain global changes which operand MSC loads into the register, so operand order shows how a module declared a global.
- MSC does not constant-propagate a relocatable far address (`p = far_array; q = r = p;` reloads `p`). The old recipe row that called `g_79b2` a struct copy was wrong.
- A pushed DGROUP "address" whose target is runtime-zero data can just be a number. Check what the callee does with it before calling it a global.

## 2026-11-03 08:50 CET: struct Video, struct Pages, two overlaps

**What I did**
- `g_front`, `g_back`, `g_8232`, `g_videoMode`, `g_8238`, `g_823a`, `g_823c`, `g_823e` and `g_8240` are now one `struct Video g_vid` at DS:822A (`front`, `back`, `f8`, `mode`, `planes`, `shift`, `bpp`, `mask`, `f16`). F_10BD9 sets all of them in one place. The extent stops at 0x8241: `g_8228` and `g_colors` are left as plain globals because nothing ties them to the struct. I renamed in 121 files, and the two asm files now read `_g_vid+12`. I measured it in a scratch copy of the tree before applying it: every matched user still matched. That includes about 45 `/Ol` functions, so no original enregisters one of these globals. `0x10AFD` and `0x22762` now match. Most partials that use these globals lost bytes (`0x16EA2` 100 to 86, `0x1783E` 27 to 15).
- `0x004F4`: 0 mismatched bytes, but it stays DIFF on lint. The original calls `F_0E5D2` far (`lcall 0:e5d2`), yet `F_0E5D2` is a NEAR asm proc ending in `ret`, so the DB has far=0 and PROTOS.H doesn't emit its prototype. This needs a tooling decision.
- `g_04fe`/`g_0516`/`g_0518`/`g_051a` are now `struct Pages g_pages` (`start[12]`, `n`, `cur`, `list`). The inner loop of `0x008B4` matches; the outer loop still keeps `pos` in `cx`.
- `0x1D925`: the dummy `{ int x, y; }` block became `max(...) - min(...) + 1` and `blt.dx = min(...)` from stdlib.h. Its two slots are `/Ol` CSE temps. `Frame.pos` is now `struct Bitmap far *`, which removes every `(struct Bitmap far *)` cast in `0x1D925`, `0x22762` and `0x23FD9`, and `d` is passed without a cast.
- `g_533e` (`struct Cell[7]`) was `g_5338 + 6`, the `w`/`h`/`x` fields of `struct Btn`. I deleted it and `struct Cell`; `0x18ADB` reads `g_5338[i9]`. `g_5a10`/`g_5a18` don't overlap anything in the DB (two 8-byte Rects, only `0x1783E` touches them), so I left them alone. `struct Rect[2]` and `(*pr2 = &g_5a18)->y1` didn't fix that function's tail either.
- `./sr check --all`: 514/514.

**What I learned**
- Experiments that touch the whole project are cheap in a copy (`rsync` without `work`/`dist`): `sr` resolves everything relative to its own tree.
- A store to `g_5a18.y1` doesn't kill MSC's knowledge that `*pr2 == &g_5a18` in ours, but it does in the original (`0x1783E`). The cause is still unknown.

## 2026-11-03 09:12 CET: block placement and switch lowering

**What I did**
- Ran small batched compiler experiments on switch lowering and block order (`work/bp/s2`..`s7`). `work/bp/x2.py` prints a `.COD` listing with its labels, `work/bp/sig.py` prints a block-order signature against the original, and `work/bp/hh.sh` runs `work/reg/h.py` with the function's `.STB` copied next to the variants.
- Matched `0x0503F`, `0x10E3D`, `0x139DE`, `0x14C90`, `0x1563A` and `0x07367`. `0x06F6B` already matched with its unchanged source, after the struct Video change. `0x1412D` is still partial, with a sharper note.
- Added "Block placement and switch lowering" to the recipe book, and corrected the `while` row (a `while` is not always top-tested).
- `./sr check --all`: 535/535.

**What I learned**
- MSC sorts the case values and merges runs of consecutive labels that share a body into one range entry. It uses a jump table at 7 or more labels when the spread is below about 3 per label. With 7 or more entries, or with 4 or more entries when one of them is a range, it builds a binary tree whose root is entry `n/2-1`. Anything else becomes a linear chain. Trailing compares that go to the default are dropped, but they still count as entries. That is why `0x0503F`'s 4-case switch is a tree: it had a run of labels on the default (`case -2: case -1:`, a guess).
- The first `continue` in source order pulls the loop's increment and test, up to the next unconditional jump, in behind it. `goto` doesn't. A `break` pulls the code after a `for (;;)` up to it in the same way. In an `if`/`else if` chain, a last `else` that ends in a jump is deferred until after the next unconditional jump. These three rules explain the "shared tail / test in the middle" layouts in five of the functions.
- Two smaller ones: `(unsigned char)(a <= b)` gives `mov al,1 / sub al,al / sub ah,ah`, and `x & 0xe0ff` narrows to `and ah,0xe0`.

## 2026-11-03 09:39 CET: operand order and register-allocation partials

**What I did**
- Worked the eleven register-allocation, CSE and operand-order partials. Nine now match: `0x1220B`, `0x0DA25`, `0x068E8`, `0x16EA2`, `0x29BFF`, `0x2992A`, `0x29170`, `0x166FA` and `0x19BFE`. `0x04036` and `0x23C3B` are still partial, with sharper notes.
- Replaced the cursor globals at DS:7aec..7b12 with `struct Cursor g_cursor` (in STRUCTS.H). Its rectangle `r` is what makes `0x1220B` match. `0x11EA8`, `0x120FE`, `0x11E9C` and `0x0AB8B` now use the members. All still match, and `0x0AB8B` is unchanged (it is still partial).
- `0x19BFE` matches with `/Oals` and its current source. The old notes about loop inversion came from an earlier version of the source.
- Gate: 535/535.

**What I learned**
- MSC picks the operand order of compares, commutative ops and `?:` min/max from the shape of the expression tree, never from the source order. Measured order, simplest first: a plain global or a one-level member, then a bp local, then a nested member (`s.a.b`, `t[1].f`), then a member through a pointer. An explicit cast node raises the rank; an implicit conversion does not. In a relational the simpler operand goes into `ax`. In `a < b ? b : a` the more complex operand is loaded and kept as the arm. `t[n] - t[n+1] >= 0` swaps the roles.
- A far pointer kept in `es:bx` across statements with no stack slot comes from cross-jumping. The source is `if (c) {stmts on g1} else {same stmts on g2}`. Likewise `c ? a->f : b->f` puts the `les` in each arm and loads the member after the join.
- `/Ol` ignores goto-built loops. That explains `0x29170`, where one loop is in registers and the other is not. Inside an `if (y < f)` guard, `do ... while (++y < f)` gives the original's layout with no `jmp` to the test.
- Test scripts: `work/op/try.sh ADDR VARIANT.C...` (installs each variant, checks it, then restores the source) and `work/op/run.sh DIR` (small `qqfn` tests). Watch out: when a variant matches, `try.sh` sets the function's status to matched, so put it back with `./sr mark` afterwards (that happened to `0x23C3B`).
- Another session is editing `src/funcs/F_0AB8B.C` at the same time. My `g_cursor` sed ran into its local-variable rename (`r` became `rival`), and I repaired that one line by hand.

## 2026-11-03 12:15 CET: CRT aliases, three matches, cross-jump and deferral rules

### What I did
- Fixed the one thing failing `0x0238C`: the refs check now knows library aliases. `crtnames` keeps every public a module gives an address (new `aliases` table, 11 rows: `aFlmul`, `aFNaldiv`/`aFNalrem`/`aFNauldiv`, `flushall`, ...) and `refs` accepts any of them for a call. Gate 551/551 after the change.
- Matched `0x23C3B` (the `r->sx / 8` CSE plus a code-less `if (p > 40) ;` before the loop: any empty `if` or an unused label works, no pragma does; the condition is a guess, review item) and `0x0B8A1` (`case 3` ends in `continue`).
- `0x0C613` from 663 mismatched bytes (+8) to 123 at the exact size: `F_03AB8(g_7d40 == 0 ? i : ...)` passes `i` from `ax`, and the switch's no-match path is a bare `return;`. `0x04036` from 526 to 18: the four node ternaries as if/else (an `if` between two uses ends the `*q` CSE). `0x049ED` 31 to 30: `g_4f46 = g_7eb0;` as its own statement. Notes on each in the DB.
- Measured on small generated functions (`work/bp/cj`, `work/bp/fp`): which copy of a merged tail survives cross-jumping, what flushes a deferred else, that `default: return;` written first leaves no code, and how MSC orders the operands of a float compare. Rows in the recipe book; the open points are in its "Open questions".
- Helpers in `work/op`: `tryv.sh ADDR V.C...` checks variants and restores the source and status, `diffv.sh ADDR V.C [--full]` diffs one, `regions.py ADDR V.C...` prints a diff as address ranges (block moves show as shifted ranges, so read them with the diff). `work/bp/x2.py DIR FLAGS...` prints small-test listings; name the files `T*.C` (8.3 names).
- The project is now a git repository, pushed to a private GitHub repo (`canassa/streetrod`); `work/` and the SQLite side files are ignored.

### What I learned
- Cross-jumping keeps the copy that falls through into the join; when every copy jumps, the first generated one. A `case` followed only by empty cases is a fall-through copy. `0x0C613`'s original keeps later copies three times over, which this rule cannot produce from any structure tried: open question.
- A deferred `else` is flushed at the next `return` statement; `goto`, `break`, pragmas and the `je +3; jmp` long form of a conditional `goto` do not flush it. A `?:` inside an argument did flush it once (`0x0C613` `r1`), so "unconditional jump" is not the whole rule.
- A code-less `if` before a loop stops `/Ol` from giving a CSE a slot; between two uses it kills the CSE. The same construct therefore cuts both ways (`0x23C3B`, `0x04036`).
- The slot a block-scoped local leaves behind is reused by `/Ol` loop temps only after another *declared* block at the same level has closed; plain braces and labels do nothing (`0x04036`, still one slot off).
- Under `/FPi` MSC evaluates the product of a float relational first and uses `ficom` for a plain int operand on either side; nothing but `/FPc` gives the original's left-to-right order in `0x049ED`.
