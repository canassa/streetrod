# Unnamed symbols (placeholders still in the tree)

Generated from db/sr.sqlite and src/ on main. Placeholder = `F_xxxxx` functions, `g_xxxx` / hash-tuned globals (e.g. `ai70e4t`, `g_6c8aaav`), `M_xxxxx` files. Runtime names (errno, _doserrno, _amblksiz) excluded.

- Functions: 62 of 573 game functions
- Globals: 420 of 826 DGROUP globals (initialized: 161, static (_BSS, MSC name hash): 51, communal (LINK hash bucket): 208)
- Far data: 7 of 23
- Files: M_02B9A.ASM

## Functions

| addr | name | file | kind | size | callers |
|---|---|---|---|---|---|
| 02374 | F_02374 | SYS.C | c | 24 | 4 |
| 02B9A | F_02B9A | M_02B9A.ASM | asm | 19 | 0 |
| 02BC0 | F_02BC0 | M_02B9A.ASM | asm | 34 | 0 |
| 02BE2 | F_02BE2 | M_02B9A.ASM | asm | 34 | 0 |
| 064A3 | F_064A3 | GARAGE.C | c | 134 | 3 |
| 06529 | F_06529 | GARAGE.C | c | 19 | 3 |
| 065AF | F_065AF | GARAGE.C | c | 417 | 2 |
| 06750 | F_06750 | GARAGE.C | c | 127 | 1 |
| 067CF | F_067CF | GARAGE.C | c | 88 | 6 |
| 06827 | F_06827 | GARAGE.C | c | 91 | 4 |
| 06AA6 | F_06AA6 | GARAGE.C | c | 50 | 2 |
| 06B06 | F_06B06 | GARAGE.C | c | 1125 | 1 |
| 06F6B | F_06F6B | GARAGE.C | c | 431 | 1 |
| 0711A | F_0711A | GARAGE.C | c | 589 | 1 |
| 07367 | F_07367 | GARAGE.C | c | 832 | 1 |
| 076A7 | F_076A7 | GARAGE.C | c | 154 | 4 |
| 07741 | F_07741 | GARAGE.C | c | 72 | 4 |
| 077B7 | F_077B7 | GARAGE.C | c | 73 | 4 |
| 07800 | F_07800 | GARAGE.C | c | 241 | 1 |
| 079EB | F_079EB | GARAGE.C | c | 185 | 1 |
| 07AA4 | F_07AA4 | GARAGE.C | c | 1090 | 1 |
| 07EE6 | F_07EE6 | GARAGE.C | c | 203 | 3 |
| 0E218 | F_0E218 | PERF.C | c | 849 | 4 |
| 0F8C2 | F_0F8C2 |  | c | 176 | 0 |
| 10AFD | F_10AFD | DASH.C | c | 75 | 7 |
| 10B7C | F_10B7C | VIDEO.C | c | 16 | 2 |
| 10B8C | F_10B8C | VIDEO.C | c | 16 | 2 |
| 10B9C | F_10B9C | VIDEO.C | c | 16 | 1 |
| 112CB | F_112CB | VIDEO.C | c | 89 | 6 |
| 11421 | F_11421 | SCREEN.C | c | 287 | 14 |
| 11540 | F_11540 | SCREEN.C | c | 83 | 10 |
| 11593 | F_11593 | SCREEN.C | c | 214 | 7 |
| 118D4 | F_118D4 | SCREEN.C | c | 228 | 12 |
| 119B8 | F_119B8 | SCREEN.C | c | 30 | 12 |
| 123F4 | F_123F4 | GARGFX.C | c | 305 | 2 |
| 12525 | F_12525 | GARGFX.C | c | 297 | 1 |
| 1264E | F_1264E | GARGFX.C | c | 310 | 2 |
| 12784 | F_12784 | GARGFX.C | c | 87 | 1 |
| 127DB | F_127DB | GARGFX.C | c | 160 | 1 |
| 12CD1 | F_12CD1 | GARGFX.C | c | 263 | 1 |
| 135E5 | F_135E5 | GARGFX.C | c | 156 | 2 |
| 142CF | F_142CF | MENU.C | c | 114 | 8 |
| 14C8B | F_14C8B | MENU.C | c | 4 | 8 |
| 15BBE | F_15BBE | DISK.C | c | 73 | 34 |
| 16381 | F_16381 | MUSIC.C | c | 61 | 6 |
| 168C8 | F_168C8 | SOUND.C | c | 43 | 1 |
| 168F3 | F_168F3 | SOUND.C | c | 80 | 1 |
| 16943 | F_16943 | SOUND.C | c | 80 | 0 |
| 16993 | F_16993 | SOUND.C | c | 78 | 2 |
| 169E1 | F_169E1 | SOUND.C | c | 78 | 1 |
| 16A2F | F_16A2F | SOUND.C | c | 43 | 0 |
| 16A5A | F_16A5A | SOUND.C | c | 78 | 2 |
| 16B61 | F_16B61 | SOUND.C | c | 54 | 2 |
| 16B97 | F_16B97 | SOUND.C | c | 42 | 2 |
| 16BC1 | F_16BC1 | SOUND.C | c | 34 | 3 |
| 16C8F | F_16C8F | SOUND.C | c | 100 | 1 |
| 16CF3 | F_16CF3 | SOUND.C | c | 38 | 4 |
| 17AAE | F_17AAE | CARPIC.C | c | 312 | 1 |
| 181C8 | F_181C8 | CARPIC.C | c | 2323 | 8 |
| 1934E | F_1934E | ARENA.C | c | 134 | 27 |
| 19438 | F_19438 | MARKS.C | c | 66 | 36 |
| 1B213 | F_1B213 | TIRES.C | c | 3849 | 1 |

