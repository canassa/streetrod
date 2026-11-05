# Item investigation

## Purpose and decision

One executable-resident parts-catalog specification: base purchase price, the
category-specific equipment variant, manufacturer compatibility and display name.
It is not an owned inventory item. Rename `Item` to `PartSpec` and its `value`
member to `basePrice`; retain `variant`, `fitMask`, `category`, `unused` and
`nameOffset`. Move the definition beside its sole storage owner, `g_parts`, in
`src/INPUT/INPUTDAT.H`. This note was written before any source renames; the
reader/writer line references below describe the original source.

## Storage, initialization and persistence

- `src/INPUT/INPUTDAT.ASM:141-273` defines the sole table, `g_parts`, with 44
  eight-byte records (352 bytes): 43 parts and a final all-zero record. The next
  global is `g_partWear` at line 274. `src/INPUT/INPUTDAT.H:59` declares the table.
- Original DGROUP range is 0x4806..0x4965; verified with `./sr data 4806 352`
  and a decoded `<hbbbbh` view of the original image. All six fields are
  initialized explicitly by MASM. Every C access is a read; no runtime writer,
  allocator or loader creates or modifies a catalog record.
- Catalog indices 0..8 are engines, 9..21 transmissions, 22..24 carburetors,
  25..39 manifolds and 40..42 tires. Index 43 is zero. The existing
  `PART_CATALOG_COUNT` includes the zero record; the advert loop filters by
  `MIN_AD_PART_PRICE` (`src/GAME/GAME.C:459-463`), excluding it and all V6s.
- The catalog is compiled into the EXE, not loaded from HOT_DATA. The 13
  destinations in `src/SYS/DISK.C:69-82,502-527` do not include it. Only the
  first 1850 bytes of `g_desc` are loaded there; part descriptions live in the
  initialized tail (`src/UI/MESSAGES.ASM:185-278`).
- Saves persist owned `Node` records, including their catalog `idx`, value and
  wear, not `Item` bytes (`src/GAME/SAVEGAME.C:157-196,199-210,336-348`). Loading
  allocates owned nodes and copies the saved records. `Node::idx` selects the
  same executable catalog after reload.

## Fixed layout and field evidence

The exact original shape is `int, char, char, char, char, int`: offsets
0, 2, 3, 4, 5, 6; size 8, word alignment. Keep order, size and signedness.
No member is Boolean. `unused` remains a named spare byte.

| Original field | Meaning and values | Evidence |
| --- | --- | --- |
| `value` (int) | Base purchase price in dollars; real entries range from 19 to 525; zero in the final record. | `INPUTDAT.ASM:142-273`; advert price displayed and charged in `GAME.C:459-495`, copied into stock owned parts at `GAME.C:336,342,348,356,366`; replacement tires at `RACE.C:438`, demo slicks at `SYS.C:854`. |
| `variant` (char) | Category-specific equipment/performance/sprite index, not the catalog index or manufacturer. Engines use `EngineVariant`, transmissions `TransmissionVariant`, carbs `CarburetorVariant`, manifolds `ManifoldVariant`, tires `TireVariant`. All five sets already exist. | Explicit constants in `INPUTDAT.ASM:143-269`; performance tables in `PERF.C:10-22,42-86,166-180`; garage fit/sprite readers listed below. |
| `fitMask` (char) | Manufacturer bits from existing `PartFit`; real entries use GM, Ford, Chrysler or ALL. Zero in final record. This alone does not guarantee intake compatibility. | `INPUTDAT.ASM:143-269`, `DISK.H:295-301`; intersection with car stock bits at `GARAGE.C:842,1127,1135-1137,1148-1150`; the latter two also check engine/manifold and manifold/carb tables. |
| `category` (char) | Existing `PartCategory`, distinguishing engine, transmission, carburetor, manifold and tires. The zero record is not a real engine despite zero category. | `INPUTDAT.ASM:143-269`; resale branches `GAME.C:178,236-251`, wear initialization `GAME.C:306`, spare-list filter `GARAGE.C:635`. Remains char: enum storage would widen it. |
| `unused` (char) | Explicit zero byte in every record; no readers or runtime writers. | `INPUTDAT.ASM:143-273`; whole-tree member-use search. Its historical intended purpose is unknown. |
| `nameOffset` (int) | Byte offset from `g_desc` to the first text of the catalog name, after the preceding line-count byte. Real values 1852..3244; zero in final record. Existing `DescriptionOffset` constants encode all real entries. | `INPUTDAT.ASM:144-270`, `MESSAGES.INC:1-43`, `MESSAGES.H:141-187`, `MESSAGES.ASM:188-278`; direct string copies `GAME.C:253,479`; list renderers `MENU.C:425,449,507`. Retype to existing int-sized enum. |

### Variant meanings, checked against text and consumers

