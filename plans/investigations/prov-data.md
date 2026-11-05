# POPUP spare BSS and VIDDET string alignment

## Decision

Retain both complete PROVISIONAL markers. Required storage and byte-equivalent
source alternatives are now established more precisely, but neither historical
declaration is settled. No production source, identifier, type, symbol metadata,
MAKEFILE or link-order change is justified. Leave DASH.C and NEEDLE.ASM alone.

This follows the zero-storage rules in docs/RECONSTRUCTION.md, "Unreferenced
zero storage and alignment", and reuses its prior padbytes/split investigation.
The initial note was written in work/investigations/prov-data.md before trials;
this committed version records the completed evidence. No identifiers were
renamed, and no constants or value sets were added (replacement list: none).

## POPUP: one active near pointer, four bytes of unknown storage

Purpose: retain the selected speech-bubble layout while its saved background is
open. g_popupLayout is a near pointer, not the Win record that it points to.

### Storage and every use

| Object | Storage | Evidence |
|---|---|---|
| g_popupLayout | DS:6C40, two-byte file-static BSS | POPUP.C:17; pu_open writes a single word and later loads it into BX. |
| g_unused24 | DS:6C42..6C45, four-byte file-static BSS | POPUP.C:18; real MSC SEGDEF length is six bytes for this module's BSS. No source use or direct original instruction operand names the tail. |
| g_popupWins | DS:04C4..04F9, initialized 54-byte table | POPUP.C:20-22; three nine-word Win records, not adjacent BSS. |
| g_popupSave | DS:04FA..04FD, initialized far bitmap pointer | POPUP.C:23; separate DATA storage, not the four BSS bytes. |

Every g_popupLayout source use is in POPUP.C:96,105,107-108,110-112,120-121,156.
The original direct word accesses are pu_open at 11C41, 11CA2 (the only store),
11CA9, 11CE4, 11D0B and 11D8A, and pu_close at 11E68. A sweep of all 572 game
functions' decoded operands finds no direct mention of 6C42..6C45. There is no
indexed access to the pointer's own storage, pointer arithmetic over its tail,
address escape to a callee, bulk copy, save/load entry or incremented pointer
that establishes a larger object's extent. Dereferences of BX access the
initialized Win table, not the BSS after the pointer.

pu_open (POPUP.C:83-127) restores any prior background, handles a negative message
index as auto-close, reads g_descHdr[abs(n)] as a one-based layout selector, and
stores &g_popupWins[selector-1]. It saves the background, draws the bubble,
centers its NUL-terminated text lines, optionally updates the front buffer,
and drops the save if auto-closing. pu_open_at (132-148) temporarily changes
the selected table entry's x/y, calls pu_open, then restores the table. Its
negative coordinates preserve the defaults. pu_close (153-161) uses the saved
pointer only when a saved bitmap exists and releases that background. The
pointer is not cleared when the save is released; its old value is guarded by
g_popupSave. There is one active save, with no observed nested layout stack.

The callers in GAME.C use this same single-popup lifetime: gm_message
(98-110), sell_part's offer (259-261), counter_offer (558-572), and gm_popup
(743-754). They pass a message identifier, optionally coordinates, then close
the window after a pause or menu choice. None receives the layout's address.
The three layout selector values establish the initialized table's size;
they provide no evidence for three saved pointers in BSS.

A Win value has nine int members (POPUP.H:11-21), hence eighteen bytes under
MSC. The three-entry table and its stride are consistent with that size.
A six-byte BSS allocation cannot be a Win value. A near-pointer array of three
entries or a six-byte record containing the live pointer and two unused words
is possible; neither is established by the live pointer operations.

### Whole-module trials

Compiled copies of the entire POPUP.C with its unchanged MAKEFILE flags,
`/AM /Oalsi /Gs /FPi /NTS0F38_TEXT`, using the real MSC 5.1 under bounded emu2.
All four variants have the identical 1066 code bytes, 58 initialized DATA
bytes and ordered, target-resolved fixups. SEGDEF lengths and COD static
allocations were inspected separately: BSS reserves storage without LEDATA.
Each alternative then replaced only POPUP.OBJ in an isolated copy of the
fresh baseline objects and linked with the unchanged packed LINK command.

| Variant | BSS size | Packed EXE vs orig/SR.EXE |
|---|---:|---|
| Retained scalar pointer + g_unused24[2] | 6 | IDENTICAL baseline |
| Delete g_unused24 only | 2 | 1426 differing bytes |
| `static struct Win *g_popupLayout[3];`, replace every live use with `[0]`, remove g_unused24 | 6 | IDENTICAL |
| Six-byte private record `{ struct Win *layout; int unused[2]; }`, replace every live use with `.layout`, remove g_unused24 | 6 | IDENTICAL |

The record definition existed only in the scratch probe, not a production C
file/header. Both grouping hypotheses preserve all code and the packed file,
so byte identity cannot choose between a spare variable, array tail, or record
tail. Four bytes beginning on an even address cannot be a WORD alignment pad.
Folding them into either unproved aggregate would relocate the uncertainty
rather than resolve it. Keep the original declaration and marker verbatim.

