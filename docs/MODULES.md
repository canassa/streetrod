# Module reconstruction status

All **572 game functions** (506 C and 66 assembly) are assigned to **147 verified
modules** under `src/mods/`. This includes nine previously unrecorded routines found
by the scanner gap pass. The 147 MSC runtime entries remain skipped and unreconstructed.
The skipped 0x0F8C2 row is retained history of a false positive inside 0x0F7A5.

A module is a verified contiguous compilation group, not a claim that its historical
filename or complete original translation unit has been recovered. Near-call spans
can cross objects and languages. Groups preserve byte matches, compiler flag effects,
alignment and MSC 5.1 heap limits; some compilation units contain one function.

There are 95 modules with multiple functions and 52 with one function.

## Verification limits

Every member passes the byte/reference/lint gate, and every module passes the stricter
whole-span gate. Internal relocations and jump-table offsets are checked. External
relocations are checked by symbol/reference; external near-call layout remains a
separate linking task. A passing module with external near calls is not evidence of
a fully linked byte-identical executable.

The whole-span gates report **198 external near calls** across the groups.

## Recovered routines

| Address | Size | Reconstruction |
| --- | ---: | --- |
| 0x02DB0 | 6 | Clear keyboard state byte |
| 0x0FEAD | 18 | Select zero palette and clear video state |
| 0x0FEBF | 18 | Restore palette and set video state |
| 0x1636C | 21 | Stop sound and restore the timer helper |
| 0x16ADA | 48 | Gated sound effect |
| 0x16B0A | 48 | Gated sound effect |
| 0x1D080 | 36 | Video controller output, clear state |
| 0x1D0A4 | 36 | Video controller output, set state |
| 0x23F4D | 26 | Assembly EGA bit-plane reader |

## Module inventory