- Engines: V6, small V8, large V8. Manufacturer varies independently. V6 base
  price is 19, intentionally below the advert threshold 20; `gm_part_value`
  instead values any owned V6 at `V6_PART_VALUE` (100), `GAME.C:174-183`.
- Transmissions: automatic, three-speed, four-speed, racing. The universal
  four-speed at index 18 shares `TRANS_FOUR_SPEED` with indices 15..17; it is
  not an extra variant. Stock index formulas skip that universal entry before
  the racing range (`GAME.C:332-335`, `PERF.C:112-116,156-160`).
- Carburetors: two-barrel, four-barrel, racing four-barrel. All fit all makes;
  the intake table can still reject a carburetor (`GARAGE.C:1148-1150`).
- Manifolds: two-barrel, four-barrel, Tri-Power (three two-barrel carbs),
  Dual-Quad (two four-barrel carbs), racing (two four-barrel carbs). Catalog
  variant 2 references the Tri-Power text at offset 2961 and variant 3 the
  Dual-Quad text at offset 2839: text order is not variant order. `g_nCarb`
  is `{1,1,3,2,2}` (`GARAGE.C:753`); fit tables at `INPUTDAT.ASM:284-294`
  and performance table at `PERF.C:16` independently agree.
- Tires: standard, brand name, racing slicks. Sprite index, tire factors, slip
  and wear behavior all use this same variant set.

## Complete reader map

There are four typed local pointers (`GAME.C:195,301`, `GARAGE.C:629,722`);
all point directly into `g_parts` and none escape or write through the pointer.
Every other use indexes the table directly through an owned node or stock index.

- `GAME.C`: `gm_part_value` (category/variant), `sell_parts` (name/category),
  `gm_add_part` (category), `gm_make_car` (base price), `buy_from_ads`
  (base price/name), `gm_car_look` (large-engine variant, line 585), garage
  car drawing at line 710 (tire variant).
- `GARAGE.C`: car-menu drawings at lines 301,325 (tire variant);
  `ga_spares` (category/name), `ga_tires` (variant before/after installation),
  `car_info` (name), `ga_transmission` (variant/fit), `ga_ready` (manifold
  variant selects carb count), engine display/install at lines 1040-1184
  (variant selects sprites/bolts/carb count; fit is checked against car and
  intake tables). The engine check at 1040 has the wrong-domain constant
  described below.
- `BOBS.C:469-472`: engine variant selects the player's car rating used when
  a rival considers accepting a stake.
- `CRUISE.C:32`: transmission variant zero means automatic.
- `GARGFX.C:536`: manifold variant chooses carburetor screen positions.
- `PERF.C:42-86,166-180`: engine, manifold, carb, transmission and tire
  variants select performance/gear/RPM/traction tables. Stock-spec helpers
  at 106-116,150-160 construct owned-node indices; they do not write the catalog.
- `RACE.C:229,257`: tire variant selects the car sprite's tire images;
  `438`: base price of replacement standard tires; `926`: tire variant and
  owned wear select slip; `1013`: standard tire variant selects wear rates.
- `SYS.C:854`: demo purchase uses racing-slick catalog base price.

The full original table-reference inventory is saved below for review.

## Changes and replacement inventory

- Rename the struct tag and all five C/header mentions from `Item` to `PartSpec`
  (four pointer declarations and the storage declaration), and move its
  definition from GAME.H to the storage owner's INPUTDAT.H.
- Rename only this type's `value` to `basePrice`: every catalog price read in
  GAME.C (seven reads), RACE.C (one) and SYS.C (one). Owned Node/Car value
  members remain untouched.
- Type `nameOffset` with existing `enum DescriptionOffset`; keep all four
  byte fields unchanged and document their existing enum value sets. Its zero
  value in the final record is not a valid description enumerator.
- Preserve the original unsized array declaration. An attempted bound using
  existing `PART_CATALOG_COUNT` compiled and produced an identical packed EXE,
  but `sr dgroup check` cannot resolve macro identifiers in dimensions (it only
  resolves enum constants). Reverting the optional bound avoids expanding this
  investigation into tooling changes; `GAME.C:459` still names the scan bound.
- Document the type and each field; add concise documentation to the existing
  EngineVariant, TransmissionVariant, CarburetorVariant, TireVariant and
  PartCategory enums so the field's explicit references resolve. Existing
  ManifoldVariant and PartFit documentation already supplies the other sets.
- No new enum, enumerator, bit flag, Boolean, count, raw-value replacement or
  runtime behavior change is needed. MASM already names every real record's
  variant, fit, category and text offset using the existing constants. The
  final all-zero record remains unchanged.

## Other misleading identifiers (outside this task)

