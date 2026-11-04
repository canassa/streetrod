#include <stdlib.h>
#include "STRUCTS.H"
#include "ROAD.H"
#include "DASH.H"
#include "DRIVE.H"
#include "MAIN.H"
#include "PC.H"
#include "RACE.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SCREEN2.H"
#include "SCREENS.H"
#include "SLOTS.H"
#include "SYS.H"
#include "VIEW.H"

#define EGA_PAGE1_BITS ((char far *)0xa2000000L)
#define EGA_PAGE2_BITS ((char far *)0xa4000000L)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_buildX;                       /* DS:6DFE */
static int g_buildEntry;                         /* DS:6E00 */
static int g_bendStep;                         /* DS:6E02 */
static int near rnd(int n);
static void near build_left_curve(void);
static void near build_right_curve(void);
static void near build_s_curve_lr(void);
static void near build_s_curve_rl(void);
static void near build_hill(void);
static void near build_dip(void);
static void near build_hill_dip(void);
static void near build_dip_hill(void);
static void near build_right_closed(void);
static void near build_narrows(void);
static void near build_left_closed(void);

int g_hillProfile[22] = {1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -4, -4, -4, -3, -2, -1};   /* DS:5C20 */
int g_hillDipProfile[38] = {1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -5, -5, -5, -5, -5, -5, -4, -3, -2, -1, 0, 0, 1, 2, 3, 4, 4, 4, 4, 3, 2, 1};   /* DS:5C4C */
int g_bumpProfile[17] = {1, 2, 3, 3, 3, 2, 1, 0, 0, 0, -1, -2, -3, -3, -3, -2, -1};   /* DS:5C98 */
int g_layoutLen = 12;   /* DS:5CBA */
int g_roadLayouts[10][12] = {{2, 3, 1, 7, 4, 10, 5, 8, 6, 2, 9, 1}, {11, 4, 8, 1, 6, 9, 2, 5, 7, 2, 3, 1}, {1, 5, 1, 3, 8, 11, 6, 7, 2, 4, 9, 1}, {3, 1, 4, 9, 2, 6, 8, 1, 3, 8, 10, 2}, {10, 9, 5, 1, 6, 2, 3, 7, 4, 2, 8, 2}, {2, 3, 8, 10, 5, 11, 4, 7, 1, 6, 9, 2}, {1, 5, 9, 1, 4, 2, 3, 10, 8, 4, 1, 9}, {2, 4, 7, 11, 2, 5, 8, 4, 1, 7, 6, 2}, {2, 6, 8, 10, 6, 1, 3, 7, 5, 1, 4, 9}, {1, 3, 1, 6, 1, 4, 9, 10, 6, 8, 2, 3}};   /* DS:5CBC */
struct Bitmap g_egaPage1 = {SCREEN_WIDTH, SCREEN_HEIGHT, 8000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0}, {EGA_PAGE1_BITS}, {0}, EGA_PITCH, VID_EGA};   /* DS:5DAC */
struct Bitmap g_egaPage2 = {SCREEN_WIDTH, SCREEN_HEIGHT, 8000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0}, {EGA_PAGE2_BITS}, {0}, EGA_PITCH, VID_EGA};   /* DS:5DDC */
int g_roadLayout = 0;   /* DS:5E0C */
int g_onShoulder = 0;   /* DS:5E0E */
int g_turnRight = 0;   /* DS:5E10 */
int g_turnLeft = 0;   /* DS:5E12 */

/* rnd @ 2645:016A (linear 0x265BA, 16 bytes)

   Near wrapper around random_int, used by the track builder */

static int near rnd(int n)
{
    return random_int(n);
}

/* build_left_curve @ 2645:017A (linear 0x265CA, 281 bytes) */

static void near build_left_curve(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 50; i++)
        g_terrain2[i] = g_buildX;
    g_terrain3[g_buildEntry] |= 0x200;
    g_buildEntry += 5;
    g_terrain0[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] &= 0x1801;
    g_terrain3[g_buildEntry] |= 0x200;
    g_buildEntry += 6;
    for (i = g_buildX / g_bendStep; i < g_buildX / g_bendStep + 40; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}

/* build_right_curve @ 2645:0293 (linear 0x266E3, 277 bytes) */

