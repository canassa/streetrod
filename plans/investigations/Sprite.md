# Sprite investigation

## Purpose and scope

`struct Sprite` is the transient state for one moving image: its current frame,
screen position, motion cursor, background/composition resources, optional
foreground restoration areas, and display-page history. The name is accurate.
This investigation covers Sprite only; Frame, Seq and Page remain unchanged.

This note was written before the member renames below. Evidence line numbers
refer to the pre-edit source (member-only replacements preserve C line numbers
until formatting).

## Storage, construction and lifetime

`SPRITE.C:51-54` defines the shared frame table, ten motion records, the single
`g_mainCarSprite`, and the two page records. `sp_setup` always overwrites and
returns that one Sprite (`SPRITE.C:86-131`), so callers cannot keep independent
live sprites. The instance is near communal DGROUP storage (original offset
0x7d18, tools/sr/symbols.json); its total size is 22 bytes. There is no Sprite
file record, resource decoder or save-game serialization: the repository's
Sprite references are restricted to the engine, its three video backends,
car/carhop/tire animation callers, and pointer declarations in their headers.
SAVEGAME.C does not refer to this type or its storage.

The struct owns no allocation. `sp_setup` borrows the caller's bitmaps and
rectangle pointers, constructs shared Frame/Page records and initially points
at the shared motion array. Carhop replaces that cursor with a four-record
stack array which remains live through the synchronous animation
(`CARHOP.C:138-161`). Arena allocations are released by the animation callers;
none of the stepping functions allocates or frees Sprite members. The stored
pointers can consequently outlive their usable backing data after an animation.

## Members and fixed layout

All offsets/sizes below are the DOS medium-model layout, not host sizes.

| Old member -> final member | Offset, bytes | Role, value set and evidence |
|---|---|---|
| frame (kept) | 0, 2 | Near pointer to the current shared Frame. Setup selects the first ring entry; advance selects a link by horizontal speed; reverse reverses the four-frame ring and selects a phase (`SPRITE.C:90-104,172-198,303-320`). Actual callers build one- or four-frame animations. It is not a heading or an image allocation. |
| img -> workBitmap | 2, 4 | Borrowed far bitmap pointer. EGA restores newly exposed page areas from it (`EGASPR.C:48-55`); CGA composes into it, restores it from background and copies the dirty area to the destination (`CGASPR.C:50-55,65,106,126`). Tandy never reads it: composition uses r_vid.back (`TANDYSPR.C:50-58,75,120-125`), and cp_backdrop explicitly returns a null work bitmap for Tandy (`CARPIC.C:212-223`). It is not the sprite's frame picture. |
| save -> background | 6, 4 | Borrowed far bitmap containing background pixels used under transparent frame pixels and in exposed areas, plus an optional foreground mask. Setup takes it from the caller (`SPRITE.C:108`); all three backends read it for composition and/or restoration (`EGASPR.C:51,64,75-94`; `CGASPR.C:51,55,65,89-106`; `TANDYSPR.C:54,58,75,101-120`). cp_backdrop copies the captured scene into this bitmap and adds the scene mask (`CARPIC.C:194-210,227-240`). It is not a buffer filled with saved pixels by the step operation. |
| rc -> overlayRects | 10, 4 | Two interchangeable nullable near Rect pointers, borrowed from setup arguments (`SPRITE.C:109-110`). All backends loop over both, intersect with current clipped bounds, then masked-blit the background over the frame (`EGASPR.C:71-95`; `CGASPR.C:85-108`; `TANDYSPR.C:97-122`). Equal frame/background pitches mean X is relative to the sprite and Y is absolute; unequal pitches mean X is absolute and Y is relative. Source offsets also account for Tandy's background origin. These are foreground restoration areas, not the sprite's clip bounds (which belong to Page). |
| x (kept) | 14, 2 | Signed screen X in pixels, before the backend aligns it to a byte. Setup copies the starting X; reverse resets it from the last displayed X; advance adds the current horizontal delta (`SPRITE.C:106,178,313,329`). Negative/offscreen positions are intentional (`CARHOP.C:141`; `TIRES.C:145`). |
| y (kept) | 16, 2 | Signed top screen row in pixels. Setup converts the caller's inclusive bottom row by subtracting frame height minus one (`SPRITE.C:91,107`). Advance adds vertical delta (`SPRITE.C:314`). Tire/jack callers also adjust it during stepping (`TIRES.C:96,161,348,412`); negative positions are allowed by the clipping path and vertical slide. |
| seq -> motionStep | 18, 2 | Near pointer to the current mutable Seq record. Setup starts at the shared array; carhop substitutes stack records (`SPRITE.C:130`; `CARHOP.C:143-145`). set_step indexes relative to this cursor and writes a following terminator (`SPRITE.C:203-210`). Advance decrements count, moves to the next record at zero, then applies that record's deltas and selects a frame (`SPRITE.C:309-319`). Backends stop on a negative count before drawing. Counts/deltas belong to Seq, not a value set stored directly in Sprite. |
| page (kept) | 20, 2 | Near pointer to a destination Page plus its previous clipped bounds. Setup builds a two-page EGA ring or one self-linked page elsewhere (`SPRITE.C:111-129`). EGA advances it before drawing and copies pages at termination (`EGASPR.C:38-48,98`); CGA/Tandy use the current entry (`CGASPR.C:48`; `TANDYSPR.C:52`). Vertical slide overrides the page bitmap with r_vid.back (`SPRITE.C:286`). |

