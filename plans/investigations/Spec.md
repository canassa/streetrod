# Spec investigation

## Purpose and storage

One immutable car-model specification supplies the purchase price, ordinal
performance rank, classified-ad order, stock equipment, displayed model name,
and base body picture. The proposed name is `CarSpec`.

`src/SYS/DISK.C:47-55` allocates 26 records in `g_carData`; the parallel
model, body-layout and service-point tables use the same indices. The original
DGROUP address recorded in tools/sr/symbols.json is 0x7d86. This is communal
storage, not initialized EXE data: `sr data` at communal addresses has no
resource contents to dump.

`DISK.C:68-82` places this array second in the 13 HOT_DATA blocks, with a byte
count of 260. `di_load_data` (`DISK.C:502-527`) opens the data file and reads
each whole block directly into its destination, rejecting errors or short
reads. Thus records occupy file offsets 396..655, with no header or per-record
conversion. The little-endian words and adjacent signed/unsigned bytes match
the MSC record layout below. Description text starts at file offset 1904.

The only writer is that raw resource read. `GAME.C:78` loads it at startup;
there are no member assignments, updates or serialization of these records.
Save/load code (`SAVEGAME.C:336-341` and its save counterpart) persists owned
cars, parts and racer state, retaining specification indices instead. Aliases
are `g_oppSpecPointer` (`BOBS.C:344`), `specn` (`GAME.C:474`), `s` (both PERF
stock-car builders), and the unused `item` assignment (`BOBS.C:281`). None
mutates the resource table.

## Members (source line numbers before editing)

| Offset | Original declaration | Meaning and evidence |
| --- | --- | --- |
| 0..1 | int price | Base price in dollars. Advert purchase reads it, prints `$`, checks cash and pays it (`GAME.C:475-500`). Pink-slip wins use it as the newly owned car's value (`RACE.C:696`). Ordinary prices span 400..3770; the King's record is zero. Rename `basePrice`. |
| 2 | unsigned char rank | Numeric ordinal 0..8, higher is stronger. Indexes the nine-entry model-score table (`PERF.C:24,52,176`), also feeds arithmetic (`85`). Copied into owned cars (`GAME.C:324`); rank distance helps rival matching (`BOBS.C:284`), rank thresholds choose classes (`212-217`, `SAVEGAME.C:90-96`), and rank controls cosmetic-modification probability (`BOBS.C:348`, `SAVEGAME.C:73`). Losing rivals receive strictly lower-ranked replacement models (`RACE.C:718`). Keep `rank`: a numeric scale, not an independently named categorical enum; existing CarClass describes the separate grouped classes. Keep unsigned byte. |
| 3 | signed char next | Next specification index in the newspaper list; negative terminates (`GAME.C:451-454`). Actual end is -1 at record 23. Rename `nextAd`; retain signed byte. Record 25 has 2 but is unreachable from the ad head. |
| 4..5 | unsigned parts | Packed stock configuration AND manufacturer compatibility. `GAME.C:332-358` decodes every field to construct purchased/won cars; `PERF.C:104-116,148-160` builds temporary stock cars the same way. `GARAGE.C:842,1127,1135,1147` intersects its low nibble with a candidate part's fit mask. Rename `stockParts`; retain unsigned word. |
| 6..7 | int nameOffset | Byte offset into loaded description text, not a pointer and not a model index. Name rendering/menus use `g_desc + offset` (`BOBS.C:154-156`, `GAME.C:452,474,611,615,647`, `GARAGE.C:337`, `RACE.C:489`). Keep name and signed word. Actual offsets are listed below. |
| 8..9 | enum PictureId bodyPic | Base body bitmap resource. Passed to bitmap load and dimension queries (`CARPIC.C:300,439`), and copied into player/rival display state (`GARAGE.C:323`, `BOBS.C:364`). Keep existing enum and name. |

Total size is 10, word aligned; no padding or spare fields. Do not change the
signedness, order, widths, or representation of any member.

## Packed stock word

| Bits | Decoding | Values and consumers |
| --- | --- | --- |
| 0..3 | word & 0xf | One manufacturer fit bit in all 26 records: GM=1, Ford=2, Chrysler=4. The part catalog (`INPUT/INPUTDAT.ASM:142-275`) proves meanings through its GM/Ford/Chrysler descriptions; fit-all parts use their union, 7. Manufacturer catalog column is `(word & 0xf) >> 1`, producing 0,1,2. |
| 4..5 | (word >> 4) & 3 | Existing TireVariant (0..2). Also chooses rival/advert tire sprites (`BOBS.C:365`, `GAME.C:533`). |
| 6..7 | (word >> 6) & 3 | Existing TransmissionVariant (0..3). Three manufacturer columns per variant except the extra universal four-speed entry, skipped by `(variant == 3)` (`GAME.C:333`, `PERF.C:113,157`). |
| 8..10 | (word >> 8) & 7 | Existing EngineVariant (0..2). Selects stock engine, large-engine drawing overlay (`GAME.C:532`, `SAVEGAME.C:74`), and betting-rating column (`BOBS.C:467-470`). |
| 11..13 | (word >> 11) & 7 | Existing ManifoldVariant encoding, resource uses 0,1,3,4. Multiplied by three then manufacturer column is added. The existing manifold naming discrepancy is recorded below. |
| 14..15 | (word >> 14) & 3 | Existing CarburetorVariant (0..2); stock resource uses 0..2. The manifold selects how many carburetors to allocate through g_nCarb. |

