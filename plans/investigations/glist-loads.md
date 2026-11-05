# Discarded list-count loads: recovered bounds-check source forms

Purpose: identify ordinary MSC 5.1 C behind the discarded list-count reads in
`your_cars` and `sg_load`, without replacing one unsupported idiom with another.

## Context and evidence before experiments

- `g_list[0]` is the item count; entries 1..count hold offsets into `g_desc`.
  `your_cars` builds it from currentCar and the otherCars chain (GAME.C:609-620).
  `scan_save_slots` builds it from readable save headers (SAVEGAME.C:254-285).
- `mn_modal_list` checks count and delegates to `mn_list` (MENU.C:334-350).
  `mn_list` consumes positive navigation keys internally and returns other keys;
  `mn_sel` returns the stored selection (MENU.C:390-585). Thus the default branch
  does not by itself prove the returned value is a valid item index.
- `your_cars` (GAME.C:591-737) is called by the garage action in `gm_run`
  (GAME.C:905). It lists/switches/sells owned cars, updates the driver car chains,
  destroys sold cars and credits cash/time. Its defaults conditionally load the
  count, then use `mn_sel()` to select the car for negotiation.
- `sg_load` (SAVEGAME.C:289-359) is called by `gm_run` (GAME.C:772). It scans save
  slots, draws the load dialog, returns cancelled/error/OK and restores serialized
  game state. Its default conditionally loads the count before mapping the returned
  row through `g_saveSlots`; the accept key instead calls `mn_sel`.