No member is a boolean, enum, bit mask, or small integer mode. The two rectangle
slots have identical semantics; their capacity is not a distinct slot-kind value
set. No type changes, enums, new constants, or raw-value replacements are needed.
All member sizes, signedness, order and pointer qualifiers stay unchanged.

## Producer and reader contexts

- `cp_show_car` supplies one or four car frame images and backend-specific band
  data, prepares work/background bitmaps via cp_backdrop, plans horizontal motion,
  and preserves the resulting page rectangle (`CARPIC.C:438,474-489,558-574`).
  Static car display exits before Sprite setup (`CARPIC.C:540-555`).
- `ch_skate` supplies one mutable shifted carhop frame, a composition bitmap,
  a masked background and one full foreground rectangle, then installs its own
  motion records (`CARHOP.C:122-160`).
- `swap_wheel` and `ti_change` use one-frame vertical movements, four-frame tire
  rolling, and jack/car lifting; optional rectangles restore the masked vehicle
  background over the moving object (`TIRES.C:88-118,143-163,310-328,388-422`).
- `sp_slide_v` captures a full-height strip, copies it into a second background
  bitmap, sets one frame and a vertical motion step, and draws to the back page
  (`SPRITE.C:264-297`).
- The video callbacks call `es_step`, `cs_step`, or `ts_step`; each reads the
  Sprite, draws the current frame, then calls sp_advance. EGA changes the page
  cursor; none writes the work/background/overlay pointers after setup.
