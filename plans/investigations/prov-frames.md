# Provisional automatic storage investigation

Purpose: determine whether ordinary MSC 5.1 source accounts for four unused
four-byte slots adjoining blit packets and vi_anim_hook's six-byte inner slot.

## Source and context before experiments

- gx_draw_bolt (src/GAME/GARGFX.C:278-312) gets the selected bolt position,
  restores its background through move, optionally allocates a shifted sprite,
  animates it, and pops one or two arena bitmaps. pict/saved own the temporary
  views; px/ypos/height/flag describe geometry. unused has no use.
- gx_draw_part (same file:397-447) gets the selected engine/manifold/carb position,
  shifts the picture to the pixel grid, writes it into hoodBg unless raised,
  animates it if enabled, and releases temporary bitmaps/bolt overlay. w, x,
  height, ypos and mask describe geometry; z is assigned zero but never read.
- gx_erase_part (same file:452-479) copies the raised part's rectangle from
  hoodBg into back and, when enabled, front. w/h and px/py describe the
  rectangle; unused has no use. The flag parameter is not read.
- sc_picture (src/UI/SCREEN.C:208-230) loads a picture, optionally clears the
  background, crops the blit height, copies it to back/front, resets placement,
  and pops the loaded bitmap. p is the arena bitmap; pkt is the request.
  unused has no use. The int function has no explicit return.
- vi_anim_hook (src/VIDEO/VIDEO.C:326-351) handles object id 2: starts a pump
  countdown and sound for a nonzero command operand, otherwise decrements the
  countdown, blinks one or two short horizontal strokes, then stops the sound.
  old receives x_colors.line but is never read. save is not read or written.
  x_colors.line is not restored on the drawing path.

Callers: GARAGE.C's ga_hood calls the three graphics functions while
installing/removing parts and tightening bolts (1196-1297); gx_draw_parts and
gx_draw_bolts also call the draw functions. sc_picture is used by scene/title,
newspaper, fame, save/load and garage code; sc_picture_at sets placement and
forwards. anim_run calls vi_anim_hook for ANIM_HOOK and continues or stops the
script according to its result (VIDEO.C:246).

Callees inspected: the blit packet is handed through g_videoOps.copy,
maskedBlit or shiftBlit; driver clipping reads the six-word prefix and can
use the additional fields in Blt. Blt is 20 bytes, Blt16 16, Clip 12, Box 28
(BITMAP.H:29-97; VIDEO.H:21-28). Colors contains five chars and consumes six
bytes as an automatic slot (VIDEO.H:41-47). Allocation/shift/pop operations
already have live operands and no evidence of another pointer's role.

Open questions: the missing slot's type/name/use, whether a larger packet was
declared in a subset of functions, and whether optimized-away uses survive as
slots. Exact frame sizes alone cannot answer these. No new constants, enums,
renames or source changes are justified yet. Outside scope: gx_draw_part's z,
vi_anim_hook's old and unrestored line color, sc_picture's implicit return;
report these facts without changing behavior or names.


## Experiments (2026-11-05)

All experiments used the real MSC 5.1 compiler through bounded
`sr.dosbox.Job` / `compile_all` jobs, never the host C compiler. Scratch scripts:
`work/prov-frames/probe.py` and `followup.py`; results, generated source copies,
COD/OBJ files and JSON matrices remain under `work/`. Total: **157 compiles**,
53 small probes and 104 complete-translation-unit variants. No tracked source
was edited. Full-file variants used each module's unchanged MAKEFILE command;
small probes used `/AM /Oalsi /Gs /FPi` and `/AM /Oas /Gs /FPi`.

Each full-file candidate was compared against the original function and the
whole module, with references resolved, initialized data checked, and internal
near-call targets checked by the module gate. MATCH below means all of those
checks passed; frame size alone was never the verdict. These experimental
matches were not installed for a linked build: no candidate satisfied the
source-evidence requirement.

### Baseline and deletion

| Function | Original bytes | Frame | Missing slot | Blt base | Delete marker declaration |
| --- | ---: | ---: | --- | --- | --- |
| gx_draw_bolt | 357 | 40 | bp-12..bp-9 | bp-32 | 28 byte differences; frame 36 |
| gx_draw_part | 449 | 44 | bp-12..bp-9 | bp-32 | 27 byte differences; frame 40 |
| gx_erase_part | 259 | 36 | bp-8..bp-5 | bp-28 | 27 byte differences; frame 32 |
| sc_picture | 228 | 28 | bp-8..bp-5 | bp-28 | 13 byte differences; frame 24 |
| vi_anim_hook | 184 | 8 | bp-8..bp-3 | n/a | one byte difference; frame 2 |