## Globals: initialized (161)

| DS | name | decl | used in |
|---|---|---|---|
| 0046 | g_0046 | `int @` | MAIN.C |
| 004A | g_004a | `int @` | MAIN.C |
| 004C | g_004c | `int @` | DASH.C, MAIN.C |
| 0252 | g_0252 | `int @` | MAIN.C, VIDDET.ASM |
| 0282 | g_0282 | `int @` | BOBS.C, GAME.C, GARAGE.C, MAIN.C |
| 0284 | g_0284 | `int @` | BOBS.C, CARPIC.C, CGASPR.C, CRUISE.C, EGASPR.C, GAME.C, GARAGE.C, MAIN.C, RACE.C, ROAD.C, SPRITE.C, TANDYSPR.C, VIEW.C |
| 0288 | g_0288 | `struct Bitmap far *@` | CARPIC.C, MAIN.C, SPRITE.C |
| 028C | g_028c | `struct Bitmap far *@` | CARPIC.C, MAIN.C, RACE.C |
| 0290 | g_0290 | `struct Bitmap far *@` | CARPIC.C, MAIN.C, RACE.C |
| 0294 | g_0294 | `struct Bitmap far *@` | ARENA.C, CARPIC.C, GARAGE.C, MAIN.C, MARKS.C |
| 0298 | g_0298 | `int @[7]` | CARPIC.C, MAIN.C |
| 02A6 | g_02a6 | `int @[7]` | CARPIC.C, MAIN.C |
| 02B4 | g_02b4 | `int @[7]` | CARPIC.C, GAME.C, MAIN.C, TIRES.C |
| 02C2 | g_02c2 | `int @[7]` | CARPIC.C, GAME.C, MAIN.C, TIRES.C |
| 02D0 | g_02d0 | `int @` | CARPIC.C, CGASPR.C, EGASPR.C, MAIN.C, TANDYSPR.C, WALKER.C |
| 02D4 | g_02d4 | `char @[]` | DASH.C |
| 02EE | g_02ee | `signed char @[24][2]` | DASH.C |
| 031E | g_031e | `unsigned char @[][2]` | DASH.C |
| 0450 | g_0450 | `int @` | VIDEO.C |
| 0472 | g_0472 | `int @` | VIDEO.C |
| 0524 | g_0524 | `int @` | ADS.C |
| 0526 | g_0526 | `int @` | ADS.C |
| 0528 | g_0528 | `int @` | ADS.C |
| 052A | g_052a | `int @` | ADS.C |
| 052C | g_052c | `int @` | ADS.C |
| 052E | g_052e | `int @` | ADS.C |
| 0530 | g_0530 | `int @[3]` | ADS.C |
| 0570 | g_0570 | `char @` | SYS.C |
| 05BA | g_05ba | `int @` | SYS.C |
| 05BC | g_05bc | `int @[8]` | SYS.C |
| 05D4 | g_05d4 | `int @` | SYS.C |
| 05D6 | g_05d6 | `int @` | DRVTICK.C, RACE.C, SYS.C |
| 05D8 | g_05d8 | `int @` | RACE.C, SYS.C |
| 05DA | g_05da | `int @` | RACE.C, SYS.C |
| 05F3 | g_05f3 | `char @` | SYS.C |
| 05F4 | g_05f4 | `int @[2]` | SYS.C |
| 05FC | g_05fc | `long @` | BOBS.C, GAME.C, GARAGE.C, SAVEGAME.C, SYS.C |
| 0600 | g_0600 | `long @` | GARAGE.C, SAVEGAME.C, SYS.C |
| 0604 | g_0604 | `long @` | GARAGE.C, SAVEGAME.C, SYS.C |
| 060A | g_060a | `int @` | SYS.C |
| 064D | g_064d | `char @` | SYS.C |
| 06A2 | g_06a2 | `int @` | SYS.C |
| 06AC | g_06ac | `int @` | SYS.C |
| 06E4 | g_06e4 | `int @` | SYS.C |
| 06EA | g_06ea | `char @` | SYS.C |
| 06EC | g_06ec | `struct Pair @[]` | CURSOR.C |
| 077E | g_077e | `char @[]` | GARGFX.C |
| 078C | g_078c | `char @[]` | GARGFX.C |
| 0792 | g_0792 | `struct Bitmap far *@` | EGASLIDE.C, GARGFX.C, SPRITE.C |
| 07C6 | g_07c6 | `int @[]` | DISK.C, MENU.C |
| 09F4 | g_09f4 | `struct Hot @[]` | GARAGE.C, GARGFX.C, HOTSPOT.C, MENU.C, RACE.C, SYS.C |
| 121C | g_121c | `struct Hot @[]` | GARAGE.C, HOTSPOT.C |
| 134E | g_134e | `struct Hot @[]` | GARAGE.C, HOTSPOT.C |
| 1804 | g_1804 | `struct Hot @[]` | GARAGE.C, HOTSPOT.C |
| 184C | g_184c | `struct Hot @[]` | GARGFX.C, HOTSPOT.C |
| 1948 | g_1948 | `struct Hot @[]` | GARGFX.C, HOTSPOT.C |
| 1A44 | g_1a44 | `struct Hot @[]` | HOTSPOT.C, RACE.C |
| 1E34 | g_1e34 | `struct Hot @[]` | GARAGE.C, HOTSPOT.C |
| 239E | g_239e | `char @[]` | BOBS.C, DISK.C, GARAGE.C, MENU.C, MESSAGES.ASM, SYS.C |
| 291F | g_291f | `char @[]` | MESSAGES.ASM, SCREEN.C |
| 2990 | g_2990 | `char @[]` | GAME.C, MESSAGES.ASM |
| 29F8 | g_29f8 | `char @[]` | GAME.C, MESSAGES.ASM |
| 29FF | g_29ff | `char @[]` | GARAGE.C, MESSAGES.ASM |
| 2A2E | g_2a2e | `char @[]` | MESSAGES.ASM, SAVEGAME.C |
| 2A5F | g_2a5f | `char @[]` | GAME.C, MESSAGES.ASM |
| 2BA5 | g_2ba5 | `int @` | GARGFX.C, MESSAGES.ASM |
| 2BA7 | g_2ba7 | `int @` | GARGFX.C, MESSAGES.ASM |
| 2C01 | g_2c01 | `char @[]` | ADS.C, MESSAGES.ASM, POPUP.C, RACE.C |
| 2C02 | g_2c02 | `char @[]` | ADS.C, BOBS.C, DISK.C, GAME.C, GARAGE.C, MENU.C, MESSAGES.ASM, SAVEGAME.C |
| 39C9 | g_39c9 | `char @[]` | GARAGE.C, MESSAGES.ASM |
| 3A80 | g_3a80 | `char @` | MESSAGES.ASM |
| 3AA2 | g_3aa2 | `char @` | MESSAGES.ASM |
| 3AC1 | g_3ac1 | `char @` | MESSAGES.ASM |
| 3AD8 | g_3ad8 | `char @` | MESSAGES.ASM |
| 3AEA | g_3aea | `char @` | MESSAGES.ASM |
| 3AFB | g_3afb | `char @` | MESSAGES.ASM |
| 3B17 | g_3b17 | `char @` | MESSAGES.ASM |
| 3B2B | g_3b2b | `char @` | MESSAGES.ASM |
| 3B4F | g_3b4f | `char @` | MESSAGES.ASM |
| 3B73 | g_3b73 | `char @` | MESSAGES.ASM |
| 3B98 | g_3b98 | `char @` | MESSAGES.ASM |
| 3BB6 | g_3bb6 | `char @` | MESSAGES.ASM |
| 3BD8 | g_3bd8 | `char @` | MESSAGES.ASM |
| 3BF8 | g_3bf8 | `char @` | MESSAGES.ASM |
| 4733 | g_4733 | `signed char @` | CURSOR.C, GARAGE.C, M_02B9A.ASM, SYS.C |
| 4734 | g_4734 | `int @` | M_02B9A.ASM |
| 4742 | g_4742 | `int @` | CURSOR.C, M_02B9A.ASM |
| 4744 | g_4744 | `int @` | CURSOR.C, M_02B9A.ASM |
| 474A | g_474a | `int @` | M_02B9A.ASM |
| 474C | g_474c | `int @[]` | M_02B9A.ASM, SYS.C |
| 4752 | g_4752 | `int @[]` | M_02B9A.ASM, SYS.C |
| 47D1 | g_47d1 | `char @[3]` | M_02B9A.ASM |
| 47D8 | g_47d8 | `int @[8]` | M_02B9A.ASM, SYS.C |
| 47E8 | g_47e8 | `int @` | JOY.ASM, KBD.ASM, M_02B9A.ASM |
| 47EA | g_47ea | `char @` | M_02B9A.ASM |
| 47EB | g_47eb | `char @` | M_02B9A.ASM |
| 47EC | g_47ec | `char @` | M_02B9A.ASM |
| 47ED | g_47ed | `char @` | M_02B9A.ASM |
| 47F0 | g_47f0 | `char @` | M_02B9A.ASM |
| 4806 | g_4806 | `struct Item @[]` | BOBS.C, CRUISE.C, GAME.C, GARAGE.C, GARGFX.C, M_02B9A.ASM, PERF.C, RACE.C, SYS.C |
| 496A | g_496a | `int @` | M_02B9A.ASM, PERF.C |
| 496C | g_496c | `int @[2]` | M_02B9A.ASM |
| 4978 | g_4978 | `char @[][5]` | GARAGE.C, M_02B9A.ASM |
| 4988 | g_4988 | `char @[][3]` | GARAGE.C, M_02B9A.ASM |
| 4998 | g_4998 | `int @[][5]` | GARAGE.C, M_02B9A.ASM |
| 49CA | g_49ca | `int @[]` | GARAGE.C, M_02B9A.ASM |
| 49D2 | g_49d2 | `int @[]` | GARAGE.C, M_02B9A.ASM |
| 49DC | g_49dc | `char *@` | BOBS.C, GAME.C, GARAGE.C, M_02B9A.ASM, RACE.C |
| 49E0 | g_49e0 | `int @[]` | GAME.C, GARAGE.C, M_02B9A.ASM, RACE.C, SAVEGAME.C |
| 4BED | g_4bed | `char @` | FAME.C |
| 4F46 | g_4f46 | `struct Car *@` | GAME.C, GARAGE.C |
| 4F48 | g_4f48 | `struct Car *@` | GAME.C, GARAGE.C |
| 4F4A | g_4f4a | `int @` | CARPIC.C, GAME.C |
| 501D | g_501d | `char @` | SAVEGAME.C |
| 5030 | g_5030 | `int @[][4]` | BOBS.C, GAME.C, GARAGE.C, RACE.C |
| 5048 | g_5048 | `int @` | GARAGE.C |
| 504A | g_504a | `int @[]` | GARAGE.C |
| 5052 | g_5052 | `int @[]` | GARAGE.C |
| 5082 | g_5082 | `char *@[]` | GARAGE.C |
| 508A | g_508a | `int @[]` | GARAGE.C |
| 50C2 | g_50c2 | `char @[][2]` | CRUISE.C, GARAGE.C |
| 50CE | g_50ce | `int @[]` | GARAGE.C |
| 511E | g_511e | `char *@[]` | GARAGE.C |
| 5126 | g_5126 | `int @[]` | GAME.C, GARAGE.C, PERF.C |
| 5130 | g_5130 | `int @[]` | GARAGE.C, GARGFX.C |
| 513C | g_513c | `int @[]` | GARAGE.C |
| 5148 | g_5148 | `int @[][6]` | GARAGE.C |
| 5184 | g_5184 | `int @[]` | GARAGE.C |
| 5186 | g_5186 | `int @[]` | GARAGE.C |
| 5198 | g_5198 | `int @[]` | GARAGE.C |
| 519A | g_519a | `int @[]` | GARAGE.C |
| 51EC | g_51ec | `int @` | BOBS.C, DRVTICK.C |
| 54D2 | g_54d2 | `int @` | RACE.C |
| 54D4 | g_54d4 | `int @` | RACE.C, ROAD.C |
| 54D6 | g_54d6 | `int @` | RACE.C |
| 54D8 | g_54d8 | `int @` | RACE.C |
| 54DA | g_54da | `int @` | RACE.C, SYS.C |
| 5504 | g_5504 | `int @[][5]` | PERF.C |
| 5568 | g_5568 | `int @[]` | PERF.C |
| 5580 | g_5580 | `unsigned char @[543]` | HERCDET.ASM |
| 58E4 | g_58e4 | `int @` | CARPIC.C, DASH.C, DRIVE.C |
| 58EE | g_58ee | `int @` | CARPIC.C, DRIVE.C, SCREEN.C |
| 5A10 | g_5a10 | `struct Rect @` | CARPIC.C |
| 5A18 | g_5a18 | `struct Rect @` | CARPIC.C |
| 5AA6 | g_5aa6 | `char @` | BITMAP.C, CARPIC.C, WALKER.C |
| 5AB0 | g_5ab0 | `struct Bitmap far *@` | TIRES.C |
| 5AB4 | g_5ab4 | `struct Bitmap far *@` | TIRES.C |
| 5AB8 | g_5ab8 | `struct Bitmap far *@` | TIRES.C |
| 5AF3 | g_5af3 | `char @` | VIDDET.ASM |
| 5E3C | g_5e3c | `int @` | VIEW.C |
| 5E62 | g_5e62 | `int @[]` | VIEW.C |
| 5E86 | g_5e86 | `int @` | VIEW.C |
| 5E88 | g_5e88 | `int @[5]` | VIEW.C |
| 5E92 | g_5e92 | `int @` | VIEW.C |
| 5E94 | g_5e94 | `int @[4]` | VIEW.C |
| 5EC6 | g_5ec6 | `int @` | SCENERY.C, VIEW.C |
| 6010 | g_6010 | `struct Run @[7]` | SCENERY.C |
| 603A | g_603a | `struct List @` | SCENERY.C |
| 603E | g_603e | `struct Run @[7]` | SCENERY.C |
| 6068 | g_6068 | `struct List @` | SCENERY.C |
| 6076 | g_6076 | `int @` | SCENERY.C |

