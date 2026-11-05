# Street Rod: curiosities, unused content and small surprises

Investigated on branch `curiosities`, source base `98346d7`. This is a static
investigation: **the game was not run**. “How to see” gives routes suggested by
the code, not tested playthroughs. Picture descriptions come from decoded
resources, not screenshots. No PNGs accompany this note.

I read all 59 C and 37 ASM files under `src/` (36,910 lines), including the text,
music, fonts, scenery and span tables; decoded the shipped HOT_DATA/HALL_DAT;
listed pictures with `./sr pic --list`; and inspected all 277 decoded pictures
(IDs 1–9 and 1000–1267) in contact sheets, enlarging odd/unused ones. Repetitive
raw span data was also parsed to check record counts and consumers. Citations
refer to the source base above. Binary-art evidence is identified by library
and picture ID, with the source declaration/use alongside it. Identifiers in
this reconstruction are descriptive names, **not recovered 1989 spellings**;
neither an appealing symbol name nor a source comment establishes author intent.

Existing exploits, builds and record-layout findings are deliberately not
re-listed. In particular, see [exploits.md](exploits.md),
[fastest-start.md](fastest-start.md), [misnames.md](misnames.md),
[Racer.md](Racer.md), [Cmd.md](Cmd.md) and [terrain.md](terrain.md) for the
already-known mechanics this note occasionally builds on.

## Best finds

### 1. An unused police passing picture — and what replaces it

**What:** picture **1245** is a 96×88 close passing view of a silver/grey sedan
quarter, wheel and bumper. It exists in both the EGA and CGA library sets; the
CGA decode is cyan/magenta/white. The police enum names it, but no source load
or picture table refers to it (`src/SYS/DISK.H:265`; decoded
`orig/freeware/LIB2`, ID 1245, and `orig/extra/LIB2C`, ID 1245).

**Evidence:** police-frame construction explicitly takes its closest passing
frame from the current opponent: `g_policeFramesTable[0] = g_spritesOpp[0]`.
It loads the other police pictures, 1241/1242/1246, separately
(`src/GAME/VIEW.C:267–277`). Every ordinary rival's passing view is picture
1010; the Corvette set uses 1003 (`src/GAME/VIEW.C:61–66`). When pursuit starts,
the code replaces opponent slots 0 through 11 with police frames and changes
the paint to white (`src/GAME/RACE.C:1359–1374`). Thus the chase inherits the
rival's passing artwork instead of displaying the dedicated 1245 image. This
extends the already-known “cop uses the King's performance spec” finding in
[misnames.md](misnames.md); the picture is separate unused content.

**How to see:** decode `./sr pic 1245 --png /tmp/police.png` to see the unused
art. A normal road-race pursuit exercises the substitution; its visible result
is a source prediction, not a captured gameplay observation. No normal trigger
for the unused picture was found.

### 2. `auto` is a real command-line driving assistant; `demo` is a staged player

**What/how to see:** the executable accepts bare, case-insensitive `auto` and
`demo` arguments, e.g. `SR auto` or `SR demo`. These are startup options, not
words entered into a cheat console (`src/SYS/MAIN.C:318–331`). Only `demo`
sets the startup flag that bypasses manual copy protection and later becomes
`g_demo`; `auto` sets a separate driving flag (`src/GAME/GAME.C:69–76`).

Both modes steer toward a lane chosen relative to the opponent, compensate
for curves, and suppress ordinary lateral drift/tire-failure pull
(`src/GAME/ROAD.C:962–978,1042–1048`). Race throttle/shift assistance depends on
whether the rival is ahead or the finish is within 60 road indices; it is not
simply “hold full throttle forever” (`src/SYS/SYS.C:1120–1135`). There is a
useful distinction: transmission-failure tests and car-collision handling skip
`g_demo`, whereas `auto` does not receive those exemptions
(`src/SYS/SYS.C:1183–1190`; `src/GAME/RACE.C:1439–1448`). Neither observation
establishes universal invulnerability or an automatic win.

