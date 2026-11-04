#include <stdlib.h>
#include "STRUCTS.H"
#include "M_265BA.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_113A2.H"
#include "M_15236.H"
#include "M_16EA2.H"
#include "M_2869E.H"
#include "SCREEN2.H"
#include "SCREENS.H"
#include "SLOTS.H"

#pragma intrinsic(abs)

static int g_6dfeaa;                       /* DS:6DFE */
static int g_6e00;                         /* DS:6E00 */
static int g_6e02;                         /* DS:6E02 */
static int near F_265BA(int n);
static void near F_265CA(void);
static void near F_266E3(void);
static void near F_267F8(void);
static void near F_26989(void);
static void near F_26B1A(void);
static void near F_26BAD(void);
static void near F_26C42(void);
static void near F_26CD5(void);
static void near F_26D6A(void);
static void near F_26E40(void);
static void near F_26F0B(void);

int g_5c20[22] = {1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -4, -4, -4, -3, -2, -1};   /* DS:5C20 */
int g_5c4c[38] = {1, 2, 3, 4, 4, 4, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -5, -5, -5, -5, -5, -5, -4, -3, -2, -1, 0, 0, 1, 2, 3, 4, 4, 4, 4, 3, 2, 1};   /* DS:5C4C */
int g_5c98[17] = {1, 2, 3, 3, 3, 2, 1, 0, 0, 0, -1, -2, -3, -3, -3, -2, -1};   /* DS:5C98 */
int g_5cba = 12;   /* DS:5CBA */
int g_5cbc[10][12] = {{2, 3, 1, 7, 4, 10, 5, 8, 6, 2, 9, 1}, {11, 4, 8, 1, 6, 9, 2, 5, 7, 2, 3, 1}, {1, 5, 1, 3, 8, 11, 6, 7, 2, 4, 9, 1}, {3, 1, 4, 9, 2, 6, 8, 1, 3, 8, 10, 2}, {10, 9, 5, 1, 6, 2, 3, 7, 4, 2, 8, 2}, {2, 3, 8, 10, 5, 11, 4, 7, 1, 6, 9, 2}, {1, 5, 9, 1, 4, 2, 3, 10, 8, 4, 1, 9}, {2, 4, 7, 11, 2, 5, 8, 4, 1, 7, 6, 2}, {2, 6, 8, 10, 6, 1, 3, 7, 5, 1, 4, 9}, {1, 3, 1, 6, 1, 4, 9, 10, 6, 8, 2, 3}};   /* DS:5CBC */
struct Bitmap g_5dac = {320, 200, 8000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0}, {(char far *)0xA2000000L}, {0}, 40, -2};   /* DS:5DAC */
struct Bitmap g_5ddc = {320, 200, 8000, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0}, {(char far *)0xA4000000L}, {0}, 40, -2};   /* DS:5DDC */
int g_5e0c = 0;   /* DS:5E0C */
int g_5e0e = 0;   /* DS:5E0E */
int g_5e10 = 0;   /* DS:5E10 */
int g_5e12 = 0;   /* DS:5E12 */

/* F_265BA @ 2645:016A: near wrapper around random_int, used by the track builder */

static int near F_265BA(int n)
{
    return random_int(n);
}

/* F_265CA @ 2645:017A (linear 0x265CA, 281 bytes) */

static void near F_265CA(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 50; i++)
        g_terrain2[i] = g_6dfeaa;
    g_terrain3[g_6e00] |= 0x200;
    g_6e00 += 5;
    g_terrain0[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] &= 0x1801;
    g_terrain3[g_6e00] |= 0x200;
    g_6e00 += 6;
    for (i = g_6dfeaa / g_6e02; i < g_6dfeaa / g_6e02 + 40; i++, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
}

/* F_266E3 @ 2645:0293 (linear 0x266E3, 277 bytes) */