## Globals: static (_BSS, MSC name hash) (51)

| DS | name | decl | used in |
|---|---|---|---|
| 6C42 | g_6c42 | `int @[2]` | POPUP.C |
| 6C4E | g_6c4e | `char @` | SYS.C |
| 6CA4 | g_6ca4rz | `int @` | RACE.C |
| 6CA6 | g_6ca6qz | `int @` | RACE.C |
| 6CA8 | g_6ca8pz | `int @` | RACE.C |
| 6CAA | g_6caaaa | `int @` | RACE.C |
| 6CD4 | g_6cd4oz | `struct Bitmap far *@` | TIRES.C |
| 6CD8 | g_6cd8lz | `int @` | TIRES.C |
| 6CDA | g_6cdaaa | `struct Bitmap far *@[3]` | TIRES.C |
| 6CE6 | g_6ce6sz | `int @` | TIRES.C |
| 6CE8 | g_6ce8rz | `struct Rect @` | TIRES.C |
| 6CF0 | g_6cf0zz | `struct Bitmap far *@` | TIRES.C |
| 6CF4 | g_6cf4wz | `struct Bitmap @` | TIRES.C |
| 6D24 | g_6d24ajz | `struct Bitmap far *@` | TIRES.C |
| 6D28 | g_6d28agz | `struct Bitmap far *@` | TIRES.C |
| 6D2C | g_6d2c0nz | `struct Bitmap far *@` | TIRES.C |
| 6D30 | g_6d30 | `struct Bitmap far *@` | TIRES.C |
| 6D34 | g_6d34 | `struct Bitmap far *@` | TIRES.C |
| 6D38 | g_6d38 | `struct Bitmap far *@` | TIRES.C |
| 6D3C | g_6d3caat | `struct Sprite *@` | TIRES.C |
| 6D3E | g_6d3eaas | `int @` | TIRES.C |
| 6D40 | g_6d400 | `struct Bitmap @` | TIRES.C |
| 6D70 | g_6d700 | `struct Bitmap @` | TIRES.C |
| 6DA0 | g_6da0 | `int @` | TIRES.C |
| 6DA2 | g_6da2 | `int @` | TIRES.C |
| 6DA4 | g_6da4 | `struct Bitmap @` | TIRES.C |
| 6DD4 | g_6dd4 | `int @` | TIRES.C |
| 6DD6 | g_6dd6 | `struct Bitmap far *@` | TIRES.C |
| 6DDA | g_6dda | `struct Bitmap far *@` | TIRES.C |
| 6DDE | g_6dde | `struct Bitmap far *@` | TIRES.C |
| 6DF2 | g_6df2a | `struct Bitmap far *@` | TIRES.C |
| 6DF6 | g_6df6a | `struct Bitmap far *@` | TIRES.C |
| 6DFA | g_6dfa7 | `struct Bitmap far *@` | TIRES.C |
| 6E04 | g_6e04aay | `int @` | VIEW.C |
| 6E06 | g_6e06aax | `int @` | VIEW.C |
| 6E08 | g_6e08aaw | `int *@` | VIEW.C |
| 6E0A | g_6e0awz | `int *@` | VIEW.C |
| 6E0C | g_6e0cvz | `int *@` | VIEW.C |
| 6E0E | g_6e0euz | `int *@` | VIEW.C |
| 6E10 | g_6e10 | `int *@` | VIEW.C |
| 6E12 | g_6e12 | `int *@` | VIEW.C |
| 6E14 | g_6e14 | `int *@` | VIEW.C |
| 6E16 | g_6e16 | `int *@` | VIEW.C |
| 6E18 | g_6e18 | `int *@` | VIEW.C |
| 6E1A | g_6e1a | `int *@` | VIEW.C |
| 6E1C | g_6e1c | `int *@` | VIEW.C |
| 6E1E | g_6e1e | `int *@` | VIEW.C |
| 6E20 | g_6e20a | `int far *@` | VIEW.C |
| 6E24 | g_6e24a | `int far *@` | VIEW.C |
| 6E28 | g_6e28a | `int *@` | VIEW.C |
| 6E2A | g_6e2a9 | `int @` | VIEW.C |

