# Misleading manifold and car-spec names

## Evidence checked before renaming

- `src/INPUT/INPUTDAT.ASM:236-252` assigns variant 2 to all three
  `DESC_*TRI_POWER*` catalog entries, and variant 3 to `DESC_*DUAL_QUAD*`.
  `src/UI/MESSAGES.ASM:252-262` spells out two 4-barrel versus three 2-barrel
  carbs. The description names already agree with their text.
- `g_nCarb` (`src/GAME/GARAGE.C:753`) is `{1, 1, 3, 2, 2}`. Its manifold
  index determines carb allocation in `gm_make_car` (`GAME.C:354-365`) and
  the required installed slots in `ga_ready` (`GARAGE.C:915-918`).
  `g_carbFit` (`INPUTDAT.ASM:289-294`) permits only 2-barrel carbs for
  variant 2, and 4-barrel/racing carbs for variants 3 and 4; `ga_hood`
  indexes it by manifold then carb variant (`GARAGE.C:1150`).
- `g_carbManifoldFit` (`src/GAME/PERF.C:16,53,177`) is indexed by carb
  variant then manifold variant. The 2-barrel row scores 100 only in
  column 2; the 4-barrel row scores 100 in column 3. Thus variant 2 is
  Tri-Power (three 2-barrel carbs), variant 3 is Dual-Quad (two 4-barrel
  carbs), and racing uses two 4-barrel carbs.
- Decoded `./sr pic 1079 1083 --scale 5 --png work/misnames/manifolds.png`
  and inspected it: 1079 has three carb pads, 1083 has two. The manifold
  row of `g_partPics` (`INPUTDAT.ASM:298`) maps those pictures to variants
  2 and 3. Both `GARGFX.C:266,373` tests concern picture 1079; `MEM.C`
  caches both pictures. Their numeric IDs and all resulting behavior stay.
- `src/SYS/DISK.C:68-82,502-527` loads the 13 HOT_DATA blocks verbatim.
  Decoded `orig/freeware/HOT_DATA` using those sizes: car-spec block starts
  at 396, description block at 1904. Record 25 at `396 + 25*10`, decoded
  as little-endian `<hBbHhh`, is `(0, 8, 2, 0xa2e1, 1412, 1201)`;
  description 1412 is `1963 Corvette`. Decoded picture 1201 and inspected
  the Corvette body. Racer 21 (18-byte record, spec at +8) has portrait
  1244, description 1567 (`The King`) and spec 25. `init_racers`
  (`SAVEGAME.C:90-111`) randomizes ordinary rivals only; `select_rival`
  (`BOBS.C:340-368`) uses that spec and body for the King.
- Every executable use of the old spec macro was read: `king_chance`
  (`BOBS.C:253`) suppresses another King encounter when spec 25 is
  selected; `cop_appears` (`RACE.C:1352,1358`) deliberately borrows its
  drivetrain and rating, then uses separate police sprites. Renaming the
  spec must preserve that police-performance reuse. `CARPIC.C:323` also
  tests raw `0x19` to skip the engine overlay on this same car body.
- Header/INC searches for King/spec names found no existing constant with
  this meaning. Reuse the existing definition, renamed `SPEC_KING`, and
  replace the renderer's raw comparison too. `SPEC_ORDINARY_COUNT` is a
  different meaning and remains unchanged. No resource bytes are edited.

## Replacement inventory

Swap the two manifold identifiers at every token use in GAME.H and
INPUTDAT.ASM; swap their picture identifiers in DISK.H, DISK.INC,
INPUTDAT.ASM, GARGFX.C and MEM.C. Document all five ManifoldVariant values
with carb count and size. Rename the spec definition and all three macro
uses in BOBS.H, BOBS.C and RACE.C; use SPEC_KING in CARPIC.C's raw check.
Correct Spec.md's resolved naming issues and stale discrepancy text.
A whole-tree search found no affected tests or other old-meaning prose;
retain historical diary entries and append the correction there.

These changes alter names and documentation only: enum values, table
contents, record layouts, police graphics and performance stay identical.
No unresolved semantic questions or additional misnames were found within
these two value sets.

## Validation

All gates passed on the final source:

- `./sr headers --check` and `./sr format --check`: clean.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- Clean `rm -rf work/layout && ./sr build`: 572/572 raw-byte exact
  functions at original positions, 748/748 exact near calls, packed EXE
  identical to `orig/SR.EXE`.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`:
  106 tests pass.
- `./dosbox.sh --make`: native DOS EXE identical to `orig/SR.EXE`.

The rename CLI accepts functions/globals/far-data symbols, not constants;
used one simultaneous whole-token replacement over tracked text files,
then reviewed all replacements and ran the normal gates.
