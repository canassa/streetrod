#include "STRUCTS.H"
#include "M_265BA.H"
#include "M_00000.H"
#include "M_0F38A.H"
#include "M_15236.H"
#include "M_16EA2.H"
#include "SCREEN2.H"
#include "SCREENS.H"
#include "SLOTS.H"

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