## Globals: communal (LINK hash bucket) (208)

| DS | name | decl | used in |
|---|---|---|---|
| 6E60 | k_6e60 | `struct Bitmap far *@` | VIEW.C |
| 6E64 | c_6e64 | `struct Bitmap far *@` | BOBS.C |
| 6E68 | am6e68x | `struct Bitmap @[]` | CARPIC.C, SPRITE.C |
| 70A8 | at70a8 | `struct Bitmap far *@` | BITMAP.C, CARPIC.C |
| 70AC | u_70ac | `struct Blt @` | EGASLIDE.C, GARGFX.C, SPRITE.C |
| 70C0 | al70c0x | `struct State70C0 @` | RACE.C, ROAD.C, VIEW.C |
| 70E0 | dz70e0 | `int @` | VIEW.C |
| 70E2 | by70e2 | `int @` | GAME.C, MAIN.C |
| 70E4 | ai70e4t | `struct Panel @[]` | BOBS.C, DISK.C, GARAGE.C |
| 72EE | q_72ee | `unsigned far *@` | ROAD.C, VIEW.C |
| 72F2 | by72f2 | `int @` | RACE.C, VIEW.C |
| 72F4 | ax72f4 | `struct Bitmap far *@` | VIEW.C |
| 72F8 | at72f8 | `int @[]` | ROAD.C |
| 7312 | a7312av | `int @` | VIEW.C |
| 7314 | br7314 | `int @` | VIEW.C |
| 7526 | ab7526x | `struct Bitmap far *@[13]` | RACE.C, VIEW.C |
| 755A | g755av | `int @` | RACE.C |
| 755C | g755cu | `struct Bitmap far *@` | DASH.C, DRIVE.C, MAIN.C |
| 7560 | af7560z | `int far *@` | ROAD.C |
| 7564 | ab7564z | `char far *@` | DISK.C, MAIN.C |
| 7568 | bm7568 | `struct Obj @[]` | GARAGE.C, VIDEO.C |
| 7644 | g7644m | `int @` | RACE.C |
| 7646 | bucount | `int @` | DISK.C, FAME.C |
| 7648 | bn7648 | `int @` | BOBS.C, RACE.C, VIEW.C |
| 764A | g764aw | `struct Bitmap far *@` | VIEW.C |
| 764E | g_764e | `struct Seq @[]` | SPRITE.C |
| 7676 | bo7676 | `int @` | BOBS.C, RACE.C |
| 7678 | bm7678 | `struct Bitmap far *@` | DASH.C, DRIVE.C, ROAD.C, VIEW.C |
| 767C | at767ct | `unsigned @` | CGALINE.ASM, DASH.C, DRAW.C, TANDYLIN.ASM, VIEW.C |
| 767E | ar767eu | `struct Bitmap far *@` | VIEW.C |
| 7682 | g7682n | `struct Spr12 @[]` | CARPIC.C, DISK.C, TIRES.C |
| 77BA | g77bal | `struct Frame @[]` | SPRITE.C |
| 784A | a784als | `struct Pos6 @[]` | CARPIC.C, DISK.C, GARAGE.C |
| 787A | av787at | `unsigned @` | CGALINE.ASM, DASH.C, DRAW.C, EGALINE.ASM, EGASPAN.ASM, TANDYLIN.ASM, VIEW.C |
| 787C | at787cu | `char @` | RACE.C, ROAD.C |
| 787E | bustyle | `struct Style *@` | ADS.C, EDIT.C, FAME.C, GAME.C, GARAGE.C, MENU.C, POPUP.C, RACE.C, SAVEGAME.C, SCREEN.C, VIDEO.C |
| 7880 | af7880z | `int far *@` | ROAD.C |
| 7884 | bq7884 | `int @[]` | ROAD.C, VIEW.C |
| 789E | g789eu | `int @` | VIEW.C |
| 78A0 | f_78a0 | `int @` | VIEW.C |
| 78A2 | b_78a2 | `int (far *@)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int)` | ADS.C, BITMAP.C, BOBS.C, CARPIC.C, CGA.C, CURSOR.C, DASH.C, DRIVE.C, EGA.C, GARAGE.C, GARGFX.C, POPUP.C, RACE.C, SCREEN.C, TANDY.C, TIRES.C, VIDEO.C, VIEW.C, WALKER.C |
| 78A6 | am78a6t | `int (far *@)(struct Bitmap far *, struct Bitmap far *, struct Blt *)` | GARGFX.C |
| 78AA | ab78aat | `int (far *@)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int)` | BITMAP.C, BOBS.C, CARPIC.C, CGASPR.C, EGASLIDE.C, GARAGE.C, GARGFX.C, RACE.C, SPRITE.C, TANDYSPR.C, TIRES.C, VIDEO.C, VIEW.C, WALKER.C |
| 78AE | a_78aem | `void (far *@)(struct Bitmap far *, int, int, char *)` | ADS.C, EDIT.C, FAME.C, GARAGE.C, MENU.C, POPUP.C, RACE.C, SCREEN.C |
| 78B2 | ap78b2z | `void (far *@)(char far *, char far *, char far *, char far *, int, int, int, int)` | CURSOR.C |
| 78B6 | dz78b6 | `void (far *@)(char far *, char far *)` | BOBS.C, CARPIC.C, DRIVE.C, RACE.C, TIRES.C, WALKER.C |
| 78BE | bl78be | `void (far *@)(void)` | BOBS.C, CRUISE.C, DASH.C, DRIVE.C, FAME.C, GAME.C, SCREEN.C |
| 78C2 | m_present | `void (far *@)(void)` | BOBS.C, DASH.C, DRIVE.C, FAME.C, GARAGE.C, RACE.C, SAVEGAME.C, SCREEN.C, SYS.C, VIDEO.C |
| 78C6 | g78c6c | `int (far *@)(struct Sprite *)` | CARPIC.C, SPRITE.C, TIRES.C, WALKER.C |
| 78CA | s_blit | `void (far *@)(char far *, char far *)` | BOBS.C, DASH.C, FAME.C |
| 78CE | bl78ce | `void (far *@)(int, int, int, int, int, struct Bitmap far *)` | GARGFX.C |
| 78D2 | a_78d2 | `void (far *@)(int, int, int, int, int)` | DASH.C, DRAW.C, VIEW.C |
| 78D6 | g78d6c | `void (far *@)(int, int, int, int)` | VIEW.C |
| 78DA | bp78da | `void (far *@)(int, int)` | VIEW.C |
| 78DE | bl78de | `int @` | ARENA.C, MARKS.C |
| 78E0 | e_78e0 | `struct Eng @` | CRUISE.C, DRIVE.C, DRVTICK.C, GAME.C, RACE.C, ROAD.C, SYS.C |
| 79A2 | as79a2z | `int @` | BOBS.C, RACE.C |
| 79A4 | d_79a4 | `int @` | BOBS.C, RACE.C |
| 79A6 | jz79a6 | `int @` | CRUISE.C, DRVTICK.C, RACE.C |
| 79A8 | fz79a8 | `char @[2]` | HOTSPOT.C |
| 79AA | bs79aa | `struct Dev @` | MUSIC.C, SOUND.C, SYS.C |
| 79C6 | ao79c6t | `struct Dev @` | MUSIC.C, SYS.C |
| 79E2 | g79e2g | `struct Eng @` | DRVTICK.C, RACE.C |
| 7AA4 | ah7aa4w | `char @[6]` | HOTSPOT.C |
| 7AAA | w_7aaa | `void (interrupt far *@)(void)` | SYS.C |
| 7AAE | aw7aaex | `int @` | ROAD.C, VIEW.C |
| 7AB0 | al7ab0u | `int @[]` | ROAD.C |
| 7ACA | az7acaz | `int @` | VIEW.C |
| 7ACC | r_7acc | `int @` | VIEW.C |
| 7ACE | n_7ace | `int @[]` | ROAD.C |
| 7AE8 | ac7ae8t | `int @` | VIEW.C |
| 7AEA | az7aeau | `int @` | VIEW.C |
| 7B14 | g7b14o | `int @` | ARENA.C, MARKS.C |
| 7D16 | a_7d16q | `int @` | DISK.C, GAME.C |
| 7D18 | g7d18m | `struct Sprite @` | SPRITE.C |
| 7D2E | ao7d2ev | `struct State7D2E @` | DRVTICK.C, RACE.C, ROAD.C, SYS.C, VIEW.C |
| 7D40 | g7d40q | `int @` | BOBS.C, RACE.C |
| 7D62 | g7d62p | `unsigned far *@` | ROAD.C, VIEW.C |
| 7D66 | g7d66n | `int @` | CRUISE.C, DRVTICK.C, VIEW.C |
| 7D68 | g7d68m | `unsigned @[][3]` | BOBS.C, DISK.C |
| 7D7A | ar7d7au | `struct Spec *@` | BOBS.C, RACE.C |
| 7D7C | ap7d7cv | `int @` | RACE.C, VIEW.C |
| 7D7E | gz7d7e | `int @` | RACE.C, ROAD.C |
| 7D80 | g7d80q | `int @` | ARENA.C, MARKS.C |
| 7D82 | g7d82p | `int @` | RACE.C, ROAD.C |
| 7D84 | g7d84o | `struct Racer *@` | BOBS.C, RACE.C |
| 7D86 | g7d86n | `struct Spec @[]` | BOBS.C, CARPIC.C, DISK.C, GAME.C, GARAGE.C, PERF.C, RACE.C, SAVEGAME.C |
| 7E8A | ar7e8av | `struct State7E8A @` | RACE.C, ROAD.C, SYS.C, VIEW.C |
| 7EAA | a_7eaap | `struct State7EAA @` | ADS.C, BOBS.C, CRUISE.C, GAME.C, GARAGE.C, PERF.C, POOLS.C, RACE.C, ROAD.C, SAVEGAME.C, SCREEN.C, SYS.C, TIRES.C |
| 7EC2 | dz7ec2 | `struct Band @[4]` | CARPIC.C |
| 7EE2 | bz7ee2 | `char @[64]` | HOTSPOT.C |
| 7F22 | g7f22s | `int @[]` | ROAD.C |
| 7F2E | e_7f2e | `int @` | VIEW.C |
| 7F30 | g7f30t | `int @` | VIEW.C |
| 7F32 | g7f32s | `char @[]` | BOBS.C, SYS.C |
| 7F82 | g7f82r | `int @` | BOBS.C, RACE.C |
| 7F84 | g7f84q | `int @` | ADS.C |
| 7F86 | bm7f86 | `int @` | BOBS.C |
| 7F88 | a_7f88d | `int @` | RACE.C |
| 7F8A | at7f8au | `int @[]` | ROAD.C |
| 7FA4 | al7fa4z | `int @` | VIEW.C |
| 7FA6 | by7fa6 | `int @` | VIEW.C |
| 7FA8 | bw7fa8 | `struct Bitmap @` | CARPIC.C |
| 7FD8 | g7fd8e | `int @[]` | ROAD.C |
| 7FF2 | dz7ff2 | `int @` | VIEW.C |
| 7FF4 | g7ff4g | `int @` | VIEW.C |
| 7FF6 | g7ff6f | `char @[2]` | HOTSPOT.C |
| 7FF8 | av7ff8 | `struct Racer @[]` | BOBS.C, DISK.C, RACE.C, SAVEGAME.C |
| 8184 | g8184y | `struct Bitmap far *@` | VIEW.C |
| 8188 | g8188w | `struct Bitmap far *@` | DRIVE.C, ROAD.C, VIEW.C |
| 818C | ap818c | `struct Spr6 @[]` | CARPIC.C, DISK.C |
| 8228 | ao8228u | `int @` | CRUISE.C, DRIVE.C, SYS.C, VIEW.C |
| 822A | r_vid | `struct Video @` | ADS.C, ARENA.C, BITMAP.C, BOBS.C, CARPIC.C, CGA.C, CGASPR.C, CRUISE.C, CURSOR.C, DASH.C, DISK.C, DRIVE.C, EDIT.C, EGASLIDE.C, EGASPAN.ASM, FAME.C, GAME.C, GARAGE.C, GARGFX.C, MAIN.C, MEM.C, MENU.C, POPUP.C, RACE.C, ROAD.C, SAVEGAME.C, SCREEN.C, SHRINK.C, SPRITE.C, SYS.C, TANDYSPR.C, TIRES.C, VIDEO.C, VIEW.C, WALKER.C |
| 8242 | x_colors | `struct Colors @` | ADS.C, CGA.C, DRAW.C, EDIT.C, EGA.C, FAME.C, GAME.C, GARAGE.C, MENU.C, POPUP.C, RACE.C, SAVEGAME.C, SCREEN.C, TANDY.C, VIDEO.C |
| 8248 | iz8248 | `char @` | VIDEO.C |
| 8249 | gz8249 | `char @` | CGA.C, CRUISE.C, DASH.C, FAME.C, GARAGE.C, SCREEN.C |
| 824A | af824ax | `char @` | DRIVE.C, SCREEN.C |
| 824C | g824cd | `int @` | ADS.C, CGA.C, EDIT.C, EGA.C, MENU.C, SCREEN.C, TANDY.C |
| 824E | bq824e | `int @` | CGA.C, EDIT.C, EGA.C, MENU.C, TANDY.C |
| 8250 | o_8250 | `int @` | RACE.C, SCREEN.C |
| 8252 | au8252x | `int @` | SCREEN.C |
| 8254 | as8254y | `int @` | SCREEN.C |
| 8256 | aq8256z | `int @` | BOBS.C, SCREEN.C, SYS.C |
| 8258 | g8258z | `int @` | SCREEN.C |
| 825A | bu825a | `struct Bitmap far *@` | CARPIC.C, SPRITE.C, VIDEO.C, WALKER.C |
| 825E | ac825et | `int @` | BOBS.C, CARPIC.C, GARAGE.C, SPRITE.C, SYS.C, VIDEO.C, WALKER.C |
| 8260 | aw8260y | `int @` | BOBS.C, CARPIC.C, GARAGE.C, SPRITE.C, VIDEO.C, WALKER.C |
| 8262 | l_8262 | `int @` | CARPIC.C, SPRITE.C |
| 8264 | h_8264 | `int @` | CARPIC.C, RACE.C |
| 8266 | d_8266 | `int @` | CARPIC.C, RACE.C, VIDEO.C |
| 8268 | ap8268t | `int @` | CARPIC.C, RACE.C, VIDEO.C |
| 826A | g826af | `int @` | CARPIC.C, RACE.C |
| 826C | ae826cv | `int @` | CARPIC.C, RACE.C |
| 826E | ar826e | `int @` | CARPIC.C, RACE.C |
| 8270 | q_8270 | `int @` | CARPIC.C, RACE.C |
| 8272 | m_8272 | `struct State8272 @` | BOBS.C, CARPIC.C, CRUISE.C, GARAGE.C, TIRES.C |
| 8290 | o_8290 | `int @[]` | GARAGE.C, GARGFX.C |
| 829A | af829ax | `int @[]` | GARAGE.C, GARGFX.C |
| 829C | ad829cy | `int @[]` | GARGFX.C |
| 82AC | ap82acz | `int @` | GARAGE.C, GARGFX.C |
| 82AE | hz82ae | `int @` | GARAGE.C, GARGFX.C |
| 82B0 | g82b0u | `char @[2]` | HOTSPOT.C |
| 82B2 | g82b2t | `int @` | GARAGE.C, GARGFX.C |
| 82B4 | g82b4s | `struct Bitmap far *@` | DASH.C, DRIVE.C, ROAD.C, VIEW.C |
| 82B8 | x_82b8 | `struct Bitmap far *@` | BOBS.C |
| 82BC | b_82bc | `long @` | RACE.C, SYS.C |
| 82C0 | ar82c0 | `int @` | BOBS.C, SCREEN.C |
| 82C2 | bp82c2 | `int @` | DRIVE.C, RACE.C |
| 82C4 | bn82c4 | `struct Tone *@` | MUSIC.C, SYS.C |
| 82C6 | bl82c6 | `int @` | MAIN.C, ROAD.C |
| 82C8 | dz82c8x | `int @` | RACE.C |
| 82CA | aytablex | `struct Entry @[]` | DISK.C, FAME.C |
| 83B0 | as83b0 | `struct Node2 @[16]` | POOLS.C |
| 8630 | bz8630z | `int @` | RACE.C |
| 8632 | s_8632 | `int @` | RACE.C |
| 8634 | aw8634x | `unsigned far *@` | ROAD.C, VIEW.C |
| 8638 | as8638y | `int @[]` | ROAD.C |
| 8644 | aw8644v | `int @` | VIEW.C |
| 8646 | k_8646 | `int @` | VIEW.C |
| 8648 | g_8648 | `int @` | RACE.C, ROAD.C |
| 864A | aj864ax | `int @[]` | ROAD.C |
| 8656 | au8656u | `int @` | VIEW.C |
| 8658 | g_8658 | `int @` | VIEW.C |
| 865A | ay865a | `int @[]` | ROAD.C |
| 8666 | au8666t | `int @` | VIEW.C |
| 8668 | as8668u | `int @` | VIEW.C |
| 866A | aj866au | `struct Node @[140]` | POOLS.C |
| 8ACA | bw8aca | `char @` | CRUISE.C, DASH.C, DRVTICK.C, RACE.C, ROAD.C, SYS.C, VIEW.C |
| 8ACC | bu8acc | `int @` | MAIN.C, RACE.C, VIEW.C |
| 8ACE | g8acew | `struct Bitmap far *@[13]` | RACE.C, VIEW.C |
| 8B02 | c_8b02 | `unsigned @[]` | ARENA.C, MARKS.C |
| 8B7C | z_8b7c | `int @` | CARPIC.C, SPRITE.C, SYS.C, TIRES.C |
| 8B7E | bz8b7ex | `struct Bitmap far *@` | VIEW.C |
| 8B82 | am8b82y | `int far *@` | VIEW.C |
| 8B86 | ai8b86y | `int @[]` | MENU.C |
| 8B8E | g8b8eb | `int @` | MENU.C, SYS.C |
| 8B90 | ao8b90v | `struct Lims @` | MENU.C, SYS.C |
| 8B98 | av8b98 | `char @` | MENU.C |
| 8B9A | a_8b9ap | `int @` | MENU.C, SAVEGAME.C |
| 8B9C | a_8b9ca | `int @` | MENU.C |
| 8B9E | az8b9ez | `int *@` | MENU.C |
| 8BA0 | x_8ba0 | `int @` | MENU.C |
| 8BA2 | az8ba2t | `struct Hot *@` | EDIT.C, MENU.C |
| 8BA4 | ax8ba4u | `char @` | MENU.C |
| 8BA6 | av8ba6v | `struct Hot *@` | MENU.C |
| 8BA8 | at8ba8v | `struct Hot *@` | MENU.C, SYS.C |
| 8BAA | az8baa | `int @` | MENU.C, SYS.C |
| 8BAC | ai8bacx | `int @` | MENU.C |
| 8BAE | ag8baey | `unsigned @[][3]` | BOBS.C, DISK.C |
| 8BC0 | cz8bc0y | `int @` | BOBS.C, SYS.C |
| 8BC2 | ay8bc2z | `int @` | RACE.C, VIEW.C |
| 8BC4 | p_8bc4 | `float @` | RACE.C |
| 8BC8 | h_8bc8 | `int @` | DISK.C |
| 8BCA | ak8bcat | `struct Bitmap far *@` | VIEW.C |
| 8BCE | ag8bcet | `int @` | GAME.C, MAIN.C |
| 8BD0 | cz8bd0y | `int @` | RACE.C, ROAD.C |
| 8DD2 | s_8dd2 | `int @` | DISK.C |
| 8DD4 | aw8dd4x | `char *@` | DISK.C |
| 8DD6 | au8dd6y | `char *@` | DISK.C |
| 8DD8 | h_8dd8 | `int @[][3]` | BOBS.C, DISK.C |
| 8DF0 | w_8df0 | `struct Model @[]` | BOBS.C, CARPIC.C, DISK.C, GARAGE.C, PERF.C, SAVEGAME.C, VIEW.C |
| 8EF4 | a8ef4fv | `char @[2]` | HOTSPOT.C |
| 8EF6 | g8ef6q | `int @` | SYS.C |
| 8EF8 | g_8ef8 | `int @` | SYS.C |
| 8EFA | aj8efax | `int @` | SYS.C |
| 8EFC | bw8efc | `int @` | SYS.C |
| 8EFE | av8efe | `int @` | MENU.C |
| 8F00 | o_8f00 | `int @` | ARENA.C, MARKS.C |

## Far data (7)

| seg:off | name | decl |
|---|---|---|
| 2F93:0000 | g_2f93_0000 | `int far @[]` |
| 2F93:002C | g_2f93_002c | `int far @[]` |
| 2F93:0050 | g_2f93_0050 | `int far @[]` |
| 2F93:007C | g_2f93_007c | `int far @[]` |
| 2F93:00A0 | g_2f93_00a0 | `int far @[]` |
| 2F93:00B0 | g_2f93_00b0 | `int far @[]` |
| 2F93:00C0 | g_2f93_00c0 | `int far @[]` |