All five baseline functions and their three complete modules MATCH.
The two live far bitmap pointers precede the unused slot in gx_draw_bolt/part;
only pict precedes it in gx_erase_part, and p in sc_picture. The blit record's
first six words follow the unused slot. In vi_anim_hook, old remains at bp-2.

### Four-byte blit cases

The following results apply to **each of the four** functions unless noted.
The declaration under test replaces `long unused` or absorbs its storage.

| Candidate | Full-file result | Interpretation |
| --- | --- | --- |
| Unused `struct Bitmap far *unused` or `*temp` | MATCH | Storage type is not recoverable from an untouched four-byte slot. The spelling temp places it between the live pointers and packet. |
| Unused `struct Bitmap far *savedBitmap` | DIFF | This name hashes before the live pointers and moves them. It does not disprove a far-pointer type. |
| `temp = pict` (or p) after di_bm | DIFF; sizes 363, 455, 265, 234 versus 357, 449, 259, 228 | A dead pointer assignment retains instructions. Same result using the name unused. |
| `unused = 0` with long unchanged | DIFF; sizes 363, 455, 267, 236 | Dead scalar stores remain. |
| `if (0) unused = 0` | MATCH | The statement vanishes but does not supply a reason for the local. |
| `(void)sizeof(unused)` or `(void)unused` | MATCH | The use vanishes but the slot remains. |
| `int unused[2]` or `char unused[4]` | MATCH | Merely another unused declaration. |
| Blt16 replacing Blt, dummy removed | DIFF | Frame shrinks eight bytes: four from the dummy and four from the record. |
| Box replacing Blt, dummy removed | DIFF | Box is 28 bytes; frame grows four bytes. |
| Clip replacing Blt, dummy removed | DIFF | Clip is 12 bytes; frame shrinks twelve bytes. |
| `struct Blt packet[1]`, dummy removed | DIFF | Same storage as deleting the dummy. |
| `struct Blt packet[2]`, dummy removed | DIFF | Forty bytes of packet storage; frame grows sixteen bytes. |
| Hypothetical `{ struct Blt blt; struct Bitmap far *image; }`, dummy removed | MATCH | A 24-byte packet has exactly the right first-member address and total storage. The tail pointer is invented and unused. |
| `int packet[12]`, first ten fields accessed as word indices | MATCH | Another 24-byte representation, without evidence for twelve elements or raw indexing. |
| `struct Clip packet[2]`, fields in element zero and unused1 as element one's w | MATCH | Also 24 bytes. No evidence that the extra write describes a second clip. |

For Box and Clip shape tests only, scratch casts kept the original Blt-prefix
writes at their original relative offsets, including unused1. In the Clip
scalar case that write would lie beyond the object; this was a compiler layout
probe, not a safe source candidate. The two-element Clip array makes the same
write in bounds, but provides no semantic explanation for it. No such casts or
out-of-bounds objects were applied to the game.

The 24-byte probe is exactly:

```c
struct ProbeBlt24 {
    struct Blt blt;
    struct Bitmap far *image;
};
```

It replaces the local declaration while the live fields and address argument
use `packet.blt`. This establishes a **possible** larger-record layout, not a
recovered type. BITMAP.H and VIDEO.H already provide Blt, Blt16, Clip and Box;
none has this 24-byte shape. Adding this otherwise unsupported type, enlarging
Blt globally, or converting readable field accesses into numeric array indices
would replace one hack with another. The other functions with Blt locals also
make a global enlargement an unjustified change.

### vi_anim_hook six-byte case

All variants preserve the inner declaration's original block unless stated.

| Candidate | Full-file result |
| --- | --- |
| `char save[5]`, `char save[6]`, or `int save[3]` | MATCH; frame 8 |
| `(void)sizeof(save)` or `(void)save` with Colors | MATCH; frame 8 |
| `save = x_colors` | DIFF; 200 bytes versus 184 |
| `save = x_colors` followed by `x_colors = save` before the drawing-path return | DIFF; 211 bytes versus 184 |
| `if (0) save = x_colors` | DIFF; 188 bytes versus 184 |
| `struct Clip save` | DIFF; frame 14 versus 8 |
| Delete old and its dead `old = x_colors.line` assignment, retain Colors save | DIFF; 176 bytes versus 184, frame 6 versus 8 |