static void near build_right_curve(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 50; i++)
        g_terrain2[i] = g_buildX;
    g_terrain0[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] &= 0x1801;
    g_terrain3[g_buildEntry] |= 0x400;
    g_buildEntry += 3;
    g_terrain3[g_buildEntry] |= 0x400;
    g_buildEntry += 3;
    for (i = g_buildX / g_bendStep; i > g_buildX / g_bendStep - 45; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}

/* build_s_curve_lr @ 2645:03A8 (linear 0x267F8, 401 bytes) */

static void near build_s_curve_lr(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 90; i++)
        g_terrain2[i] = g_buildX;
    g_buildEntry++;
    g_terrain0[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] |= 0x2000;
    g_buildEntry += 6;
    g_terrain1[g_buildEntry] |= 0x2000;
    for (i = g_buildX / g_bendStep; i < g_buildX / g_bendStep + 40; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
    for (; i > g_buildX / g_bendStep - 43; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}

/* build_s_curve_rl @ 2645:0539 (linear 0x26989, 401 bytes) */

static void near build_s_curve_rl(void)
{
    int i;

    for (i = g_buildEntry; i < g_buildEntry + 90; i++)
        g_terrain2[i] = g_buildX;
    g_buildEntry++;
    g_terrain0[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] |= 0x4000;
    g_buildEntry += 6;
    g_terrain1[g_buildEntry] |= 0x4000;
    for (i = g_buildX / g_bendStep; i > g_buildX / g_bendStep - 35; i--, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
    for (; i < g_buildX / g_bendStep + 48; i++, g_buildEntry++) {
        if (g_buildEntry < 0x442) {
            g_terrain3[g_buildEntry] |= 8;
            g_terrain2[g_buildEntry] = i * g_bendStep;
        } else
            break;
    }
    g_buildX = i * g_bendStep;
}

/* build_hill @ 2645:06CA (linear 0x26B1A, 147 bytes) */

static void near build_hill(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 20;
    for (i = 0; i < 22; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = g_hillProfile[i];
        else
            break;
    }
    g_buildEntry += 8;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_dip @ 2645:075D (linear 0x26BAD, 149 bytes)

   Builds a terrain hill section */

static void near build_dip(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 10;
    for (i = 0; i < 22; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = -g_hillProfile[i];
        else
            break;
    }
    g_buildEntry += 18;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_hill_dip @ 2645:07F2 (linear 0x26C42, 147 bytes)

   Builds a terrain hill section */

static void near build_hill_dip(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 6;
    for (i = 0; i < 38; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = g_hillDipProfile[i];
        else
            break;
    }
    g_buildEntry += 6;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_dip_hill @ 2645:0885 (linear 0x26CD5, 149 bytes)

   Builds a terrain hill section */

static void near build_dip_hill(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 8;
    for (i = 0; i < 38; i++) {
        if (g_buildEntry < 0x442)
            g_terrain4[g_buildEntry++] = -g_hillDipProfile[i];
        else
            break;
    }
    g_buildEntry += 4;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_right_closed @ 2645:091A (linear 0x26D6A, 214 bytes) */

static void near build_right_closed(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 3;
    g_terrain1[g_buildEntry] |= 0x800;
    g_buildEntry += 4;
    g_terrain3[g_buildEntry++] &= 0xff7;
    g_terrain1[g_buildEntry++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_buildEntry] |= 0x40;
        g_terrain3[g_buildEntry++] &= 0xff7;
    }
    g_terrain3[g_buildEntry++] &= 0xff7;
    g_buildEntry += 3;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_narrows @ 2645:09F0 (linear 0x26E40, 203 bytes) */

static void near build_narrows(void)
{
    int i, start;

    start = g_buildEntry;
    g_terrain3[g_buildEntry] |= 0x800;
    g_buildEntry += 4;
    g_terrain0[g_buildEntry] &= 0x1801;
    g_terrain1[g_buildEntry] &= 0x1801;
    g_terrain3[g_buildEntry] |= 0x800;
    g_buildEntry += 6;
    for (i = 0; i < 20; i++) {
        g_terrain3[g_buildEntry] |= 0x80;
        g_terrain3[g_buildEntry++] &= 0xff7;
    }
    g_terrain3[g_buildEntry] &= 0x0fff;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* build_left_closed @ 2645:0ABB (linear 0x26F0B, 214 bytes) */

static void near build_left_closed(void)
{
    int i, start;

    start = g_buildEntry;
    g_buildEntry += 4;
    g_terrain0[g_buildEntry] |= 0x800;
    g_buildEntry += 4;
    g_terrain3[g_buildEntry++] &= 0xff7;
    g_terrain0[g_buildEntry++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_buildEntry] |= 0x20;
        g_terrain3[g_buildEntry++] &= 0xff7;
    }
    g_terrain3[g_buildEntry++] &= 0xff7;
    g_buildEntry += 2;
    for (i = start; i < g_buildEntry; i++)
        g_terrain2[i] = g_buildX;
}

/* road_build_layout @ 2645:0B91 (linear 0x26FE1, 439 bytes)

   Builds the track profile arrays from the section list of the chosen layout. */

void road_build_layout(int n)
{
    int i;

    g_buildEntry = 0x186;
    g_buildX = 0;
    g_bendStep = g_hasSlowCpu ? 2 : 3;
    for (i = 0x17c; i < 0x460; i++) {
        g_terrain2[i] = 0;
        g_terrain4[i] = 0;
        g_terrain3[i] &= 0xf117;
        g_terrain1[i] &= 0x87ff;
        g_terrain0[i] &= 0xe7ff;
        if ((i + 1) % (g_hasSlowCpu * 4 + 4) == 0)
            g_terrain3[i] |= 8;
    }
    if (n == 100)
        g_roadLayout = rnd(10);
    for (i = 0; i < g_layoutLen; i++) {
        switch (g_roadLayouts[g_roadLayout][i]) {
        case 1: build_left_curve(); break;
        case 2: build_right_curve(); break;
        case 3: build_hill(); break;
        case 4: build_dip(); break;
        case 5: build_hill_dip(); break;
        case 6: build_dip_hill(); break;
        case 7: build_right_closed(); break;
        case 8: build_left_closed(); break;
        case 9: build_narrows(); break;
        case 10: build_s_curve_lr(); break;
        case 11: build_s_curve_rl(); break;
        }
    }
    for (; g_buildEntry < 0x460; g_buildEntry++) {
        g_terrain2[g_buildEntry] = g_buildX;
        g_terrain4[g_buildEntry] = 0;
    }
}

/* road_generate @ 2645:0D48 (linear 0x27198, 2517 bytes)

   Builds the terrain flag arrays for a new track: start/finish area, hills,
   then the random features, and finally calls the track builder. */

void road_generate(void)
{
    int cnt, i, j;

    if (r_vid.mode == VID_EGA) {
        bm7678 = &g_egaPage0;
        g8188w = &g_egaPage1;
        g82b4s = &g_egaPage2;
    } else {
        bm7678 = r_vid.front;
        g8188w = r_vid.back;
        g82b4s = &g_screens[2];
    }
    cnt = g_hasSlowCpu;
    for (i = 0; i < 0xb4; i++) {
        g_terrain4[i] = 0;
        g_terrain2[i] = 0;
        g_terrain3[i] = 0;
        g_terrain1[i] = 0;
        g_terrain0[i] = 0;
        g_terrain3[i] |= 1;
        if ((i + 1) % (cnt * 4 + 4) == 0)
            g_terrain3[i] |= 8;
        if ((i + 2) % 16 == 0) {
            g_terrain3[i + 2] |= 4;
            g_terrain3[i + 1] |= 4;
            g_terrain3[i] |= 4;
            g_terrain1[i - 1] |= 0x40;
            g_terrain0[i + 3] |= 0x40;
            g_terrain3[i + 1] |= 1;
            g_terrain3[i + 2] |= 1;
            g_terrain3[i + 3] |= 1;
            g_terrain4[i + 1] = 0;
            g_terrain2[i + 1] = 0;
            g_terrain4[i + 2] = 0;
            g_terrain2[i + 2] = 0;
            g_terrain4[i + 3] = 0;
            g_terrain2[i + 3] = 0;
            i += 3;
        } else {
            if ((i + 4) % 16 == 0 || (i + 8) % 16 == 0 || (i + 12) % 16 == 0) {
                if (rnd(2) == 0) {
                    g_terrain0[i - 1] |= 4;
                    g_terrain0[i] |= 4;
                    if (rnd(3) == 0) {
                        g_terrain0[i - 1] |= 8;
                        g_terrain0[i] |= 8;
                    }
                    g_terrain0[i] |= 0x10;
                } else {
                    g_terrain0[i - 2] |= 4;
                    g_terrain0[i] |= 4;
                    g_terrain0[i - 1] |= 4;
                    if (rnd(3) == 0) {
                        g_terrain0[i - 2] |= 8;
                        g_terrain0[i] |= 8;
                        g_terrain0[i - 1] |= 8;
                    }
                    g_terrain0[i - 1] |= 0x10;
                }
            } else
                g_terrain0[i] |= 0x80;
            if ((i + 5) % 16 == 0 || (i + 9) % 16 == 0 || (i + 12) % 16 == 0) {
                if (rnd(3) != 0) {
                    g_terrain1[i - 1] |= 4;
                    g_terrain1[i] |= 4;
                    if (rnd(2) == 0) {
                        g_terrain1[i - 1] |= 8;
                        g_terrain1[i] |= 8;
                    }
                    g_terrain1[i] |= 0x10;
                } else {
                    g_terrain1[i - 2] |= 4;
                    g_terrain1[i] |= 4;
                    g_terrain1[i - 1] |= 4;
                    if (rnd(4) == 0) {
                        g_terrain1[i - 2] |= 8;
                        g_terrain1[i] |= 8;
                        g_terrain1[i - 1] |= 8;
                    }
                    g_terrain1[i - 1] |= 0x10;
                }
            } else
                g_terrain1[i] |= 0x80;
            if (rnd(cnt * 40 + 6) == 0 && !(g_terrain0[i] & 0x10))
                g_terrain0[i] |= 0x20;
            if (rnd(cnt * 40 + 7) == 0 && !(g_terrain1[i] & 0x10))
                g_terrain1[i] |= 0x20;
            if (rnd(cnt * 40 + 3) == 0 && !(g_terrain0[i] & 0x10))
                g_terrain0[i] |= 0x100;
            if (rnd(cnt * 40 + 4) == 0 && !(g_terrain1[i] & 0x10))
                g_terrain1[i] |= 0x100;
        }
    }
    g_terrain3[rnd(45) + 10] |= 0x4000;
    g_terrain3[rnd(45) + 10] |= 0x8000;
    g_terrain3[rnd(55) + 65] |= 0x4000;
    g_terrain3[rnd(55) + 65] |= 0x8000;
    g_terrain3[rnd(55) + 115] |= 0x4000;
    g_terrain3[rnd(55) + 115] |= 0x8000;
    g_terrain1[44] |= 1;
    g_terrain0[29] |= 0x1000;
    g_terrain0[24] |= 0x800;
    g_terrain0[108] |= 1;
    g_terrain0[87] |= 0x1000;
    g_terrain0[82] |= 0x800;
    g_terrain0[156] |= 1;
    g_terrain0[135] |= 0x1000;
    g_terrain0[130] |= 0x800;
    for (i = 0xb4; i < 0x460; i++) {
        g_terrain4[i] = 0;
        g_terrain2[i] = 0;
        g_terrain3[i] = 0;
        g_terrain0[i] = 0;
        g_terrain1[i] = 0;
        if ((i + 1) % (g_hasSlowCpu * 4 + 4) == 0)
            g_terrain3[i] |= 8;
        if (i % (cnt * 4 + 4) == 0) {
            g_terrain0[i] |= 1;
            g_terrain1[i] |= 1;
        }
        if ((i + 2) % (cnt * 8 + 8) == 0)
            g_terrain3[i] |= 2;
        if (rnd(cnt * 200 + 7) == 0)
            g_terrain0[i] |= 0x20;
        if (rnd(cnt * 200 + 9) == 0)
            g_terrain1[i] |= 0x20;
        if (cnt == 0) {
            if (rnd(35) == 0) {
                g_terrain0[i] |= 0x40;
                g_terrain0[i - 1] |= 0x40;
                g_terrain0[i - 2] |= 0x40;
                g_terrain0[i] &= ~0x20;
                g_terrain0[i - 1] &= ~0x20;
                g_terrain0[i - 2] &= ~0x20;
            }
            if (rnd(25) == 0) {
                g_terrain1[i] |= 0x40;
                g_terrain1[i - 2] |= 0x40;
                g_terrain1[i] &= ~0x20;
                g_terrain1[i - 2] &= ~0x20;
            }
        } else {
            if (rnd(cnt * 100 + 35) == 0) {
                g_terrain0[i] |= 0x40;
                g_terrain0[i] &= ~0x20;
            }
            if (rnd(cnt * 100 + 25) == 0) {
                g_terrain1[i] |= 0x40;
                g_terrain1[i] &= ~0x20;
            }
        }
        if (rnd(cnt * 100 + 7) == 0 && !(g_terrain0[i] & 0x80))
            g_terrain0[i] |= 0x80;
        if (rnd(cnt * 100 + 9) == 0 && !(g_terrain1[i] & 0x80))
            g_terrain1[i] |= 0x80;
        if (rnd(cnt * 100 + 21) == 0)
            g_terrain0[i] |= 4;
        if (rnd(cnt * 100 + 25) == 0)
            g_terrain1[i] |= 4;
        if (rnd(cnt * 100 + 23) == 0 && !(g_terrain0[i] & 4))
            g_terrain0[i] |= 8;
        if (rnd(cnt * 100 + 30) == 0 && !(g_terrain1[i] & 4))
            g_terrain1[i] |= 8;
        if (rnd(cnt * 50 + 4) == 0) {
            g_terrain0[i] |= 0x100;
            if (rnd(2) == 0)
                g_terrain0[i] |= 0x200;
        }
        if (rnd(cnt * 50 + 4) == 0) {
            g_terrain1[i] |= 0x100;
            if (rnd(2) == 0)
                g_terrain1[i] |= 0x200;
        }
    }
    g_terrain0[379] &= 0x1801;
    g_terrain1[379] &= 0x1801;
    g_terrain3[379] |= 0x1010;
    g_terrain0[1019] &= 0x1801;
    g_terrain1[1019] &= 0x1801;
    g_terrain3[1019] |= 0x2010;
    {
        i = rnd(140) + 220;
        j = (g_terrain1[i] & 2) ? i - 1 : i;
        g_terrain0[j] &= 0x1801;
        g_terrain1[j] &= 0x1801;
        g_terrain3[j] |= 0x8000;
        i = rnd(600) + 390;
        j = (g_terrain1[i] & 2) ? i - 1 : i;
        g_terrain0[j] &= 0x1801;
        g_terrain1[j] &= 0x1801;
        g_terrain3[j] |= 0x8000;
    }
    i = rnd(600) + 390;
    j = (g_terrain1[i] & 2) ? i - 1 : i;
    g_terrain0[j] &= 0x1801;
    g_terrain1[j] &= 0x1801;
    g_terrain3[j] |= 0x4000;
    i = 220;
    do {
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = g_bumpProfile[j];
        i += 20;
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = -g_bumpProfile[j];
        i += 30;
    } while (i + 90 < 0x17c);
    g_terrain3[385] |= 0x100;
    road_build_layout(100);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
unsigned far *q_72ee;
int g_roadLXArray[13];
int far *g_hereCurvePtr;
int far *af7880z;
int bq7884[13];
int aw7aaex;
int g_yBuildingTopArray[13];
int g_rightXProjectionTable[13];
unsigned far *g7d62p;
int g_lhsRdMirrorX[6];
int g_xOuterLProjArray[13];
int g_rhsOuterXArray[13];
struct MirrorRoad g_sharedMirrorRoadSet; /* DS:8634 */
struct MirrorEdges g_videoMirrorEdgesSet; /* DS:864A */

/* road_seek @ 2645:171D (linear 0x27B6D, 82 bytes)

   Points the terrain pointers at entry n of each terrain array. */

void road_seek(int n)
{
    q_72ee = &g_terrain0[n];
    g_sharedMirrorRoadSet.terrain = &g_terrain1[n];
    g7d62p = &g_terrain3[n];
    af7880z = &g_terrain4[n];
    g_hereCurvePtr = &g_terrain2[n];
}

/* road_proj_x @ 2645:176F (linear 0x27BBF, 66 bytes) */

int road_proj_x(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = ao7d2ev.lateralX - b;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return 0xa0 - u;
}

/* road_proj_y @ 2645:17B1 (linear 0x27C01, 60 bytes) */

int road_proj_y(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - ao7d2ev.cameraY;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return u + 0x3c;
}

/* road_mirror_x @ 2645:17ED (linear 0x27C3D, 59 bytes) */

int road_mirror_x(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = ao7d2ev.lateralX - b;
    if (a == 0)
        a++;
    u = v / a;
    return 0x118 - u;
}

/* road_mirror_y @ 2645:1828 (linear 0x27C78, 53 bytes) */

int road_mirror_y(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - ao7d2ev.cameraY;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}

/* road_reset @ 2645:185D (linear 0x27CAD, 158 bytes)

   Resets the terrain scroll state for a new track. */

void road_reset(void)
{
    if (bw8aca)
        ao7d2ev.roadIndex = 200;
    else
        switch (g_scene) {
        case 1:
            ao7d2ev.roadIndex = 10;
            break;
        case 2:
            ao7d2ev.roadIndex = 65;
            break;
        case 3:
            ao7d2ev.roadIndex = 115;
            break;
        }
    road_seek(ao7d2ev.roadIndex);
    g_leftEdgeX = 1500;
    g_edgeXR = 3300;
    ao7d2ev.roadFraction = 0;
    ao7d2ev.lateralX = 2560;
    ao7d2ev.cameraY = 1900;
    ao7d2ev.m7d3a = 0;
    ao7d2ev.m7d3e = 0;
    ao7d2ev.m7d3c = 0;
    ao7d2ev.distanceStep = 0;
    g_rivalLeft = 1;
    g_swapArmedRivalValue = !random_int(2);
    g_ramAllowed = 1;
}

/* road_project_ahead @ 2645:18FB (linear 0x27D4B, 721 bytes)

   Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. road_project_behind is the backward twin. */

void road_project_ahead(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p++ & 0xe0;
    p11 = g_rightXProjectionTable;
    p2 = g_roadLXArray;
    p3 = bq7884;
    p42 = g_xOuterLProjArray;
    p5l = g_rhsOuterXArray;
    p6 = g_yBuildingTopArray;
    rowz = 4;
    d1 = ao7d2ev.m7d3e - *af7880z++;
    d2 = ao7d2ev.m7d3c - *g_hereCurvePtr++;
    *p3++ = road_proj_y(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = road_proj_x(rowz, 0xaf0, d2);
            *p2++ = road_proj_x(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = road_proj_x(rowz, 0xce4, d2);
            *p2++ = road_proj_x(rowz, 2000, d2);
        } else {
            *p11++ = road_proj_x(rowz, 0xaf0, d2);
            *p2++ = road_proj_x(rowz, 1500, d2);
        }
    } else {
        *p11++ = road_proj_x(rowz, 0xce4, d2);
        *p2++ = road_proj_x(rowz, 1500, d2);
    }
    *p5l++ = road_proj_x(rowz, 0xe74, d2);
    *p42++ = road_proj_x(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = road_proj_y(rowz, 0x76c, d1);
    else
        p6++;
    rowz = 12 - ao7d2ev.roadFraction;
    k7 = 14;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p++;
        d1 = ao7d2ev.m7d3e - *af7880z;
        af7880z++;
        d2 = ao7d2ev.m7d3c - *g_hereCurvePtr;
        g_hereCurvePtr++;
        if (bits) {
            if (bits & 0x80) {
                *p11 = road_proj_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = road_proj_x(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = road_proj_x(rowz, 0xce4, d2);
                p11++;
                *p2 = road_proj_x(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = road_proj_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = road_proj_x(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = road_proj_x(rowz, 0xce4, d2);
            p11++;
            *p2 = road_proj_x(rowz, 1500, d2);
            p2++;
        }
        *p3 = road_proj_y(rowz, 2000, d1);
        p3++;
        *p5l = road_proj_x(rowz, 0xe74, d2);
        p5l++;
        *p42 = road_proj_x(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = road_proj_y(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* road_project_behind @ 2645:1BCC (linear 0x2801C, 722 bytes)

   Fills the per-row projection tables (8 rows, the first one peeled) from the
   terrain pointers, walking the track backward. road_project_ahead is the forward twin. */

void road_project_behind(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p-- & 0xe0;
    p11 = g_videoMirrorEdgesSet.rightX;
    p2 = g_lhsRdMirrorX;
    p3 = g_sharedMirrorRoadSet.y;
    p42 = al70c0x.m70c2;
    p5l = al70c0x.m70d2;
    p6 = g_videoMirrorEdgesSet.buildingY;
    rowz = 8;
    d1 = *af7880z-- - ao7d2ev.m7d3e;
    d2 = *g_hereCurvePtr-- - ao7d2ev.m7d3c;
    *p3++ = road_mirror_y(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = road_mirror_x(rowz, 0xaf0, d2);
            *p2++ = road_mirror_x(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = road_mirror_x(rowz, 0xce4, d2);
            *p2++ = road_mirror_x(rowz, 2000, d2);
        } else {
            *p11++ = road_mirror_x(rowz, 0xaf0, d2);
            *p2++ = road_mirror_x(rowz, 1500, d2);
        }
    } else {
        *p11++ = road_mirror_x(rowz, 0xce4, d2);
        *p2++ = road_mirror_x(rowz, 1500, d2);
    }
    *p5l++ = road_mirror_x(rowz, 0xe74, d2);
    *p42++ = road_mirror_x(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = road_mirror_y(rowz, 0x76c, d1);
    else
        p6++;
    rowz = ao7d2ev.roadFraction + 8;
    k7 = 7;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p--;
        d1 = *af7880z - ao7d2ev.m7d3e;
        af7880z--;
        d2 = *g_hereCurvePtr - ao7d2ev.m7d3c;
        g_hereCurvePtr--;
        if (bits) {
            if (bits & 0x80) {
                *p11 = road_mirror_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = road_mirror_x(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = road_mirror_x(rowz, 0xce4, d2);
                p11++;
                *p2 = road_mirror_x(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = road_mirror_x(rowz, 0xaf0, d2);
                p11++;
                *p2 = road_mirror_x(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = road_mirror_x(rowz, 0xce4, d2);
            p11++;
            *p2 = road_mirror_x(rowz, 1500, d2);
            p2++;
        }
        *p3 = road_mirror_y(rowz, 2000, d1);
        p3++;
        *p5l = road_mirror_x(rowz, 0xe74, d2);
        p5l++;
        *p42 = road_mirror_x(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = road_mirror_y(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* road_draw_frame @ 2645:1E9E (linear 0x282EE, 94 bytes)

   Rebuilds the projection tables for both directions, then waits 9 ticks. */

void road_draw_frame(void)
{
    long deadline;

    road_seek(ao7d2ev.roadIndex);
    road_project_ahead();
    road_seek(ao7d2ev.roadIndex);
    road_project_behind();
    road_seek(ao7d2ev.roadIndex);
    deadline = g_tick + 9;
    view_draw();
    while (deadline > g_tick)
        ;
}

/* road_advance @ 2645:1EFC (linear 0x2834C, 367 bytes) */

void road_advance(void)
{
    int bits7, goal6, diff;

    ao7d2ev.roadIndex += ao7d2ev.roadFraction >> 3;
    road_seek(ao7d2ev.roadIndex);
    ao7d2ev.roadFraction &= 7;
    bits7 = *g7d62p & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g_leftEdgeX = 2000;
            g_edgeXR = 0xaf0;
        } else if (bits7 & 0x20) {
            g_leftEdgeX = 2000;
            g_edgeXR = 0xce4;
        } else {
            g_leftEdgeX = 1500;
            g_edgeXR = 0xaf0;
        }
    } else {
        g_leftEdgeX = 1500;
        g_edgeXR = 0xce4;
    }
    bits7 = g7d62p[4] & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            al70c0x.leftEdge = 2000;
            ao7d2ev.rightEdge = 0xaf0;
        } else if (bits7 & 0x20) {
            al70c0x.leftEdge = 2000;
            ao7d2ev.rightEdge = 0xce4;
        } else {
            al70c0x.leftEdge = 1500;
            ao7d2ev.rightEdge = 0xaf0;
        }
    } else {
        al70c0x.leftEdge = 1500;
        ao7d2ev.rightEdge = 0xce4;
    }
    ao7d2ev.m7d3e = *af7880z;
    if (g_demo != 0 || g_autoDrive != 0 || (a_7eaap.raceType == 0 ? 0x17c : 0x3fc) < ao7d2ev.roadIndex) {
        goal6 = 0x960;
        if (ar7e8av.lateralX > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = ao7d2ev.lateralX - goal6;
        if (abs(diff) > 100)
            ao7d2ev.lateralX = goal6 + (goal6 < ao7d2ev.lateralX ? 100 : -100);
        else
            ao7d2ev.lateralX = goal6;
        if (abs(*g_hereCurvePtr - ao7d2ev.m7d3c) > 0) {
            g_driveEng.steering = (ao7d2ev.m7d3c - *g_hereCurvePtr) * 2;
            if (abs(g_driveEng.steering) > 24)
                g_driveEng.steering = g_driveEng.steering > 0 ? 24 : -24;
        }
        ao7d2ev.m7d3c = *g_hereCurvePtr;
    }
}

/* road_check_crash @ 2645:206B (linear 0x284BB, 169 bytes)

   Returns 1 when the car at the current terrain index would hit something. */

int road_check_crash(void)
{
    g_onShoulder = 0;
    if (g_leftEdgeX - 0x104 < ao7d2ev.lateralX - 0x1e && ao7d2ev.lateralX + 0x46 < g_edgeXR + 0x104) {
        if (g_leftEdgeX > ao7d2ev.lateralX - 0x1e) {
            if ((g_terrain0[ao7d2ev.roadIndex] & 0x1000) || (g_terrain0[ao7d2ev.roadIndex - 1] & 0x1000))
                return 1;
            if (g_terrain3[ao7d2ev.roadIndex + 1] & 0xf000)
                return 1;
            g_onShoulder = 1;
        } else if (ao7d2ev.lateralX + 0x46 > g_edgeXR) {
            if ((g_terrain1[ao7d2ev.roadIndex] & 0x1000) || (g_terrain1[ao7d2ev.roadIndex - 1] & 0x1000))
                return 1;
            if (g_terrain3[ao7d2ev.roadIndex + 1] & 0xf002)
                return 1;
            g_onShoulder = 1;
        }
        return 0;
    }
    return 1;
}

/* road_start @ 2645:2114 (linear 0x28564, 18 bytes) */

void road_start(void)
{
    g_turnRight = g_turnLeft = 0;
    road_reset();
    view_start();
}

/* road_turn @ 2645:2126 (linear 0x28576, 23 bytes) */

void road_turn(int dir)
{
    if (dir == 1)
        ao7d2ev.m7d3c += 3;
    else
        ao7d2ev.m7d3c -= 3;
}

/* road_update @ 2645:213D (linear 0x2858D, 272 bytes) */

void road_update(void)
{
    if (g_driveEng.steeringInput && g_driveEng.speed && abs(ao7d2ev.m7d3c - *g_hereCurvePtr - g_driveEng.steeringInput) < 200)
        ao7d2ev.m7d3c -= (g_driveEng.steeringInput >> 1) + 1;
    if (!bw8aca) {
        if (g_hasSlowCpu)
            ao7d2ev.distanceStep = 6;
        else
            ao7d2ev.distanceStep = 4;
    }
    if (ao7d2ev.distanceStep > 0 && aw7aaex > 0)
        aw7aaex -= 4;
    road_draw_frame();
    if (g_demo == 0 && g_autoDrive == 0) {
        if (g_failureTire)
            ao7d2ev.lateralX += 25;
        else if (g_driveEng.speed)
            ao7d2ev.lateralX -= (ao7d2ev.m7d3c - *g_hereCurvePtr) * (ao7d2ev.distanceStep + g_driveEng.tireSlip) >> 2;
    }
    if (!bw8aca) {
        ao7d2ev.roadFraction += ao7d2ev.distanceStep;
        if (ao7d2ev.roadFraction >= 8)
            road_advance();
        if ((g_terrain0[ao7d2ev.roadIndex + 7] & 1) || g_turnLeft) {
            g_turnLeft = 1;
            road_turn(1);
        }
        if ((g_terrain1[ao7d2ev.roadIndex + 6] & 1) || g_turnRight) {
            road_turn(-1);
            g_turnRight = 1;
        }
    }
}
