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

## 2026-11-05 07:30 CET: Named fixed PC memory addresses

### What I did
- The bare far-address casts became named constants. PC facts are in the new shared header src/include/PC.H (no owning .C, like STRUCTS.H): `BIOS_EQUIP` (0000:0410), `BIOS_TICKS` (0000:046C, both old spellings unified), `MOUSE_VECTOR` (the int 33h vector at 0000:00CC), `CGA_VRAM`, `MONO_VRAM`, `EGA_VRAM`, `BIOS_ROM` (searched for "TANDY"). Single-file addresses are `#define`s at the top of their file (ROAD.C's EGA page buffers, CURSOR.C's EGA save area, MAIN.C's Tandy second screen, ARENA.C's fixed base). `./sr headers` accepts standalone constant headers and includes PC.H where its names are used. Done by a Codex agent in a Rex pane.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

### What I learned
- MSC 5.1 offers no helpers for fixed addresses (dos.h has only FP_SEG/FP_OFF; MK_FP and peek/poke are Turbo C), so a cast long constant was the period idiom; the macro names cost nothing at codegen.
## 2026-11-04 13:05 CET: Name BIOS equipment video bits

### What I did
- Added EQUIP_VIDEO_MASK (0x30), EQUIP_VIDEO_COLOR80 (0x20), and SET_EQUIP_VIDEO(m) to PC.H. Replaced both equipment-byte update pairs in MAIN.C's init_screens with SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80), retaining the two read-modify-writes and adding no source comments.
- The requested comma expression with ~EQUIP_VIDEO_MASK matched on the first attempt; no casts, alternative masks, statement macro, or option-1 fallback were needed.
- From empty work: ./sr check --all reported 572/572 still match (551 MATCH, 21 MATCH*). Removed work again before ./sr build: Packed EXE vs orig/SR.EXE: IDENTICAL; 572/572 game functions at original positions and 748/748 near-call encodings exact.
- ./sr headers --check and ./sr dgroup check clean; unittest discover: 41 tests OK; ./dosbox.sh --make: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL.

### What I learned
- The comma-form macro preserves the byte-access sequence of the original two statements under MAIN.C's /Oalsi. Recorded this C shape in .claude/skills/msc51-recipe-book/SKILL.md.
## 2026-11-04 13:13 CET: Name interrupt and BIOS/DOS/mouse API constants

### What I did
- Added BIOS_VIDEO (10h), DOS_INT (21h), MOUSE_INT (33h), and TIMER_INT (8/IRQ0) in PC.H and used them at every C interrupt/vector call site. Kept intdos as the DOS API wrapper it already was.
- Named BIOS mode/page/CGA palette/EGA palette/DCC functions, DCC read and VGA colour codes, modes 03h/04h/09h/0Dh, CGA palette selection and colour sets, EGA palette-register limit and set-all subfunction, DOS write-string and seek/start-of-file codes, mouse reset/position/motion functions, and mouse left/right button masks. DASH.C's two main_set_palette arguments now use the same palette names as MAIN.C.
- Preserved statement structure: AX constants use shifts and OR; mouse AH and AL assignments remain separate. No functions, globals, locals, or files renamed, no assembly edited, no source comments added. ./sr headers generated PC.H includes and MAKEFILE dependencies for the new users.
- All gates passed: empty-work ./sr check --all: 572/572 still match (551 MATCH, 21 MATCH*); removed work again before ./sr build: Packed EXE vs orig/SR.EXE: IDENTICAL, 572/572 functions at original positions, 748/748 near-call encodings exact. Headers and DGROUP clean; 41 unit tests OK; DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL.
- ASM suggestion only: a separate shared src/include/PC.INC with MASM EQU definitions for interrupt numbers and AH/AL selectors would be natural for HERCDET, KBDGET, KBDOFF, M_02B9A, and VIDDET. Keep byte/word register loads as written and add INCLUDE dependencies when implementing it; C PC.H pointer expressions are not MASM input.

### What I learned
- API meanings checked against Ralf Brown's Interrupt List: BIOS functions/modes/palettes in https://raw.githubusercontent.com/cirosantilli/ralf-brown-interrupt-list/master/inter61a/INTERRUP.A ; DOS seek and write-string in https://raw.githubusercontent.com/cirosantilli/ralf-brown-interrupt-list/master/inter61b/INTERRUP.F ; mouse reset/position/motion at https://www.delorie.com/djgpp/doc/rbinter/id/75/59.html , https://www.delorie.com/djgpp/doc/rbinter/id/78/59.html , https://delorie.com/djgpp/doc/rbinter/id/86/59.html .
- No unidentified literal API function codes remain at C call sites. Game menu IDs 1..6 and internal negative display modes stay literals because they are not BIOS mode numbers. VIDEO.C's colour conversion threshold 7 and bit 10h, palette sizes 16/17, hardware retrace port 3DAh/mask 8, and other hardware-port values remain outside this interrupt-API naming scope. The palette-versus-border selector remains the original boolean expression (now reg > EGA_PALETTE_LAST); generic zero/null/carry tests, the DOS '$' terminator, and shifts by byte/word widths are unchanged. ASM API literals are intentionally retained per task.
- Named constant expressions preserve the original packed register stores; the MSC recipe book records the verified AX and split mouse-register forms.

## 2026-11-05 08:30 CET: Named interrupt numbers and BIOS/DOS/mouse function codes

### What I did
- PC.H names the interrupts (`BIOS_VIDEO`, `DOS_INT`, `MOUSE_INT`, `TIMER_INT`), the BIOS video functions and modes used (`VIDEO_SET_MODE`, `VIDEO_SET_PAGE`, `VIDEO_SET_PALETTE`, `VIDEO_PALETTE_REGS`, `VIDEO_GET_DCC`, `MODE_TEXT80`, the CGA/Tandy/EGA graphics modes, the CGA palettes), the DOS functions (`DOS_WRITE_STRING`, `DOS_SEEK`) and the mouse functions; the 12 int86/int86x/intdos sites and the timer vector calls use them. `BIOS_EQUIP`'s video bits go through `SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80)`, a comma macro that keeps both read-modify-writes. Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers check clean, tests OK.
## 2026-11-04 13:21 CET: Name I/O ports and hardware registers