The demo actually operates menus. It chooses actions from current keyboard
maps, finds matching hotspots, aims the cursor at their centres, and injects
key presses followed by releases (`src/SYS/SYS.C:714–718,725–805,808–865`).
The transmission menu even has a repeating action script
(`src/SYS/SYS.C:769–772`). It is a running simulation rather than a recorded
movie.

Its stage management goes beyond the previously documented Geek opponent:

- New-demo cash is $10,000 (`src/GAME/SAVEGAME.C:124–126`;
  `src/GAME/SAVEGAME.H:14–15`). An unaffordable purchase receives an extra
  $1,999 before the normal affordability test
  (`src/GAME/GAME.C:485–490`; `src/GAME/GAME.H:38`).
- If the spare-parts list is empty at the newspaper, it creates catalog part
  42 (`src/SYS/SYS.C:854–855`).
- Rival selection inserts the demo racer twice, and makes the first-arrival
  pool contain only that racer (`src/GAME/BOBS.C:310–316`). This explains why
  the Geek is encountered so readily, beyond his initialization described in
  [Racer.md](Racer.md).

### 3. The Tandy ending stops one kiss frame short

**What:** the ending's lips sheet, picture **7**, has nine animation cutouts.
The last is the largest, 224×101 pixels (`src/GAME/BOBS.C:700–703`;
`src/SYS/DISK.H:17`; decoded `orig/freeware/LIB1`, ID 7). The ending normally
iterates all nine, but Tandy explicitly shrinks the arena by 10,000 bytes and
decrements the count to eight (`src/GAME/BOBS.C:917–926`). Consequently Tandy
omits the biggest lips frame.

**How to see:** complete the King's final road race in Tandy mode and compare
the kiss with another backend; or inspect the sheet and the loop statically.
The connection to memory pressure is a reasonable inference from the adjacent
`ar_shrink(10000)`, not a recovered explanation from the developers. The
`if (flag8 == 0)` around the kiss is not evidence of a selectable alternate
ending: this function initializes that local to zero and never changes it
(`src/GAME/BOBS.C:843,917`). The hero message still follows
(`src/GAME/BOBS.C:966`).

### 4. Copy protection has music-controlled jukebox lights

**What/how to see:** the manual question is drawn over jukebox picture **8**.
On EGA/Tandy its palette entries 6, 7 and 8 rotate while the question waits,
but only when music is enabled **and the speaker is on**. Thus muting music
also stops the light effect (`src/GAME/FAME.C:55–80`;
`src/SYS/SYS.C:418–426`; `src/SYS/DISK.H:18`). The condition tests low timer
bits in a polling loop, so it should not be described as exactly one rotation
per sixteen ticks (`src/SYS/SYS.C:418–426`).

There are exactly 51 stored page/colour pairs: the first 18 ask about car
icons, the rest car keys (`src/GAME/FAME.C:30–35,51–67`;
`src/GAME/FAME.H:16–17,41–45`). The selector uses 1–0 for choices 1–10 and
F1/F2 for 11/12 (`src/INPUT/KEYMAPS.ASM:407–411`). A wrong answer returns
false and immediately calls the ordinary exit routine; there is no retry loop
or later sabotage in this path (`src/GAME/FAME.C:80–85`;
`src/GAME/GAME.C:69–71`). That routine restores hooks/video, silences the
speaker and exits with DOS status **5**, also used by ordinary callers of
`sy_exit` (`src/SYS/SYS.C:186–201`).

## Hidden-input leads, resolved carefully

### The mysterious `0x62`/`0x63` values are demo shutdown states

**What:** in demo mode most real keys are discarded. Ctrl and M remain
exceptions; Escape writes `0x63` (`src/INPUT/KBD.ASM:85–97`). The driving loops
turn that into `0x62` immediately before exiting
(`src/GAME/DRIVE.C:129–132`; `src/GAME/DRVTICK.C:100–103`). A menu notices
`0x63` and calls exit directly (`src/SYS/SYS.C:414–417`).

