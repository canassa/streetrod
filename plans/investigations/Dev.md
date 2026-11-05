# Dev investigation

## Purpose and scope

`struct Dev` is the saved interpreter state of one PC-speaker music/effect
stream, not a hardware device. Rename it `MusicVoice`. The background and effect
states share one selected voice and one physical speaker; they are not mixed.
Only this type, its members, and corresponding accesses are in scope.

Evidence below uses the pre-change source lines. The note was written before
renaming. The final type belongs in SYS.H because SYS.C defines both instances
and its timer interprets them; SOUND.H needs only a forward declaration for its
selected-voice pointer. Related Tone/ToneSlot definitions remain with MUSIC.H.

## Storage, initialization, and persistence

SYS.C:42-43 tentatively defines `g_songBgState` and
`g_secondaryMusicStateData`. The symbol metadata places them at DGROUP offsets
0x79aa and 0x79c6, respectively: two consecutive 28-byte records, word aligned.
SOUND.C:67 initializes `g_voice` to the background instance. SYS.C:297-303
initializes the background stream, both table bases/cursors, and both playing
flags. MUSIC.C:40-43 supplies the secondary stream and repeat table before
starting it. Ordinary C startup zero-initializes tentative storage. No file loader populates
either record; the far streams are linked
in SONG.ASM and EFFECTS.ASM. SAVEGAME.C:337-345, 466-478 serialize game state,
not these records or their pointers. Playback state persists in memory when
another voice is selected, and is not part of a saved game.

## Members and fixed layout

| Offset | Original / final | Meaning and complete access evidence |
|---|---|---|
| 0 | divisor (unchanged) | Signed int word overlaid with two unsigned bytes. SYS.C:929 reads the next stream divisor; zero means rest. SYS.C:896,901,936 use the base divisor for vibrato, and :938-939 send low/high bytes to PIT channel 2. Actual effect words include 0xffff; preserve signedness and the unsigned cast used for vibrato gain. |
| 2 | ticksLeft (unchanged) | Signed countdown in calls to the normal timer ISR. Start writes cnt+1 (:629); ISR decrements (:895), checks zero (:906), then reads a nonzero duration (:907). |
| 4 | repeatCount / phrasePasses | Completed passes of the current phrase, initially zero (:626), incremented at each nonterminal phrase boundary (:917), reset on advancing/restarting (:924). A repeat-table value N permits N additional passes, so the last comparison observes N+1. |
| 6 | active / playing | Char boolean. Pause clears (:93-94), resume sets (:106-107), initialization clears (:297,302), start toggles (:623,633,635), stop/end clears (:642,911), ISR gates on it (:895,906). Preserve char size and signedness; name assignments with TRUE/FALSE. |
| 7 | dirty / speakerPending | Char boolean requesting speaker re-enabling on the next audible note. Set by pause/resume (:97,108), start/stop (:627,645), stream end (:912), and rest (:930). Tested with speaker state (:940,943), cleared only after enabling (:944). It does not mean the pitch/divisor is dirty: each audible note programs the PIT regardless. |
| 8 | song / stream | Far pointer to signed instruction words, including the initial stop pair. Initialized (:298), used for initial/reset cursors (:299,628,646,913), loop restart skips two words (:919), and resume eligibility compares with it (:106). MUSIC.C:21 compares stream identity; :40 replaces the secondary stream. |
| 12 | phraseStart (unchanged) | Far pointer to the current phrase's first duration. Initialized/reset with nextWord (:299,628,646,913), advanced at a boundary (:923), used to replay the phrase (:926), and compared with stream to distinguish stopped from paused (:106). MUSIC.C:40 sets secondary base. |
| 16 | cur / nextWord | Far pointer to the next instruction word. Initialized/reset as above; post-incremented to read duration, boundary argument, and divisor (:907-908,929); rewound to stream+2 (:919) or phraseStart (:926). MUSIC.C:40 initializes it. |
| 20 | repeatTable (unchanged) | Near base of signed word counts of extra passes, ending in a negative loop sentinel. Set for background (:300) and secondary (MUSIC.C:41), used to reset repeatCursor (:920). |
| 22 | repeatCursor (unchanged) | Near pointer to current phrase's extra-pass count. Initialized with repeatTable; consulted at each boundary (:917), advanced only after all passes (:918), reset at negative sentinel (:920). |
| 24 | toneTable (unchanged) | Near base of enum ToneSlot selections, one per phrase pass rather than per unique phrase. Initialized for each voice (:301,303), resets toneCursor at start (:630) and stream wrap (:921). |
| 26 | toneCursor (unchanged) | Near pointer to current pass's tone selection. Incremented at every nonterminal boundary (:916), including repeats. Read on start (:631), boundaries (:927), and background re-selection (MUSIC.C:32). |

All members retain their size, signedness, order, and pointer distance. The
anonymous divisor union retains int w and unsigned char b[2]. Total size is 28.
Neither boolean can become BOOL because that would enlarge the record.
There is no new enum: counts/durations/divisors are quantities, booleans use the
existing TRUE/FALSE constants, and tone pointers already use enum ToneSlot.

## Stream and table interpretation

1. GAME.C:61 starts background at word offset 2 after ten ticks;
   MUSIC.C:43 starts an effect at offset 2 with no extra delay. The offset skips
   each stream's initial (0,0) stop pair. Start sets ticksLeft to delay+1.