### What I did
- Named all 17 literal I/O port addresses used by the C sources in PC.H: PPI_B=61h; PIT_CH0=40h, PIT_CH2=42h, PIT_CMD=43h; PIC_CMD=20h; GAME_PORT=201h; GC_INDEX/DATA=3CEh/3CFh; SEQ_INDEX/DATA=3C4h/3C5h; CRTC_INDEX/DATA=3D4h/3D5h; INPUT_STATUS=3DAh; MONO_STATUS=3BAh; MONO_MODE=3B8h; CGA_MODE=3D8h; TANDY_PAGE=3DFh. Used them throughout the 18 C files that access ports.
- Named the used GC register indices (colour compare 2, read map 4, mode 5, bit mask 8), GC write modes 0/1/2 and read mode 1 (08h), all-bits mask FFh, sequencer map mask 2 and plane masks 1/2/4/8/0Fh, CRTC start high/low 0Ch/0Dh and row offset 13h, colour vertical retrace 08h and Hercules vertical sync 80h, graphics/video/blink mode bits 02h/08h/20h, PPI speaker gate/data 01h/02h, PIT channel 0/2 low-then-high binary mode-3 commands 36h/B6h, PIC nonspecific EOI 20h, and game-port button/first-joystick-axis masks F0h/03h.
- Preserved all source comments, braces, semicolons, GCREG/SEQREG block boundaries, and statement order; no symbols renamed, no assembly edited, no comments added. ./sr headers regenerated the necessary PC.H includes and MAKEFILE dependencies.
- Gates: from empty work, ./sr check --all: 572/572 still match (551 MATCH, 21 MATCH*). Removed work again before ./sr build: Packed EXE vs orig/SR.EXE: IDENTICAL; 572/572 functions at original positions and 748/748 near-call encodings exact. ./sr headers --check and ./sr dgroup check clean; unittest discover: 41 tests OK; ./dosbox.sh --make: DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL.
- Retained literals: MAIN.C's F6h is the game's Tandy CRT/CPU page/address-mode configuration; DASH.C's CRTC row-offset value 14h and zero display-start bytes are screen-layout data; SYS.C's channel-0 divisor bytes 00h/40h and channel-2 bytes 1Eh/00h are selected timing/frequency data, as are the computed note divisors. Game-port writes of 0 trigger one-shots regardless of the value. Computed colours, compare values, plane selections, and edge masks remain expressions/data. Existing palette pixel conversions and other non-port arithmetic remain outside scope. No unidentified C port addresses remain. Unchanged ASM literals are intentional.
- Suggested MASM follow-up: src/include/PC.INC with absolute numeric EQUs, e.g. `GC_INDEX EQU 03CEh`, `GC_DATA EQU 03CFh`, `SEQ_DATA EQU 03C5h`, `CRTC_INDEX EQU 03D4h`, `INPUT_STATUS EQU 03DAh`, `MONO_STATUS EQU 03BAh`, `MONO_MODE EQU 03B8h`, `HERC_CONFIG EQU 03BFh`, and `GAME_PORT EQU 0201h`, then INCLUDE PC.INC and replace only the immediate operands. The same absolute values should preserve instruction encodings; no scratch ASM substitution experiment was run. Include dependencies and full ASM/build gates would still be required in that separate change.

### What I learned
- Verified ports and fields against Ralf Brown's port list (tables P0015, P0380, P0392/P0394, P0542, P0654/P0656, P0670/P0672, P0700/P0704/P0708, P0817/P0818): https://raw.githubusercontent.com/cirosantilli/ralf-brown-interrupt-list/master/inter61d/PORTS.A and https://raw.githubusercontent.com/cirosantilli/ralf-brown-interrupt-list/master/inter61d/PORTS.B . Tandy's 1000 SX Technical Reference, page 47, confirms 3DFh as the CRT Processor Page Register, with CRT page bits 0-2, processor page bits 3-5, and address-mode bits 6-7: https://www.lo-tech.co.uk/downloads/manuals/tandy/Tandy_1000SX_Technical_Reference_Manual.pdf . It is not a Hercules port.
- A plain ~speaker-mask is a 16-bit complement and changed MUSIC/GARAGE/SOUND/SYS instructions, initially reducing the gate to 482/572. Byte-width complements `(0xff ^ PPI_SPEAKER_MASK)` and `(0xff ^ PPI_SPEAKER_DATA)` restored the exact original FCh/FDh masks. The MSC recipe book records the distinction. Graphics register naming, including EGASHIFT's existing braced macros, matched without structural changes.

## 2026-11-05 09:30 CET: Named I/O ports and hardware registers

### What I did
- PC.H names the 17 I/O ports the C code uses (EGA graphics controller, sequencer, CRTC, input status, mono/Hercules and CGA mode control, Tandy page register, PPI port B, PIT channels and command, PIC command, game port) and the documented register indices and bits written to them (GC_MODE, GC_BIT_MASK, GC_READ_MAP, write/read modes, SEQ_MAP_MASK and plane bits, CRTC_START_HI/LO, CRTC_OFFSET, STATUS_VRETRACE, mode-control bits, speaker gate/data, PIT mode bytes, PIC_EOI, game-port masks). All outp/inp sites use them. Game data stays literal (Tandy page mapping value, CRTC start/stride data, PIT divisors, computed colours and masks). Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers check clean, tests OK.

### What I learned
- Clearing speaker bits needs `0xff ^ MASK`, not `~MASK`: the int-promoted complement changes MSC's byte code.


## 2026-11-04 13:20 CET: Apply reviewed naming evidence

### What I did
- Fast-forwarded naming2 to main (6328a8d), then applied the reviewed names in gated function, initialized-data, static, communal, far-data and member/parameter batches.
- Applied 127 of the 256 approved entries, including Car.value, Part.color and game_make_car's value parameter; retained 129 placeholders where tested spellings missed hash/budget constraints or changed linked placement. The complete applied/skipped ledger and gate logs are in work/naming/PHASE2-RESULTS.md.
- Every committed batch passes 572/572 function checks, a clean byte-identical packed build, header/DGROUP checks and all 41 unit tests. The final DOS-native build is also byte-identical to orig/SR.EXE.

### What I learned
- The boundary-tie caveat in CLAUDE.md rules 4e/4f matters here: function gates passed while linked static/communal addresses moved. Linker-map diagnostics identified the affected candidates; restoring placeholders or using original-bucket/strict-window alternatives restored byte identity.
- The reviewed g_scene spelling survives the compiler budget in the complete initialized-data batch; the g_loc fallback was unnecessary. No tooling or declaration-layout changes were needed.

## 2026-11-05 10:30 CET: Second naming pass (Codex investigation, Claude review)

### What I did
- A Codex agent investigated every remaining placeholder (490 entries) and proposed 254 names with file:line evidence; an independent Claude review approved 205, changed 48 to more accurate or modest names (e.g. compose/restore buffers `g_wheelBuf`, not "save"; overlay areas, not "clip"; `make_bolt_overlay`, not `save_bolt`) and rejected one (`g_5568`). It confirmed three wrong existing names: `Car.weight` is the car's value, the matching `game_make_car` parameter, and `Part.h` is the span colour.
- Codex applied 127 of the 256 approved entries (all functions, initialized globals, far data and the three member/parameter corrections; 16 of 31 statics and 15 of 129 communals). The other statics and communals keep placeholders: their accurate names miss the hash window that fixes their address (trial ties moved 83 and 280 packed bytes while every function still matched).
- plans/unnamed.md is regenerated from the sources: 49 functions, 313 globals (85 initialized, 35 statics, 193 communals), 2 far-data labels and M_02B9A.ASM remain.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

### What I learned
- Function gates alone do not catch hash placement: only the linked build does. Communal windows are narrow enough that most descriptive names cannot be placed; those symbols are understood but stay placeholders.

## 2026-11-04 13:25 CET: Shared MASM PC constants