| Module | Functions | Original span | External near calls |
| --- | ---: | --- | ---: |
| [M_00000.C](../src/mods/M_00000.C) | 5 | 0x00000–0x00226 | 0 |
| [M_00226.C](../src/mods/M_00226.C) | 2 | 0x00226–0x004F4 | 2 |
| [M_004F4.C](../src/mods/M_004F4.C) | 1 | 0x004F4–0x005C6 | 4 |
| [M_005C6.C](../src/mods/M_005C6.C) | 1 | 0x005C6–0x00659 | 0 |
| [M_00659.C](../src/mods/M_00659.C) | 2 | 0x00659–0x008B3 | 6 |
| [M_008B4.C](../src/mods/M_008B4.C) | 3 | 0x008B4–0x00D58 | 0 |
| [M_00D58.C](../src/mods/M_00D58.C) | 13 | 0x00D58–0x01843 | 0 |
| [M_01843.C](../src/mods/M_01843.C) | 8 | 0x01843–0x01BAE | 0 |
| [M_01BAE.C](../src/mods/M_01BAE.C) | 1 | 0x01BAE–0x01FE2 | 1 |
| [M_01FE2.C](../src/mods/M_01FE2.C) | 1 | 0x01FE2–0x02374 | 3 |
| [M_02374.C](../src/mods/M_02374.C) | 2 | 0x02374–0x02B9A | 0 |
| [M_02B9A.ASM](../src/mods/M_02B9A.ASM) | 4 | 0x02B9A–0x02C04 | 0 |
| [M_02C04.ASM](../src/mods/M_02C04.ASM) | 1 | 0x02C04–0x02C2B | 0 |
| [M_02C2B.ASM](../src/mods/M_02C2B.ASM) | 2 | 0x02C2B–0x02DB0 | 0 |
| [M_02DB0.C](../src/mods/M_02DB0.C) | 1 | 0x02DB0–0x02DB6 | 0 |
| [M_02DB6.ASM](../src/mods/M_02DB6.ASM) | 2 | 0x02DB6–0x02DE2 | 0 |
| [M_02DE2.ASM](../src/mods/M_02DE2.ASM) | 1 | 0x02DE2–0x02DF4 | 0 |
| [M_02DF4.ASM](../src/mods/M_02DF4.ASM) | 1 | 0x02DF4–0x02E93 | 0 |
| [M_02E93.ASM](../src/mods/M_02E93.ASM) | 1 | 0x02E93–0x02FC8 | 0 |
| [M_02FC8.C](../src/mods/M_02FC8.C) | 3 | 0x02FC8–0x032F4 | 0 |
| [M_032F4.C](../src/mods/M_032F4.C) | 2 | 0x032F4–0x0339F | 0 |
| [M_0339F.C](../src/mods/M_0339F.C) | 1 | 0x0339F–0x03613 | 3 |
| [M_03614.C](../src/mods/M_03614.C) | 1 | 0x03614–0x036A8 | 0 |
| [M_036A8.C](../src/mods/M_036A8.C) | 8 | 0x036A8–0x03967 | 1 |
| [M_03967.C](../src/mods/M_03967.C) | 1 | 0x03967–0x039BF | 0 |
| [M_039C0.C](../src/mods/M_039C0.C) | 8 | 0x039C0–0x03C8D | 0 |
| [M_03C8D.C](../src/mods/M_03C8D.C) | 1 | 0x03C8D–0x03FB1 | 3 |
| [M_03FB1.C](../src/mods/M_03FB1.C) | 3 | 0x03FB1–0x04036 | 1 |
| [M_04036.C](../src/mods/M_04036.C) | 3 | 0x04036–0x0442B | 8 |
| [M_0442B.C](../src/mods/M_0442B.C) | 1 | 0x0442B–0x04940 | 4 |
| [M_04940.C](../src/mods/M_04940.C) | 2 | 0x04940–0x049ED | 0 |
| [M_049ED.C](../src/mods/M_049ED.C) | 1 | 0x049ED–0x04FCA | 9 |
| [M_04FCA.C](../src/mods/M_04FCA.C) | 1 | 0x04FCA–0x04FFA | 0 |
| [M_04FFA.C](../src/mods/M_04FFA.C) | 2 | 0x04FFA–0x0542E | 24 |
| [M_0542E.C](../src/mods/M_0542E.C) | 2 | 0x0542E–0x056A9 | 0 |
| [M_056A9.C](../src/mods/M_056A9.C) | 7 | 0x056A9–0x05B4A | 4 |
| [M_05B4A.C](../src/mods/M_05B4A.C) | 3 | 0x05B4A–0x0641E | 11 |
| [M_0641E.C](../src/mods/M_0641E.C) | 1 | 0x0641E–0x06475 | 0 |
| [M_06476.C](../src/mods/M_06476.C) | 1 | 0x06476–0x064A3 | 0 |
| [M_064A3.C](../src/mods/M_064A3.C) | 2 | 0x064A3–0x0653C | 1 |
| [M_0653C.C](../src/mods/M_0653C.C) | 4 | 0x0653C–0x06827 | 4 |
| [M_06827.C](../src/mods/M_06827.C) | 9 | 0x06827–0x07367 | 4 |
| [M_07367.C](../src/mods/M_07367.C) | 5 | 0x07367–0x07800 | 0 |
| [M_07800.C](../src/mods/M_07800.C) | 6 | 0x07800–0x081AB | 8 |
| [M_081AB.C](../src/mods/M_081AB.C) | 1 | 0x081AB–0x08D25 | 6 |
| [M_08D26.C](../src/mods/M_08D26.C) | 2 | 0x08D26–0x08E2D | 2 |
| [M_08E2D.C](../src/mods/M_08E2D.C) | 1 | 0x08E2D–0x08EA8 | 1 |
| [M_08EA8.C](../src/mods/M_08EA8.C) | 1 | 0x08EA8–0x0914B | 0 |
| [M_0914C.C](../src/mods/M_0914C.C) | 1 | 0x0914C–0x09256 | 0 |
| [M_09256.C](../src/mods/M_09256.C) | 2 | 0x09256–0x09524 | 4 |
| [M_09524.C](../src/mods/M_09524.C) | 8 | 0x09524–0x09AF4 | 3 |
| [M_09AF4.C](../src/mods/M_09AF4.C) | 6 | 0x09AF4–0x0A416 | 10 |
| [M_0A416.C](../src/mods/M_0A416.C) | 3 | 0x0A416–0x0B045 | 15 |
| [M_0B045.C](../src/mods/M_0B045.C) | 2 | 0x0B045–0x0B3C9 | 4 |
| [M_0B3CA.C](../src/mods/M_0B3CA.C) | 2 | 0x0B3CA–0x0B456 | 0 |
| [M_0B456.C](../src/mods/M_0B456.C) | 18 | 0x0B456–0x0CDAF | 17 |
| [M_0CDAF.C](../src/mods/M_0CDAF.C) | 6 | 0x0CDAF–0x0D624 | 2 |
| [M_0D624.C](../src/mods/M_0D624.C) | 3 | 0x0D624–0x0DA25 | 6 |
| [M_0DA25.C](../src/mods/M_0DA25.C) | 1 | 0x0DA25–0x0DCBD | 5 |
| [M_0DCBE.C](../src/mods/M_0DCBE.C) | 1 | 0x0DCBE–0x0E0E2 | 0 |
| [M_0E0E2.C](../src/mods/M_0E0E2.C) | 2 | 0x0E0E2–0x0E569 | 1 |
| [M_0E56A.ASM](../src/mods/M_0E56A.ASM) | 3 | 0x0E56A–0x0E5D1 | 0 |
| [M_0E5D2.ASM](../src/mods/M_0E5D2.ASM) | 1 | 0x0E5D2–0x0E60F | 0 |
| [M_0E610.ASM](../src/mods/M_0E610.ASM) | 2 | 0x0E610–0x0E6C1 | 0 |
| [M_0E6C2.ASM](../src/mods/M_0E6C2.ASM) | 3 | 0x0E6C2–0x0E925 | 0 |
| [M_0E926.ASM](../src/mods/M_0E926.ASM) | 1 | 0x0E926–0x0E9EF | 0 |
| [M_0F38A.C](../src/mods/M_0F38A.C) | 17 | 0x0F38A–0x10677 | 0 |
| [M_10677.C](../src/mods/M_10677.C) | 6 | 0x10677–0x10B7C | 0 |
| [M_10B7C.C](../src/mods/M_10B7C.C) | 13 | 0x10B7C–0x11324 | 0 |
| [M_11324.C](../src/mods/M_11324.C) | 1 | 0x11324–0x113A0 | 0 |
| [M_113A2.C](../src/mods/M_113A2.C) | 1 | 0x113A2–0x11421 | 0 |
| [M_11421.C](../src/mods/M_11421.C) | 4 | 0x11421–0x1169A | 0 |
| [M_1169A.C](../src/mods/M_1169A.C) | 5 | 0x1169A–0x119D6 | 0 |
| [M_119D6.C](../src/mods/M_119D6.C) | 2 | 0x119D6–0x11A72 | 0 |
| [M_11A72.C](../src/mods/M_11A72.C) | 6 | 0x11A72–0x11E9C | 0 |
| [M_11E9C.C](../src/mods/M_11E9C.C) | 4 | 0x11E9C–0x123F3 | 0 |
| [M_123F4.C](../src/mods/M_123F4.C) | 5 | 0x123F4–0x1287B | 0 |
| [M_1287B.C](../src/mods/M_1287B.C) | 14 | 0x1287B–0x135E5 | 0 |
| [M_135E5.C](../src/mods/M_135E5.C) | 1 | 0x135E5–0x13681 | 0 |
| [M_13682.C](../src/mods/M_13682.C) | 5 | 0x13682–0x139DD | 0 |
| [M_139DE.C](../src/mods/M_139DE.C) | 12 | 0x139DE–0x14C8B | 0 |
| [M_14C8B.C](../src/mods/M_14C8B.C) | 1 | 0x14C8B–0x14C8F | 0 |
| [M_14C90.C](../src/mods/M_14C90.C) | 5 | 0x14C90–0x15236 | 0 |
| [M_15236.C](../src/mods/M_15236.C) | 1 | 0x15236–0x15396 | 0 |
| [M_15396.C](../src/mods/M_15396.C) | 19 | 0x15396–0x1628A | 0 |
| [M_1628A.C](../src/mods/M_1628A.C) | 4 | 0x1628A–0x163BE | 0 |
| [M_163BE.C](../src/mods/M_163BE.C) | 34 | 0x163BE–0x16D19 | 0 |
| [M_16D1A.C](../src/mods/M_16D1A.C) | 1 | 0x16D1A–0x16D57 | 0 |
| [M_16D57.C](../src/mods/M_16D57.C) | 3 | 0x16D57–0x16EA1 | 0 |
| [M_16EA2.C](../src/mods/M_16EA2.C) | 1 | 0x16EA2–0x174A6 | 0 |
| [M_174A6.C](../src/mods/M_174A6.C) | 7 | 0x174A6–0x18ADB | 0 |
| [M_18ADB.C](../src/mods/M_18ADB.C) | 1 | 0x18ADB–0x18BDB | 0 |
| [M_18BDC.C](../src/mods/M_18BDC.C) | 3 | 0x18BDC–0x191DD | 0 |
| [M_191DD.C](../src/mods/M_191DD.C) | 1 | 0x191DD–0x1923B | 0 |
| [M_1923C.C](../src/mods/M_1923C.C) | 2 | 0x1923C–0x1929A | 0 |
| [M_1929A.C](../src/mods/M_1929A.C) | 1 | 0x1929A–0x192AC | 0 |
| [M_192AC.C](../src/mods/M_192AC.C) | 2 | 0x192AC–0x193D4 | 0 |
| [M_193D4.C](../src/mods/M_193D4.C) | 4 | 0x193D4–0x1947A | 0 |
| [M_1947A.C](../src/mods/M_1947A.C) | 2 | 0x1947A–0x1956E | 0 |
| [M_1956E.C](../src/mods/M_1956E.C) | 8 | 0x1956E–0x19DB7 | 0 |
| [M_19DB7.C](../src/mods/M_19DB7.C) | 2 | 0x19DB7–0x19ED0 | 0 |
| [M_19ED0.C](../src/mods/M_19ED0.C) | 8 | 0x19ED0–0x1A79B | 0 |
| [M_1A79B.C](../src/mods/M_1A79B.C) | 4 | 0x1A79B–0x1A9B2 | 0 |
| [M_1A9B2.C](../src/mods/M_1A9B2.C) | 3 | 0x1A9B2–0x1C11C | 0 |
| [M_1C11C.ASM](../src/mods/M_1C11C.ASM) | 4 | 0x1C11C–0x1C2CA | 0 |
| [M_1C2CA.C](../src/mods/M_1C2CA.C) | 7 | 0x1C2CA–0x1D0C8 | 0 |
| [M_1D0C8.C](../src/mods/M_1D0C8.C) | 4 | 0x1D0C8–0x1D590 | 0 |
| [M_1D590.C](../src/mods/M_1D590.C) | 3 | 0x1D590–0x1D8C3 | 0 |
| [M_1D8C4.C](../src/mods/M_1D8C4.C) | 2 | 0x1D8C4–0x1DD22 | 0 |
| [M_1DD22.ASM](../src/mods/M_1DD22.ASM) | 2 | 0x1DD22–0x1DFD1 | 4 |
| [M_1DFD2.ASM](../src/mods/M_1DFD2.ASM) | 3 | 0x1DFD2–0x1E16E | 2 |
| [M_21962.ASM](../src/mods/M_21962.ASM) | 2 | 0x21962–0x21A05 | 0 |
| [M_21A06.ASM](../src/mods/M_21A06.ASM) | 2 | 0x21A06–0x21AAB | 0 |
| [M_21AAC.C](../src/mods/M_21AAC.C) | 5 | 0x21AAC–0x22077 | 0 |
| [M_22077.C](../src/mods/M_22077.C) | 3 | 0x22077–0x22351 | 0 |
| [M_22352.ASM](../src/mods/M_22352.ASM) | 1 | 0x22352–0x22364 | 0 |
| [M_22364.C](../src/mods/M_22364.C) | 4 | 0x22364–0x226F3 | 0 |
| [M_226F4.C](../src/mods/M_226F4.C) | 2 | 0x226F4–0x22B27 | 0 |
| [M_22B28.ASM](../src/mods/M_22B28.ASM) | 1 | 0x22B28–0x22B3A | 0 |
| [M_22B3A.C](../src/mods/M_22B3A.C) | 1 | 0x22B3A–0x22F07 | 0 |
| [M_22F08.C](../src/mods/M_22F08.C) | 1 | 0x22F08–0x230DC | 0 |
| [M_230DC.C](../src/mods/M_230DC.C) | 1 | 0x230DC–0x23122 | 0 |
| [M_23122.C](../src/mods/M_23122.C) | 1 | 0x23122–0x2322D | 0 |
| [M_2322E.C](../src/mods/M_2322E.C) | 1 | 0x2322E–0x23262 | 0 |
| [M_23262.C](../src/mods/M_23262.C) | 3 | 0x23262–0x2382D | 0 |
| [M_2382D.C](../src/mods/M_2382D.C) | 1 | 0x2382D–0x238A2 | 0 |
| [M_238A2.C](../src/mods/M_238A2.C) | 3 | 0x238A2–0x23DBA | 0 |
| [M_23DBC.ASM](../src/mods/M_23DBC.ASM) | 3 | 0x23DBC–0x23F67 | 3 |
| [M_23F68.C](../src/mods/M_23F68.C) | 2 | 0x23F68–0x242EC | 0 |
| [M_242EC.C](../src/mods/M_242EC.C) | 1 | 0x242EC–0x24602 | 0 |
| [M_24602.ASM](../src/mods/M_24602.ASM) | 1 | 0x24602–0x2462D | 0 |
| [M_2462E.ASM](../src/mods/M_2462E.ASM) | 6 | 0x2462E–0x24879 | 5 |
| [M_2487A.ASM](../src/mods/M_2487A.ASM) | 4 | 0x2487A–0x24BE1 | 5 |
| [M_24BE2.ASM](../src/mods/M_24BE2.ASM) | 2 | 0x24BE2–0x24D83 | 0 |
| [M_24D84.ASM](../src/mods/M_24D84.ASM) | 2 | 0x24D84–0x24E78 | 0 |
| [M_24E78.ASM](../src/mods/M_24E78.ASM) | 1 | 0x24E78–0x24EFB | 0 |
| [M_24EFB.ASM](../src/mods/M_24EFB.ASM) | 1 | 0x24EFB–0x24F4A | 0 |
| [M_24F4A.ASM](../src/mods/M_24F4A.ASM) | 1 | 0x24F4A–0x25953 | 0 |
| [M_25954.ASM](../src/mods/M_25954.ASM) | 1 | 0x25954–0x26349 | 0 |
| [M_2634A.ASM](../src/mods/M_2634A.ASM) | 2 | 0x2634A–0x26457 | 0 |
| [M_26458.ASM](../src/mods/M_26458.ASM) | 5 | 0x26458–0x265BA | 1 |
| [M_265BA.C](../src/mods/M_265BA.C) | 14 | 0x265BA–0x27B6D | 0 |
| [M_27B6D.C](../src/mods/M_27B6D.C) | 14 | 0x27B6D–0x2869D | 0 |
| [M_2869E.C](../src/mods/M_2869E.C) | 1 | 0x2869E–0x286D8 | 0 |
| [M_286D8.C](../src/mods/M_286D8.C) | 34 | 0x286D8–0x2B6CA | 0 |
| [M_2B6CA.C](../src/mods/M_2B6CA.C) | 1 | 0x2B6CA–0x2B6DB | 1 |
| [M_2B6DC.C](../src/mods/M_2B6DC.C) | 6 | 0x2B6DC–0x2BEAB | 0 |