- `GARAGE.C:1040` compares the variant of `car->rack.parts[0]` (the engine)
  against `MANIFOLD_TWO_BARREL`. It actually distinguishes V6 from V8 while
  selecting the manifold sprite. `ENGINE_V6` has the right domain and same
  numeric value; leave this unrelated identifier use for a separate change.
- `RACE.C:925-926` local `car` is a `Node *` pointing to the owned tires, not
  a car. Its wear and variant determine traction. Leave local naming alone.
- `GAME.C:469-495` local `used` is true for part adverts and false for car
  adverts. It means a part purchase branch, not whether the merchandise is
  used; leave the function's locals for its own investigation.
- `Node` is an owned part with wear/value/catalog index, not a generic node;
  `INPUTDAT` also owns game catalog tables beyond input. Neither is renamed.

## Edge cases and remaining uncertainty

- No lookup validates catalog indices or category/variant coherence. Save data
  supplies owned indices without catalog validation. This is original behavior.
- The final zero record participates in the bounded advert scan but fails the
  minimum-price test. No observed producer creates an owned part for index 43.
  Its historical purpose (sentinel versus reserved entry) is not proved; call
  it a zero record and preserve it.
- A fit-mask match is necessary but not sufficient for manifold/carb installs.
  Do not document it as an unconditional installation guarantee.
- Original historical type/member spellings and the spare byte's purpose are
  unrecoverable from current evidence. The proposed names describe observed
  behavior. There is no unresolved behavioral question about a live field.

## Validation

All required gates pass:

- `./sr headers --check`: include blocks/dependencies current and declarations
  owned correctly. `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics; the struct and enum references resolve.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- `./sr diff gm_make_car`: entire GAME.C module matches; function 848/848
  bytes with zero mismatches, preserving the original local frame.
- `nix-shell shell.nix --run 'python3 tools/scripts/check_data_types.py'`:
  158 types, 712 real-compiler sizeof/offsetof checks, zero mismatches.
  PartSpec remains eight bytes with offsets 0,2,3,4,5,6.
- `rm -rf work/layout && ./sr build`: 96/96 objects pass; all 572 functions
  are raw-byte exact at their original positions; 748/748 near calls exact;
  `Packed EXE vs orig/SR.EXE: IDENTICAL`.
- `./sr dgroup check` and `./sr dgroup order`: correct owners and link order.
- `nix-shell shell.nix --run 'python3 -m unittest discover -s tests'`:
  106 tests pass.
- `./dosbox.sh --make`: DOS-native packed executable IDENTICAL. Both it and
  the original have SHA1 `3a5e28b795313028b7e77d147122f41cb29de334`.
- `git -c core.whitespace=cr-at-eol diff --check`: clean; retain the required
  DOS MAKEFILE CRLF endings.

The optional macro-bound experiment and its reverted declaration are described
above. No tooling or comparison masks were changed. The auxiliary unpacked
comparison printed by `sr build` reports header/relocation-order differences and
14 missing load-image bytes; the authoritative packed output is exactly equal
with no masking, under both build paths. Logs remain in
`work/investigations/` (untracked).

## Original table-reference inventory

```text
src/GAME/BOBS.C:469:    item1 = g_parts[g_driver.currentCar->rack.parts[0]->idx].variant;
src/GAME/CRUISE.C:32:            g_autoTrans = !g_parts[g_driver.currentCar->transmission->idx].variant;
src/GAME/GAME.C:178:    if (g_parts[n->idx].category == PART_ENGINE && g_parts[n->idx].variant == ENGINE_V6)
src/GAME/GAME.C:209:            g_list[nl] = (ip = &g_parts[(list8[nl] = p7)->idx])->nameOffset;
src/GAME/GAME.C:236:        switch (g_parts[type8].category) {
src/GAME/GAME.C:253:        strcpy(g_salePartName, &g_desc[g_parts[type8].nameOffset]);
src/GAME/GAME.C:305:    it = &g_parts[idx];
src/GAME/GAME.C:336:    node->value = g_parts[w].value;
src/GAME/GAME.C:342:    node->value = g_parts[w].value;
src/GAME/GAME.C:348:    node->value = g_parts[w].value;
src/GAME/GAME.C:356:    node->value = g_parts[w].value;
src/GAME/GAME.C:366:        node->value = g_parts[w].value;
src/GAME/GAME.C:460:        if (g_parts[k3].value >= MIN_AD_PART_PRICE) {
src/GAME/GAME.C:461:            g_list[n++] = g_parts[k3].nameOffset;
src/GAME/GAME.C:471:            k3 = g_parts[i5 = list4[n + off6]].nameOffset;
src/GAME/GAME.C:472:            price6 = g_parts[i5].value;
src/GAME/GAME.C:585:    if (g_parts[c->rack.parts[0]->idx].variant == ENGINE_V8_LARGE)
src/GAME/GAME.C:710:                        0, g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
src/GAME/GARAGE.C:301:                        g_tirePics[g_parts[g_old->tires->idx].variant]);
src/GAME/GARAGE.C:325:            g_carDraw.tireVariant = g_parts[g_driver.currentCar->tires->idx].variant;
src/GAME/GARAGE.C:524:                    g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
src/GAME/GARAGE.C:635:        if ((ent7 = &g_parts[p->idx])->category == t) {
src/GAME/GARAGE.C:711:    y = g_parts[car->tires->idx].variant;
src/GAME/GARAGE.C:712:    cur = g_parts[tbl[sel]->idx].variant;
src/GAME/GARAGE.C:735:            it = &g_parts[node->idx];
src/GAME/GARAGE.C:806:    g_carDraw.transmissionPic = owner->transmission ? g_transPics[g_parts[owner->transmission->idx].variant] : 0;
src/GAME/GARAGE.C:841:                g_carDraw.transmissionPic = g_transPics[g_parts[tbl[k]->idx].variant];
src/GAME/GARAGE.C:842:                if (g_carData[owner->specIndex].stockParts & g_parts[tbl[k]->idx].fitMask & SPEC_FIT_MASK) {
src/GAME/GARAGE.C:915:    n = g_nCarb[g_parts[car->rack.parts[1]->idx].variant];
src/GAME/GARAGE.C:1040:            g_parts[p6->idx].variant == MANIFOLD_TWO_BARREL ? PIC_MANIFOLD_TWO_BARREL_V6 : PIC_MANIFOLD_TWO_BARREL_V8;
src/GAME/GARAGE.C:1042:        g_partsPics[key] = (p6 = car->rack.parts[key]) ? g_partPics[key][g_parts[p6->idx].variant] : 0;
src/GAME/GARAGE.C:1046:    n = (p6 = car->rack.parts[1]) ? g_nCarb[g_parts[p6->idx].variant] + 2 : 5;
src/GAME/GARAGE.C:1048:        tmp29 = g_parts[p6->idx].variant;
src/GAME/GARAGE.C:1127:                ok = g_parts[list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK;
src/GAME/GARAGE.C:1135:                ok = (g_parts[k = list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK) !=
src/GAME/GARAGE.C:1137:                     g_manifoldFit[g_parts[car->rack.parts[0]->idx].variant][tmp29 = g_parts[k].variant] != 0;
src/GAME/GARAGE.C:1140:                n = g_nCarb[g_parts[list4[key]->idx].variant] + 2;
src/GAME/GARAGE.C:1148:                ok = (g_parts[k = list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK) !=
src/GAME/GARAGE.C:1150:                     g_carbFit[tmp29 = g_parts[car->rack.parts[1]->idx].variant][g_parts[k].variant] != 0;
src/GAME/GARAGE.C:1184:            g_partsPics[sel2] = k = g_partPics[sel2][g_parts[list4[key]->idx].variant];
src/GAME/GARGFX.C:536:            slot = g_carbPosBase[g_parts[car->rack.parts[1]->idx].variant];
src/GAME/PERF.C:42:    a3 = g_parts[qn[0]->idx].variant;
src/GAME/PERF.C:43:    bl = g_parts[qn[1]->idx].variant;
src/GAME/PERF.C:44:    c3 = g_parts[qn[2]->idx].variant;
src/GAME/PERF.C:45:    d = g_parts[n6k->idx].variant;
src/GAME/PERF.C:46:    e1 = g_parts[m->idx].variant;
src/GAME/PERF.C:85:        (int)((int)(0x82 - g_carData[c->specIndex].rank) * 5.0 / 50.0 + g_tireFactors[g_parts[m->idx].variant]);
src/GAME/PERF.C:166:    a3 = g_parts[qn[0]->idx].variant;
src/GAME/PERF.C:167:    bl = g_parts[qn[1]->idx].variant;
src/GAME/PERF.C:168:    c3 = g_parts[qn[2]->idx].variant;
src/GAME/PERF.C:169:    d = g_parts[n6k->idx].variant;
src/GAME/PERF.C:170:    e1 = g_parts[m->idx].variant;
src/GAME/RACE.C:229:                0, 0xa9, 1, g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
src/GAME/RACE.C:257:                0xa9, 1, g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
src/GAME/RACE.C:438:        gm_add_part(PART_TIRES_FIRST, g_parts[PART_TIRES_FIRST].value);
src/GAME/RACE.C:926:    p->tireSlip = g_tireSlip[g_parts[car->idx].variant][car->wear / TIRE_WEAR_BAND];
src/GAME/RACE.C:1013:        slot->wear += (g_parts[slot->idx].variant == TIRES_STANDARD
src/INPUT/INPUTDAT.H:59:extern struct Item g_parts[];
src/SYS/SYS.C:854:        gm_add_part(0x2a, g_parts[42].value);

```