### What I did
- Merged main through 141fb0e, then added src/include/PC.INC using PC.H's names for shared facts, including MONO_MODE, GC_MASK_ALL, SEQ_ALL_PLANES, and the port and interrupt constants. Added assembler-only names for BIOS services/data offsets, Hercules configuration, and the register indices the assembler uses.
- Replaced hardware literals in 16 code assembler files and the two screen descriptor data files. Sources are assembled unchanged; INCLUDE PC.INC follows .MODEL, or precedes the explicit segment in the data files.
- Added C:\INCLUDE to the linked/native DOS build environment. The check backends already expose src/include as E:\ on INCLUDE. Extended sr headers to maintain recursive MASM .INC dependencies; tested recursion, case handling, cycles, dependency removal, idempotence, and agreement with PC.H's numeric macros.
- From empty work directories, sr check --all passed 572/572 and sr build reported Packed EXE vs orig/SR.EXE: IDENTICAL. Native dosbox.sh --make also reported IDENTICAL (SHA1 3a5e28b795313028b7e77d147122f41cb29de334). All 748 near calls are exact; sr headers --check, sr dgroup check, and all 43 unit tests passed. sr show crtc_wait_vsync confirms the original instructions. All 36 MASM objects retain identical segment bytes, publics, externs, and fixups compared with the pre-edit baseline.