Exit converts `0x62` to zero, or another nonzero demo state to one, before
displaying “Quit demo” (`src/SYS/SYS.C:186–191`). This makes the driving exit's
message use ordinary input again. They are not extra startup modes or values
reachable by typing hexadecimal numbers. **How to see:** press Escape during
`SR demo`; compare the menu and driving paths in the citations. No external
runtime input was tried.

### There is a `0x7f` exit test, but no working Delete shortcut was established

**What:** positive input equal to `ASCII_DELETE` (`0x7f`) calls `sy_exit`.
However, raw text capture returns before this test
(`src/INPUT/KBDGET.H:15–16`; `src/SYS/SYS.C:428–435`). The normal scan-code
translation table contains no `0x7f` (`src/INPUT/INPUTDAT.ASM:99–105`), and no
shipped keyboard-map entry supplies it (`src/INPUT/KEYMAPS.ASM:1–476`).

**How to see:** no ordinary key recipe was found. This is an input-handler
remnant, not sufficient evidence for an advertised “press Delete to quit”
cheat. The hardware handler stores the translated byte, not DOS's extended-key
sequence (`src/INPUT/KBD.ASM:117–118,165–167`).

### Two keyboard-map slots have no activating caller

**What:** map 17 is a bare terminator; map 40 contains N→3, L→2, Q→4,
S→1 (`src/INPUT/KEYMAPS.ASM:251–252,413–418`). Their addresses remain in the
map directory (`src/INPUT/KEYMAPS.ASM:448,471`), but a full source search finds
no activation through `KM_UNUSED_17` or `KM_UNUSED_40`. The letters resemble
new/load/quit/save controls; that is an inference, not proof of a cut screen.
**How to see:** source/data inspection only; no normal menu route was found.

## Bob's, jail and genuine bounds bugs

### “Order” is a probability nudge, not a food purchase

**What/how to see:** at Bob's, use Order when available and wait through rival
arrivals/departures. The action only sets `g_ordered = 1` while the carhop is
absent; it neither charges money nor creates food
(`src/GAME/BOBS.C:1013,1152–1155`). Arrival attempts require at least three
departures and pass a random threshold (`src/GAME/BOBS.C:77–85,186–187`;
`src/GAME/BOBS.H:21`). The threshold is 5 + 75 if ordered + 10 at jail +
8×demo-state; ordinary Bob's attempts therefore rise from 5% to 80% after
ordering (`src/GAME/BOBS.C:161–165,195–200`;
`src/GAME/BOBS.H:27–29`). These are chances **per eligible attempt**, not per
frame or per second. The departure/quick-leave rolls can still remove her
(`src/GAME/BOBS.C:86–101,161–165,195–200`). This adds the game-state rules to
the rendering account in [ch_skate.md](ch_skate.md).

### The “jail cell animation” uses the carhop's idle sheet

**What:** picture **1225** is the same 14-cell sheet in both contexts
(`src/GAME/BOBS.C:748–758,778–785`; `src/SYS/DISK.H:245`). There is a 25%
chance to play it when entering Bob's, and a 6% chance per jail timeout
(`src/GAME/BOBS.C:1003–1004,1246–1250`;
`src/GAME/BOBS.H:50–51`). The jail case copies a 16-pixel strip of foreground
over its right edge before drawing, preserving the bars
(`src/GAME/BOBS.C:759–770`). The jail scene itself starts with Bob's background
and adds wall, bars and window resources 1255–1257
(`src/GAME/BOBS.C:1196–1216`; `src/SYS/DISK.H:275–277`).

**How to see:** wait at Bob's or in the debt-jail scene; repeated visits or
timeouts give further rolls, not a guarantee on the first visit. There is no
separate prisoner sheet hidden behind the animation's reconstructed name.

### The idle sequence reads a 31st entry from a 30-entry table

**Proven defect:** `g_animSteps` contains 30 two-byte records
(`src/GAME/BOBS.C:689–692`; `src/GAME/BOBS.H:226–229`). The playback index
starts at zero but stops only when `i >= 31`; index 30 is read for both frame
and delay (`src/GAME/BOBS.C:786,798–804`). This is a one-entry out-of-bounds
read in the shared Bob's/jail animation, not merely an unused table row.