- `sp_clip_span` reads only X/current-frame width to limit jail-window drawing
  (`SPRITE.C:325-341`; each backend's jail branch).

Supporting callees confirm the bitmap roles: cb_compose reads frame bits/mask
and background pixels and writes the composition destination (`CGABLT.C:53-72`);
tb_compose does the same in four interleaved banks (`TANDYBLT.C:76-120`);
eb_sprite uses the third bitmap as background under the frame (`EGABLIT.C:98ff`).
Rect subtraction selects exposed areas; intersection and box clipping apply
screen bounds and horizontal byte alignment (`SPRITE.C:19-81,136-168`).

## Behaviour, side effects and edge cases

A step draws at the current coordinates before advancing. sp_advance records the
last displayed X and frame in g_xCoordSprite/g_spritePic before changing state.
The movement count is decremented before deciding which deltas apply: the first
planned phase includes one extra count for the initial stationary draw. A
negative count terminates the next callback, so the cursor normally points at
an end record once a loop finishes; setup must reset it before another plan.
A zero-length intermediate motion phase is not skipped in a loop: count can
become negative and stop early. These are existing semantics, not changes.

The engine assumes valid frame rings, live background data, and a motion
terminator. Setup has no zero-frame/bounds validation. Frame selection uses
abs(dx) >> 2 to index three links (built as one-, two- and four-frame skips),
matching car/tire speeds 2, 4 and 8 (carhop also uses 1 and 3). The signed coordinates and
nullable overlay pointers must not be retyped as unsigned or boolean values.
EGA has a final page synchronization path; CGA and Tandy return immediately on
negative count. Independent concurrent Sprites are unsupported by shared tables.

## Other misleading identifiers (outside scope)

- `g_mainCarSprite` is also the carhop, tire, jack and slide Sprite, as proved by
  every sp_setup call above. Its car-only name overstates its scope.
- `Frame.pos` is the actual far frame bitmap, not a position; setup copies an
  image pointer and the backends read its dimensions/bits (`SPRITE.C:93`;
  `EGASPR.C:52-68`; `CGASPR.C:53-76`).
- `Frame.data` is backend-dependent band data: EGA casts it to Mask, while
  CGA/Tandy read it as a Bitmap (`SPRITE.C:94-98`; `EGASPR.C:64`;
  `CGASPR.C:52,57,69`; `TANDYSPR.C:55,60,79`). Its declared Bitmap type does
  not describe the EGA interpretation. Do not propagate that type into new docs.
- `tb_compose` parameters `dst` and `src` are opposite their actual roles:
  dst supplies frame bits and mask, src receives the writes (`TANDYBLT.C:76-120`).
- `sp_reverse`'s existing heading comment is misleading: it reverses frame-ring
  traversal and chooses a phase from screen X, rather than reading a heading
  (`SPRITE.C:180-197`).
- `bm_copy_box`'s parameter names dst/src are reversed relative to its copy
  calls (see its implementation before reusing those labels); callers in the
  sprite engine pass the source first (`SPRITE.C:291-295`).

## Planned changes and replacement list

Keep Sprite, frame, x, y and page. Rename only the four proven members:
img -> workBitmap, save -> background, rc -> overlayRects, seq -> motionStep.
Replace the Sprite-qualified uses in SPRITE.C, EGASPR.C, CGASPR.C,
TANDYSPR.C, and CARHOP.C; unrelated img/save members remain unchanged.
Add the header's Doxygen file block and one concise description per Sprite
member. No functions, locals, globals, or related types are renamed.

## Open questions

The original programmers' spelling and the original upper limit on simultaneous
Sprite objects cannot be recovered from these names. The reconstructed engine
has one actual instance and shared tables. No unresolved member role blocks the
renames. Frame.data's polymorphic representation needs its own investigation.

## Validation

- `sr headers --check`: includes and dependencies current.
- `sr format --check`: 157 files checked.
- `sr docs`: zero diagnostics; Sprite appears in the generated XML.
- MSC 5.1 probe: sizeof 22; offsets 0, 2, 6, 10, 14, 16, 18, 20, all nine checks passed.
- `sr check --all`: 572/572 (551 MATCH, 21 MATCH*).
- `sr diff sp_advance`: zero mismatched bytes, 139-byte function; whole SPRITE.C span matches.
- Clean `sr build` after removing only work/layout: 96/96 objects pass; packed EXE IDENTICAL.
- `dosbox.sh --make`: DOS-native EXE IDENTICAL (SHA1 3a5e28b795313028b7e77d147122f41cb29de334).
- `sr dgroup check` and `sr dgroup order`: passed.
- Nix-shell unittest discovery: 106 tests, OK.

The build also prints a separate unpacked `linkcheck` diagnostic: that full
file differs from orig/SR_unpacked.EXE (header/relocation ordering and a
14-byte load-image tail difference). All 572 mapped functions are at their
original positions and raw-byte exact, with 748/748 near calls exact. The
build explicitly treats this unpacked comparison as diagnostic
(tools/sr/linklayout.py:146-161); the requested packed-byte gate and native
packed build both pass. No tool settings or comparison rules were changed.

Additional outside-scope documentation issue: EGASPR.C:34-35 calls the background
local a save buffer and the work bitmap a sprite image. Their actual uses at
lines 50-64 establish the roles described in the member table. Existing comments
and local names are preserved for a separate function investigation.