2. The ISR (SYS.C:895-945) decrements the countdown. A nonzero countdown holds
   the current note and optionally modulates its divisor. On zero it consumes
   (duration, divisor) pairs. A positive duration with zero divisor is a rest.
3. A zero duration consumes a control word: zero stops and resets stream cursors;
   nonzero ends a phrase. All linked phrase controls are (0,1).
4. Each phrase boundary advances the tone cursor. The completed-pass counter is
   incremented and compared against the current repeat count. Further passes
   rewind to phraseStart. Exhaustion advances repeatCursor and phraseStart,
   clears phrasePasses, and a negative repeat entry wraps all three cursors to
   their bases (the stream skips its initial two words).
5. An audible note programs low then high PIT bytes and enables the speaker when
   sound is allowed and the saved speaker state or speakerPending requires it.
   A rest/end/pause silences the speaker and leaves speakerPending set.

SONG.ASM has 2368 signed words, 18 nonterminal phrase boundaries, and positive
note/rest durations from 5 to 99 timer calls. SOUND.C:26 supplies 18 extra-pass
counts (0..2) and -1. Their 36 total phrase passes match 36 tone selections plus
a trailing TONE_END at SOUND.C:28-64. The ISR resets toneCursor before consuming
that terminal selection. Effect streams each have one phrase boundary, followed
by a stop pair; their positive durations span 1..36 calls. The char array
`g_jingleRepeats = {0,0,-1,-1}` (SOUND.C:27) is intentionally passed through void*
and read as the two signed words 0,-1 on little-endian x86; do not retype it as
an int array of four elements. Effect tone tables contain TONE_PROGRAMMABLE
followed by TONE_END. The negative repeat sentinel resets the cursors before
either that terminal tone or the trailing stop pair is read, so effects loop
until their wrappers select the background stream again. The background also
loops through its negative repeat sentinel.

## Selection, side effects, and edge cases

MUSIC.C:19-46 identifies a return to the background by stream-pointer identity:
it restores the user's music setting, selects the retained background record
and tone, and does not restart its cursor/countdown. Any other stream replaces
the secondary record's stream and repeat pointers, selects it, and starts it.
The shared vibrato phase/gain/countdown live outside this record, so switching
voices does not preserve a completely independent oscillator state.

Pause/stop/start and the ISR also change the speaker ports and g_speakerOn;
pause/resume update the garage hotspot label and hover state. Music enable and
saved-enable globals govern selection/start/resume independently of playing.
SYS.C:438 checks the separate g_voiceNum marker to disallow the music hotkey
during an effect; GARAGE.C:961-962,1002-1003 pauses/resumes the selected voice
and changes that marker while manually generating tuning tones.
Stop does not clear every member; pointers, counts, divisor and pending flag
are meaningful only in their playback context. Resume rejects phraseStart ==
stream, which represents a stopped stream. Start with music disabled only clears
playing. No bounds validation is performed on stream/table pointers, durations,
repeat counts, or tone slots; all callers supply linked internal data.

## Other misleading identifiers (out of scope)

- MUSIC.C:19 `mu_play` parameter `tones` is assigned to repeatTable/repeatCursor
  (:41); it is a repeat-count table, not a tone-selection list. Its existing
  leading comment is correspondingly wrong. Leave these outside this type task.
- `g_secondaryMusicStateData` and `g_songBgState` are playback voices for an
  interrupting effect and the background song; their inconsistent suffixes hide
  this relationship (SYS.C:42-43, MUSIC.C:29-43). Global renames are out of scope.
- `g_voiceNum` is a background-music-toggle permission/selection marker, not a
  number of simultaneously active voices: MUSIC.C:30,39 write 1 or 2, and
  SYS.C:438 permits the music hotkey only for 1. GARAGE.C:962,1002 also writes
  2/1 around manual timing tones without changing g_voice. Thus it is not
  strictly a voice index either; only g_voice is advanced by the ISR.

## Open questions and validation

No unresolved member semantics. Historical identifier spellings are unknown;
new names describe the observed program. The stream/table reader trusts its
inputs; malformed-data behavior is not an API contract.

Replacement inventory: all Dev tags in SOUND.H/C and SYS.H/C; member accesses
for repeatCount, active, dirty, song, cur on these voices only in SYS.C and
MUSIC.C. Other records' active/dirty/cur/song members are untouched. Char flags'
0/1 assignments become existing FALSE/TRUE. No layout or logic changes.

## Validation results

- Real MSC 5.1 sizeof/offsetof probe: 28 bytes; offsets
  0,2,4,6,7,8,12,16,20,22,24,26, exactly the original layout.
- `./sr headers --check`: pass; `./sr format --check`: 157 files checked.
- `./sr docs`: zero diagnostics.
- `./sr check --all`: 572/572 match (551 MATCH, 21 MATCH*).
- Clean `rm -rf work/layout && ./sr build`: 96/96 object gates;
  572/572 raw game functions at original positions; 748/748 near-call encodings;
  packed executable IDENTICAL to orig/SR.EXE.
- `./sr dgroup check` and `./sr dgroup order`: pass.
- `nix-shell shell.nix --run "python3 -m unittest discover -s tests"`:
  all 106 tests pass.
- `./dosbox.sh --make`: DOS-native SR.EXE IDENTICAL to orig/SR.EXE
  (SHA-1 3a5e28b795313028b7e77d147122f41cb29de334).

The build's unpacked-image diagnostic reports different MZ header/relocation
ordering and 14 missing load-image tail bytes; the required packed comparison
is byte-identical. No tooling or comparison masks were changed.