**How to encounter:** let that animation reach its tail without dismissing it
(`src/GAME/BOBS.C:790–808`). In the supplied image the read lands on the first
two bytes of the adjacent `g_animCells`: **1, 15**. It therefore selects
`g_animCells[2]`, cell **7**, for a 15-tick delay, the same cell used to restore
the idle pose afterward (`src/GAME/BOBS.C:693,802–803,810–811`;
`tools/sr/symbols.json:179–193`: offsets 21022 and 21082, exactly 60 bytes
apart). `./sr data 0x525a 16` confirms the original bytes start
`01 0f 07 08 ...`. This bounds error has a concrete, comparatively mundane
tail-frame consequence; no crash or visible glitch was observed.

### Jail can overfill the rival pool

**Proven defect:** the pool has 12 integer slots (`src/GAME/BOBS.C:45–46`).
Ordinary selection caps it at nine, but a nonzero flag raises the cap to
`RACER_KING`, i.e. **21** (`src/GAME/BOBS.C:287–301`;
`src/GAME/BOBS.H:88,272`). Jail passes precisely that flag via
`pick_rival(who, 1)`; unless the direct King roll succeeds, this reaches
`build_rival_pool(1)` (`src/GAME/BOBS.C:321–330,1237,1253`). When more than 12
eligible rivals remain, writes can cross the array boundary; with all 21
eligible, that means nine excess entries. Removed rivals are skipped
(`src/GAME/BOBS.C:280–285,294–302`).

**How to encounter:** the jail's timed rival pass-bys reach this path. **Limit:**
this proves the bad bound; what neighbouring state gets changed and how that
looks to a player were not established by running the game. It should not be
marketed as a reproducible jailbreak or useful cheat.

## Art, jokes and the newspaper

### The garage bottle is clickable and does exactly one joke

**How to see:** click the small bottle at garage coordinates x=3–13,
y=159–186. Its hotspot is “Wanna drink ?” and action 17
(`src/UI/HOTSPOT.C:127`; `src/UI/MESSAGES.ASM:68`). The handler only displays
“Don't drink and drive, bub!” and stays in the garage
(`src/GAME/GAME.C:946–949`; `src/UI/MESSAGES.ASM:430`). There is no intoxication
or drink inventory effect in this handler.

### Sticker names and sticker artwork disagree

**What/how to see:** hover the stickers in the garage selector, then compare
the art. The shipped HOT_DATA text says **'Ranger'**, while picture **1192**
plainly reads **RINGER**. Another hotspot says **'Destiny can wait'**, while
picture **1189** reads **DEAD END KIDS**. These are actual label/art mismatches,
not just questionable modern enum names (HOT_DATA bytes 3756–3955;
`src/SYS/DISK.C:69–82`; `src/UI/HOTSPOT.C:290–291`;
`src/SYS/DISK.H:209,212`; decoded `orig/freeware/LIB2`, IDs 1189/1192).

The other shipped captions include “not bullet proof”, “Love bandit”,
“Big Chief” and “I am not a cop”; the selector associates them with its normal
eight choices (`src/UI/HOTSPOT.C:289–296`; same HOT_DATA block). There is no
unused ninth sticker demonstrated by these mismatches. All eight choices are
handled (`src/GAME/GARAGE.C:573–581`); picture 1218 is the separately named
wipe resource, not an extra sticker (`src/SYS/DISK.H:238`).

### The classifieds advertise things the player cannot buy

**What/how to see:** look at the decorative margins of the newspaper classifieds.
Picture **1027** includes clipped adverts such as **“1967 LOTUS TU…”** and
**“TURBO SPRITE”**. The former is particularly odd beside the printed in-game
year 1963 (`orig/freeware/LIB2`, ID 1027; `src/SYS/DISK.H:47`;
`src/GAME/GARAGE.C:143–158`). Do not expand the cropped Lotus name beyond what
the pixels show.