static void near F_266E3(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 50; i++)
        g_terrain2[i] = g_6dfeaa;
    g_terrain0[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] &= 0x1801;
    g_terrain3[g_6e00] |= 0x400;
    g_6e00 += 3;
    g_terrain3[g_6e00] |= 0x400;
    g_6e00 += 3;
    for (i = g_6dfeaa / g_6e02; i > g_6dfeaa / g_6e02 - 45; i--, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
}

/* F_267F8 @ 2645:03A8 (linear 0x267F8, 401 bytes) */

static void near F_267F8(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 90; i++)
        g_terrain2[i] = g_6dfeaa;
    g_6e00++;
    g_terrain0[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] |= 0x2000;
    g_6e00 += 6;
    g_terrain1[g_6e00] |= 0x2000;
    for (i = g_6dfeaa / g_6e02; i < g_6dfeaa / g_6e02 + 40; i++, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
    for (; i > g_6dfeaa / g_6e02 - 43; i--, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
}

/* F_26989 @ 2645:0539 (linear 0x26989, 401 bytes) */

static void near F_26989(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 90; i++)
        g_terrain2[i] = g_6dfeaa;
    g_6e00++;
    g_terrain0[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] |= 0x4000;
    g_6e00 += 6;
    g_terrain1[g_6e00] |= 0x4000;
    for (i = g_6dfeaa / g_6e02; i > g_6dfeaa / g_6e02 - 35; i--, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
    for (; i < g_6dfeaa / g_6e02 + 48; i++, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_terrain3[g_6e00] |= 8;
            g_terrain2[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfeaa = i * g_6e02;
}

/* F_26B1A @ 2645:06CA (linear 0x26B1A, 147 bytes) */

static void near F_26B1A(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 20;
    for (i = 0; i < 22; i++) {
        if (g_6e00 < 0x442)
            g_terrain4[g_6e00++] = g_5c20[i];
        else
            break;
    }
    g_6e00 += 8;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26BAD (near, builds a terrain hill section) */

static void near F_26BAD(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 10;
    for (i = 0; i < 22; i++) {
        if (g_6e00 < 0x442)
            g_terrain4[g_6e00++] = -g_5c20[i];
        else
            break;
    }
    g_6e00 += 18;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26C42 (near, builds a terrain hill section) */

static void near F_26C42(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 6;
    for (i = 0; i < 38; i++) {
        if (g_6e00 < 0x442)
            g_terrain4[g_6e00++] = g_5c4c[i];
        else
            break;
    }
    g_6e00 += 6;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26CD5 (near, builds a terrain hill section) */

static void near F_26CD5(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 8;
    for (i = 0; i < 38; i++) {
        if (g_6e00 < 0x442)
            g_terrain4[g_6e00++] = -g_5c4c[i];
        else
            break;
    }
    g_6e00 += 4;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26D6A @ 2645:091A (linear 0x26D6A, 214 bytes) */

static void near F_26D6A(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 3;
    g_terrain1[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_terrain3[g_6e00++] &= 0xff7;
    g_terrain1[g_6e00++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_6e00] |= 0x40;
        g_terrain3[g_6e00++] &= 0xff7;
    }
    g_terrain3[g_6e00++] &= 0xff7;
    g_6e00 += 3;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26E40 @ 2645:09F0 (linear 0x26E40, 203 bytes) */

static void near F_26E40(void)
{
    int i, start;

    start = g_6e00;
    g_terrain3[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_terrain0[g_6e00] &= 0x1801;
    g_terrain1[g_6e00] &= 0x1801;
    g_terrain3[g_6e00] |= 0x800;
    g_6e00 += 6;
    for (i = 0; i < 20; i++) {
        g_terrain3[g_6e00] |= 0x80;
        g_terrain3[g_6e00++] &= 0xff7;
    }
    g_terrain3[g_6e00] &= 0x0fff;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26F0B @ 2645:0ABB (linear 0x26F0B, 214 bytes) */

static void near F_26F0B(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 4;
    g_terrain0[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_terrain3[g_6e00++] &= 0xff7;
    g_terrain0[g_6e00++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_terrain3[g_6e00] |= 0x20;
        g_terrain3[g_6e00++] &= 0xff7;
    }
    g_terrain3[g_6e00++] &= 0xff7;
    g_6e00 += 2;
    for (i = start; i < g_6e00; i++)
        g_terrain2[i] = g_6dfeaa;
}

/* F_26FE1 @ 2645:0B91 (linear 0x26FE1, 439 bytes)

   Builds the track profile arrays from the section list of the chosen layout. */

void F_26FE1(int n)
{
    int i;

    g_6e00 = 0x186;
    g_6dfeaa = 0;
    g_6e02 = bl82c6 ? 2 : 3;
    for (i = 0x17c; i < 0x460; i++) {
        g_terrain2[i] = 0;
        g_terrain4[i] = 0;
        g_terrain3[i] &= 0xf117;
        g_terrain1[i] &= 0x87ff;
        g_terrain0[i] &= 0xe7ff;
        if ((i + 1) % (bl82c6 * 4 + 4) == 0)
            g_terrain3[i] |= 8;
    }
    if (n == 100)
        g_5e0c = F_265BA(10);
    for (i = 0; i < g_5cba; i++) {
        switch (g_5cbc[g_5e0c][i]) {
        case 1: F_265CA(); break;
        case 2: F_266E3(); break;
        case 3: F_26B1A(); break;
        case 4: F_26BAD(); break;
        case 5: F_26C42(); break;
        case 6: F_26CD5(); break;
        case 7: F_26D6A(); break;
        case 8: F_26F0B(); break;
        case 9: F_26E40(); break;
        case 10: F_267F8(); break;
        case 11: F_26989(); break;
        }
    }
    for (; g_6e00 < 0x460; g_6e00++) {
        g_terrain2[g_6e00] = g_6dfeaa;
        g_terrain4[g_6e00] = 0;
    }
}

/* F_27198 @ 2645:0D48 (linear 0x27198, 2517 bytes)

   Builds the terrain flag arrays for a new track: start/finish area, hills,
   then the random features, and finally calls the track builder. */

void F_27198(void)
{
    int cnt, i, j;

    if (r_vid.mode == -2) {
        bm7678 = &g_2fa3_0000;
        g8188w = &g_5dac;
        g82b4s = &g_5ddc;
    } else {
        bm7678 = r_vid.front;
        g8188w = r_vid.back;
        g82b4s = &g_screens[2];
    }
    cnt = bl82c6;
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
                if (F_265BA(2) == 0) {
                    g_terrain0[i - 1] |= 4;
                    g_terrain0[i] |= 4;
                    if (F_265BA(3) == 0) {
                        g_terrain0[i - 1] |= 8;
                        g_terrain0[i] |= 8;
                    }
                    g_terrain0[i] |= 0x10;
                } else {
                    g_terrain0[i - 2] |= 4;
                    g_terrain0[i] |= 4;
                    g_terrain0[i - 1] |= 4;
                    if (F_265BA(3) == 0) {
                        g_terrain0[i - 2] |= 8;
                        g_terrain0[i] |= 8;
                        g_terrain0[i - 1] |= 8;
                    }
                    g_terrain0[i - 1] |= 0x10;
                }
            } else
                g_terrain0[i] |= 0x80;
            if ((i + 5) % 16 == 0 || (i + 9) % 16 == 0 || (i + 12) % 16 == 0) {
                if (F_265BA(3) != 0) {
                    g_terrain1[i - 1] |= 4;
                    g_terrain1[i] |= 4;
                    if (F_265BA(2) == 0) {
                        g_terrain1[i - 1] |= 8;
                        g_terrain1[i] |= 8;
                    }
                    g_terrain1[i] |= 0x10;
                } else {
                    g_terrain1[i - 2] |= 4;
                    g_terrain1[i] |= 4;
                    g_terrain1[i - 1] |= 4;
                    if (F_265BA(4) == 0) {
                        g_terrain1[i - 2] |= 8;
                        g_terrain1[i] |= 8;
                        g_terrain1[i - 1] |= 8;
                    }
                    g_terrain1[i - 1] |= 0x10;
                }
            } else
                g_terrain1[i] |= 0x80;
            if (F_265BA(cnt * 40 + 6) == 0 && !(g_terrain0[i] & 0x10))
                g_terrain0[i] |= 0x20;
            if (F_265BA(cnt * 40 + 7) == 0 && !(g_terrain1[i] & 0x10))
                g_terrain1[i] |= 0x20;
            if (F_265BA(cnt * 40 + 3) == 0 && !(g_terrain0[i] & 0x10))
                g_terrain0[i] |= 0x100;
            if (F_265BA(cnt * 40 + 4) == 0 && !(g_terrain1[i] & 0x10))
                g_terrain1[i] |= 0x100;
        }
    }
    g_terrain3[F_265BA(45) + 10] |= 0x4000;
    g_terrain3[F_265BA(45) + 10] |= 0x8000;
    g_terrain3[F_265BA(55) + 65] |= 0x4000;
    g_terrain3[F_265BA(55) + 65] |= 0x8000;
    g_terrain3[F_265BA(55) + 115] |= 0x4000;
    g_terrain3[F_265BA(55) + 115] |= 0x8000;
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
        if ((i + 1) % (bl82c6 * 4 + 4) == 0)
            g_terrain3[i] |= 8;
        if (i % (cnt * 4 + 4) == 0) {
            g_terrain0[i] |= 1;
            g_terrain1[i] |= 1;
        }
        if ((i + 2) % (cnt * 8 + 8) == 0)
            g_terrain3[i] |= 2;
        if (F_265BA(cnt * 200 + 7) == 0)
            g_terrain0[i] |= 0x20;
        if (F_265BA(cnt * 200 + 9) == 0)
            g_terrain1[i] |= 0x20;
        if (cnt == 0) {
            if (F_265BA(35) == 0) {
                g_terrain0[i] |= 0x40;
                g_terrain0[i - 1] |= 0x40;
                g_terrain0[i - 2] |= 0x40;
                g_terrain0[i] &= ~0x20;
                g_terrain0[i - 1] &= ~0x20;
                g_terrain0[i - 2] &= ~0x20;
            }
            if (F_265BA(25) == 0) {
                g_terrain1[i] |= 0x40;
                g_terrain1[i - 2] |= 0x40;
                g_terrain1[i] &= ~0x20;
                g_terrain1[i - 2] &= ~0x20;
            }
        } else {
            if (F_265BA(cnt * 100 + 35) == 0) {
                g_terrain0[i] |= 0x40;
                g_terrain0[i] &= ~0x20;
            }
            if (F_265BA(cnt * 100 + 25) == 0) {
                g_terrain1[i] |= 0x40;
                g_terrain1[i] &= ~0x20;
            }
        }
        if (F_265BA(cnt * 100 + 7) == 0 && !(g_terrain0[i] & 0x80))
            g_terrain0[i] |= 0x80;
        if (F_265BA(cnt * 100 + 9) == 0 && !(g_terrain1[i] & 0x80))
            g_terrain1[i] |= 0x80;
        if (F_265BA(cnt * 100 + 21) == 0)
            g_terrain0[i] |= 4;
        if (F_265BA(cnt * 100 + 25) == 0)
            g_terrain1[i] |= 4;
        if (F_265BA(cnt * 100 + 23) == 0 && !(g_terrain0[i] & 4))
            g_terrain0[i] |= 8;
        if (F_265BA(cnt * 100 + 30) == 0 && !(g_terrain1[i] & 4))
            g_terrain1[i] |= 8;
        if (F_265BA(cnt * 50 + 4) == 0) {
            g_terrain0[i] |= 0x100;
            if (F_265BA(2) == 0)
                g_terrain0[i] |= 0x200;
        }
        if (F_265BA(cnt * 50 + 4) == 0) {
            g_terrain1[i] |= 0x100;
            if (F_265BA(2) == 0)
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
        i = F_265BA(140) + 220;
        j = (g_terrain1[i] & 2) ? i - 1 : i;
        g_terrain0[j] &= 0x1801;
        g_terrain1[j] &= 0x1801;
        g_terrain3[j] |= 0x8000;
        i = F_265BA(600) + 390;
        j = (g_terrain1[i] & 2) ? i - 1 : i;
        g_terrain0[j] &= 0x1801;
        g_terrain1[j] &= 0x1801;
        g_terrain3[j] |= 0x8000;
    }
    i = F_265BA(600) + 390;
    j = (g_terrain1[i] & 2) ? i - 1 : i;
    g_terrain0[j] &= 0x1801;
    g_terrain1[j] &= 0x1801;
    g_terrain3[j] |= 0x4000;
    i = 220;
    do {
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = g_5c98[j];
        i += 20;
        for (j = 0; j < 17; j++)
            g_terrain4[i++] = -g_5c98[j];
        i += 30;
    } while (i + 90 < 0x17c);
    g_terrain3[385] |= 0x100;
    F_26FE1(100);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
unsigned far *q_72ee;
int at72f8[13];
int far *af7560z;
int far *af7880z;
int bq7884[13];
int aw7aaex;
int al7ab0u[13];
int n_7ace[13];
unsigned far *g7d62p;
int g7f22s[6];
int at7f8au[13];
int g7fd8e[13];
unsigned far *aw8634x;
int as8638y[6];
int aj864ax[6];
int ay865a[6];

/* F_27B6D @ 2645:171D (linear 0x27B6D, 82 bytes)

   Points the terrain pointers at entry n of each terrain array. */

void F_27B6D(int n)
{
    q_72ee = &g_terrain0[n];
    aw8634x = &g_terrain1[n];
    g7d62p = &g_terrain3[n];
    af7880z = &g_terrain4[n];
    af7560z = &g_terrain2[n];
}

/* F_27BBF @ 2645:176F (linear 0x27BBF, 66 bytes) */

int F_27BBF(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = ao7d2ev.m7d36 - b;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return 0xa0 - u;
}

/* F_27C01 @ 2645:17B1 (linear 0x27C01, 60 bytes) */

int F_27C01(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - ao7d2ev.m7d38;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return u + 0x3c;
}

/* F_27C3D @ 2645:17ED (linear 0x27C3D, 59 bytes) */

int F_27C3D(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = ao7d2ev.m7d36 - b;
    if (a == 0)
        a++;
    u = v / a;
    return 0x118 - u;
}

/* F_27C78 @ 2645:1828 (linear 0x27C78, 53 bytes) */

int F_27C78(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - ao7d2ev.m7d38;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}

/* F_27CAD @ 2645:185D (linear 0x27CAD, 158 bytes)

   Resets the terrain scroll state for a new track. */

void F_27CAD(void)
{
    if (bw8aca)
        ao7d2ev.m7d30 = 200;
    else
        switch (g_0284) {
        case 1:
            ao7d2ev.m7d30 = 10;
            break;
        case 2:
            ao7d2ev.m7d30 = 65;
            break;
        case 3:
            ao7d2ev.m7d30 = 115;
            break;
        }
    F_27B6D(ao7d2ev.m7d30);
    g7d82p = 1500;
    cz8bd0y = 3300;
    ao7d2ev.m7d32 = 0;
    ao7d2ev.m7d36 = 2560;
    ao7d2ev.m7d38 = 1900;
    ao7d2ev.m7d3a = 0;
    ao7d2ev.m7d3e = 0;
    ao7d2ev.m7d3c = 0;
    ao7d2ev.m7d34 = 0;
    g_54d4 = 1;
    g_8648 = !random_int(2);
    gz7d7e = 1;
}

/* F_27D4B @ 2645:18FB (linear 0x27D4B, 721 bytes)

   Fills the per-row projection tables (15 rows, the first one peeled) from the
   terrain pointers, walking the track forward. F_2801C is the backward twin. */

void F_27D4B(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p++ & 0xe0;
    p11 = n_7ace;
    p2 = at72f8;
    p3 = bq7884;
    p42 = at7f8au;
    p5l = g7fd8e;
    p6 = al7ab0u;
    rowz = 4;
    d1 = ao7d2ev.m7d3e - *af7880z++;
    d2 = ao7d2ev.m7d3c - *af7560z++;
    *p3++ = F_27C01(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = F_27BBF(rowz, 0xaf0, d2);
            *p2++ = F_27BBF(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = F_27BBF(rowz, 0xce4, d2);
            *p2++ = F_27BBF(rowz, 2000, d2);
        } else {
            *p11++ = F_27BBF(rowz, 0xaf0, d2);
            *p2++ = F_27BBF(rowz, 1500, d2);
        }
    } else {
        *p11++ = F_27BBF(rowz, 0xce4, d2);
        *p2++ = F_27BBF(rowz, 1500, d2);
    }
    *p5l++ = F_27BBF(rowz, 0xe74, d2);
    *p42++ = F_27BBF(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = F_27C01(rowz, 0x76c, d1);
    else
        p6++;
    rowz = 12 - ao7d2ev.m7d32;
    k7 = 14;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p++;
        d1 = ao7d2ev.m7d3e - *af7880z;
        af7880z++;
        d2 = ao7d2ev.m7d3c - *af7560z;
        af7560z++;
        if (bits) {
            if (bits & 0x80) {
                *p11 = F_27BBF(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27BBF(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = F_27BBF(rowz, 0xce4, d2);
                p11++;
                *p2 = F_27BBF(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = F_27BBF(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27BBF(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = F_27BBF(rowz, 0xce4, d2);
            p11++;
            *p2 = F_27BBF(rowz, 1500, d2);
            p2++;
        }
        *p3 = F_27C01(rowz, 2000, d1);
        p3++;
        *p5l = F_27BBF(rowz, 0xe74, d2);
        p5l++;
        *p42 = F_27BBF(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = F_27C01(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* F_2801C @ 2645:1BCC (linear 0x2801C, 722 bytes)

   Fills the per-row projection tables (8 rows, the first one peeled) from the
   terrain pointers, walking the track backward. F_27D4B is the forward twin. */

void F_2801C(void)
{
    int *p5l, k7, bits, *p11, rowz, flag8, *p2, *p3, d1, d2, *p42, *p6;

    flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
    bits = *g7d62p-- & 0xe0;
    p11 = ay865a;
    p2 = g7f22s;
    p3 = as8638y;
    p42 = al70c0x.m70c2;
    p5l = al70c0x.m70d2;
    p6 = aj864ax;
    rowz = 8;
    d1 = *af7880z-- - ao7d2ev.m7d3e;
    d2 = *af7560z-- - ao7d2ev.m7d3c;
    *p3++ = F_27C78(rowz, 2000, d1);
    if (bits) {
        if (bits & 0x80) {
            *p11++ = F_27C3D(rowz, 0xaf0, d2);
            *p2++ = F_27C3D(rowz, 2000, d2);
        } else if (bits & 0x20) {
            *p11++ = F_27C3D(rowz, 0xce4, d2);
            *p2++ = F_27C3D(rowz, 2000, d2);
        } else {
            *p11++ = F_27C3D(rowz, 0xaf0, d2);
            *p2++ = F_27C3D(rowz, 1500, d2);
        }
    } else {
        *p11++ = F_27C3D(rowz, 0xce4, d2);
        *p2++ = F_27C3D(rowz, 1500, d2);
    }
    *p5l++ = F_27C3D(rowz, 0xe74, d2);
    *p42++ = F_27C3D(rowz, 0x44c, d2);
    if (flag8)
        *p6++ = F_27C78(rowz, 0x76c, d1);
    else
        p6++;
    rowz = ao7d2ev.m7d32 + 8;
    k7 = 7;
    do {
        flag8 = (*g7d62p & 1) || (*g7d62p & 0xf000);
        bits = *g7d62p & 0xe0;
        g7d62p--;
        d1 = *af7880z - ao7d2ev.m7d3e;
        af7880z--;
        d2 = *af7560z - ao7d2ev.m7d3c;
        af7560z--;
        if (bits) {
            if (bits & 0x80) {
                *p11 = F_27C3D(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27C3D(rowz, 2000, d2);
                p2++;
            } else if (bits & 0x20) {
                *p11 = F_27C3D(rowz, 0xce4, d2);
                p11++;
                *p2 = F_27C3D(rowz, 2000, d2);
                p2++;
            } else {
                *p11 = F_27C3D(rowz, 0xaf0, d2);
                p11++;
                *p2 = F_27C3D(rowz, 1500, d2);
                p2++;
            }
        } else {
            *p11 = F_27C3D(rowz, 0xce4, d2);
            p11++;
            *p2 = F_27C3D(rowz, 1500, d2);
            p2++;
        }
        *p3 = F_27C78(rowz, 2000, d1);
        p3++;
        *p5l = F_27C3D(rowz, 0xe74, d2);
        p5l++;
        *p42 = F_27C3D(rowz, 0x44c, d2);
        p42++;
        if (flag8)
            *p6 = F_27C78(rowz, 0x76c, d1);
        p6++;
        rowz += 8;
    } while (--k7);
}

/* F_282EE @ 2645:1E9E (linear 0x282EE, 94 bytes)

   Rebuilds the projection tables for both directions, then waits 9 ticks. */

void F_282EE(void)
{
    long deadline;

    F_27B6D(ao7d2ev.m7d30);
    F_27D4B();
    F_27B6D(ao7d2ev.m7d30);
    F_2801C();
    F_27B6D(ao7d2ev.m7d30);
    deadline = g_tick + 9;
    F_2AFDC();
    while (deadline > g_tick)
        ;
}

/* F_2834C @ 2645:1EFC (linear 0x2834C, 367 bytes) */

void F_2834C(void)
{
    int bits7, goal6, diff;

    ao7d2ev.m7d30 += ao7d2ev.m7d32 >> 3;
    F_27B6D(ao7d2ev.m7d30);
    ao7d2ev.m7d32 &= 7;
    bits7 = *g7d62p & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            g7d82p = 2000;
            cz8bd0y = 0xaf0;
        } else if (bits7 & 0x20) {
            g7d82p = 2000;
            cz8bd0y = 0xce4;
        } else {
            g7d82p = 1500;
            cz8bd0y = 0xaf0;
        }
    } else {
        g7d82p = 1500;
        cz8bd0y = 0xce4;
    }
    bits7 = g7d62p[4] & 0xe0;
    if (bits7) {
        if (bits7 & 0x80) {
            al70c0x.m70c0 = 2000;
            ao7d2ev.m7d2e = 0xaf0;
        } else if (bits7 & 0x20) {
            al70c0x.m70c0 = 2000;
            ao7d2ev.m7d2e = 0xce4;
        } else {
            al70c0x.m70c0 = 1500;
            ao7d2ev.m7d2e = 0xaf0;
        }
    } else {
        al70c0x.m70c0 = 1500;
        ao7d2ev.m7d2e = 0xce4;
    }
    ao7d2ev.m7d3e = *af7880z;
    if (g_0610 != 0 || g_0612 != 0 || (a_7eaap.m7ec0 == 0 ? 0x17c : 0x3fc) < ao7d2ev.m7d30) {
        goal6 = 0x960;
        if (ar7e8av.m7e90 > 0x960)
            goal6 -= 0x140;
        else
            goal6 += 0xa0;
        diff = ao7d2ev.m7d36 - goal6;
        if (abs(diff) > 100)
            ao7d2ev.m7d36 = goal6 + (goal6 < ao7d2ev.m7d36 ? 100 : -100);
        else
            ao7d2ev.m7d36 = goal6;
        if (abs(*af7560z - ao7d2ev.m7d3c) > 0) {
            e_78e0.f4 = (ao7d2ev.m7d3c - *af7560z) * 2;
            if (abs(e_78e0.f4) > 24)
                e_78e0.f4 = e_78e0.f4 > 0 ? 24 : -24;
        }
        ao7d2ev.m7d3c = *af7560z;
    }
}

/* F_284BB @ 2645:206B (linear 0x284BB, 169 bytes)

   Returns 1 when the car at the current terrain index would hit something. */

int F_284BB(void)
{
    g_5e0e = 0;
    if (g7d82p - 0x104 < ao7d2ev.m7d36 - 0x1e && ao7d2ev.m7d36 + 0x46 < cz8bd0y + 0x104) {
        if (g7d82p > ao7d2ev.m7d36 - 0x1e) {
            if ((g_terrain0[ao7d2ev.m7d30] & 0x1000) || (g_terrain0[ao7d2ev.m7d30 - 1] & 0x1000))
                return 1;
            if (g_terrain3[ao7d2ev.m7d30 + 1] & 0xf000)
                return 1;
            g_5e0e = 1;
        } else if (ao7d2ev.m7d36 + 0x46 > cz8bd0y) {
            if ((g_terrain1[ao7d2ev.m7d30] & 0x1000) || (g_terrain1[ao7d2ev.m7d30 - 1] & 0x1000))
                return 1;
            if (g_terrain3[ao7d2ev.m7d30 + 1] & 0xf002)
                return 1;
            g_5e0e = 1;
        }
        return 0;
    }
    return 1;
}

/* F_28564 @ 2645:2114 (linear 0x28564, 18 bytes) */

void F_28564(void)
{
    g_5e10 = g_5e12 = 0;
    F_27CAD();
    F_2B6CA();
}

/* F_28576 @ 2645:2126 (linear 0x28576, 23 bytes) */

void F_28576(int dir)
{
    if (dir == 1)
        ao7d2ev.m7d3c += 3;
    else
        ao7d2ev.m7d3c -= 3;
}

/* F_2858D @ 2645:213D (linear 0x2858D, 272 bytes) */

void F_2858D(void)
{
    if (e_78e0.f2 && e_78e0.f0 && abs(ao7d2ev.m7d3c - *af7560z - e_78e0.f2) < 200)
        ao7d2ev.m7d3c -= (e_78e0.f2 >> 1) + 1;
    if (!bw8aca) {
        if (bl82c6)
            ao7d2ev.m7d34 = 6;
        else
            ao7d2ev.m7d34 = 4;
    }
    if (ao7d2ev.m7d34 > 0 && aw7aaex > 0)
        aw7aaex -= 4;
    F_282EE();
    if (g_0610 == 0 && g_0612 == 0) {
        if (at787cu)
            ao7d2ev.m7d36 += 25;
        else if (e_78e0.f0)
            ao7d2ev.m7d36 -= (ao7d2ev.m7d3c - *af7560z) * (ao7d2ev.m7d34 + e_78e0.fc) >> 2;
    }
    if (!bw8aca) {
        ao7d2ev.m7d32 += ao7d2ev.m7d34;
        if (ao7d2ev.m7d32 >= 8)
            F_2834C();
        if ((g_terrain0[ao7d2ev.m7d30 + 7] & 1) || g_5e12) {
            g_5e12 = 1;
            F_28576(1);
        }
        if ((g_terrain1[ao7d2ev.m7d30 + 6] & 1) || g_5e10) {
            F_28576(-1);
            g_5e10 = 1;
        }
    }
}