No new duplicate equipment enums are needed: GAME.H already defines all five
variant sets. Search of headers and INC files found no existing manufacturer
fit enum or stock-field masks/shifts. Add named masks and shifts without
changing expression shape. Add `PartFit` for manufacturer bits and replace
all 43 non-sentinel catalog fit-byte initializers (including all-fit rows);
its MASM definitions belong in the already included DISK.INC. Keep char fit
storage. A complete replacement inventory will follow validation.

## Resource observations

Decoded orig/freeware/HOT_DATA with explicit little-endian `<hBbHhh` at
396 + 10*index, and resolved names relative to file offset 1904. Values are
resource observations, not historical-model specifications.

| Index | Price | Rank | Next | Stock hex | Name offset | Body picture | Model text (backticks omitted here) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | 400 | 0 | 1 | 0001 | 2 | 1039 | 1940 Chevrolet 2-dr coupe. |
| 1 | 2865 | 7 | 16 | 4ad1 | 54 | 1040 | 1940 Chevrolet Roadster. |
| 2 | 1755 | 5 | 0 | 5a91 | 131 | 1047 | 1938 Chevrolet Master Deluxe 2-dr. |
| 3 | 1450 | 3 | 4 | 4981 | 214 | 1048 | 1956 Corvette. |
| 4 | 2250 | 5 | 5 | 4a91 | 258 | 1053 | 1961 Corvette. |
| 5 | 3770 | 8 | 6 | a2e1 | 301 | 1054 | 1963 Corvette. |
| 6 | 625 | 0 | 8 | 4904 | 371 | 1060 | 1955 Dodge Custom Royal Lancer. |
| 7 | 1700 | 4 | 10 | 4992 | 429 | 1061 | 1932 Ford. Customized deuce coupe. |
| 8 | 2050 | 6 | 7 | 0144 | 485 | 1112 | 1962 Dodge Polara. |
| 9 | 1200 | 2 | 20 | 0104 | 529 | 1113 | 1962 Plymouth Savoy 2-dr. |
| 10 | 2680 | 7 | 11 | 5a92 | 570 | 1118 | 1932 Ford Sedan. |
| 11 | 2450 | 6 | 12 | 4992 | 624 | 1119 | 1940 Ford Deluxe 2-dr. |
| 12 | 1950 | 5 | 13 | 4992 | 682 | 1126 | 1955 Ford Fairlane Victoria. |
| 13 | 640 | 1 | 14 | 0142 | 739 | 1127 | 1957 Ford Fairlane 500 2-dr. |
| 14 | 900 | 1 | 15 | 4952 | 789 | 1131 | 1954 Mercury Monterey. |
| 15 | 455 | 0 | 18 | 0002 | 832 | 1135 | 1956 Mercury Custom 2-dr. |
| 16 | 475 | 1 | 24 | 0101 | 892 | 1145 | 1949 Chevrolet 2dr Styleline. |
| 17 | 1260 | 2 | 3 | 0111 | 958 | 1149 | 1958 Chevrolet Impala. |
| 18 | 1850 | 4 | 19 | 4911 | 1023 | 1156 | 1949 Olds 88 2dr.Customized. |
| 19 | 2055 | 6 | 9 | 4a91 | 1082 | 1157 | 1952 Olds 88. Customized. |
| 20 | 3100 | 7 | 21 | 4aa1 | 1136 | 1164 | 1950 Pontiac Silver Streak. |
| 21 | 3350 | 8 | 22 | 5ad2 | 1207 | 1165 | 1957 T-Bird. |
| 22 | 1350 | 3 | 23 | 0044 | 1260 | 1171 | 1961 Valiant V-200. |
| 23 | 1575 | 4 | -1 | 4a44 | 1313 | 1172 | 1963 Valiant V-100 2-dr. |
| 24 | 950 | 2 | 17 | 4941 | 1361 | 1179 | 1955 Chevrolet Bel-Air Sport coupe. |
| 25 | 0 | 8 | 2 | a2e1 | 1412 | 1201 | 1963 Corvette |