These are baked-in border pixels: the renderer draws its two 64-pixel halves
at the left and right margins and fills the middle for the actual list
(`src/GAME/ADS.C:66–68,92–98`). The purchasable list is built separately from
the used-car/catalog chains (`src/GAME/GAME.C:451–464`). Thus the border is not
evidence for a working hidden Lotus or Sprite, nor enough evidence by itself
to reconstruct a cut car roster.

### Headline events are random decoration, not a historical calendar

**What:** pictures **1022–1026** contain the Dodgers/tidal-wave news,
drag-racing crackdown, Japanese cars, Oscars/Atlas rocket, and Viet Nam
headlines (`src/SYS/DISK.H:42–46`; decoded `orig/freeware/LIB2`, those IDs).
On a new game day the newspaper chooses one of five pictures. If the first
pick repeats yesterday's, it retries **once**, so repeats remain possible
(`src/GAME/GAME.C:135–141`). There is no selection by month/day or by a recent
crash in this code.

**How to see:** open the newspaper across game days; reopening on the same
day keeps the chosen picture (`src/GAME/GAME.C:135–139`). The displayed date
is generated from a fixed June–September sequence ending on September 15,
with a literal 1963 year (`src/GAME/GARAGE.C:80–100,143–158`). This does not
establish the real dates or accuracy of the headline events.

### Small inscriptions and credits live in pictures

**What/how to see:** open the Hall of Fame and inspect picture **5**:
“VINNIE WAS HERE” is scratched above the right-hand panel. The artwork also
has other blue graffiti, but I do not assign authorship to it
(`orig/freeware/LIB1`, ID 5; `src/SYS/DISK.H:15`;
`src/GAME/FAME.C:194–200`).

The introductory licence-plate picture **4** credits “COMPUTOGRAPHY”, sound
by **DOROTA BLASZCZAK**, development/programming by **P.Z.KAREN CO.
DEVELOPMENT GROUP**, and original concept by **MAGIC PARTNERS**. These
attributions are present in the pixels, independently of reconstructed source
names (`orig/freeware/LIB1`, ID 4; `src/SYS/DISK.H:14`). Let the introduction
continue to see it (`src/GAME/DASH.C:355–382`).

## Unused sounds and data; machine-level tricks

### Sound routines with no path from play

**What:** a whole-source reference search finds no callers for these entries:

| Entry | Evidence | What can safely be said |
| --- | --- | --- |
| `so_low_warble` | `src/SOUND/SOUND.C:320–327`; `src/DATA/EFFECTS.ASM:34` | Would start/stop the linked low-warble stream; that stream is referenced only by this unused wrapper. |
| `so_low_pulse` | `src/SOUND/SOUND.C:353–359`; `src/DATA/EFFECTS.ASM:21` | Equivalent unused wrapper around another effect stream. |
| `so_warble1`, `so_warble2` | `src/SOUND/SOUND.C:391–408` | Two concrete parameter sets for the private warble synthesizer, with no callers. |
| `sk_play` | `src/SOUND/SPEAKER.ASM:49–118` | An unused table-driven synthesizer that directly toggles port 61h/PPI_B; not the timer-driven music playback path. |

**How to see/hear:** no normal trigger was found. These names describe the
code; no recognizable tune, abandoned scene or intended sound meaning was
identified. Nothing was played for this investigation.

### Engine, passing-car pitch and siren share one speaker

**What/how to hear:** rev, watch a car pass at Bob's, or enter a pursuit.
The driving timer alternates which sound programs PIT channel 2: one phase
uses an RPM-squared-derived divisor (with a special redline change), or the
squared horizontal separation of an animated car; the other handles the
siren/tire squeal (`src/SYS/SYS.C:1011–1053`). The passing pitch calculation
uses screen displacement and a cap, rather than a recorded engine sample
(`src/SYS/SYS.C:1027–1035`). The siren takes priority over tire squeal in its
phase (`src/SYS/SYS.C:1037–1053`). These are code-level synthesis rules;
their subjective sound was not assessed.