## VIDDET: real string bytes, unresolved alignment origin

Purpose: vd_detect saves the original BIOS mode and equipment byte and records
video-adapter probe results; vd_restore restores the saved mode/equipment.
MAIN.C:148 calls detection; SYS.C:197 calls restoration on exit. The five
state bytes are DS:5AEE..5AF2 (VIDDET.ASM:9-18). The source has EVEN at local
DATA offset 5, then strings at offsets 6,12,18 / DS:5AF4,5AFA,5B00.
The fresh MASM listing shows EVEN emitting one zero; DATA has length 22.
MASM does not automatically align DB strings, nor align DW merely by type.

vd_detect (VIDDET.ASM:57-121) writes the saved mode, saved equipment, EGA
monochrome, memory and switch values and shared EGA/force flags. vd_restore
(32-50) reads the saved equipment and video mode only. Neither routine
loads, returns, prints, compares or indexes any of the three strings. There
is no pointer table or data fixup to those strings. The original game-code
operand sweep likewise finds no direct address in 5AF3..5B03; the DGROUP
report classifies all three as strings without pointer names.

### Actual setup-menu and ROM-search consumers

MAIN.C:63-67 defines six complete menu rows, including long CGA and Tandy
rows, rather than composing rows from short adapter names. show_video_menu
(72-109) reads those row pointers, prints all six, and appends the selected
row at character 20 to the recommendation text. The loaded mode comes from
integer detection flags and enum choices, not a name returned by VIDDET.

MAIN's DATA listing puts the CGA menu row at local offset 000C and Tandy row
at 00DD, with g_videoMenu at 0202. The independent "Tandy" and "TANDY" literals
are at 0214 and 021A. choose_video (142-175, specifically line 161) passes
those literals to ms_find to search BIOS_ROM; MEMSTR.ASM:12-40 compares a
near NUL-terminated needle against a far buffer. These are live ROM signatures.
The unused VIDDET strings are "Tandy", "Tandy", "CGA": the second string
also differs in case from the second live ROM needle. Matching the first
string's text does not make the two storage locations aliases.

The short unused names could have belonged to removed diagnostics, a previous
menu implementation or another inert data module. None of those histories is
supported by a surviving caller or fixup. There is no basis to name such a
consumer or attach these bytes to MAIN's current setup UI.

### Whole-module and packed-link trials

Assembled whole VIDDET copies with MASM 5.1 /Ml. Its code is 177 bytes and
all variants retain the same code and ordered target-resolved fixups.
Each trial replaced only VIDDET.OBJ in a private baseline link directory;
the split alone adds VDSTRS.OBJ directly after VIDDET.OBJ, before CGA.OBJ.
Production MAKEFILE and SR.LNK were untouched.

| Variant | DATA length | Packed EXE vs orig/SR.EXE |
|---|---:|---|
| Retained EVEN + three strings | 22 | IDENTICAL baseline |
| Replace EVEN with `DB 0` | 22 | IDENTICAL; same object DATA bytes/fixups |
| Delete EVEN, retain strings | 21 | 16 differing bytes; strings move one byte earlier |
| Delete EVEN and all three strings | 5 | 9623 differing bytes |
| Five-byte VIDDET + separate 16-byte string-only VDSTRS, no EVEN | 5 + 16 | IDENTICAL; LINK supplies the alignment byte |

Every linked variant has size 259785 and no unresolved symbols or LINK
warnings/errors. The split repeats the documented padbytes result using
freshly compiled objects. A one-byte unused variable can emit the same zero
as DB 0. No relocation in these inert bytes distinguishes a variable, EVEN,
or LINK's alignment before an independent WORD DATA contribution. Retain the
source's complete two-line PROVISIONAL explanation and EVEN unchanged.

## Other identifiers noticed (outside scope)

Win is reused for menu-list records: MENU.C:433-437 treats w/h as coordinates
and tx/ty as endpoints, unlike POPUP's dimensions and text offsets;
SAVEGAME.C:437-442 does the same with g_menuList.lineBase. This reuse does not
connect those menu records to POPUP's private pointer/tail. No rename or type
split was attempted; these meanings require a separate interface investigation.

## Reproduction and validation

Scratch sources, compiler listings/objects, bounded link runs and JSON results
are under work/prov-data/ (probe.py, probe-results.json, probe-links.log,
references.py and references.log). These ignored artifacts are not required
to understand the retained findings: the exact transformations, flags,
segment extents and link insertion above suffice to repeat the trials using
`tools.sr.dosbox.Job`/`compile_all` and `tools.sr.build.link`.

Final gates are recorded in the diary. The packed EXE is the identity verdict.
The baseline unpacked linkcheck reports its already-documented differences:
unpacker header/relocation order and fourteen trailing image filler bytes
(docs/RECONSTRUCTION.md, the orig/SR.EXE build note). No masks or tooling were changed.