### What I learned
- Verified VGA ports, register indices, map masks, write modes and status bits against IBM's [VGA/XGA Technical Reference](https://bitsavers.trailing-edge.com/pdf/ibm/pc/cards/IBM_VGA_XGA_Technical_Reference_Manual_May92.pdf), VGA Function pp. 2-41, 2-45, 2-47 through 2-88. Verified PC ports/BIOS data and interrupt services against the [IBM XT Technical Reference](https://www.minuszerodegrees.net/manuals/IBM/IBM_5155_5160_Technical_Reference_6280089_MAR86.pdf). The [Hercules GB101 owner's manual](https://minuszerodegrees.net/manuals/Hercules/Hercules%20Graphics%20Card%20GB101%20-%20Owner%27s%20Manual.pdf), pp. 8-14, confirms 3B8h is display mode control, 3BFh is the configuration switch, and bit 0 enables graphics mode. Names follow actual code use: CRTC's 201h is an extraction mask, while JOY's 201h is GAME_PORT.
- Literals retained: HERCGFX's packed 6845 timing words (3500h, 2801h, 2C02h, 803h, FC04h, 705h, 6406h, 7207h, 208h, 209h); CRTC's mode-specific packed masks/defaults/biases (11h, EFh, BFh, 201h, 1FFh, 3FFh, C8h, 64h); VIDDET's switch comparisons (0Fh, 1, 0Eh, 8, 3) and 5Ah writable-register probe; HERCDET's 3FFFh/7FFFh RAM probe and polling bounds; JOY's 400h polling bound and shifted two-button mask 3; graphics bank offsets, pitches, dimensions, and bit-manipulation counts. These are mode settings, probe values, or algorithm/geometry facts rather than independently established shared hardware constants; no speculative names were introduced for them.
- MASM 5.1 rejects arithmetic on the full 32-bit EGA far pointer (A2029). EGA_VRAM_PAGE1 therefore directly defines A000:2000 as a literal EQU. The sr-cli skill records this and the include-path/dependency behavior.

## 2026-11-05 11:00 CET: Shared MASM constants in PC.INC

### What I did
- src/include/PC.INC gives the assembler files the same names as PC.H for the same facts (interrupts, BIOS/DOS functions, BIOS data addresses, video segments, ports, register indices and bits), plus assembler-only facts (DOS get/set vector, keyboard interrupt, Hercules configuration port, extra CRTC/GC registers). 18 .ASM files `INCLUDE PC.INC`; MASM finds it like CL finds the headers, in `./sr build`, `./sr check` and the native MAKE build. Done by a Codex agent.
- Result: every asm object's code bytes and fixups unchanged; `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

## 2026-11-04 13:51 CET: Evidence-backed numeric modes

### What I did
- Surveyed numeric mode arguments and states in work/args/SURVEY.md; left scene, bet, sprite direction and undocumented video values numeric where the semantic evidence was incomplete.
- Added 15 constants in CURSOR.H, ARENA.H, WALKER.H and MAIN.H: four cursor controls, four arena allocation modes, two carhop requests and five original video menu choices. Updated selectors and literal callers, including arena mode forwarding through bitmap_grab, menu_grab and F_15BBE. No symbols renamed, data tables changed or code comments added.
- Fixed module-header macro validation in tools/sr/fileheaders.py, preserving duplicate-name, missing-owner and missing-function/data errors; expanded the tests and updated the sr-cli skill and CLAUDE.md.
- Kept each logical concern in its own commit and ran the gates after every batch. Merged main (already up to date), then reran all gates: 572/572 functions match, Packed EXE vs orig/SR.EXE: IDENTICAL, headers and DGROUP clean, 45 unit tests pass, DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL.

### What I learned
- Module header constants need no duplicate C definition; the checker already used them for include ownership but mistakenly required that duplicate. The rule is now documented in CLAUDE.md and .claude/skills/sr-cli/SKILL.md and covered by tests/test_fileheaders.py.
- Allocation modes 0/3 advance low/high persistent boundaries, while 1/2 push low/high markers. MARKS.C's release/pop behavior confirms their lifetime. The evidence table remains in work/args/SURVEY.md.
- Original video menu strings support names for choices 1 through 5; internal choice 6 and signed driver encodings need a separate audit.

## 2026-11-05 12:00 CET: Named mode and argument constants

### What I did
- Mode arguments whose meaning the callee defines are named in the owning module's header: `CURSOR_SHOW`/`CURSOR_SHOW_NOW`/`CURSOR_HIDE`/`CURSOR_HIDE_NOW` (cursor_ctl: the _NOW variants copy the cursor box back->front at once on double-buffered video), `ARENA_LOW_BASE`/`ARENA_LOW_STACK`/`ARENA_HIGH_STACK`/`ARENA_HIGH_BASE` (arena allocation modes, also forwarded by the bitmap and menu grab helpers), `WALKER_ENTER`/`WALKER_LEAVE`, and the video menu choices `VIDEO_CHOICE_CGA..TANDY`. `./sr headers --check` accepts constant definitions in module headers. Survey of other candidates kept by the Codex agent in its worktree. Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-04 14:11 CET: Decode and display picture libraries

### What I did
- Added tools/sr/pics.py and sr pic: list picture ids/dimensions/packed sizes/libraries; decode EGA and CGA; write a stdlib-only RGBA PNG or display inline using kitty graphics with 4096-byte base64 chunks. Added integer nearest-neighbour scaling, two-row cell cutting in WALKER.C order, vertical stacking of multiple PNG inputs, palette selection, and input/format error reporting. No src files changed.
- Derived the format from DISK.C's pic_index/read_index/load_pic/unpack_pic, RLE.ASM, MEM.C, VIDEO.C, MASKBLT.ASM, MENU.C and WALKER.C. RLE dictionary arrays, display palettes and the CGA id map are read from their C initializers. LIB1/LIB2 contain 9+268 EGA entries; LIB1C and orig/extra/LIB2C contain 4+273 CGA entries. All 277 ids in each mode decode successfully, including four mask-only pictures per mode.
- Rendered and inspected the Street Rod title, California Dreams logo and AD 1989 credits: text is readable with no shearing. Opened a background Rex pane beside the current codex-pcinc pane (inherited REX_BLOCK was stale; identified the live pane via rex block ls and capture). The walker-sheets pane displays ids 1142 and 1141, --cells 6 11 --scale 3; both commands exited successfully. Each sheet is 240x82, giving eleven 40x41 cells and a 1320x123 scaled row. The figure shows a blonde woman on roller skates carrying a tray, facing right in the first sequence and left in the second, with turning poses at the end.
- All 53 unit tests pass, including known title/walker RGBA checksums, all 554 resource decodes, library framing, RLE edge cases, id mapping, cell order, scaling/stacking, PNG CRCs/scanlines, kitty chunk reassembly and output/errors. sr check --all: 572/572; rm -rf work followed by sr build: Packed EXE vs orig/SR.EXE IDENTICAL, 748/748 near calls exact. sr show rle_decode also inspected. No push or merge.

### What I learned
- Directory bytes 7..22 are dictionary tokens, not RGB palette entries. Picture color indices use VIDEO.C's g_palette by default; DASH.C changes to g_introPalette for the credits. The command exposes explicit palette choices rather than executing scene-specific animation/palette scripts. CGA uses bright cyan/magenta/white by default, with the alternate bright green/red/yellow palette available. Tandy output remains outside this command's supported modes.
- Packed spans include the leading BC sentinel but exclude the trailing sentinel; adjacent entries share that boundary byte. Word +4 is unused by the game and does not describe the actual expanded length of mask-only EGA pictures. Sprite transparency comes from the complement of the OR of the EGA planes, or the stored CGA mask (all visible mask pixels in the resources are 0 or 3). Details and examples are recorded in the sr-cli skill's Picture resources section.

## 2026-11-05 13:00 CET: `./sr pic` decodes the game's pictures

### What I did
- New `./sr pic` (tools/sr/pics.py): reads the resource libraries in orig/freeware (LIB1/LIB2 and the CGA variants), decodes a picture exactly as the game's loader and RLE unpacker do (EGA planar and CGA forms), and writes a PNG or prints it inline with the kitty graphics protocol (`--list`, `--scale`, `--cells` to cut sprite sheets like `cut_sheet`, palettes). All 554 pictures decode. Done by a Codex agent.
- Used it on the walker's sprite sheets: two 11-frame rows of a blonde woman on roller skates carrying a tray, facing right then left. The drive-in figure is a roller-skating carhop.

## 2026-11-05 13:30 CET: The walker is a carhop

### What I did
- The drive-in figure's sprite sheets (pictures 1141/1142) show a blonde woman on roller skates carrying a tray, so the module is CARHOP.C again: `carhop_skate` (was walker_walk; skates in from the left or back out), `carhop_save_bg`, `maybe_carhop` (BOBS.C), `CARHOP_ENTER`/`CARHOP_LEAVE`, and the state static `g_carhop` (-1 none, 0 may come, 1 present; chosen to keep its MSC hash slot, key 99).
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

### What I learned
- The game's pictures are evidence too: the review's modest "walker" was right on the code alone, and the sprite settled it.

## 2026-11-05 14:00 CET: What produced `bits.p + 0x46 * pitch`?

### What I did
- Investigated the three `g_pageBmps[2]->bits.p + 0x46 * pitch` sites (CARPIC.C make_bands and make_frames, the carhop's grab_strip) with 27 candidate source forms compiled by MSC 5.1. Exact at all three: the literal, a #define or enum, a row macro, either operand order, long variants, and the structural forms `sizeof` of a 70-byte record, a constant member offset (`offsetof` after `char backdrop[70]`) and a typed 70-byte row pointer. Different: any runtime source of 70 (struct field, bitmap height, table, helper call, sub-bitmap header) and `70U` (`mul` instead of `imul`).
- The rest of the evidence favours a fixed 70-row reserve at the top of a reusable page (the backdrop is at most 70 rows, `70 - g_spriteBottomCut`; other 70s are the car display height; TIRES.C reuses the same pages differently; no declarations or consumers back a record). No source changed; the geometry pass names it `BACKDROP_ROWS`, recorded as a choice among identical forms.

### What I learned
- MSC canonicalizes `70 * p` and `p * 70` to `mov ax,70; imul p`, so operand order says nothing here; signed vs unsigned does (`imul` vs `mul`).

## 2026-11-04 14:20 CET: Name struct members from typed-use evidence

### What I did
- Surveyed all 69 named project types and two anonymous nested records: 411 members, with MSC offsets/types and all 6,289 typed C member accesses, read/write/address classification, function/file:line evidence and aggregate/raw-layout contexts. The report is work/members/SURVEY.md; original source coordinates are explicitly tied to its baseline commit. All 58 C sources were covered; the only 74 unresolved accesses belong to MSC REGS/SREGS, not project records.
- Named or split 155 original fields across 29 structs in 18 small commits. Colors.c became five char fields (text, textBg, fill, line, frame), so those 155 original fields yield 159 named fields. Named picture metadata, parts/car/spec/art coordinates, rival statistics, sound/vibrato state, hot spots/timers/animation state, drivetrain/RPM bands, video/cursor/page state, player inventory, road positions, body-paint state, glyph rows and wiring state. Kept 256 reviewed members, including descriptive existing names and ambiguous/unused placeholders.
- Retained overloaded Cmd operands, Hot.f6 (hit code versus link index), Win popup/menu geometry views, Pair coordinates/control values, the heterogeneous Eng.f34 array, unproven body-feature overlays and unused/reset-only fields. Glyph.pad is live row data: EGA text drawing reads its eight bytes through a byte pointer, so only its name became rows. No locals, parameters, globals, functions or type tags renamed; no source comments added or changed.
- Every one of the 18 naming commits passed ./sr check --all (572/572), a fresh-layout ./sr build (Packed EXE vs orig/SR.EXE: IDENTICAL), headers/DGROUP checks and all 43 pre-integration unit tests. The real MSC compiler verified 475 sizeof/offsetof checks across all 69 named types with zero mismatches. A separate scalar/pointer leaf-layout audit verified unchanged sizes, alignments and offsets, including the five-byte Colors split and the 80-byte Style records; all other changes are pure member-token renames.
- Integrated the manager-requested main updates (mode constants/header macro ownership, plus ./sr pic through 591265d). Resolved 62 adjacent member/mode conflict blocks in 20 files by combining disjoint token edits, preserving both names and constants. The complete merged-tree gate and final DOS-native build outputs are recorded in work/members/g19-*.log and work/members/final-dosbox.log.

### What I learned
- Value flow prevents plausible but wrong names: Spr12.b10/b11 and State8272.m8272/m8274 locate the saved body-paint region and shade overlays, not the engine. Hot.f6 and Win.w/h have contradictory interpretations across record views, so a one-role rename would mislead.
- The five Colors slots are accessed only by constant indices; removing the obsolete initializer-array braces preserves both code and initialized bytes. Style palettes remain arrays because their indices vary; no packed/overlaid record was retyped.
- Survey-only local follow-ups include EDIT.C edit_line pos (saved fill colour), GARAGE.C tune_timing gear (ignition timing), DISK.C disk_read_index height (largest packed byte count), DISK.C disk_pic_size x (flags parameter), and EGASPR bitmap/frame pointer locals named height/key/box. They need an independent stack-layout pass.

## 2026-11-04 14:27 CET: Name shared screen geometry

### What I did
- Named EGA_PITCH in PC.H/PC.INC at confirmed planar-byte strides; named BACKDROP_ROWS in CARPIC.H at the scratch reserve and car-display height, with its equivalent-source-form choice recorded in docs/RECONSTRUCTION.md.
- Named SCREEN_W/SCREEN_H, SCREEN_BODY_H/STATUS_BAR_H and DASH_Y/DASH_H at confirmed screen bounds, status-strip geometry and dashboard origins/buffer heights. No function, global or struct-member names changed; no source comments were added.
- Surveyed other recurring geometry in work/geom/SURVEY.md, including numeric coincidences left literal: CGA word counts, interleaved EGA reveal steps, wheel sentinels, road distances, asset sizes and mirror clip coordinates.
- Merged main and reran every gate: 572/572 functions match; clean packed build and DOS-native MAKE are IDENTICAL to orig/SR.EXE; header and DGROUP checks clean; all 53 unit tests pass.

### What I learned
- SCREEN_H and DASH_H were header guards as well as useful dimension names; their guards must be SCREEN_H_INCLUDED and DASH_H_INCLUDED. Shared numeric definitions in PC.H must also be defined in PC.INC (test_shared_pc_constants).
- DRIVE's previous-wheel value 100 and DASH's b != 100 are a sentinel, distinct from the dashboard origin 100. EGACOPY's 200-byte row step is five EGA scanlines, distinct from the screen height. These distinctions are recorded in work/geom/SURVEY.md.

## 2026-11-05 15:00 CET: Named screen geometry

### What I did
- `EGA_PITCH` (40, the EGA row stride per plane) and `SCREEN_WIDTH`/`SCREEN_HEIGHT` (320x200) in PC.H; `BACKDROP_ROWS` (70, the reserve at the top of the scratch page) in CARPIC.H, recorded in RECONSTRUCTION.md as a choice among identical forms; `SCREEN_BODY_HEIGHT`/`STATUS_BAR_HEIGHT` (190/10) in SCREEN.H; `DASH_Y`/`DASH_HEIGHT` (100/100) in DASH.H. Done by a Codex agent; the first spelling (`SCREEN_H`, `DASH_H`) collided with the SCREEN.H/DASH.H include guards, so the dimensions are spelled out and the guards stay standard.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-04 14:33 CET: Name video mode codes

### What I did
- Defined VID_EGA (-2), VID_CGA (-3), VID_HERCULES (-4), and VID_TANDY (-6) in MAIN.H, the header of MAIN.C, which owns r_vid; supplied the corresponding MASM equates in MAIN.INC for the far screen descriptors.
- Replaced format literals in mode comparisons, switches, bitmap flags, bitmap constructor arguments, and screen descriptors. Preserved alignment masks, generic bitmap flags, and menu result codes.
- Verified 572/572 function matches, a clean byte-identical packed build, clean header and DGROUP checks, 53 passing unit tests, and a byte-identical native DOSBox MAKE build.

### What I learned
- video_init assigns front->flags to r_vid.mode: its EGA path uses four 1-bit planes, its CGA default uses 2 bits per pixel, and its Tandy path uses 4 bits per pixel. init_screens supplies the Hercules and hardware selection evidence.
- Negative format constants preserve the bytes of the previous hexadecimal signed-byte assignments and MASM DB screen descriptors; both build paths remain identical to orig/SR.EXE.

## 2026-11-05 16:00 CET: Named video mode codes

### What I did
- `VID_EGA` (-2), `VID_CGA` (-3), `VID_HERCULES` (-4), `VID_TANDY` (-6) name the pixel format held by `r_vid.mode` and the screen bitmaps' `flags` (EGA 4 planes 8 px/byte; CGA 2 bpp; Hercules mono at B000; Tandy 4 bpp 2 px/byte), in MAIN.H for C and PC.INC for the assembler; all ~190 comparisons and the flags assignments use them. Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-04 14:41 CET: Correct reviewed struct member names

### What I did
- Resolved the in-progress main merge across 12 files, retaining both diary histories, main's video/geometry constants and the branch's member names; committed as 73f6945. Conflict-marker and whitespace checks were clean, and all required gates passed before the review corrections.
- Corrected Spec/Car.rank, Dlg.fillsLeft, Racer/Look.sticker, Spr12/State8272 sticker coordinates and Video.hoodBg. Confirmed the optional Hot.palIndex and State7EAA.otherCars corrections from their palette and car-list consumers. Updated only existing stale member references and the wrong appearance comment; no source comments added, parameters or globals renamed.
- Both the merged baseline and corrected tree passed 572/572 function checks, fresh-layout byte-identical packed builds, clean headers/DGROUP checks, all 53 unit tests and byte-identical native DOSBox MAKE builds. Logs: work/members/merge-*.log and work/members/fix-*.log. A source audit confirmed only member declarations/accesses and existing comment corrections changed.

### What I learned
- Rank is an ordinal 0..8 used for score lookup, class buckets, rival distance and won-car comparisons; each refuel fill adds three gallons. Appearance bits 8..12 and the $5 overlays represent stickers, distinct from the $20 paint scheme. These corrections now live in src/include/STRUCTS.H and their consumers.
- The persistent hood background stores picture 0x42d with installed parts, Hot selects one Colors palette, and otherCars omits currentCar; their names now express those roles.

## 2026-11-05 17:00 CET: Struct member names

### What I did
- A Codex agent surveyed 71 records and named 155 struct members from their uses (e.g. `Colors` = `text`, `textBg`, `fill`, `line`, `frame`, split from `char c[5]` because every access used a constant index; the hot-spot record's `kind`, `palIndex`, `textOffset`, `next`); 256 members keep placeholders. An independent review confirmed 143 and corrected the rest: six "paint" names are stickers (the game's "Stickers - $5" overlays, distinct from "New paint job - $20"), as was the older `Look.shade`; `modelKind` is a 0..8 `rank`; `gallonsLeft` counts 3-gallon `fillsLeft`; `Video.scratch` holds the garage hood picture (`hoodBg`). Comments naming old members were updated.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-04 14:42 CET: Name critical-error values

### What I did
- Named the int 24h device-error flag, retry permission, and write-protect/not-ready/paper-out codes in PC.H, with matching PC.INC equates. Reused MSC 5.1 DOS.H's _HARDERR_RETRY and _HARDERR_ABORT in sys_crit_error. No symbol or member renames and no source comments added.
- Verified 572/572 function matches, a clean byte-identical packed build, clean header and DGROUP checks, 53 passing unit tests, and a byte-identical native DOSBox MAKE build.

### What I learned
- toolchain/msc51/INCLUDE/DOS.H:146-152 supplies the resume actions; BIN/README.DOC and BIN/CL.HLP contain no _harderr documentation (CL.HLP covers compiler flags).
- Microsoft's MS-DOS 3.3 Programmer's Reference section 1.11.2 (https://www.pcjs.org/documents/books/mspl13/msdos/dosref33/) confirms AH bits 7/4 and DI codes 0/2/9. CRIT_DEVICE_ERROR is broader than character-device-only: AH bit 7 can also indicate a bad in-memory FAT. The handler's messages and retry/exit branches agree with these meanings.
- Tree-wide searches found no other uses with critical-error semantics. PC.H's numeric constants require matching PC.INC equates, enforced by test_shared_pc_constants.

## 2026-11-05 17:30 CET: Named critical-error values

### What I did
- sys_crit_error (the `_harderr` handler) uses MSC 5.1's own `_HARDERR_RETRY`/`_HARDERR_ABORT` from <dos.h> for `_hardresume`, and PC.H/PC.INC name the int 24h facts: `CRIT_DEVICE_ERROR` (0x8000), `CRIT_RETRY_ALLOWED` (0x1000), `CRIT_WRITE_PROTECT` (0), `CRIT_NOT_READY` (2), `CRIT_PAPER_OUT` (9). Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

## 2026-11-04 14:51 CET: Readable animation script commands

### What I did
- Named the thirteen animation opcodes, four box-position modes and wait copy mode in VIDEO.H; applied them to anim_set_box, anim_run and all 44 g_animScripts records.
- Renamed Box coordinates to sx/sy/dx/dy while preserving the 28-byte record and 44-byte Obj stride. Retained casts to the 20-byte Blt.
- Merged main (already current), then verified 572/572 functions, a clean build with packed EXE IDENTICAL, headers, DGROUP ownership, all 53 tests and DOS-native EXE IDENTICAL.

### What I learned
- Box shares Blt's first six words, but its 16-byte tail has no established member meanings in current accesses; substituting the smaller Blt would change the object array layout.
- The DGROUP C-data reader cannot parse typedef member types. Cmd.op/sub therefore retain explicit signed-char storage, as allowed by the task; enum values live in VIDEO.H.

## 2026-11-05 18:00 CET: The animation scripts read as code

### What I did
- `enum AnimOp` names the op codes anim_run interprets (`ANIM_BLIT`, `ANIM_BLIT_MASKED`, `ANIM_HOOK`, `ANIM_STOP`, `ANIM_JUMP`, `ANIM_LOAD_IMAGE`, `ANIM_SET_BOX`, `ANIM_SET_SIZE`, `ANIM_WAIT`, `ANIM_COPY_TO_FRONT`, `ANIM_TILE_CAR`; ops 1 and 3 do nothing in anim_run and are `ANIM_NOP_1`/`ANIM_NOP_3`), `enum BoxSet` the box updates (`BOX_KEEP`, `BOX_SET_SOURCE`, `BOX_SET_DESTINATION`, `BOX_SET_BOTH`) and `enum AnimWaitMode` the wait variants; the `g_animScripts` initializer uses them. `struct Cmd`'s op/sub stay 1-byte chars (MSC 5.1 enums are int-sized). `struct Box`'s first fields are the blit's `sx`/`sy`/`dx`/`dy` (source frame in the sheet, destination on screen); it stays a 28-byte record. Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-04 14:50 CET: mouse record evidence and tail padding

### What I did
- Surveyed every mouse global reference and neighboring initialized data in SYS. Compared nine struct boundaries with the real compiler and all-file gates; recorded the experiments in work/mouse/REPORT.md.
- Folded DS:05E2..05F2 into struct Mouse g_mouse; its compiler tail padding supplies the previously unreferenced zero byte at 05F3. Kept neighboring input flags, unused 05F4 words and clocks separate. Updated all six C consumers; no assembly source references these values.
- Verified 572/572 functions, the packed build and DOS-native MAKE byte-identical, 53 unit tests OK, and headers/DGROUP clean before merging main. The final gates are repeated after that merge.

### What I learned
- The nine mouse fields are undecidable as separate globals versus a struct: both forms have identical code and no budget effect. This is documented as a natural reconstruction choice in docs/RECONSTRUCTION.md.
- Folding g_tick while keeping earlier deadlines separate reverses long-comparison operands in SYS and DRVTICK. The tested clock extensions are ruled out.
- A trailing unreferenced initialized char can represent struct padding; retaining it separately after folding inserts extra bytes. Added this evidence to the msc51 recipe book.

## 2026-11-04 14:52 CET: mouse gates after merging main

### What I did
- Merged main at 9d23432, kept both diary entries and the critical-error constants, and repeated all required checks on the combined source.
- Result: 572/572 functions and every file gate pass; clean packed build and native DOSBox MAKE IDENTICAL; headers/DGROUP clean; 53 unit tests OK. Final evidence is in work/mouse/REPORT.md.

### What I learned
- The mouse record and main's critical-error changes compose without any byte changes.

## 2026-11-04 14:55 CET: mouse gates after animation merge

### What I did
- Main advanced during final verification; merged its 7e49ad5 animation work, preserving both diary sides and the automatically merged struct definitions.
- Repeated all gates again: 572/572 functions, clean packed build IDENTICAL, native MAKE IDENTICAL, headers/DGROUP clean, 53 tests OK. Updated work/mouse/REPORT.md with the final merged state.

### What I learned
- Main's animation naming changes also compose with the mouse record without byte differences.

## 2026-11-05 19:00 CET: Rule: enum or #define

### What I did
- CLAUDE.md: a closed set in int-sized things is an enum and the holder is typed with it; a closed set in char fields uses enum constants with char (or typedef'd char) members, because MSC 5.1 enums are int-sized; lone values are #defines. File-local constants at the top of the file, module interface constants in its header, PC facts in PC.H/PC.INC.

## 2026-11-04 15:07 CET: MSC 5.1 runtime constants

### What I did
- Inventoried C runtime call sites throughout src/ against the actual toolchain/msc51/INCLUDE headers, and reviewed every MASM EXTRN (none refer to runtime routines). Detailed per-call inventory: work/msconst-inventory.txt; header/name cross-check: work/msconst-audit.py.
- Replaced 11 _dos_open access flags with O_RDONLY (nine in DISK.C, two in SAVEGAME.C) and three _dos_creat attributes with _A_NORMAL (two in DISK.C, one in SAVEGAME.C). FCNTL.H defines O_RDONLY as 0x0000; DOS.H defines _A_NORMAL as 0x00. Added fcntl.h to both system-include blocks; sr headers preserves these includes and needs no extension.
- Runtime inventory by family: DOS (_dos_open, _dos_creat, _dos_close, _dos_read, _dos_write, _dos_getdrive, _dos_getvect, _dos_setvect, _harderr, _hardresume, _disable, _enable, int86, int86x, intdos, segread); BIOS (_bios_equiplist); allocation (_fmalloc, _ffree); console (getch, kbhit, inp, outp); strings/memory (memcmp, memcpy, memmove, memset, movedata, strcat, strchr, strcmp, strcmpi, strcpy, strcspn, strlen, strncpy); formatting/conversion (sprintf, puts, atoi, itoa, ltoa); math/utility (abs, fabs, sin, cos, _rotl, _rotr); time (time); process (exit); files (unlink). Existing FP_SEG/FP_OFF and min/max macros were also reviewed.
- Checked the requested negative cases: no O_WRONLY/O_RDWR, other attributes, S_IREAD/S_IWRITE, SH_*, BIOS keyboard/disk/serial/printer/time commands, stdio seek/buffering/EOF constants, signal constants, process modes, heap-status constants or graph calls apply. _HARDERR_RETRY/_HARDERR_ABORT are already named. errno is assigned from _doserrno, never compared to an error literal. No literal null-pointer argument occurs at an MSC runtime call. exit(5) has no corresponding exit-status constant in these headers. Read/write sizes, numeric conversion radices, rotate counts and device-register values retain their meanings; no runtime header names apply to them.
- Gates: sr check --all 572/572; clean-layout sr build packed EXE IDENTICAL, 95/95 objects and 748/748 near calls exact; sr headers --check and sr dgroup check clean; 53 unit tests pass. No prototypes changed code, and no symbols were renamed.

### What I learned
- MSC 5.1's fcntl.h supplies plain open-flag constants without prototypes, so including it at the existing dos.h call sites preserves code generation. The existing header tool already preserves arbitrary system includes in their original order.
- Final integration validation: git merge main was already up to date; repeated sr check --all (572/572), a clean-layout build (packed EXE IDENTICAL), header/DGROUP checks and all 53 unit tests successfully. DOS-native dosbox.sh --make also produced IDENTICAL SR.EXE (SHA-1 3a5e28b795313028b7e77d147122f41cb29de334).

## 2026-11-04 15:11 CET: load_pic results and picture markers

### What I did
- Typed load_pic and disk_unpack_pic's res as enum LoadResult, named all four outcomes, and made disk_load_list's allocation-failure comparison explicit.
- Defined PIC_MARK and NOT_IN_VRAM in DISK.C after verifying all vramPlane accesses stay in that file. Kept the member byte-sized and added no code comments.
- Verified the full DISK.C module and all 572 functions match; res remains a word at [bp-24]. Clean sr build and DOS-native MAKE both produce an EXE byte-identical to orig/SR.EXE. Header and DGROUP checks and all 53 unit tests pass.

### What I learned
- The three load_pic callers respectively retain the result to clear the shared-buffer reference after LOAD_READ, ignore it while caching, and test LOAD_NO_MEMORY while loading a list.
- This change confirms the existing CLAUDE.md enum rule without requiring a new code-generation rule: the enum local preserves its stack slot and comparison bytes.

## 2026-11-05 19:45 CET: CURSOR_EGA_SAVE derived, in CURSOR.H

### What I did
- The cursor's EGA background save area (A000:1F40) is now `#define CURSOR_EGA_SAVE (EGA_VRAM + EGA_PITCH * SCREEN_HEIGHT)` in CURSOR.H: the first byte past the visible 8000-byte page 0 in each plane, in the gap before page 1 at A000:2000. It must be video memory because cursor_draw restores it with the write-mode-1 latch copy (egalatch_to_page). MSC folds the expression to the same constant.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, tests OK.

## 2026-11-04 15:33 CET: Racer table indices and status values

### What I did
- Added racer-table constants in DISK.H and replaced only the associated numeric literals; kept all symbols and the char status field unchanged.
- Verified all 572 functions match, clean linked and DOS-native builds are IDENTICAL, headers and DGROUP pass, and all 53 unit tests pass.

### What I learned
- HOT_DATA contains 22 18-byte racer records; slot 21 is named The King and is excluded from the ordinary rival pool. SAVEGAME.C assigns seven racers each from rank pools below 3, 3 through 5, and 6 upward.
- Slot 10 is Steve, an ordinary middle-group rival, also reused by cop_appears for the chase shift penalty. RACER_COP_ENTRY describes that use without claiming a dedicated police-only slot.
- RACE.C removes rivals after losing their car with status 0xff; BOBS.C excludes status -1 from its pool, marks selected rivals 1, and lists only 1. Status 0 is initially not met and also hides the King until available. This evidence is recorded with the constants in the commit body.

## 2026-11-05 20:30 CET: Racer table constants

### What I did
- DISK.H: `NUM_RACERS` 22 (the 18-byte racer records, 396 bytes in the save game), `RACER_KING` 21 (named "The King" in HOT_DATA; ending, rematch, king_status, reset on defeat), `RACER_COP_ENTRY` 10 (an ordinary rival, "Steve", whose entry cop_appears borrows for the chase's shift penalty; the police drivetrain itself is car spec 25), `RACERS_PER_GROUP` 7 (init_racers fills three rank groups of seven), and `enum RacerStatus` (`RACER_EMPTY` -1, `RACER_NOT_MET`, `RACER_MET`) for the char field Racer.status. Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-05 21:00 CET: The racer table lives with the racer code

### What I did
- `struct Racer av7ff8[NUM_RACERS]` is defined in BOBS.C (most racer logic) instead of DISK.C, and its constants (`NUM_RACERS`, `RACER_KING`, `RACER_COP_ENTRY`, `RACERS_PER_GROUP`, `enum RacerStatus`) and extern moved from DISK.H to BOBS.H. DISK.C still mentions the table first in link order (its save table g_dataBlocks), so LINK's communal slot is unchanged. CLAUDE.md rule 4e now says a communal may be defined in any file at or after its first mention.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup checks clean, tests OK.

## 2026-11-05 21:30 CET: The keymap tables are their own data module

### What I did
- HERCDET.ASM held the three Hercules/mono detection routines and, by its data run, the keymap tables at DS:5580 (`g_keyMapData`, and `g_keyMaps` at odd DS:579F, so assembler). Nothing references across the two, and the tables (near pointers) produce no MZ relocations, so the relocation evidence never tied them to that object. They are now a data-only KEYMAPS.ASM, linked directly before HERCDET.OBJ, written with ordinary MASM directives; one file versus two is undecidable from the EXE (RECONSTRUCTION.md). Done by a Codex agent.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup/order checks clean, tests OK.

## 2026-11-05 22:30 CET: Phase 3 names (hash-fitting spellings), paused for the structs

### What I did
- A Codex agent searched honest alternative spellings for the reviewed statics and communals whose first names missed their hash windows (30+ candidates each, checked with `./sr fit`, confirmed by full builds), and for names kept only for length; 146 renames were applied. Phase 3 was paused so the struct grouping comes first. The branch conflicted with 33 files on main, so the 146 renames were replayed onto main with `./sr rename` from its ledger.
- The renames only work as a set: applying the 114 full-length ones alone broke 28 functions and 32 bytes, because the 32 cramped names (e.g. `g_gar`, `g_pad`, `g_bIdx`) repay the per-file name budget the others spend in GARAGE.C/VIEW.C and some spellings share hash buckets. The 32 are listed in plans/cramped-names.md, to be renamed properly once the structs free budget.
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup/order checks clean, tests OK.


## 2026-11-04 17:09 CET: Group evidenced global state into records

**What I did**

- Merged main 2223600 and mapped candidates by DS address after the global-renaming pass. Implemented 14 non-overlapping records: Hooks, RenderRows, TireWork, StatusText, BodyPoints, MirrorRoad, MirrorEdges, BuildingRows, ListDialog, TimerPriorities, PartSelection, InputState, Wager and the existing Blt for the ad packet. This replaces 114 symbols with 14 objects (100 fewer symbols); typed driver tables share Hooks and installers use direct assignments.
- Restored the cramped spellings as members: g_fnShow becomes g_videoOps.show and g_xTextR becomes g_statusLine.textEndX. Kept unrelated cramped names outside the records.
- Selected narrow groups, excluded the backdrop reveal-height word from BuildingRows, and skipped the mixed startup flags. Added one reconstruction-choice entry per group. Kept private type definitions in VIEW.H, TIRES.H and SYS.H.
- Committed four focused tooling/test fixes separately: named callback fields, int-sized enum members, static ownership without a symbol-count threshold, and fallback data regions inside source aggregates. The overlapping-source-global check stays strict.
- Each group passed 572/572 functions, a clean packed build IDENTICAL, header/DGROUP checks and the unit suite. Final suite: 58 tests OK. Real MSC confirmed 602 sizeof/offsetof checks across 82 shared/owner-header types; native DOS MAKE is IDENTICAL (SHA1 3a5e28b795313028b7e77d147122f41cb29de334). Full mapping and logs are in work/structs/impl/REPORT.md. No work/ artifacts committed, no push or merge back to main.

**What I learned**

- Members remove hash and external-name budget constraints, but the one remaining object name must fit its new neighbor window; deleting the old members can widen that window.
- Separate globals and a natural record can both match. The callback copies support Hooks; the other chosen records remain explicit reconstruction choices.
- Unused word-sized storage is not automatic word-alignment padding. Preserve unknown/reset-only fields without inventing semantic names, and do not let recovered fallback ranges reintroduce their old symbols.

Tooling lessons are recorded in [sr-cli](../.claude/skills/sr-cli/SKILL.md); the type-header ownership rule is in [CLAUDE.md](../CLAUDE.md).


## 2026-11-04 18:02 CET: Correct the independent struct review findings

**What I did**

- Split generic text endpoints into TextEnd g_penPoint at 824C and narrowed StatusText to status-line state at 8250. Extracted the right-side scenery cursor at 8634 from mirror Y rows at 8638. InputState now contains only any/key; restored 05D4..05DB as separate placeholder globals, preserving the 32767 sentinel and drive-reset writes.
- Applied the reviewed member names: RenderRows.lineWidth, TireWork.liftBuf/liftHeader/wheelX1. Replaced the five misleading object spellings with g_sysTimerPriorities, g_slotSelec, g_mirGeom, g_raceWagerTerms and g_rightSideTerrainRow (separate from g_mirYProj).
- Tested four natural array variants with real MSC and full builds; all were byte-identical. Adopted g_yBuildingTopArray[15], g_mirYProj[8] and two eight-element MirrorEdges arrays. No short arrays plus terminal scalar fields remain in those reviewed projection groups.
- Used BetKind constants at wager-kind writers/tests and typed the corresponding locals/parameter. Changed PartSelection's unused word to int, corrected the old hook comment, and made ADS function-pointer calls consistent.
- Merged main (already up to date), checked conflicts, and passed 572/572 functions, clean packed build IDENTICAL, headers/DGROUP/order and 58 unit tests. Native DOS MAKE is IDENTICAL; real MSC verified 586 size/offset checks across 81 shared/owner-header types. No further tooling fixes were needed. Current evidence: work/structs/fixes/REPORT.md.

**What I learned**

- Byte identity permits broader groups than a source reconstruction should use. Trace each field's writers/readers to separate generic text output, status-line state, forward/mirror scenery cursors and drive-reset words.
- Array writers can reveal the true capacity where reconstruction introduced terminal scalar members. The compiler-verified full-array result is recorded in the [recipe book](../.claude/skills/msc51-recipe-book/SKILL.md); explicit unused/reset-only fields remain distinct from automatic padding.

## 2026-11-06 00:00 CET: Natural records for grouped globals

### What I did
- A Codex survey probed 60 candidate groups with the real compiler (1 supported, 48 undecidable-natural, 11 ruled out by operand order or gates). Implemented 14 records from the best single-role groups: the video driver's `struct Hooks g_videoOps` (the installers copy it as one struct; now `.copy`, `.drawLine`, `.fillSpan`...), and natural records for render rows, the tire-change sprites, the status line, the car body points, the mirror and road rows (as full 15/8-entry arrays), the list dialog, timer priorities, part selection, input, the wager and the ad blit. An independent review confirmed about 100 of 114 members; its findings were fixed: mixed groups split (the last-text-end position, the shared right-scenery cursor, the drive words), wrong names (`liftBuf`, `lineWidth`, `wheelX1`), misleading object names replaced by honest hash-fitting ones, `BetKind` constants used, `int unused` for an unreferenced word. Four tooling fixes with tests (function-pointer members, static ownership, fallback data, enum member sizes).
- Result: `./sr build` IDENTICAL from an empty work dir, native MAKE IDENTICAL, `./sr check --all` 572/572, headers/dgroup/order checks clean, tests OK.

### What I learned
- Members carry no hash or budget constraint, so records turn hash-blocked globals into freely nameable members; only the object name must fit.

## 2026-11-04 18:17 CET: Subsystem source layout

### What I did
- Proved the 87-character SRENV.BAT INCLUDE line with native DOSBox-X MAKE on a scratch copy of the decided layout before moving files; packed EXE was IDENTICAL.
- Moved 197 files with git mv in a separate commit, preserving their bytes. All names under src are uppercase DOS 8.3; sources and same-name headers share their subsystem folder, shared headers are in SYS, assets and their headers in DATA.
- Updated MAKEFILE paths and dependencies, source/header indexing, check-job header staging, build environment, rename tooling, DGROUP, picture loading, backend comparison, tests and documentation. Object names and SR.LNK are unchanged; C, ASM, H and INC contents are unchanged.
- From empty work directories: ./sr check --all passed 572/572 and ./sr build ended IDENTICAL. Native ./dosbox.sh --make also ended IDENTICAL. Headers, DGROUP ownership and order were clean; 61 unit tests passed, and ./sr show main succeeded.
- Repeated all required gates in a fresh clone under work/fresh: 572/572, host and native EXEs IDENTICAL, headers and DGROUP clean, 61 unit tests passed.
- Full emucmp passed IDENTICAL: 623 source/flag jobs, 138 sweep jobs and 49 probes; zero substantive differences. Nothing under work is committed; no push or merge.

### What I learned
- DOSBox command sequences need call SRENV.BAT to continue to MAKE after the batch file. MASM needs INCLUDE even for headers beside its source. The shared environment and staging rules are recorded in .claude/skills/sr-cli/SKILL.md.
- A recursive filename index lets compiler jobs stage unchanged headers either flat for E: or in their original folders for the linked build; duplicate filenames are rejected so search order cannot silently select another header.