### `nouemem` disables an offscreen EGA picture cache

**What/how to see:** `SR nouemem` sets the flag checked by the picture-cache
loader (`src/SYS/MAIN.C:330–331`; `src/SYS/DISK.C:393–401`). The cache stores
compressed picture data in otherwise available video planes. Its allocator
walks four plane budgets and returns addresses starting at offset `0x7d00`
(`src/SYS/MEM.C:394–408`). Loading selects a write plane, reads into a work
buffer, copies to that VRAM address, and later selects the read plane before
unpacking (`src/SYS/DISK.C:234–265,329–339`). This is the concrete mechanism
behind the odd switch; its spelling alone should not be taken as evidence of
an EMS/XMS manager. The timing effect of disabling it was not measured.

### Writing to CS is real; opcode rewriting is not established

**What:** the masked blitter stores its saved BP and pitches in words embedded
in the code segment, jumped over on entry (`src/VIDEO/MASKBG.ASM:31–46`). It
calculates an entry point inside an 80-times-unrolled copy body, accounting for
the different instruction lengths around displacement 0x80
(`src/VIDEO/MASKBG.ASM:47–86`). This removes a per-word inner loop.

Likewise, the CGA/Hercules span routine writes a `CS:dbl` flag that was declared
as a data byte (`src/CGA/CGALINE.ASM:356–373`). **How to see:** source/disassembly
inspection. These are striking writable-code-segment tricks, but the cited
writes change scratch data, not instruction opcodes. They do not establish a
self-modifying opcode trick or hidden feature.

### “VGA” selects EGA, while Hercules and Tandy reuse assets differently

**What/how to see:** choose VGA at startup. Selection sets `g_vga` and returns
the EGA choice (`src/SYS/MAIN.C:170–172`); it does not select a separate
256-colour art set. Tandy routes picture decompression through `tp_unpack`
and mask conversion instead of EGA's `rl_decode`
(`src/SYS/DISK.C:335–352`). CGA's span painter uses different odd/even-row
dither tables (`src/CGA/CGALINE.ASM:14–23,427–435`), while the shared Hercules
copy path duplicates odd rows into an additional video bank
(`src/CGA/CGADRV.ASM:61–97`). Inspecting the same scenes with those adapters
would expose the backend differences; no hardware display was tested here.

### HOT_DATA has 52 bytes its loader ignores

**What:** summing the thirteen load sizes gives **4064** bytes
(`src/SYS/DISK.C:69–82`). The supplied `orig/freeware/HOT_DATA` is **4116**
bytes. Its trailing 52 bytes are two little-endian words, **35, 0**, followed
by a byte-for-byte duplicate of the 48-byte decal-layout block at offsets
4016–4063. The loader closes the file after the thirteen fixed reads
(`src/SYS/DISK.C:519–526`).

**How to see:** inspect the file bytes; there is no gameplay route because
this tail is never read by the loader. It is a real packaging/data curiosity,
but not evidence for an extra police specification, additional car, or ninth
sticker. Its historical purpose is unknown. Reproduction without launching
anything:

```python
from pathlib import Path
import struct
p = Path('orig/freeware/HOT_DATA').read_bytes()
assert len(p) == 4116
assert struct.unpack('<HH', p[4064:4068]) == (35, 0)
assert p[4068:] == p[4016:4064]
```

## Verification and boundaries

- Source reference searches covered declarations, static tables, direct calls
  and callback assignments before describing an entry as unused. A whole-tree
  negative search establishes no path in this source base, not what an earlier
  unrecovered version might have done.
- Picture inspection used `./sr pic --list` and `./sr pic ID --png ...`;
  1245 was additionally decoded with `--mode cga`. No image is committed.
- `./sr check --all` passed: **572/572** functions still match, comprising
  551 MATCH and 21 MATCH*. Only this note and the appended diary entry change.
- No gameplay verification, crafted-save experiment, game launch, source edit,
  merge or push was performed. Bad bounds are reported as bad bounds; inferred
  author intent and unobserved crash symptoms are not presented as facts.
