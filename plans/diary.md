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

## 2026-11-03 13:54 CET: 0x081AB reconstructed to a partial, block-placement rules

### What I did
- Reconstructed `0x081AB` (the garage parts screen, 2938 bytes, the last todo) from scratch: 2951 bytes with the body matching except two block-layout points (notes in the DB). Declared `g_49a2`, `g_49ca`, `g_4998[][5]`, `g_4978[][5]`, `g_4988[][3]`, `g_513c`, `g_5148[][6]`, `g_5184`, `g_5186`, `g_5198`, `g_519a`; fixed `F_12F99`'s prototype to two arguments.
- Measured on small functions how MSC places the code after a `switch` (pulled up behind the first `break`, split at its first unconditional jump), when a `goto` label disables `/Ol` on a following loop (only a backward goto), and how a `switch` with `default:` first is dispatched. Rows in the recipe book.
- dagon's VPN name resolution dropped mid-session; its LAN address 192.168.1.70 (same host key) works.

### What I learned
- The install sequence of `0x081AB` is case 0's fall-through: the after-switch code is pulled up behind the first `break` up to its first real `jmp` (a `?:` arm's), the rest sits after the jump table, and the two stub jumps around the table are real code that `./sr show` decodes as garbage.
- A label before a loop stops `/Ol` from optimizing it only when the `goto` is backward in source order. The original of `0x081AB` has a rotated loop and a backward jump into the block before it, which this rule cannot produce: open question, same family as the cross-jump survivor in `0x0C613`.
- A duplicated block that contains a loop is cross-jumped only up to the loop's back edge.

## 2026-11-03 14:12 CET: F_008B4 matched (accumulate inside the inner loop test)

**What I did**
- `0x008B4` (259 bytes, `/Oalsi`) now matches. After 34 variants the only divergence left was `/Ol` keeping `pos` in `cx` across the outer `while (num--)` loop. Measured in `work/bp/ol2` and `work/bp/ol4` (`x2.py` on isolated copies of the function): a goto-form outer loop, a label before the inner loop, `for (;;)` + `break`, a nested block for `pos`, `volatile int pos`, `&pos` after the loops, `continue`, `> 0` and `for (pos += w; pos > 0xbd; pos += w)` all keep `cx`; the goto-form outer loop also turns `/Ol` off for the inner loop (no hoisting). `/Ols` (no `/Oa`) turns registers off in both loops, which showed the original is a per-loop effect, not a flag. The fix is the source shape: `while ((pos += 9 * g_2c01[*ptr] + 3) > 0xbd) { g_pages.start[g_pages.n++] = first - num; pos = 0; }`, the same semantics as the separate `pos += w;` statement.
- Corrected the earlier `0x008B4` row in the recipe book (it described the two-statement form) and added a row for the rule. `./sr check --all`: 554/554.

**What I learned**
- `/Ol` extends an inner loop's register variable to the enclosing loop when the variable is updated by a statement of the outer body; when the only outer-body update is the one inside the inner loop's test expression, the variable lives in memory outside the inner loop and gets its exit store right after it. The guard (`add [mem]; cmp [mem]; jle`) is the rotated first evaluation of that test.
- When `./sr sweep` ranks a flag set without `/Oa` closer, look at what it changes: here it switched the register allocation off in every loop, which said the original's outer-loop difference had to come from the source.

## 2026-11-03: F_03C8D matched (jump table from a split range, `(float)` cast)

**What I did**
- `0x03C8D` (804 bytes, `/Oasi`) now matches. Four changes: `case 0: ; case 1:` in the second switch (gives the jump table), `price1 = (float)price1 * ((rl + 70) / 100.0)` (gives `fild dword; fmulp` instead of `fimul`), `case -4: return;` written first in the first switch (drops a dead `jmp` epilogue block), and `if (nl == -0x15) continue;` (puts the loop increment where the original has it). `./sr check --all`: 555/555.

**What I learned**
- Switch lowering counts *entries*, not labels or values: 3 entries over `0..4` stay a chain whatever the bodies are (the earlier "trivial body gives a table" did not reproduce), 4 entries with a range become a table. A null statement between two labels splits one range into two entries at the same address.
- `(float)` on the integer operand of a double multiply turns `fimul dword [tmp]` into `fild dword [tmp]; fmulp`; no `(double)`/`(long)` cast or operand order does.

## 2026-11-03: F_22B3A matched (`/Ot`, not `/Oa`; `for` with an increment)

**What I did**
- `0x22B3A` (973 bytes) now matches. There were two causes, and only one of them was in the loop. (1) The flags are `/AM /FPi /Gs`, which defaults to `/Ot`, not `/Oa` like `0x22F08`. With `/Oa` ours kept `es:bx`/`al` after a store through a far pointer. The original reloads them (`*r = c; ch = *r;`, `q[1] = 0; r = q;`). The DB note's "everything after the loop matches" was not true under `/Oa`. (2) The loop is `for (; *str; str)`. A `for` with an increment is generated top-tested, and the body tail that jumps back to the increment is moved in front of the test. That gives the original's `[jmp T][render][T: test, front, cols test][cols -= wd]`. The increment emits no code (`(void)0` also matches), so the real one is unknown.
- `tools/sr/match.py`: a non-`/Os` compile ends with an alignment `nop` inside the procedure. The scanner leaves the original's matching `0x90` (0x22F07) out of the size, so `compare` now ignores exactly one trailing `0x90` when the original has it too.

**What I learned**
- Before reordering blocks, check the code outside them under each flag set: a store-then-reload pattern decides `/Oa` vs no `/Oa`, and `volatile` has no effect in 5.1 (small tests, `work/agent_22b3a/t4`).
- MSC moves a comma condition `while (A && (B, C))` back into the body, so a loop condition can't hold the body head. Of the loop forms tried, only a `for` with an increment puts the head after the test (`work/agent_22b3a/t1`..`t5`).

## 2026-11-03 14:35 CET: F_1412D matched (label inside the `then` branch)

**What I did**
- `0x1412D` (276 bytes, `/Oals`) now matches. Source: `while ((kind = p->f0) > 9) { if (kind == 14) { p++; continue; } if (p->x1 > x) { ...; break; } if (p->x2 <= x) { ...; next: if (kind == 12) p = &g_09f4[p->f6]; else p++; } else { y-chain; else { if (kind == 12) { p++; continue; } found; return ...; } goto next; } }`. Variants in `work/agent_1412d/`.

**What I learned**
- A deferred `else` is flushed at the first unconditional jump *after the whole `if` statement*. A jump inside the `then` branch does not flush it. With the shared tail after the `if` (`goto nx` form), the tail's own `jmp test` flushed the y-chain too early. With the label inside the `then` branch, the flush happens after the rotated `kind == 14` jump past the loop test, which is where the original has it.
- A `while` whose body ends in an `if` whose branches all end in a transfer (`continue`/`return`) becomes top-tested (R-series). The `goto next` back into the `then` keeps it bottom-tested.

## 2026-11-03: modules (ported from bak-scratch-area's compilation units)

- New `./sr module` (tools/sr/module.py) groups matched singletons into one source file under `src/mods/`, built as one object with one CL line. The module gate compares the whole span: bytes between functions, near-call displacements, and in-module EXTDEF calls and jump-table offsets, which it resolves itself. Before this, those last two were masked, which is where `MATCH*` came from.
- Same-file evidence: a near call to a near function, or `push cs; call near` to a far one. `./sr module units` takes the hull of each connected group. 35 spans were found. The first one covers 0x00000-0x0e569 (189 functions); that is either one huge main file or near calls across files, so it is still open.
- 30 modules were created, holding 187 functions (53 KB), all verified. Every fully matched span was grouped except the two that contain asm, plus the tail of 0x163be. Every span verified under the union of its members' /O letters, although almost every span mixed /Oas with /Oals (some also /Oasi, /Oalsi). Per-function flags were an artefact of matching singletons.
- Gate pitfalls I hit: (1) MSC emits calls to functions in the same file as self-relative EXTDEF fixups (0000 in the object), so the linker resolves them. (2) A jump-table fixup keeps the target offset in the LEDATA word, not in the fixup displacement. (3) Callers reach some members through a different frame (0e6c vs 0f38 in 0x14c90-0x15236), so a code-segment offset is relative to the frame of the member that holds it.
- Holes: 0x16ADA and 0x16B0A are two 48-byte far C functions missing from the DB (no `push bp` prologue). M_163BE.C stops before them. The spans 0x27b6d and 0x286d8 contain asm-kind functions, so they are not grouped yet.

## 2026-11-03: F_049ED matched (`(float)` cast on a float compare)

**What I did**
- `0x049ED` (1501 bytes, `/Oalsi`) now matches: `if ((float)offer < (float)cash * 0.9)`. `cash * 0.9 > (float)offer` matches too. Tiny tests in `work/agent_49ed/t1`.

**What I learned**
- The `(float)` cast from `0x03C8D` also drives float compares: a cast int operand is an FP subtree, loaded with `fild` instead of folded into `ficom`, so the compare is `fcompp`. When both sides are FP subtrees MSC evaluates the left one first. That closes the recipe book's "FP relational operand order" open question.

## 2026-11-03 14:40 CET: F_1C2CA matched (a reference count in the inner loop decided the outer-loop register)