The natural hypothesis of saving/restoring the drawing colors fails: the
compiler emits the five-byte copy and, for restoration, the reverse copy.
Even a constant-false Colors assignment changes code: it leaves SI/DI saves
and restores although the copy itself disappears. A dead assignment cannot
explain the original's untouched slot using the tested forms.

### Small-probe controls

The following self-contained pattern isolates storage policy:

```c
struct Blt { int w, h, sx, sy, dx, dy, u, v, mx, my; };
struct Colors { char text, textBg, fill, line, frame; };
extern int value;
extern struct Colors colors;
extern void use(struct Blt *);
extern void sink(int);
void probe(void)
{
    /* substitute the declaration and statement being tested here */
    struct Blt move;
    move.w = value;
    use(&move);
}
```

Under both tested optimization sets:

- No extra local gives a 20-byte frame; unreferenced long, far pointer,
  int[2], and char[4] give 24; Colors, int[3], char[5], char[6] give 26.
- `saved = 0`, `saved = value`, and a null far-pointer assignment retain stores.
  An int assigned zero also retains its two-byte slot and store.
- Scalar `if (0) saved = value`, `if (0 && saved) sink(value)`, sizeof,
  discarded read/address, and self-assignment retain the slot while emitting
  no instructions for the use.
- A Colors copy emits two MOVSW and one MOVSB plus register setup/saves;
  even `if (0) saved = colors` leaves SI/DI saves/restores. sizeof and
  a discarded Colors expression emit no code.
- Blt[1] is 20 bytes; Blt[2] 40; a Blt plus far pointer and int[12] are 24.

## Decision and limits

**Retain all five declarations and all five complete PROVISIONAL markers.**
The original text cannot be recovered from the measured candidates. The
bytes admit multiple unused scalars/arrays/records, and admit an unsupported
larger blit record; none explains the original local's role. No enums,
constants, renames, or replacement uses were introduced. This investigation
narrows the problem and documents positive alternatives as well as failures;
it does not claim a unique original from matching machine code.

## Reproduction approach

For a full-file trial, copy the original translation unit into a directory
under work, change only the target function as listed above, then use this
pattern inside `nix-shell shell.nix --run 'python3 work/trial.py'`:

```python
import sys
from pathlib import Path
sys.path.insert(0, 'tools')
from sr import cod, dosbox, module, source
from sr.exe import image
from sr.match import compare
from sr.omf import Obj
from sr.refs import verify

f = module.find_file('VIDEO.C')
j = dosbox.Job(Path('work/trial/VIDEO.C'), f.command, 'trial')
dosbox.compile_all([j], name='trial')
o = Obj(j.obj)
ps = cod.parse(j.cod)
o.statics = cod.statics(j.cod)
o.data_labels = cod.data_labels(j.cod)
r = source.resolve('vi_anim_hook')
p = cod.find(ps, 'vi_anim_hook')
im = image()
res = compare(o, p, im.img, r['addr'], r['size'])
res.refs = verify(o, p, im, r['addr'], r, ps)
g = module.gate(o, ps, im.img, module.members(r['src']), data=True)
res.module = g
print(res.line(), p.locals)
print(g.line('VIDEO.C'))
```

Use the other source/function names for the four-byte cases. Internal near
calls reported by compare are resolved by the module gate; the official sr
check command normalizes that count after the gate. The experiment's final
reports use that same rule.

## Final validation

- `./sr headers --check` and `./sr format --check`: pass.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- Removed only work/layout and ran `./sr build`: 96/96 objects pass,
  572/572 mapped game functions raw-byte exact at original positions,
  748/748 near calls exact, packed EXE versus orig/SR.EXE **IDENTICAL**.
  The tool also reports differences in the separate unpacked-link diagnostic
  (including MZ relocation order); no claim of unpacked-file identity is made.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests, OK.
- `./sr diff vi_anim_hook`: baseline function/module MATCH; original locals
  old=-2 and save=-8 retained.

No game source, headers, flags, layout metadata or build tooling changed.