- The relevant locals are `item` (your_cars' dialog key, later selected row), `r`
  (sg_load's saved copyToFront, then dialog key/row and physical slot), and `cntl`/`i`
  (occupied-row counter and physical-slot cursor). The count load has no live result
  or store. Other locals and callees are outside this expression-only scope.
- Both complete baseline modules pass `sr check`: GAME 6766 code bytes, SAVEGAME
  4167 code bytes. Exact flags are `/AM /Oalsi /Gs /FPi /NTS0000_TEXT`.
- docs/RECONSTRUCTION.md:195-203 already records failed bare reads/casts/arithmetic,
  plus byte-compatible but unjustified self-assignment and empty-switch forms.

No symbols/types/constants will be introduced or renamed. No raw-value replacement
is needed. No other misleading identifiers are established by this investigation.

## Initial question

Can bounds, discarded expressions or ordinary macro expansions reproduce the whole
objects while expressing a supported operation? A match alone is insufficient if
its source is merely an artificial dead operation.

## Result

Replace the two self-assignments with ordinary conditional switch exits:

```c
/* GAME.C, default at the end of your_cars' dialog switch */
if (item > 0 && item <= g_list[0])
    break;

/* SAVEGAME.C, default at the end of sg_load's dialog switch */
if (r >= 1 && r <= g_list[0])
    break;
```

These are redundant range checks at the end of a switch: a failed condition also
falls out of the switch. They do **not** reject invalid keys. MSC eliminates the
upper comparison and conditional transfer, but leaves its load of the count.
The first short-circuit condition and its skip survive. The source is ordinary
range-check/`break` code, with no synthetic store, fabricated macro or empty switch.
The instruction sequence does not establish unique original spelling or intent;
this supplies the natural matching form requested by the task.

`> 0` versus `>= 1` matters to MSC even though the values are equivalent: using
`>= 1` in GAME adds two bytes; using `> 0` in SAVEGAME removes two. Preserve each
file's existing relational form. SAVEGAME's extra unconditional default `break`
is removed: its conditional `break` and fall-through now both reach the switch end.

## Method and comparison scope

Compile full copies of GAME.C and SAVEGAME.C, never isolated functions, through
`tools.sr.dosbox.Job` / `compile_all` under emu2 using each `module.File.command`.
Every job uses its original basename and unmodified project headers. The common
compiler command also supplies `/c /W3 /Fc`, as `sr check` does.

For each trial compare all OMF segment data, ordered fixups, segment/group names,
publics, externs and communal declarations with a baseline compiled in the same
batch. Also compare the **entire raw OBJ file** with no masks or normalization.
The applied variants pass the raw comparison, so every OMF record, metadata byte,
segment byte and relocation matches. The baseline itself passed the project's
original-image module and function gates. A final clean packed build gates the
original EXE rather than relying on baseline agreement.

SAVEGAME's object code segment has 4168 bytes including terminal alignment; its
original-image module span is 4167. GAME has 6766 bytes for both measures.

Scratch reproduction: `work/prov-glist/probe.py`, `finalize.py`, `results.json`
and `final-results.json`; compiler outputs are in the recorded per-job dirs.
The table below preserves the substitutions so the findings do not depend on
ignored scratch files. Substitute `V` with `item` or `r`. All first-batch `V >= 1`
forms deliberately use that spelling in **both** files; the final batch checks
the exact original lower-bound spelling separately. Sizes cover whole code
segments, not only the target function. `=` means the entire raw OBJ matches.

## First batch: 45 trials per file (including baselines)

| Trial | Replacement | GAME bytes | SAVEGAME bytes |
| --- | --- | ---: | ---: |
| baseline | `unchanged baseline` | 6766 = | 4168 = |
| remove | `(remove statement)` | compile error | 4158 |
| lower_empty | `if (V >= 1) ;` | 6760 | 4158 |
| bounds_empty | `if (V >= 1 && V <= g_list[0]) ;` | 6768 | 4168 = |
| bounds_break | `if (V >= 1 && V <= g_list[0]) break;` | 6768 | 4168 = |
| bounds_gt_break | `if (V > 0 && V <= g_list[0]) break;` | 6766 = | 4166 |
| bounds_nested_break | `if (V >= 1) { if (V <= g_list[0]) break; }` | 6768 | 4168 = |
| bounds_nested_empty | `if (V >= 1) { if (V <= g_list[0]) ; }` | 6768 | 4168 = |
| bounds_discard | `V >= 1 && V <= g_list[0];` | 6788 | 4180 |
| bounds_void | `(void)(V >= 1 && V <= g_list[0]);` | 6788 | 4180 |
| count_first_empty | `if (V <= g_list[0] && V >= 1) ;` | 6762 | 4162 |
| count_first_break | `if (V <= g_list[0] && V >= 1) break;` | 6762 | 4162 |
| outside_empty | `if (V < 1 \|\| V > g_list[0]) ;` | 6768 | 4168 = |
| outside_break | `if (V < 1 \|\| V > g_list[0]) break;` | 6768 | 4168 = |
| unsigned_bounds | `if ((unsigned)(V - 1) < (unsigned)g_list[0]) break;` | 6760 | 4160 |
| upper_empty | `if (V <= g_list[0]) ;` | 6762 | 4162 |
| upper_break | `if (V <= g_list[0]) break;` | 6762 | 4162 |
| lower_count_read | `if (V >= 1) g_list[0];` | 6760 | 4158 |
| lower_count_void | `if (V >= 1) (void)g_list[0];` | 6760 | 4158 |
| lower_compare | `if (V >= 1) V <= g_list[0];` | 6782 | 4180 |
| lower_void_compare | `if (V >= 1) (void)(V <= g_list[0]);` | 6782 | 4180 |
| lower_compare_break | `if (V >= 1) { if (g_list[0] >= V) break; }` | 6768 | 4168 = |
| lower_comma_read | `if (V >= 1) (g_list[0], 0);` | 6760 | 4158 |
| lower_conditional | `if (V >= 1) V <= g_list[0] ? 0 : 1;` | 6782 | 4180 |
| lower_max_discard | `if (V >= 1) max(V, g_list[0]);` | 6778 | 4158 |
| lower_min_discard | `if (V >= 1) min(V, g_list[0]);` | 6778 | 4158 |
| lower_count_minus | `if (V >= 1) g_list[0] - V;` | 6760 | 4158 |
| lower_count_post | `if (V >= 1) g_list[0]++;` | 6768 | 4168 |
| lower_empty_switch | `if (V >= 1) switch (g_list[0]) {}` | 6768 | 4168 = |
| lower_empty_case | `if (V >= 1) switch (g_list[0]) { case 1: break; }` | 6768 | 4168 = |
| lower_unsigned_cast | `if (V >= 1) (unsigned)g_list[0];` | 6760 | 4158 |
| lower_volatile_read | `if (V >= 1) *(volatile int *)g_list;` | 6760 | 4158 |
| lower_plus_zero | `if (V >= 1) g_list[0] += 0;` | 6760 | 4158 |
| lower_or_zero | `if (V >= 1) g_list[0] \|= 0;` | 6760 | 4158 |
| lower_count_and_true | `if (V >= 1 && g_list[0]) break;` | 6760 | 4158 |
| lower_count_or_true | `if (V >= 1) { if (g_list[0] \|\| 1) break; }` | 6760 | 4158 |
| lower_count_and_false | `if (V >= 1) { if (g_list[0] && 0) break; }` | 6760 | 4158 |
| lower_ternary_read | `V >= 1 ? g_list[0] : 0;` | 6776 | 4172 |
| lower_ternary_compare | `V >= 1 ? V <= g_list[0] : 0;` | 6788 | 4180 |
| lower_ternary_void | `if (V >= 1) (g_list[0] ? (void)0 : (void)0);` | 6766 | 4166 |
| lower_sizeof | `if (V >= 1) sizeof(g_list[0]);` | 6760 | 4158 |
| lower_selfassign_macro | `if (V >= 1) SELF(g_list[0]);` | 6768 | 4168 = |
| bounds_macro_break | `if (VALID(V, g_list[0])) break;` | 6768 | 4168 = |
| assert_bounds | `assert(V >= 1 && V <= g_list[0]);` | 6818 | 4218 |
| assert_bounds_ndebug | `assert(V >= 1 && V <= g_list[0]);` | 6760 | 4158 |

`SELF(x)` expands to `((x) = (x))`; `VALID(v,n)` to
`((v) >= 1 && (v) <= (n))`. The normal assert includes MSC's `assert.h`; the
second defines NDEBUG first. No macros are added to production source.
The GAME remove trial needs a null statement after its terminal default label
(MSC rejects a label immediately before `}`); `lower_empty` is the valid no-load
control. The failed syntax trial supplies no codegen evidence.

## Interpretation of failed alternatives

- A bare count read, cast to void/unsigned/volatile, comma-discard, arithmetic,
  `sizeof`, or a simple no-op conditional loses the needed load.
- A standalone relational value or logical expression materializes a result:
  it differs from a condition whose outgoing branches coincide.
- Ordinary min/max expansions, enabled assertions, reversed test order and an
  unsigned range test fail whole-object identity. Disabled assertions remove
  the load; a macro merely preserves its expansion's behavior.
- Empty switches and self-assignment explain the bytes but are the previously
  known artificial forms. The bounds-check source requires neither.

## Replacement list and limits

Only the two named PROVISIONAL statements change; both uses are replaced.
No new enum, macro, symbol, declaration, include, type, local name or layout is
introduced. There are no known out-of-scope identifier corrections.

The exact historical reason for a redundant bounds check remains unknown. This
investigation establishes a natural source form and the compiler behavior,
not a unique recovery of the programmers' intent.

## Final exact-spelling batch: five trials per file

| Trial | GAME raw OBJ equal | SAVEGAME raw OBJ equal |
| --- | --- | --- |
| gt_empty | yes | no |
| gte_break | no | yes |
| gt_break | yes | no |
| lower_empty_switch | yes | yes |
| bounds_macro | no | yes |

Here `gt_empty` is `if (V > 0 && V <= g_list[0]) ;`, `gte_break` uses
`V >= 1`, and `gt_break` uses `V > 0`; the latter two end in `break`.
For SAVEGAME's `gte_break`, remove the following unconditional `break` too.
`lower_empty_switch` preserves each file's original lower test and evaluates
`switch (g_list[0]) {}`. `bounds_macro` expands a three-argument `IN_RANGE`
macro using `>= 1` and `<= count`, followed by `break`.

Baseline and applied raw-OBJ SHA-256 (identical within each pair):

- GAME.C: `15d79566557bdd195dd4443211fdb74ba633cfd159c9d60bb2498bbcec3bf240`
- SAVEGAME.C: `de66161013cec1b9671984cf88874f3e66f705de449dc74a421d18eb519f862b`

## Final gates

All required gates passed on the applied source:

- `./sr headers --check`: includes/dependencies and ownership are current.
- `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- `rm -rf work/layout && ./sr build`: 572 functions at their original positions,
  748/748 near calls exact, and `Packed EXE vs orig/SR.EXE: IDENTICAL`.
- `./sr dgroup check`: every initialized global is in its original owner unit.
- `./sr dgroup order`: agrees with src/SR.LNK.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`:
  106 tests, OK.

No function/global/type renames or flags changed. Work stays on branch prov-glist;
no merge or push is part of this task.