**What I did**
- `0x1C2CA` (1337 bytes, `/Oalsi`, the CGA text renderer) now matches; `./sr check --all` 558/558. Measured on tiny copies of the text loop in `work/agent_1c2ca/t1`..`t5` (`x2.py` with the function's flags) before touching the source.
- Fixes, in order of effect: (1) the else-loop body `++gp; *p3 |= *gp >> b; p3[1] |= *gp << (16 - b);` became `*p3 |= *++gp >> b; p3[1] |= *gp << (16 - b);`. That alone removed the `di` preload of `i` before the outer loop, restored `i`'s exit stores after each inner loop and the memory guard of the rotated else loop, put `gp` back in memory (`les bx,[gp]`, `es:[bx]` read twice) and dropped the `*gp` CSE temp. (2) `if ((c4 = *s++ - 0x20) < 0 || c4 > 0x5a) c4 = 0;` moves `inc [s]` up to right after the loop test. (3) `col4 += (wid4 = *(gp = &g_cgafont.g[c4].w) * 2);` gives the `mov es,[seg word]; lea bx,[bx+4]` form (same trick as `0x22B3A`). (4) `col4 = bitoff1 = shl;`, `b0 = b1 = bm->bits.p;` (store order), `r6 = 0;` before `s1 = b1; s0 = b0;` in the two byte loops and before `pp2 = buf` in the rotate loop (where the exit value lands), the `movedata` loop counts `i` (its exit store is `mov [i],di`) and tests `(y + i) & 1`. (5) Layout: the byte loops' buffer pointer is a bucket-7 name declared before `dseg4` (`fill`), the `movedata` one a bucket-4 name between `buf7` and `s1` (`from`).

**What I learned**
- The 0x008B4 hint ("where the update lives decides the register's extent") was the right family but the wrong variable here: `i`'s spread to the outer loop was collateral. `/Ol` ranks the inner loop's candidates by reference count; a separate `++gp;` gave `gp` 3 references, it took `di` (a far pointer's offset can be enregistered as `es:di`), and the displaced `i` ended up spread across the outer loop with a miscompile (ours clobbered `di` with `les di`). With 2 references `i` keeps `di` and nothing spreads. So when a variable is enregistered too widely, look at what *displaced* it in the inner loop, not only at its own statements.
- Under `/Ol` a plain `c = *s++ - K;` statement sinks `inc [s]` to after the store; the same expression inside an `if` condition keeps it right after the pointer load. Without `/Ol` both are early.
- A goto-form outer loop turns `/Ol` off for the inner loops too (confirmed again, `t3/T7`); without `/Oa`, `/Ol` enregisters nothing in these loops.
- Four recipe rows added (end of the `/Ol` table).

## 2026-11-03: 0x04036, why the q slot isn't reused (agent_4036, still 18 bytes off)

- Built `work/agent_4036/bat.py`, which batch-compiles a directory of variants in one DOSBox run (about 6 a second) and prints the frame, the loop-temp slots and the locals. With it I searched all 3776 block placements of `j`/`wd2`/`q`/`wd` (`work/agent_4036/e1p`). None gives the original's named slots and its temps together.
- A block holding only `static`/`extern`/`typedef`/tag/prototype declarations doesn't release `q`. A block with several autos keeps one slot from `/Ol` temps after it closes, and it isn't always the first one: `{q; zz}` keeps the unused `zz`. Recorded as an open question in the recipe book.
- Lead: without a named `wd` (the `*q` reads written in place), `q`'s slot is freed and every loop temp lands where the original has it. But the `*q` CSE goes to `si`, plus a hidden slot. So `[bp-12h]` is probably MSC's own CSE temp, spilled to the stack. The spill trigger is still unknown; expression reshapes don't change it. Details are in the DB note.
- Follow-up on 0x04036, what puts a CSE on the stack: only a loop right after the CSE's block (calls don't break this, an `if`/label does) or `si` and `di` both being taken (two `register` locals) does it. Flags and type don't matter, and `#pragma loop_opt(off)` drops the loop temps too. Forcing the spill with register locals brings back the held `q` slot, so the CSE idea doesn't explain the original either. Recipe row added.


## 2026-11-03 15:31 CET: resume 0x04036 (18 to 5 mismatched bytes, still partial)

### What I did
- Read CLAUDE.md, the decompile-function/msc51-recipe-book/wrap-up skills, DB notes, diary, and the earlier work/op/v4036 and work/agent_4036 experiments before changing source. Reproduced the 18-byte baseline.
- Replaced the later named wd2 with repeated table reads. Official ./sr check 4036 now reports 5 mismatched bytes, exact 848-byte size; all five are q/wd stack displacements. Kept status partial.
- Ran scope, name, initializer, expression, bitfield, and loop-shape experiments in work/resume_4036 without swapping the tracked source during the search. Saved the baseline, candidate, generators, build paths, failed hypotheses, and exact remaining offsets in work/resume_4036/README.md.
- Regression: ./sr check --all reports 559/559 still match (work/resume_4036/regression.log). No production tooling or comparer changes.

### What I learned
- The later packed-word value can be an anonymous CSE, and letting MSC make it fixes the entire suffix's stack layout and frame. This is the new lead in .claude/skills/msc51-recipe-book/SKILL.md under Open questions; it is not proof of the complete source layout.
- q/wd still need an extra word ahead of them early without shifting the later temps. Adding dummy autos moves the early locals but breaks later offsets. None were kept. References remain unchecked until a byte match, and isolated near calls remain unverifiable.


## 2026-11-03 15:51 CET: 0x04036 matched (all three apparent locals were CSEs)

### What I did
- Reconstructed all six packed-word reads directly as g_7d88[idx].f2 and restored the four early node assignments to ternaries. Removed the q/wd nested blocks. The function now has only kind, car, node, w, i and j as locals. Official check: MATCH*, 0 mismatches, 848/848 bytes, 27 refs verified.
- Tested all 16 combinations of early ternary/if statements with no q/wd locals. Only masks 14 and 15 match (the first node assignment can be either form; the later three must be ternaries). Retained the consistent all-ternary form, work/continue_4036/allcse/T015.C.
- Verified the six actual near-call encodings in a scratch build with the real contiguous neighbours, at their original relative offsets: target MATCH, 27 refs verified, six exact E8 instructions. The context preserves 0x03C8D's recorded non-/Ol setting with a local pragma for validation only; no production module or flags changed. The broader module probe still has an out-of-target F_03AB8 placeholder, so it is not claimed as a whole-module match.
- ./sr check --all: 560/560 still match. Logs and reproduction: work/continue_4036/README.md, target.log, regression.log. Updated the recipe book and decompile-function skill; the prior partial-result entries remain historical.

### What I learned
- The prior assumption that bp-16 and bp-18 represented q and wd was wrong. Both are compiler-generated CSEs, like the later bp-14 value. Reconstructing just one temporary at a time hid the source shape; all three and their surrounding conditional-expression region had to change together.
- A correct instruction sequence does not prove a named local, and an equivalent if/else can break the CSE region even when the conditional instructions themselves match. See the new C-shapes row in .claude/skills/msc51-recipe-book/SKILL.md.
- The original source's exact names and first conditional spelling are not recoverable from this match. The earlier nested-block allocation anomaly is a separate compiler observation, no longer a blocker for this function.

## 2026-11-03 16:31 CET: 0x0AB8B race loop, /Ol scope measurements

**What I did**
- Measured why the race loop in `0x0AB8B` gets `/Ol` in ours and not in the original (variants in `work/agent_ab8b`). Another session matched the function in the meantime with `test:` before the `while` and `goto test;` at the end of the body. I left that version installed.

**What I learned**
- `loop_opt` pragmas work per function, never per loop, and a pragma just before the closing `}` is ignored. `/Ol` drops a loop that has too many branch blocks (two extra ifs were enough) or a jump into it. String-literal addresses follow source order, so they show whether a loop test was written before or after the body. Rows in the recipe book.

## 2026-11-03 16:33 CET: 0x0C613 block order (deferred else flush, cross-jump survivor)

**What I did**
- Took `0x0C613` from 123 to 37 mismatched bytes. Adding `goto done;` at the end of the list-building else, with `done:` before `F_13F21(1)`, made that else a codegen-deferred block. It is then emitted right after the `goto ret2` jmp, as in the original (`work/agent_c613/l2.C`). During my rate-limit pause the resume agent added the `goto finishstop` arms on top of l2 and matched the function (MATCH*, refs verified). `./sr check --all`: 562/562.
- Measured in small tests (`work/agent_c613/t1`..`t5`, `s1`..`s5`) what flushes a deferred block and which cross-jump copy survives. Recipe-book rows added under Block placement.

**What I learned**
- There are two kinds of deferral. Codegen deferral (an else ending in a jump) flushes at any unconditional jump after the whole if, including a conditional goto's jmp, a ?: jump, an if/else jump and a switch jump. Post-pass deferral (an else that only ends in a jump after cross-jumping) flushes only at a return's jmp, because folding has already turned `if (c) goto L` into `jcc L`. The earlier rule "goto never flushes" described only the second kind.
- When the cross-jump survivor looks wrong for the arms of an `if/else if`, give the branch its own end label. The inner if's end is otherwise the outer chain's end, every copy jumps there, and the first one wins.

## 2026-11-03 17:36 CET: Resuming remaining functions — active checkpoint

### What I did
- Recovered previous agents' source attempts and read project instructions and skills before continuing.
- Matched 0x0AB8B (1210 bytes, 133 refs, MATCH*) and 0x0C613 (1948 bytes, 165 refs, MATCH*). Regression after both: 562/562; work/resume_remaining/regression-c613.log.
- Reduced 0x081AB to four stack-temporary bytes at exact length2938 and corrected initial g_829a indexing. It remains partial, never marked matched. Diagnostic references:190 verified,10 singleton jump-table offsets unchecked,none wrong.
- Preserved all inherited work. No compiler flags, comparison masks, or tooling gates changed.

### What I learned
- Both successful control-flow patterns are in .claude/skills/msc51-recipe-book/SKILL.md; evidence and current 081AB mismatch details are in work/resume_remaining/README.md.
- Unrolling the first slot scan diagnostically restores later temporary offsets; removing individual jump paths does not. A named register scan pointer occupies bp-472 and produces the same four mismatches, so the extra bp-474 slot arises later. Exact cause remains unresolved; work continues.

## 2026-11-03 17:56 CET: All three inherited partial functions matched

### What I did
- Finished0x081AB:2938bytes,zero mismatches,190 references. Together with0x0AB8B and0x0C613 this resolves all three inherited partials. Full regression:563/563 (work/resume_remaining/regression-final.log).
- Verified081AB in an actual-neighbor build spanning076A7..081AB:5758-byte module gate passes; target verification checks all6 near-call displacements and10 jump-table offsets. The span's one outside placeholder call belongs to079EB. Evidence: work/resume_remaining/verify81-target.log.
- Refreshed progress, DB notes, recipe book, and work/resume_remaining/README.md. Preserved inherited changes; no flags, masks or production tooling changed.

### What I learned
- Shared removal belongs after the outer switch. The empty scan body must be continue rather than a semicolon: this lets the first switch break pull the complete loop exit and cleanup into position. Putting the shared block directly in case13 gave matching instructions but wrong temporary homes. See the new recipe-book entry.
- Corrected an earlier inference: the post-switch scan was already loop-optimized; its exit was displaced. Auditing saved outputs for frame size and actual loop instructions exposed the useful alternative. All1380 saved081AB compiler outputs had been no better than four bytes until this final variant.
- AB8B and C613 have official MATCH* results; their outside near-call displacements remain outside singleton verification. No broader module-match claim was made for those two.

## 2026-11-03 18:02 CET: emu2 compile backend (ported from ../msc51)

### What I did
- Ported msc51's emu2 backend: `tools/sr/emu2.py`, the `SR_EMU` switch in `tools/sr/dosbox.py`, `tools/nix/emu2.nix` (upstream pin + flush-before-fork patch), `tools/scripts/emucmp.py`, and the nix PATH cache in `./sr`.
- `./sr check --all`: 563/563 under both backends; 13 s with emu2 (first run, including the nix build) against 45 s with DOSBox-X; a single `./sr check` now takes 0.9 s.

### What I learned
- msc51's equivalence run already covered all 725 streetrod sources: OBJ/COD/CC.LOG byte-identical under emu2 and DOSBox-X. bak-scratch-area's findings (DOSBox tuning doesn't help, qemu+FreeDOS slower) are in the msc51 diary.

## 2026-11-03 18:18 CET: finish source grouping and repair scanner coverage

### What I did
- Assigned every real game function to `src/mods/`: 506 C + 66 assembly = 572 functions in 147 compilation groups (95 multi-function, 52 standalone). Existing work was preserved; no commit or reset.
- Added bounded gap discovery to the scanner. Recovered and byte-matched nine game routines at 02DB0, 0FEAD, 0FEBF, 1636C, 16ADA, 16B0A, 1D080, 1D0A4, 23F4D. Two new runtime entries stay skipped; all 147 MSC runtime entries remain unreconstructed. Corrected eleven stale C/ASM classifications through the CLI.
- Added homogeneous MASM module support, local-label namespacing, multiline STB handling, covered-retired-row validation, and correct handling/counting of external placeholder publics in the module gate.
- Final full regression: **572/572**, all 147 whole-span gates pass. Eight scanner/module regression tests pass. A subsequent scan finds no new routines. Refreshed docs/PROGRESS.md and added docs/MODULES.md with complete inventory and verification limits.
- The skipped 0F8C2 false-positive row remains as history, wholly covered by F_0F7A5. It is not reconstructed or counted among the 572 game functions.

### What I learned
- Earlier “all functions matched” meant all recorded functions; the gap audit exposed missing scanner coverage. Prologue-free, unreferenced routines need bounded control-flow discovery, with data and malformed gaps rejected.
- A near-call hull is not proof of a single original C file: the first candidate includes MASM and also exceeds the MSC compiler heap. Flag effects and alignment can require smaller contiguous groups. Grouping is complete, but original filenames/file boundaries are not claimed.
- **198 external near calls still require linked placement verification.** Source grouping and passing module gates do not yet establish a fully linked byte-identical executable. See docs/MODULES.md; the gate now reports external EXTDEF placeholder calls honestly.
- Lessons recorded in .claude/skills/sr-cli/SKILL.md and CLAUDE.md. Scratch baselines, proposals, final regression and tests: work/group_modules/.

## 2026-11-03 19:07 CET: Reconstruct data and build the game with MSC

### What I did
- Added the host Makefile and ./sr build, with prepare-only and objects-only modes. Generated ENV.BAT, BUILD.BAT, MSC MAKEFILE and LINK.RSP in work/game. Compiled all 147 code modules and 12 data modules and linked the original MSC libraries without comparison-only STB definitions.
- Reconstructed initialized globals, literal strings, pointer tables, car-mask Run/List and Part records, fonts, sound streams, Bitmap descriptors, track points, Slot and terrain storage as MASM source under src/data. The user explicitly chose source reconstruction instead of reusing original EXE sections. Normal builds never extract data from the EXE.
- Staging exposes real near interfaces in four shared code segments, maps F_0066F to main, and binds three runtime globals to errno, _doserrno and _amblksiz. Corrected the M_2487A assembly call's external spelling to its actual in-module public name. Updated the unsigned _amblksiz declaration through ./sr global.
- Added data authoring/audit tools and build tests; documented usage in README.md, CLAUDE.md, src/data/README.md and the sr-cli skill.
- Validation: 159/159 objects compile, LINK has zero unresolved symbols or errors, 572/572 function checks and all module gates pass, show/diff 24ac7 passes, 11 tooling tests pass, and all 482 MSC sizeof/offsetof checks pass. Linked data verifies 36,610 DGROUP bytes, 391 near-pointer entries, 191 far pointers and 11 far segments. The whole-span gate still reports 198 external near calls; the runnable link does not prove the original layout.
- Built once through the host Makefile and again from clean objects through MSC MAKE 4.07 in DOSBox-X. Both produce SHA-256 b78b35442379c34da7230622da32487dbd87cf00a8576e1990bf971534004331. The same EXE was smoke-tested in DOSBox-X through the correctly rendered video menu, VGA title screen and retail manual-color protection screen. Full gameplay was not tested.
- Executable: work/game/SR.EXE (306654 bytes). Logs, native-build comparison and screenshots are in work/game-build, work/msc-make and work/game-run. These changes remain uncommitted.

### What I learned
- See the sr-cli skill's linked-build lessons: MSC MAKE is a sequential dependency processor, emu2 cannot run this LINK workload, and CL requires serial compilation in a shared directory.
- Successful linking was insufficient: MASM EQU exports lose their PUBDEF group, making C accesses use the wrong data base. Real LABEL exports with ORG preserve overlapping views and DGROUP association; near initializers also need an explicit DGROUP frame. The audit now verifies the public group as well as all data bytes and pointer targets.
- Original DS:629C..6C21 belongs to runtime storage, and unpacked FF filler above the BSS boundary is not initialization. The original CGA font prefix overlaps the last EGA glyph's raster bytes; the renderer skips that prefix. Details and reconstruction provenance are in src/data/README.md.
- A working linked executable is now available, but its layout and cross-object call encodings differ from the shipped executable. A whole-executable byte match remains separate work.

## 2026-11-03 20:39 CET: Linked code layout recovered; whole-file data reconstruction continues

### What I did
- Read ../bak-scratch-area/CLAUDE.md and the July 4–5 journal/endgame plans, including the independently verified 453904-byte cmp result. Applied its raw-file/relocation-order/data-ownership workflow, not its Borland-specific mechanics.
- Added ./sr linkcheck and tools/sr/linkmatch.py: unmasked whole-file, MZ-header, load-image and relocation-order comparisons, plus mapped game-function and near-call diagnostics. It returns failure while any file byte differs. Regression tests reject a changed relocated word, row-order permutations and extra bytes.
- Added ./sr build --layout, tools/sr/linklayout.py, src/link/LAYOUT.JSON and CRTORDER.ASM. The plan compiles 103 code units plus 12 data modules and an empty segment-order anchor, and selects 100 untouched MSC/EM archive objects. Every source unit is compiled and gated before linking; preparation never extracts game bytes from the original. Native MSC MAKEFILE/ENV.BAT/BUILD.BAT are also emitted.
- Latest maintained layout build: 116/116 source jobs compile and pass gates; no link diagnostics; **572/572 game functions at original addresses; 748/748 near-call encodings byte-exact**. All existing function checks still pass, 572/572. Seventeen tooling tests pass. The formerly outstanding 198 module-external near calls are verified by this linked placement, while the individual module gates retain their scope counts.
- Whole-file status remains RED: 38567 differing load-image bytes and 12732 extra load-image bytes; linked file has 2215 MZ relocations versus 2218 original. Report work/layout/BYTE-MATCH.JSON. MZ header differs by 304 bytes of size as well as contents; target remains orig/SR_unpacked.EXE until packing is reconstructed too.
- Fixed a real startup bug hidden by false library aliases: F_005C6 now uses signed long division by 95 and 10, not unsigned remainders. The original helper is __aFNaldiv. Library matching now verifies far/near call targets and data-table identities, includes tiny jump shims/no-public code members, and excludes incidental matches contained in larger members. crtnames refresh removes stale aliases at reidentified addresses.
- OMF now retains DATA publics and full member bytes, handles COMDEF/LCOMDEF external indices, and selects a nonempty named code segment after an empty MASM default. New library/parser tests cover the division wrapper, sin/sinh distinction, common indices and named segments.
- Real builds retain near declarations from STB scaffolding without definitions. Combined C uses MSC's loop_opt pragma, not unsupported modern optimize pragmas. Linked source groups recover the short far-call encodings and avoid extra word alignment. Harmonized the _g_47e8 assembly type with explicit BYTE PTR accesses where necessary.
- Corrected far-data interpretations: CGA font is a glyph array at 0ED2:0004, WORD aligned after the full EGA font, with no fake header or overlap. Changed the DB through ./sr global and the C reader, preserving its byte match. PARTS2 ends at 3884:0000; the next 0x170 bytes are actual EM.LIB EMULATOR_DATA, followed by SLOTS at 389B. Removed three blitter ORGs that duplicated low paragraph offsets; their gates still pass.
- Added inspect_layout_data.py. All 94 numeric constant operands currently equal the original bytes. Compiler DATA/CONST ownership is recorded in work/layout/DATA-OWNERSHIP.JSON.

### What I learned / active next work
- Data remains the substantive work. DGROUP.ASM currently duplicates compiler-owned literals, reserves runtime intervals, and emits uninitialized game storage as initialized zeros. Those must become actual C-owned initializers/pools, real runtime data, and BSS/COMMON storage. Do not patch objects, copy original sections or mask the differences.
- First DATA pilot is solved in work/binary-match/menu_probe.py: defining g_videoMenu with six actual string literals just before F_00082 produces the correct menu string-pool/table order. Add the three initialized flags g_0250/g_0252/g_0254 before F_00226 to reproduce 004E..0262 exactly; original game-state words 0042..004E belong before this C contribution. The initializer is not integrated yet. C data can be emitted at declaration/function boundaries, so multiple inferred pool bases do not automatically mean multiple TUs.
- CONST: F_0DCBE and F_0E218 share the original numeric pool at 6988. Their two current source units need combining in the layout plan to avoid 128 duplicate bytes (not done yet). The original 68E4/68E6/68E8 words are three 2E47 segment constants; only 68E4 is currently referenced by the reconstructed track-array expressions. Investigate ownership, do not invent filler.
- Runtime-data forensics (work/binary-match/runtime_data.py/json): _bufout is 7326..7526, _bufin 7B16..7D16, _buferr 8BD2..8DD2. The original game private BSS appears to be 6C22..6E2C (522 bytes), then 50 bytes of real library BSS ending at 6E5E (cvt's private pointer at 6E5C), then paragraph-aligned COMMON at 6E60..8F02. Stack starts 8F10 and its real saved EXEPACK header specifies SP=A00, SS=4787. This explains the apparent runtime/game BSS overlaps: the variables live in interleaved COMMON allocation after private BSS.
- LINK symbol hash is now recovered from its NE segment 2:4060 and independently agrees with all 45-name actual LINK allocations, forward and reverse declaration input: hash16 = length + sum((ASCII byte | 20h) << (1-based position & 3)); bucket = low byte. Allocator enumerator at NE 2:3FEE walks 256 buckets. Bucket chains insert at head. Buffer buckets: __bufout=43, __bufin=102, __buferr=233. Helper tools/sr/linkhash.py. MASM itself emits COMDEFs in its own symbol order, so source declaration reversal alone does not change those records; inspect actual OBJ communal order when handling collisions.
- Historical global names cannot be recovered directly. As for the already load-bearing auto-local names, any reconstructed COMMON names must reproduce the observed hash/allocation order, with that limitation documented. Do not claim placeholder names are historical names. No name-selection or BSS conversion has been implemented yet.
- COMMON inventory work/binary-match/common_inventory.py/json: 244 candidate storage clusters / 262 views, plus some unreferenced storage gaps. Several DB arrays are overlapping views, not separate allocations. g_7316 has four used pointers ending exactly at 7326; the earlier authoring bound 14 is wrong and still needs updating. g_7312 overlaps those pointer views and has a segment-half view g_7314. Refine aliases/sizes before emitting COMM.
- NE disassembly tooling and all probe ledgers live in work/binary-match. Do not use the first failed links as proof: link-probe-2 failed on conflicting EXTRN widths, and libraries-1/2 had duplicate/unresolved runtime objects before the maintained build succeeded. Current maintained baseline is maintained-layout-1.log. Changes remain uncommitted; unrelated pre-existing pycache changes and files =, bits.p, f are preserved.

## 2026-11-04 00:10 CET: DGROUP ownership recovered from positions; data definitions move into the modules

### What I did
- Found why the linked DGROUP never matched: the original's `_DATA` is laid out per translation unit in *object order*, and that order is not the code order. Objects from the two big named code segments (regions 0000 and 0F38) were interleaved on the link line; the literal pools of each unit pin it (`1169A`'s strings sit at DS:0490, before `00D58`'s at DS:0614). Code order per segment is preserved, so the merge is recoverable.
- Measured MSC 5.1's data emission (work/dg/cc/T*.C): string literals go out immediately in parse order and are never merged; named initialized data is buffered and flushed after the next function's literals; scalars `char` are not aligned, everything else (arrays, structs, pointers) is word-aligned; `static` uninitialized goes to `_BSS`, other uninitialized globals are COMDEFs. `/Zp1` does not change this.
- Wrote `tools/sr/dgroup.py` (`./sr dgroup report|problems|order|register|aliases|apply|asm`): items from the DB and the literal pools of the layout objects, per-unit ownership by position (runs between pools, sole referrers, forced owners for assembler data and data-only units), declaration slots, C/MASM definition text with symbolic pointers, source insertion marked `/* DS:xxxx */`, the remainder as `src/data/DGROUP.ASM`, the recovered `link_order` in `src/link/LAYOUT.JSON`. `tools/sr/datacheck.py` gates a module's own data against the original (base from the code's references, bytes, every data fixup resolved); the module gate reports it, strictly once a module defines data.
- Converted every unit with data below DS:09F4 and above DS:4AFE (prefix and tail), the assembler units' own data, the keymap/pointer table block as assembler data (`FORCE_OWNER`), and the car-mask Run/List tables as a data-only unit `src/mods/CARMASKS.C`. Rewrote 23 overlapping DB "aliases" as views of their base arrays (`g_02be` -> `g_02b4[5]`, asm `_g_4766+14`). Fixed `struct Style` (fifth colour bank, 80 bytes), `struct Blt12` for the clip record with four scalar clip globals, `g_525a[16]`, the TRACKPTS/PARTS far symbols (`g_parts1`, `g_parts2`), and three functions that used named strings as literals (`g_28a1`.., `g_3a5a`, `g_07be`).
- Result: `./sr check --all` 572/572, `./sr build --layout` 119/119 gated, 572/572 functions at original positions, 748/748 near calls, DGROUP byte-identical from DS:0042 to DS:09F3; load image differences 38567 -> 36095 (the untyped middle tables still sit in DGROUP.ASM at the end, shifting everything after DS:09F4). Nothing committed.

### What I learned
- Data positions are a source-recovery tool: `g_07be` ("   ") referenced twice at one address is a named array, not two literals; F_0C613's literal order (" " before "") only comes out with `if (...) goto pay;` ahead of the fined branch; `strcpy(buf, "\0")` is a two-byte literal.
- A unit's frame layouts change with the number of file-scope definitions in the unit (40 dummy `int zz_n = 0;` break F_2969D, 10 do not), so a function that matched without the tables proves the tables were in another file: the car masks are a data-only TU.
- A pointer table at an odd address (DS:579F) cannot come from the C compiler: assembler data. A struct member access compiles differently from a scalar global (`g_clip_ymin` vs `g_5e42.fe`), so the clip limits are separate globals after a 12-byte record.
- Runtime data differs from our MLIBCR in a few bytes (crt0dat's initialized `_osfile`); the uncracked TGOD disk EXE has the same bytes, so that is the developers' library build, not the crack.
- Remaining for a whole-file match: the DS:09F4..4AFE tables (hotspot/menu records, message block, car tables) need typing and owners; `_BSS`/COMMON names for the hash order; the CONST duplicate of 0DCBE/0E0E2; the runtime data-only modules' pull order; EXEPACK.

## 2026-11-04 01:30 CET: Load image byte-identical; packed EXE 331 bytes from the shipped file

### What I did
- Assigned the middle of DGROUP: DS:09F4..239D is a data-only C file (`src/mods/HOTSPOT.C`), DS:239E..4717 assembler data (`src/mods/MESSAGES.ASM`: odd-address ints and arrays, count-prefixed message records), DS:4718..4AFD data of assembler unit 0x02B9A. Unreferenced even text in a C unit becomes a named array; `./sr dgroup apply all` now strips and rewrites every generated definition.
- Merged layout units the evidence joins: 0542E+05B4A (05B4A's pool starts at an odd address), 0DCBE+0E0E2 (shared constant pool), and per the original relocation order {00000..005C6}, {02FC8, 032F4}, {0542E..0641E}, {0914C, 09256}, {09524, 09AF4}, {0B3CA, 0B456}, {10B7C, 11324}, {113A2..119D6}, {1923C..192AC}, {1947A, 1956E}, {230DC, 23122}, {2869E..2B6CA}. Moved F_00D58 into `M_008B4.C` and made 00D78..02B99 one object (F_01FE2 near-calls into M_01BAE).
- Split the car masks into two files (the table at DS:6078 precedes the second set) and wrote each list descriptor before its run array, which reproduces the original's one record per array.
- Fixed the storage objects: GBSS/GCOMMON exported their labels without DGROUP (`ASSUME DS:DGROUP` was missing); COMMON is linked after the runtime (LINK creates communal space after crt0's XOB/XO/XOE); the far SLOTS storage is now far communal C data (`src/data/SLOTS.C`, FAR_BSS), so the emulator objects can sit after `fltused` as in the original.
- Runtime order from the original data: `_cflush`/`_file` after `dosret`, `cmiscdat`/`ctype` after `amalloc`, `days` after `getch`, `timeset` after `tzset`, `huge` after `87cdisp`, `fixups` before `matherr`. Declared segment lengths count (ctype +128, nmalloc +10, cfin +18, cfout +25 bytes of `DUP (?)`).
- Typed the three video-driver hook tables as far function pointer arrays owned by the C driver files 1C2CA, 22364 and 22B3A, made F_24BE2 load `SEG _g_font`, added a trailing `EVEN` to M_0E926.ASM, and gave seven table targets real prototypes.
- Result: `./sr check --all` 572/572, layout 104/104 gated, load image identical up to BSS (the 14 extra bytes in the unpacked reference are unpacker filler), 2218/2218 relocations, `/EXEPACK` output same size and header as `orig/SR.EXE`, 331 bytes differ, all in the packed relocation table's order. `./sr build --layout` now prints that comparison.

### What I learned
- MSC 5.1 data rules (all measured): no packing flag places an int or array at an odd address; an initializer that takes the address of a later definition in the same file makes MSC emit that definition first (one record each); a far access the optimizer removes still leaves its SEG constant, but a repeated symbol shares one; every call to a function defined anywhere in the same file becomes `push cs; call near`, so a far call between two functions proves two files.
- LINK 5.01 lists relocations object by object, each code/data record's fixups in descending offset (MASM ascending). That gives the original object boundaries (records of ~945 bytes from the object start) and the link order, independent of code addresses. Far communals are allocated in the same hash-bucket order as near ones; the far storage names `g_slot`, `g_terrain0..4` are placeholders chosen for that order.
- Provisional source: two unreachable far accesses in F_0F4B7/F_0F62E (`x_trk0`, `x_trk1`) stand for two SEG constants nothing reads. Their statements and symbols are unknown; marked PROVISIONAL in the source.
- Remaining: 00D78..08D25 relocation order wants M_07367.C in the same object as M_07800.C, but then F_07AA4's register allocation changes (its source already carries a dummy local for a frame hole). Header size is not the cause (trimmed headers change nothing). Then the packed file should be identical.

## 2026-11-04 03:00 CET: `./sr build --layout` reproduces orig/SR.EXE byte for byte

### What I did
- Made 06476..08D25 one object as the relocation table requires. F_07AA4 then lost its loop register variables. Bisected to a compiler budget: MSC 5.1's code generator degrades (here: no `/Ol` register promotion of the inner loop) once the external symbols referenced so far in the file exceed a budget measured in symbols plus name characters. Locals, statics, unused prototypes, literals, initialized data and defined functions do not count; external functions, external data and communals do.
- Brought the object under budget with source structure the evidence supports: two COMMON clusters became structs (`struct State7EAA` over DS:7EAA..7EC1, `struct State8272` over DS:8272..828F; a wider 8272 struct changed F_081AB's loop temps, so the arrays from 8290 stay separate globals), and the four strings F_0711A uses are offsets into the message block (`g_239e + n`). Folding the whole message block was wrong: F_0B045 computes `g_28ec - g_239e` at run time and F_13xxx index `g_2bad[]` directly, so those are separate symbols.
- Record cuts: MSC ends a code record at ~944 bytes or at 99 fixups. Matching the original cuts made 0914C..0B3C9 one object (M_0914C, M_09256, M_09524, M_09AF4, M_0A416, M_0B045). A detour that moved F_0A35C into M_0A416.C was wrong (MSC pads an object ending at an odd function length; the original has no gap there).
- Result: the /EXEPACK link `work/layout/SRPACKED.EXE` is identical to `orig/SR.EXE` (SHA-1 3a5e28b795313028b7e77d147122f41cb29de334), also from an empty work directory. `./sr check --all` 572/572, 101/101 layout jobs, 17 tests pass.

### What I learned
- Global names and the number of distinct external symbols are load-bearing, like local names: a function can match alone and fail in its real file. When a merged file changes a late function's register allocation, first count the external symbols before it (`work/dg/a7/try.py` style bisection: replace bodies with declarations, then add dummy externals of different name lengths).
- MSC code records: cut at 99 fixups or ~944 bytes; with LINK's per-record descending relocations this pins object start offsets.
- Open: the two PROVISIONAL unreachable far accesses in M_0F38A.C (unrecoverable statements), the struct member names (address-based placeholders), the far storage names, and COMMON still comes from explicit labels (GCOMMON.ASM) rather than C communals in hash order.

## 2026-11-04 05:00 CET: COMMON storage from per-file communals

### What I did
- Replaced the explicit labels of `src/link/GCOMMON.ASM` (DS:6E60..8F02) with C tentative definitions in the file that first mentions each block: 243 definitions in 39 files, plus 14 unreferenced blocks as `char` communals in HOTSPOT.C.
- Chose names that make LINK's allocation reproduce the original addresses. A dynamic program over the hash buckets kept 6 names, gave 165 a same-length variant and 86 a longer one; only symbols first created in different objects may share a bucket. 237 globals renamed in the DB, sources and stubs.
- Fourteen typed views overlap the next symbol (for example `struct Panel ai70e4t[]` with 4 bytes before `at70e8`). They stay as exact-size `COMM NEAR` records in GCOMMON.ASM. The odd 2-byte gap at DS:8245 became two 1-byte communals.
- Lint now accepts function-pointer variable definitions; `./sr dgroup apply` keeps a data unit's communal block.
- Result: `./sr check --all` 572/572, 17 tests, `./sr build --layout` IDENTICAL to orig/SR.EXE from an empty work dir.

### What I learned
- LINK 5.01 communal order: hash bucket ascending, within a bucket newest-created first; creation is the first mention of the name in link order, so an EXTDEF in an early object fixes the slot and the COMDEF may come from any later object.
- The 14 overlapping views are the next structural question: each marks where our symbol split differs from the original (a struct or array spanning several of our globals).

## 2026-11-04 07:00 CET: COMMON views folded into the original arrays and structs

### What I did
- Resolved the 14 overlapping views from the previous entry. The save-game table (DS:4EA4 pointers, DS:4EBE byte counts) gives the original blocks: 22 racers at DS:7FF8 (396 bytes), 26 specs at DS:7D86, 26 models at DS:8DF0, 26 panels at DS:70E4, and 48 bytes from DS:784A.
- Rewrote the views as accesses to those blocks: `at70e8[i].x` became `ai70e4t[i].r[1].x`, the `Dir` view became `g77bal[i].nxt[k]` (12 frames at DS:77BA), the `Rec10`/`Ent10` views became `Spec` fields (its byte at +3 is `next`), and the `Rec18` view and the last racer's loose fields became `Racer` fields. The `Pos6` table now starts at DS:784A and is indexed `[k - 1]`; the table entry is `&a784als` instead of `(char *)&g7844n + 6`.
- Folded loose fields into their structs: DS:78EA..7905 and DS:799C into `e_78e0`, DS:7A9E into `g79e2g`, DS:8240 into `r_vid.f16`, DS:8244 into `x_colors.c[2]`, and 7314..7325 into `bp7316[4]`. The 2 bytes after the model table are a `char` communal in HOTSPOT.C, named to land in the same hash bucket.
- One byte stayed wrong: `ac7d30z > e_78e0.fa` loaded DS:7D30 into AX, the original loads DS:78EA. MSC keeps the more complex operand in memory, so DS:7D30 is a struct member too. Folding DS:7D2E..7D3F then broke comparisons with DS:7E8A..7E8F and an addition with DS:70C0, the same way; folding those runs as well (`State7D2E`, `State7E8A`, `State70C0`) made everything identical.
- src/link/GCOMMON.ASM is deleted; COMMON is now entirely C.
- Result: `./sr build --layout` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- Operand order is evidence for declarations: in a two-memory-operand comparison or addition MSC loads the simpler operand; ties load the right side of a comparison and the left side of an addition. Every place our "plain" global sits against a struct member is a test of whether it was a member.
- Merging two address-adjacent communals never moves anything else, because adjacent blocks are adjacent in LINK's allocation order; only the merged block's first mention can change, which the bucket check covers.
- `State70C0` looks like two 16-byte records (an int, six ints, an int) for player and opponent; it is still a flat placeholder.

## 2026-11-04 08:30 CET: Per-file headers instead of GLOBALS.H / PROTOS.H

### What I did
- Each original source file now has its own header in src/include: one per link unit (named after its first source, e.g. `M_0914C.H` for M_0914C.C..M_0B045.C) and one per standalone data or assembler file (`SCREENS.H`, `GBSS.H`, `MESSAGES.H`, ...). 100 headers.
- A header declares what its file defines: the owner of each global, far datum and far function is found from its definition (C file-scope definitions, MASM `PUBLIC`s); the declaration text still comes from the DB.
- Every C source's include block is generated: `STRUCTS.H`, its own header, then the headers of the files it uses. `./sr headers` rewrites the blocks; any header regeneration refreshes the headers (new `tools/sr/fileheaders.py`).
- The game now reads `errno`, `_doserrno` and `_amblksiz` by their real names through `<stdlib.h>` / `<malloc.h>`; CRT.H's `#define g_630d errno` mapping is gone. The reference checkers compare global names without leading underscores.
- MAKEFILE dependencies list each job's actual headers; the dgroup data-unit writer keeps a file's include block; lint messages point at the defining file's header.
- GLOBALS.H / PROTOS.H remain generated for the check scaffolding only (.STB stubs, new src/funcs files).
- Result: `./sr build --layout` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK, `work/dg/run.sh` regenerates to IDENTICAL.

### What I learned
- The include lists do not affect code: unused declarations cost nothing in MSC's external-symbol budget, as expected.
- The plain `./sr build` (all-files game build, not the layout link) has not linked since before the DGROUP work: at a497236 it had 368 unresolved data symbols, now 158 (the private BSS at DS:6C22..6E2C and other data only the layout link provides). `./sr build --layout` is the build that matters.

## 2026-11-04 10:30 CET: File-local `_BSS` storage as C statics

### What I did
- Replaced src/link/GBSS.ASM (labels for DS:6C22..6E2C) with `static` definitions in the sources: 103 blocks in 18 link units, every one used by a single original file. The per-unit blocks follow link order, so each object's `_BSS` is its own run.
- Measured MSC 5.1's order of uninitialized statics in `_BSS` with scratch compiles (135 names, both declaration orders): key `sum(byte & 0xDF) mod 256` ascending, ties newest-declared first. Not declaration order, not LINK's hash.
- Renamed 43 statics so each file's keys increase with address (suffixes on the placeholders, as for COMMON). Sized `g_6c72a[16]` and `g_6c8aaav[12]`; the 4 unreferenced bytes after `g_6c40` are a PROVISIONAL `static int g_6c42[2]`.
- Tooling: `cod.statics()` reads `_BSS` names and `ORG` offsets from the listing; refs and datacheck resolve `_BSS` fixups through it (any static may be the base, as in `a[i - 1]`); headers and GLOBALS.H skip file-local statics; `module.merge` hoists and dedupes a unit's shared static lines; the layout link no longer assembles GBSS.ASM. The M_0B456.STB placeholder for F_0B425 no longer writes a static.
- Result: `./sr check --all` 572/572, `./sr build --layout` IDENTICAL from an empty work dir, tests OK. The plain `./sr build` is down to 55 unresolved symbols (from 158).

### What I learned
- MSC's `_BSS` order for statics is a 256-bucket table keyed by the name's byte sum with bit 5 cleared; a static declared after an `extern` of the same name (C4074) becomes public `_name` instead of `$Snnn_name`, so headers must not declare statics.
- Open: `work/dg/run.sh` no longer round-trips, also on 0aef06c: `./sr dgroup apply` moves `g_4fa6` from M_0542E.C to M_04940.C (then U_0542E fails to compile) and adds a blank line to six .ASM files.


## 2026-11-04 11:30 CET: What stands between us and a faithful build

### What I did
- Read the build pipeline end to end and listed every step the developers could not have had: extracted runtime members in a recovered order, the CRTORDER anchor, generated `U_*` files merged from the `M_*` split, injected `#pragma loop_opt`, stripped `static` on near functions, prepended CRT.H and STB prototypes, `#define F_0066F main`, flags as a union of per-function flags. The plan is `plans/faithful-build.md`.
- Linked the layout objects against the libraries instead of extracted members (work/nat). Component libraries in LINK's list: 55876..61916 bytes differ. A combined library built with LIB 3.11 as `MLIBCE = MLIBCR + LIBH + EM + MLIBFP`, linked as the objects' default library without `/NOD`: packed EXE IDENTICAL. Other component orders: 5381, 5389 and 12426 bytes.
- Without CRTORDER.OBJ the link differs in 57336 bytes. Without the injected loop pragmas U_00D78, U_039C0 and U_23262 stop matching.

### What I learned
- The developers linked against a SETUP-style combined MLIBCE.LIB with the component order MLIBCR, LIBH, EM, MLIBFP; the "recovered runtime order" was just the order LINK extracts members from it.
- Three units need the injected `#pragma loop_opt` with our current source; the other nine whose sources differ in `/Ol` do not. That is not proof the originals had pragmas: those functions may match under the file's one flag set with a different source formulation (correction from the user).

## 2026-11-04 12:00 CET: GLOBALS.H and PROTOS.H removed

### What I did
- Deleted the two shared headers; the original most likely had one header per file, which the sources already use. They were only left for the `.STB` stubs and the `./sr new` template.
- Stubs include `STRUCTS.H` (they are prepended to a module that includes its own headers); six stub bodies that wrote the communal `g7f82r` are now empty (`{ }` / `{ return 0; }`); bodies are placeholders and never reach the executable.
- `./sr new` writes `#include "STRUCTS.H"`; `src/funcs` files get their include block from `./sr headers` like any other file. Docs and skills updated.
- Result: `./sr check --all` 572/572, `./sr build --layout` IDENTICAL from an empty work dir, tests OK.

### What I learned
- Nothing in the check depended on the stub bodies touching a communal (no external-symbol budget effect at these sizes).


## 2026-11-04 13:00 CET: The runtime comes from a combined MLIBCE.LIB

### What I did
- The layout build no longer extracts 100 MSC library members. LIB 3.11 builds `MLIBCE.LIB` from the untouched MLIBCR+LIBH+EM+MLIBFP (in that order, as MSC SETUP combines them), as a real build step and the first rule of the generated MAKEFILE/BUILD.BAT. LINK runs `/MAP /STACK:2560 /EXEPACK` with an empty library field and no `/NOD`: the objects' default-library record picks MLIBCE. `_archive_objects`, `RUNTIME\` and LAYOUT.JSON's `runtime`/`emulator` lists are gone.
- Result: `./sr build --layout` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- The "recovered runtime order" was the order LINK extracts members from this combined library; other component orders differ by 5381..12426 bytes. The combined library keeps all 402 component modules, so only the order matters.

## 2026-11-04 13:30 CET: Loop pragmas are source text, not injected

### What I did
- `linklayout.prepare` no longer writes `#pragma loop_opt` before each member source; a unit is the plain concatenation of its sources.
- Five functions lose their match under one flag set per file: F_01BAE, F_01FE2, F_0238C (unit 00D78), F_03C8D (039C0), F_2382D (23262). Each diverges at a structured loop the shipped code keeps as an /Ol-off memory loop. for/while/do rewrites (and the `unsigned char` trick for F_2382D's latch copy) all still get registers and loop temps; only `goto` loops escape /Ol, which is less plausible than a pragma. The sources now carry the real MSC 5.1 pragma with a comment: one `loop_opt(off)` before F_01BAE (to the end of the file), off/on around F_03C8D, off before F_2382D.
- The other members recorded without `l` (M_039C0, M_03FB1, M_04940) match under the file's /Ol unchanged.
- Result: `./sr build --layout` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

## 2026-11-04 14:00 CET: `main`, no CRT.H, public near functions

### What I did
- F_0066F is `main` in M_00659.C and the DB; the layout build no longer defines it or renames it for the gate.
- CRT.H (only a comment by then) is deleted with every place that prepended it, listed it as a dependency or kept it.
- The 29 near functions called from other link units (`call near` within a shared `/NT` code segment) are public `T near F(...)` in their sources, with prototypes in their owner's header (`./sr proto` accepts `T near @(...)`). Callers lost their `static` forward declarations and cross-unit `.STB` placeholders; same-unit placeholders are `@NAME` lines built from the DB prototype. The layout build's static-stripping regex and its prepend of near prototypes harvested from `.STB` files are gone. linkmatch places file-static functions (absent from the MAP) relative to a public function of their own object.
- Result: `./sr build --layout` IDENTICAL from an empty work dir (0 unresolved, 572/572 mapped, 748/748 near calls), `./sr check --all` 572/572, tests OK.

### What I learned
- No file crossed MSC's external-symbol budget: the merged units already referenced these functions as externals once `static` was stripped; only the source now says so.

## 2026-11-04 14:30 CET: No CRTORDER anchor: M_21962.ASM is a small-model file

### What I did
- Listing each code segment's first SEGDEF in link order shows the runtime's `_TEXT` had to be declared between U_1C11C's `S1C11_TEXT` and U_21962's `S2196_TEXT`, which are adjacent; no data-only unit fits there.
- M_21962.ASM is now `.MODEL SMALL` with its far PROCs in an explicit `S2196_TEXT SEGMENT` (small model rejects `.CODE name`, A2063). MASM's `.MODEL SMALL` emits the empty `_TEXT` SEGDEF first, where the anchor was. `src/link/CRTORDER.ASM` and its insertion are deleted; `merge_asm` keeps the source's `.MODEL`.
- Found on the way: the header scan stripped quoted strings across an apostrophe in an asm `;` comment and the `'CODE'` class, hiding M_21962's PUBLICs; it now drops asm comments first.
- Result: `./sr build --layout` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- MASM 5.1's `.MODEL` declares the default code segment immediately (`FILE_TEXT` for MEDIUM, `_TEXT` for SMALL), so the model line alone fixes segment order at that point of the link. The EXE only proves that some object in that window declared `_TEXT`; a full-segment-directive header is also IDENTICAL, so the exact directive form is not recoverable.

## 2026-11-04 15:00 CET: The faithful build is the build

### What I did
- Retired the legacy `./sr build` (work/game: renamed G0..G3 code segments, staging edits, `.STB` near-declaration harvesting, `/NOD` with component libraries; it no longer linked). `./sr build` runs the layout build (`--layout` is an alias); build.py keeps `run_dos`, `link`, `link_report`, `header_deps`; the Makefile drives the layout build; test_build checks the generated MAKEFILE, BUILD.BAT and LINK.RSP.
- Removed the empty `src/data/DGROUP.ASM` from the link and the repo (still IDENTICAL); `./sr dgroup asm` only reports unowned data. Deleted the unused `src/data/SLOTS.ASM` and the obsolete `tools/scripts/reconstruct_data.py` and `inspect_layout_data.py`.
- README.md and src/data/README.md describe the faithful build; dosbox.sh mounts the prepared tree.
- Ran the prepared tree natively in DOSBox-X: `BUILD.BAT` and `MAKE MAKEFILE` (MAKE 4.07) each produce an SR.EXE identical to orig/SR.EXE, with no host tooling involved.
- Result: `./sr build` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- The last, empty `_DATA` contribution had no effect on segment order. The generated DOS tree is a complete 1988-tools build (C: work/layout, D: msc51, E: its INCLUDE, F: masm51).

## 2026-11-04 16:30 CET: One source file per original object

### What I did
- Merged the split sources of the 82 code units into one file each (src/mods: 189 files -> 86; each file keeps its first source's name). C files are exactly the text the layout build compiled; asm files lost the `; sr-member` markers, prefix only clashing labels and name their segment in `.CODE`. Compiled under the old names, all 82 objects were byte-identical to the previous `U_*` objects.
- Each file has one command line, LAYOUT.JSON `units[].flags` (the union of the old per-function flags, which the build already used); `functions.flags` and `functions.module` are dropped, `src` points at the file. `./sr check` compiles the whole file and gates it; the build compiles each file unchanged under its own name.
- Removed the `.STB` sidecars, stb.py, merge/split, `module create/add/dissolve/list/show`, `./sr new`, `./sr src`, src/funcs support and docs/MODULES.md. An asm function's extent comes from its PROC length (M_1DD22 holds a data byte between two procedures).
- Untracked the `__pycache__` directories (now in .gitignore).
- Result: `./sr check --all` 572/572 (551 MATCH, 21 MATCH* = near calls to public near functions of other objects, verified by the linked build 748/748), `./sr build` IDENTICAL from an empty work dir, tests OK, emucmp IDENTICAL.

### What I learned
- Declaration order inside a file shows in the object's symbol records (EXTDEF/LEXTDEF/COMDEF order), not in the EXE: reordered include blocks changed records and the EXE stayed identical.
- Open: `./sr dgroup apply` still moves `g_4fa6` between M_0542E.C and M_039C0.C depending on the previous build; both are IDENTICAL, so ownership of DS:4FA6 is not decided by the evidence the tool uses.

## 2026-11-04 17:00 CET: Deterministic `dgroup apply`, system headers first

### What I did
- `./sr dgroup apply` moved DS:4FA6 (`g_4fa6`) between M_0542E.C and M_039C0.C on every run: `refs_to` counted a C initializer's reference to another file's global (EXTDEF fixup in `_DATA`) but not the same reference to the file's own `_DATA`, and credited own-`_DATA` code fixups (literals included) to the preceding public. DS:4FA6's only user is M_0542E's `char *g_4ff2 = g_4fa6;`. References now count the same wherever the global is defined; the report changes only at DS:4FA6, which stays in M_0542E. Apply is a fixed point from both placements and after a clean build.
- Include blocks: system headers first (existing order), then STRUCTS.H, the file's own header, the used headers; 29 sources reordered.
- Result: `./sr build` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- Ownership evidence must be symmetric under moving a definition, or the previous apply feeds the next one.
- DS:4FA6 holds the text "The game must have a name !" but is declared `int[14]`; a `char[]` string is the likely original (open).

## 2026-11-04 18:00 CET: The MAKEFILE is checked in; LAYOUT.JSON is gone

### What I did
- `src/MAKEFILE` (MSC MAKE 4.07, in the style of MSC 5.1's own STARTUP\MAKEFILE: `CC`/`ASM` macros, the MLIBCE.LIB block, one block per object with its command line and header dependencies, the link last) and `src/SR.LNK` (97 objects in link order) are the build definition. `src/` is the build directory (C:), MSC on D:, MASM on F:, SETUP's environment. `./dosbox.sh --make` runs `MAKE MAKEFILE` in DOSBox-X on a copy of `src/`: SR.EXE SHA-1 3a5e28b7..., identical to orig/SR.EXE.
- `./sr build` copies `src/` and runs each block as written (emu2 for CL/MASM/LIB, DOSBox-X for LINK); that part needs neither the DB nor orig/ (`--no-verify`). The object gates and the comparison are a separate verification step.
- LAYOUT.JSON is deleted: flags and `/NT` segments are MAKEFILE lines, link order is SR.LNK, file extents come from the DB, and the data-unit/order evidence stays in dgroup.py; `./sr dgroup order` checks SR.LNK against it, `./sr flags` edits the MAKEFILE.
- Headers are source: build and check no longer regenerate them; `./sr headers` maintains headers, include blocks and MAKEFILE dependencies, `./sr headers --check` (also a test) reports drift. Headers are LF with a one-line banner.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

### What I learned
- A source path (`MODS\X.C`) changes only the OBJ's THEADR record, never code or segment names.

## 2026-11-04 19:00 CET: Codegen dummies: four replaced by natural source, three documented

### What I did
- F_0A544's empty `{ }` was no longer needed. F_1783E's `if (0) ;` became the clip-rect stores through `*pr1`/`*pr2`: the pointer stores kill the known `*pr2` and `(*pa)->height`, which is why the original reloads them. F_23C3B's `if (p > 40) ;` became braced register-write macros `GCREG`/`SEQREG` used for every port pair of M_238A2.C (a reconstruction: any braced block ends the basic block holding the CSE; bracing 38 other pairs elsewhere changes no bytes). F_07AA4's `int n` became a key-indexed `int flags[3]` whose element 0 is the frame hole (an interpretation).
- F_11213's three unused ints are one unused inner-block `struct Colors save`; it, the four `long unused` (F_118D4, F_12A17, F_12DD8, F_12F99) and the x_trk0/x_trk1 dead far accesses in M_0F38A.C stay, each commented PROVISIONAL in the source and listed in plans/faithful-build.md. Lint now rejects `extern` at any block depth except those two declarations. DB notes updated.
- Result: `./sr build` IDENTICAL from an empty work dir, `./sr check --all` 572/572, tests OK.

### What I learned
- A non-empty braced block is the same basic-block boundary as a code-less `if`; before inventing a boundary, check whether stores through pointers explain the reloads.

## 2026-11-04 19:30 CET: C source cleanup

### What I did
- Removed the duplicate forward declarations the merge left (M_163BE, M_0914C, M_15396; F_1563A takes `char far *`). One `#pragma intrinsic` per file after the includes.
- Every C function has a `/* NAME @ SSSS:OOOO (linear 0xAAAAA, N bytes)` header from the DB with its description below (fixed 158DE, 286D8, 0F418/0F62E); comments separated from their function by data or pragmas sit next to it again (the data did not move).
- Warnings fixed without byte changes: casts in `g_4ea4` and the F_2869E blit, Car pointers in M_039C0, `g_0358[0]`, `F_12F99(int n, int flag)`, `(void)` interrupt prototypes; `g_4fa6` is `char[] = "The game must have a name !"`. Left as probably original: C4035 for `main` and F_118D4, C4033 in M_0B3CA, a few C4047 that need struct retyping.
- Every statics and communal block says its names are fixed by hash (CLAUDE.md 4e/4f); HOTSPOT.C's fillers and g_6c42 are marked as such.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

### What I learned
- Removing a duplicate prototype reorders MSC's EXTDEF records without touching the EXE.
- Open: the DB gives segment 0E6C to 11 functions in segments 0F38/1C11; `./sr dgroup apply` would revert the comment moves and initializer casts in M_15396/M_0F38A (its writer still owns those lines).

## 2026-11-04 20:30 CET: Assembler sources as 1989 MASM; `dgroup check` instead of `apply`

### What I did
- Each .ASM is one MASM module: `.MODEL`, one `.DATA` (sorted EXTRNs, then the file's data), one `.CODE`, the procedures, `END`. The `L<addr>_` label prefixes are gone (73 names shared between procedures of a file are numbered); header comments come from the DB (F_02BE2 said 2BC0). `g_4730` is `char` (EXTRN BYTE, `mov WORD PTR _g_4730,ax` for the pair), `g_47e8` is `int`.
- DGROUP data in M_02B9A, M_0E56A, M_0E5D2, M_0E610, M_1DD22, M_21962 and MESSAGES.ASM (8797 -> 459 lines) is natural MASM: typed labels on the first value, several values per line, strings, `DUP`, `EVEN`; MASM prints no warnings. src/data/*.ASM: labels instead of BASE+EQU, no DB `@` syntax.
- `./sr dgroup apply` (and `asm`) are gone: the sources are the truth for data definitions. `./sr dgroup check` verifies every initialized item is defined in the file that owned it (exit 1 otherwise). omf.py reads LIDATA.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, `dgroup check` and `headers --check` clean, tests OK.

### What I learned
- MASM 5.1 writes EXTDEFs in its own symbol order, so deduplicating EXTRN lines leaves the OBJ unchanged; local code labels never reach the OBJ. `DUP` becomes LIDATA; `EVEN` pads 00h in data, 90h in code.

## 2026-11-04 21:30 CET: Final cleanliness pass; docs/RECONSTRUCTION.md

### What I did
- Deleted the unused `Steps`, `Strip` and `CgaFont` structs (the CGA font has no header) and dead tool code; pyflakes is clean.
- The scanner gives routines found without a far caller the segment that contains them (nearest base below): 8 game functions moved to 0F38, 3 to 1C11, 7 runtime rows to 1E16, F_2197D to 2196. All 572 header comments agree with the DB.
- Rewrote 31 stale DB notes that described the split-era workflow or long-resolved divergences.
- `./sr dgroup register`/`aliases` (the last source-writing modes) are gone; `dgroup check` also fails on alias views and on layout globals the DB lacks.
- "Click here to edit" (`g_290c`) and " Gear: " (`g_47fe`) are strings instead of char/int pieces (no code reference into their middle).
- New docs/RECONSTRUCTION.md lists every reconstruction choice with its evidence and limits; plans/faithful-build.md is marked done; docs no longer depend on scratch scripts in work/.
- Result: `./sr build` IDENTICAL from an empty work dir, `./dosbox.sh --make` IDENTICAL, `./sr check --all` 572/572, tests OK, `headers --check`, `dgroup check` and `dgroup order` clean.

## 2026-11-04 23:00 CET: Second audit fixed; the faithful build is finished

### What I did
- A second independent audit (read-only, fresh clone) confirmed the build runs exactly the commands in src/MAKEFILE, writes nothing into src/, and works without orig/ and db/; it listed small remaining issues, all fixed:
- HOTSPOT.C is all `struct Hot`: DS:09F4..239E is 365 18-byte records in one index space (f10 is the next record's index into g_09f4). The split/flattened tables are Hot arrays starting at the record the code names (`g_11b0[6]`, `g_1ac2[39]`, `g_1d80[10]`, `g_1e34[35]`); the Link/Button/NewLink/Row8/Blt12 views are gone.
- `g_0288` is `struct Bitmap far *` (two C4047 gone); `g_05d6..g_05f0` have 4-digit names; the keymaps are 43 labelled pair lists; EFFECTS streams are `int far[]`; M_02DB0.H exists; MAKEFILE blocks follow link order (checked by the build); source comments no longer cite tools or dates; STRUCTS.H comments are in place; generated view casts became plain indexing.
- The `g_49e0[0] = g_49e0[0];` dead load in F_049ED/F_05B4A stays, PROVISIONAL (only an empty `switch` gives the same bytes). docs/RECONSTRUCTION.md gained the layout gotos, the budget structs and message offsets, undecided return types and a warnings section (109 at /W3, which are believed original).
- Verified in a fresh clone with orig/ and db/ removed: `./sr build --no-verify` gives SR.EXE SHA-1 3a5e28b7..., the shipped file.
- Result: `./sr build` IDENTICAL from an empty work dir, `./dosbox.sh --make` IDENTICAL, `./sr check --all` 572/572, `headers --check`, `dgroup check`, `dgroup order` clean, tests OK.

### What I learned
- Link, NewLink, Button and Win were all views of one 18-byte hot-spot record; the f10 index chain makes the table one array.
- Whether a table is `char` or `unsigned char` shows in callers (cbw vs zero extension): evidence for the element type.

## 2026-11-05 00:00 CET: Naming conventions adopted

### What I did
- Adopted the naming conventions of the Betrayal at Krondor reconstruction (its CONTRIBUTING.md / CLAUDE.md) in CLAUDE.md: `camelCase` locals, `g_camelCase` globals, `SCREAMING_SNAKE_CASE` constants and enum values, `PascalCase` types, `camelCase` members, `module_snake_case` public functions, `static snake_case` private ones; plus DOS 8.3 upper-case file names named for the module, "name only from evidence", "never add comments" and `NULL` for pointers.
- Added what makes a rename load-bearing here: local frame order, LINK's communal hash and MSC's static hash, and the external-symbol budget.

## 2026-11-05 01:00 CET: `/Oi` instead of `#pragma intrinsic`

### What I did
- The 25 per-file `#pragma intrinsic` lists were the reconstruction's per-function pragmas, merged. In the 7 files that already had `/Oi` they were redundant; the other 18 match with the pragma deleted and `i` added to their command line. No game C code calls a library routine that has an inline form (only the runtime calls strcpy/strlen/memcpy/memset), and all 38 C files without `/Oi` match with it added, so `/Oi` alone explains every inline inp/outp/abs/_rotl/_rotr/memcpy/memset/_enable/_disable/sin/cos. No pragma intrinsic/function is left in src.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

### What I learned
- The bytes cannot tell `/Oi` from the pragma, so `/Oi` is the simpler explanation, not a proven fact (recorded in RECONSTRUCTION.md). A pragma only has to precede the first use (M_1D8C4.C: between the two functions it works, at the end of the file it does not).

## 2026-11-05 01:30 CET: The loop pragmas survive a second search

### What I did
- Retested the five `loop_opt(off)` functions (F_01BAE, F_01FE2, F_0238C in M_00D58.C; F_03C8D in M_039C0.C; F_2382D in M_23262.C) against every alternative: the files without `l` (13 other functions break: they hold plain locals in cx or si/di with exit stores, e.g. F_01843, F_236D5, F_049ED), extra `/On`/`/Op`/`/Ow`/`/Or`, a 32-case matrix of `volatile`, `register`, static/global counters, `&i`, calls, `setjmp`, intrinsics and `interrupt`, and goto/`while(1)`/`for(;;)`/`continue`/`?:`/comma rewrites of the real functions. Object membership is proven three ways (word-aligned code segments with F_2382D and F_03C8D at odd addresses, `_BSS` statics shared across M_00D58's pragma, CONST doubles shared by F_03C8D and F_049ED). No source changed; RECONSTRUCTION.md records the result.

### What I learned
- MSC 5.1 ignores `volatile`. `/Ol` leaves a loop alone only when it is entered by a jump; a `goto` to the very next statement makes F_0238C match, but nothing else jumps to that loop head, so the pragma is the better explanation. M_039C0.C also matches as `/Oasi` plus one `loop_opt(on)` after F_03C8D. An `if` + goto loop inside a `for` crashes C1 under `/Ol` (C1001).

## 2026-11-05 02:30 CET: Review of the assembler routines; the hook installers are C

### What I did
- Reviewed all 66 routines of the 27 MASM objects against MSC 5.1 output, compiling C candidates for the borderline ones (F_02C2B, F_21A06, F_26458, the hook stubs). 63 are hand-written (register calling conventions, inline `int`, `lds`/`les` on arguments, `xchg`/`loop`/`xlat`, code-segment scratch words, computed jumps, flags as results, MSC-impossible idioms such as `xor ah,ah` or `push si` before `push di`); the evidence is in DB notes.
- F_1C2B8, F_22352 and F_22B28 copy a driver's 15 far hook pointers over DS:78A2 as one struct assignment: MSC 5.1's `rep movsw` is F2 A5, MASM's F3 A5, and the assembler source had to spell it `repne movsw`. Each is now C, the first function of the driver file whose table it copies (`struct Hooks` in STRUCTS.H); M_22352.ASM and M_22B28.ASM are gone (95 objects). Their placement has no relocation evidence and is a reconstruction choice (RECONSTRUCTION.md).
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, `headers --check`, `dgroup check`, `dgroup order` clean, tests OK.

### What I learned
- The `rep` prefix byte classifies code: every F2 movs in SR.EXE is in C, every F3 movs in asm or the runtime. MSC's `inp`/`outpw` reload `dx` each pass and load the value before the port.
- MSC emits far initialized data as PARA 'FAR_DATA' segments, so most of src/data could have been C; the EXE cannot tell. The hook block DS:78A2..78DE was one struct in the original (copied as one); its 15 separate globals are a reconstruction.

## 2026-11-05 03:00 CET: F_2382D without `#pragma loop_opt`

### What I did
- F_2382D (M_23262.C, `/Oalsi`) matches without the pragma once its row loop is entered at its test: `row = 0; goto test; next: row++; src += pitch; test: if (row < h) { for (col = 0, p = src; col < w; col++) *dst++ = *p++; goto next; }` — same 117 bytes and frame. The pragma and its comment are gone; M_00D58.C and M_039C0.C keep theirs.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

### What I learned
- MSC 5.1's `/Ol` only optimizes a loop entered at the target of its back edge. A label-at-entry goto loop (`again: ... goto again;`) is optimized like a `for`; a loop entered elsewhere (`goto test` into a `next:` loop, F_29170's shape) is skipped together with the loops nested in it. Forward gotos to a loop label change nothing. Which of the matching forms (or the pragma) the original had is not recoverable.

## 2026-11-05 03:30 CET: F_2382D keeps its `#pragma loop_opt`

### What I did
- Reverted F_2382D to the plain nested `for` loops under `#pragma loop_opt(off)`. The goto form (row loop entered at its test) also matches, but needs knowledge of an undocumented `/Ol` rule; the pragma is MSC's documented mechanism, the same idiom stays in M_00D58.C and M_039C0.C, and the latch copy has a reason to avoid `rep movsw`. RECONSTRUCTION.md records both forms. The recipe book keeps the `/Ol` entry-point rule.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572.

## 2026-11-05 05:00 CET: Real names for files, functions and globals

### What I did
- Gave evidence-based names to 84 of the 85 src/mods files (plus SPANS1/2 and NEEDLE in src/data; CARMASKS/CARMASK2 are SCENERY/SCENERY2, the roadside sprites), about 500 game functions, about 390 initialized globals and statics, and a few communals. Seven read-only analysis agents gathered the evidence (strings, ports, callers, data use); low-confidence items keep their placeholders (M_02B9A.ASM, the jingle starters, most communals). New `./sr rename` (DB, sources, MASM, MAKEFILE, SR.LNK, docs, headers in one pass) and `./sr fit` (hash window of a communal or static).
- An independent skeptical review (all functions, ~350 globals, all files) found two inverted names (`entry_beats_score` -> `score_beats_entry`, `g_tireGrip` -> `g_tireSlip`), two unsupported (`g_hotAttendant`, the write-only cursor hot point -> placeholders) and some over-specific ones (`siren_sweep` -> `sweep_up_down`, `g_garagePics` -> `g_gamePics`, `sys_car_halt` -> placeholder, the carhop -> a walker, WALKER.C, ...); all corrected. `g_pageBmps` and `g_walkState` were chosen to keep their hash slots.
- egablit_latch_pack (was F_2382D) keeps its `#pragma loop_opt(off)` and plain `for` loops.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, `headers --check`, `dgroup check`, `dgroup order` clean, tests OK.

### What I learned
- MSC's per-file budget counts name characters of static functions and the static variables a file references too, not only externals (rule 4d corrected). GARAGE.C (before F_07AA4) and VIEW.C (before F_2969D) are at the limit, so most of their symbols keep placeholders.
- Communal windows are 0-3 hash buckets wide; few descriptive names fit them.

## 2026-11-05 06:30 CET: No database; the tools read the sources

### What I did
- Deleted db/sr.sqlite. tools/sr/source.py builds every table the tools used from the files: game functions from the comment above each definition (name, seg:off, linear address, size), globals and far data from the headers' extern lines and their address comments, file statics from their definitions; runtime functions, their library names and the library's DGROUP data come from the scanner and the MSC libraries matched in the EXE (cached under work/). Function notes were dropped (git history keeps them).
- Removed the commands that only wrote rows (`name`, `global`, `proto`, `mark`, `kind`, `note`, `next`, `status`) and docs/PROGRESS.md; declarations and prototypes are header edits. `./sr headers` maintains include blocks and MAKEFILE dependencies and checks every header against its file's definitions; `./sr scan` and `crtnames` only report; `./sr rename` works on files alone.
- A DB-vs-loader comparison agreed except for a retired scanner row, a stale prototype, the interrupt handlers' near/far reading and the choice between two library names for one address; `check --all` and the four dgroup outputs were byte-identical with and without the DB.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, `./sr scan` agrees with the sources, 38 tests OK.

### What I learned
- The address comments in the headers are now what `refs` verifies global addresses against, so they are load-bearing.
## 2026-11-04 12:39 CET: Name fixed PC memory addresses

### What I did
- Added PC.H with BIOS_EQUIP (0000:0410, unsigned char), BIOS_TICKS (0000:046C, long), MOUSE_VECTOR (0000:00CC, long), CGA_VRAM (B800:0000), MONO_VRAM (B000:0000), EGA_VRAM (A000:0000), and BIOS_ROM (F000:0000); video and ROM pointers are char far pointers. MAIN.C, MEM.C, MOUSE.C, and SOUND.C include it through ./sr headers.
- Named the game-specific ARENA_FIXED_BASE (8000:0000, ARENA.C), TANDY_SCREEN1_BITS (9000:0000, MAIN.C), CURSOR_EGA_SAVE (A000:1F40, CURSOR.C), and EGA_PAGE1_BITS/EGA_PAGE2_BITS (A200:0000/A400:0000, ROAD.C) after their actual uses. No unexplained fixed pointer casts remain outside macro definitions.
- Audited assembler literals: M_02B9A.ASM and VIDDET.ASM access the equipment byte at 0410h; CGADRV.ASM, CGALINE.ASM, and HERCDET.ASM use the monochrome segment B000h. Kept these MASM operands literal: PC.H defines C pointer expressions, not assembler offsets or segments. HERCDET's CCh bytes are key-map data, not mouse-vector addresses. The unsigned 80000000h bound in MEM.C remains integer address arithmetic.
- Extended header ownership to macros with replacement text and standalone PC.H; added tests for usage-driven inclusion, removal, idempotence, standalone ownership, and module macro definitions. Updated the sr-cli skill's tooling map.
- Gates: clean-work ./sr check --all: 572/572 still match (551 MATCH, 21 MATCH*); separately removed work before ./sr build: Packed EXE vs orig/SR.EXE: IDENTICAL, 572/572 at original positions, 748/748 near-call encodings exact. ./sr headers --check and ./sr dgroup check clean; unittest discover: 41 tests OK; ./dosbox.sh --make: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL. Repeated check --all after the tooling refinement: 572/572.

### What I learned
- Header generation previously tracked address-annotated declarations only. Macro ownership now participates too, excluding empty include guards; the sr-cli skill documents the standalone-header exception.
- Platform names were checked against the 1988 Microsoft Press reference (equipment flags at 0040:0010, ticks at 0040:006C, display segments) and Microsoft's mouse reference (INT 33h; vector offset 33h * 4 = CCh): https://msarchive.pcjs.org/mspl13/msdos/norton/ and https://bitsavers.trailing-edge.com/pdf/microsoft/mouse/Microsoft_Mouse_Programmers_Reference_1989.pdf .
- Recasting the shared char far video pointers as int far pointers, and as long for the original packed-pointer limit arithmetic, preserves every compiled byte. Full clean builds verify the macro substitutions in initialized bitmap data too.