The separate ad head is 2. Traversal is
2,0,1,16,24,17,3,4,5,6,8,7,10,11,12,13,14,15,18,19,9,20,21,22,23,-1.
Names contain literal periods/backticks (line-break formatting), except the
King's shorter name. Retain those resource strings unchanged.

## Edge cases and remaining uncertainty

- The loader checks byte counts, not member ranges, list cycles or text bounds.
  Consumers trust the shipped resource. Invalid ranks could index past nine
  model scores; invalid stock variants could exceed their existing tables.
- nextAd terminates on any negative value, though only -1 is shipped.
- The King's zero price is still consumed if his car is won; it does not mean
  a missing record. His next-ad value has no effect on the ordinary ad chain.
- No historical real-world horsepower or engine-liter interpretation is implied
  by rank. It is an ordinal game tuning scale with several separate consumers.
- There are no unresolved questions about the six fields' representations or
  readers/writers. Equipment variant labels carry the pre-existing issue below.

## Other misleading identifiers (outside the rename scope)

- `SPEC_POLICE` (`BOBS.H:39`) names index 25, but that HOT_DATA record has the
  King's Corvette body picture 1201 and the King's racer uses this index. It
  is not a police-car specification. Leave the identifier unchanged here.
- `MANIFOLD_DUAL_QUAD` / `MANIFOLD_TRI_POWER` (`GAME.H`) appear reversed against
  catalog text: INPUTDAT.ASM assigns the former to the three TRI_POWER
  descriptions, and the latter to the DUAL_QUAD descriptions (entries 31..36).
  Investigating those variant names is separate from this record's bit layout.
- `item` in build_rival_pool (`BOBS.C:276,281`) receives a specification pointer
  that is never read. No deletion or local rename is needed for this task.

## Implemented changes and replacement inventory

- `struct Spec` -> `struct CarSpec` at its definition, array definition/extern,
  rival-pointer definition/extern/forward declaration, and the two PERF pointer
  locals, GAME advert pointer and BOBS unused pointer. The member names
  `price`, `next`, `parts` become `basePrice`, `nextAd`, `stockParts` at every
  use of this record. Retained `rank`, `nameOffset`, `bodyPic` are accurate.
- Added the struct and member Doxygen contracts plus the file brief in DISK.H.
  Every member retains its original type, signedness and offset.
- Added `PartFit` in DISK.H, matching EQU definitions in the already included
  DISK.INC, and replaced all 43 live part-catalog compatibility bytes in
  INPUTDAT.ASM: GM/Ford/Chrysler engine, transmission and manifold entries;
  the universal four-speed, all carburetors and all tires. The all-zero
  sentinel retains its zeros. Item.fitMask remains char and refers to PartFit.
- Named all five stock-variant shifts and extraction masks, plus SPEC_FIT_MASK.
  Replacement sites: GAME gm_make_car (all six fields), buy_from_ads (engine
  and tires); PERF pf_drivetrain_spec and pf_vmax (all six fields, including
  both manifold reads); BOBS select_rival (tires) and accepts_bet (engine);
  SAVEGAME sg_init_racer (engine); GARAGE ga_transmission and ga_hood (all four
  manufacturer intersections). Preserved parentheses, relational forms and
  unsigned arithmetic. Used existing TRANS_RACING in each of the three
  stock-transmission catalog-offset calculations instead of raw 3.
- No new equipment variant enum, duplicate count or rank category was added.
  The byte rank is an arithmetic tuning scale; categorical CarClass is separate.
- `sr rename Spec CarSpec` rejected the type name without changing files:
  this command only resolves function/global/far-data symbols. Applied the tag
  and member edits directly to the mapped source uses and documented the tool
  limitation in .claude/skills/sr-cli/SKILL.md.

## Validation

All required gates passed on the final source:

- `./sr headers --check` and `./sr format --check`: clean.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- `rm -rf work/layout && ./sr build`: 96/96 object gates, 572/572 raw-byte
  exact functions at their original positions, 748/748 exact near calls,
  and `Packed EXE vs orig/SR.EXE: IDENTICAL`.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`:
  106 tests pass.
- `nix-shell shell.nix --run "python3 tools/scripts/check_data_types.py"`:
  157 types, 711 sizeof/offsetof checks, zero mismatches. This includes the
  unchanged ten-byte CarSpec layout.
- `./dosbox.sh --make`: DOS-native packed EXE IDENTICAL; both files' SHA-1
  is `3a5e28b795313028b7e77d147122f41cb29de334`.

The build's separate unpacked-reference diagnostic reports header/relocation
ordering differences and a 14-byte load-image length difference; it reports
zero differing load-image bytes. Both independent packed-file comparisons
above are byte-for-byte identical to the shipped EXE. No game behavior or
record-layout change was made.
