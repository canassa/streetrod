#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "STRUCTS.H"
#include "M_0F38A.H"
#include "BITMAP.H"
#include "CRTC.H"
#include "FONTDATA.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_02DB6.H"
#include "M_02DF4.H"
#include "M_039C0.H"
#include "M_06476.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11E9C.H"
#include "M_15396.H"
#include "M_163BE.H"
#include "M_16D1A.H"
#include "M_174A6.H"
#include "M_193D4.H"
#include "M_2869E.H"
#include "TRACKPTS.H"

#pragma intrinsic(abs, cos, sin, outp, inp)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_6c22[3];                      /* DS:6C22 */
static struct Bitmap far *g_6c28gzz[3];    /* DS:6C28 */
static void near F_0F418(int x, int y, int w, int h, int kind);
static void near F_0F38A(int x, int y, int w, int h, int kind);
static struct Track *g_6c34;               /* DS:6C34 */
static int g_6c36;                         /* DS:6C36 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap far *bm7678;
unsigned at767ct;
unsigned av787at;
int (far *b_78a2)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
void (far *bl78be)(void);
void (far *m_present)(void);
void (far *s_blit)(char far *, char far *);
void (far *a_78d2)(int, int, int, int, int);
char gz8249;
struct Bitmap far *g82b4s;
char bw8aca;

int g_02d2 = -1;   /* DS:02D2 */
char g_02d4[18] = {0, 8, 7, 15, 14, 1, 9, 11, 0, 2, 4, 12, 13, 10, 5, 6, 0, 0};   /* DS:02D4 */
struct Bitmap far *g_02e6 = 0;   /* DS:02E6 */
struct Bitmap far *g_02ea = 0;   /* DS:02EA */
signed char g_02ee[24][2] = {{87, 69}, {88, 64}, {90, 58}, {91, 54}, {93, 49}, {95, 45}, {98, 40}, {101, 36}, {104, 33}, {108, 29}, {111, 26}, {114, 23}, {118, 20}, {121, 19}, {125, 16}, {-127, 14}, {-122, 12}, {-117, 11}, {-113, 9}, {-108, 8}, {-104, 7}, {-99, 7}, {-95, 6}, {-91, 6}};   /* DS:02EE */
unsigned char g_031e[25][2] = {{170, 6}, {175, 6}, {179, 6}, {183, 7}, {188, 7}, {192, 8}, {197, 9}, {201, 11}, {206, 12}, {211, 14}, {215, 16}, {219, 19}, {222, 20}, {226, 23}, {229, 26}, {232, 29}, {236, 33}, {239, 36}, {242, 40}, {245, 45}, {247, 49}, {249, 54}, {250, 58}, {252, 64}, {253, 69}};   /* DS:031E */
int g_0350 = 0;   /* DS:0350 */
int g_0352[3] = {148, 174, 73};   /* DS:0352 */
signed char g_0358[11][2] = {{14, 17}, {14, 28}, {13, 12}, {13, 2}, {13, 20}, {22, 2}, {13, 12}, {13, 2}, {13, 20}, {22, 2}, {22, 20}};   /* DS:0358 */
struct Track g_036e[3] = {{2, 1229, 1232, 8, g_0358[0]}, {4, 1227, 1230, 19, g_0358[2]}, {5, 1228, 1231, 19, g_0358[6]}};   /* DS:036E */
struct Pair g_038c[5] = {{0, 162}, {32, 162}, {64, 162}, {96, 162}, {264, 102}};   /* DS:038C */

/* F_0F38A @ 0F38:000A (linear 0x0F38A, 142 bytes) */

static void near F_0F38A(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = 0x108;
    pkt.sy = (r_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    pkt.dx = x & 0xfff8;
    pkt.dy = y;
    (*b_78a2)(r_vid.mode == -2 ? bm7678 : g82b4s, bm7678, &pkt, 1);
}

/* F_0F418 @ 0F38:0098 (linear 0x0F418, 159 bytes) */

static void near F_0F418(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = x & 0xfff8;
    pkt.sy = y;
    pkt.dx = 0x108;
    pkt.dy = (r_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    (*b_78a2)(g_58e4 ? g755cu : bm7678, r_vid.mode == -2 ? bm7678 : g82b4s, &pkt, 1);
}

/* F_0F4B7 @ 0F38:0137 (linear 0x0F4B7, 155 bytes) */

void F_0F4B7(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > 0x82)
        i = 0x82;
    *x1 = g_2e47_0000[0][i][0] + 0xc4;
    *y1 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[0][i][1] + 0x2a;
    *x2 = g_2e47_0000[1][i][0] + 0xc4;
    *y2 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[1][i][1] + 0x2a;
    /* PROVISIONAL: the original CONST holds two more SEG constants for the track segment
       (DS:68E6 with this function's, DS:68E8 later) that no instruction reads: MSC leaves
       them behind for unreachable far accesses to two other far symbols. The statements and
       symbol names are not recoverable; these keep the bytes and the relocation order. The
       block-scope extern is part of that provisional text (the symbols are labels in
       TRACKPTS.ASM; see docs/RECONSTRUCTION.md). */
    return;
    { extern char far x_trk0[][2]; *x1 = x_trk0[i][0]; }
}

/* F_0F552 @ 0F38:01D2 (linear 0x0F552, 220 bytes) */

void F_0F552(int i)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = (r_vid.mode == -2 || r_vid.mode == -6) ? 11 : 3;
    F_0F4B7(i, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    if (g_58e4 == 0) {
        saveSeg = av787at;
        saveOff = at767ct;
        av787at = r_vid.mode == -2 ? 0xa000 : r_vid.mode == -4 ? 0xb000 : 0xb800;
        at767ct = 0;
        (*a_78d2)(x11, y1, x2, y2, color);
        av787at = saveSeg;
        at767ct = saveOff;
    }
}

/* F_0F62E @ 0F38:02AE (linear 0x0F62E, 155 bytes) */

void F_0F62E(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > 0x82)
        i = 0x82;
    *x1 = g_2e47_0000[0][i][0] + 0x91;
    *y1 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[0][i][1] + 0x2a;
    *x2 = g_2e47_0000[1][i][0] + 0x91;
    *y2 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[1][i][1] + 0x2a;
    /* PROVISIONAL: second unreachable far access, see F_0F4B7. */
    return;
    { extern char far x_trk1[][2]; *x2 = x_trk1[i][0]; }
}

/* F_0F6C9 @ 0F38:0349 (linear 0x0F6C9, 220 bytes) */

void F_0F6C9(int v)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = r_vid.mode == -2 ? 11 : 3;
    F_0F62E(v / 0x2b, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    if (g_58e4 == 0) {
        saveSeg = av787at;
        saveOff = at767ct;
        av787at = r_vid.mode == -2 ? 0xa000 : r_vid.mode == -4 ? 0xb000 : 0xb800;
        at767ct = 0;
        (*a_78d2)(x11, y1, x2, y2, color);
        av787at = saveSeg;
        at767ct = saveOff;
    }
}

/* F_0F7A5 @ 0F38:0425 (linear 0x0F7A5, 461 bytes) */

int F_0F7A5(int cur, int old, int flag)
{
    int miny3, minx4, y1, x2, x11, y2;
    unsigned saveOff, saveSeg;

    if (g_02d2 < 0)
        g_02d2 = (r_vid.mode == -2 || r_vid.mode == -6) ? 11 : 3;
    saveSeg = av787at;
    saveOff = at767ct;
    av787at = r_vid.mode == -2 ? 0xa000 : r_vid.mode == -4 ? 0xb000 : 0xb800;
    at767ct = 0;
    if (flag) {
        F_0F62E(old / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F38A(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    } else {
        F_0F4B7(old, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F38A(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    }
    if (flag) {
        F_0F62E(cur / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    } else {
        F_0F4B7(cur, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    }
    (*a_78d2)(x11, y1, x2, y2, g_02d2);
    av787at = saveSeg;
    at767ct = saveOff;
    return cur;
}

/* F_0F972 @ 0F38:05F2 (linear 0x0F972, 1339 bytes)

   Draws the clock: the hour hand (width 5 or 8) and the minute hand (width 4 or 8), each a
   diamond of five lines around the face centre, from the time F_064A3 returns. */

void F_0F972(void)
{
    int a2, x2, y2, x3, x4;
    double pi3;
    unsigned soffn;
    int h4;
    double rad5;
    int y3, m, w6;
    unsigned ssegk;
    int y13, al, x14, y4, rk;

    pi3 = 3.1415926535;
    if (g_02d2 < 0)
        g_02d2 = (r_vid.mode == -2 || r_vid.mode == -6) ? 11 : 3;
    ssegk = av787at;
    soffn = at767ct;
    av787at = r_vid.mode == -2 ? 0xa000 : r_vid.mode == -4 ? 0xb000 : 0xb800;
    at767ct = 0;
    F_064A3(&h4, &m);
    h4 >>= 1;
    h4 += 0x14;
    h4 = h4 % 12 + 1;
    al = 0x5a - (30 * h4 + (m >> 1));
    rk = abs(al - al / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 5;
    rad5 = al * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 13.0) + 0x91;
    y2 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 13.0) + 0x2a;
    x3 = (int)(cos((al + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((al - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*a_78d2)(x14, y13, x2, y2, g_02d2);
    (*a_78d2)(x14, y13, x3, y3, g_02d2);
    (*a_78d2)(x14, y13, x4, y4, g_02d2);
    (*a_78d2)(x2, y2, x3, y3, g_02d2);
    (*a_78d2)(x2, y2, x4, y4, g_02d2);
    a2 = 0x5a - 6 * m;
    rk = abs(a2 - a2 / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 4;
    rad5 = a2 * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 18.0) + 0x91;
    y2 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 18.0) + 0x2a;
    x3 = (int)(cos((a2 + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((a2 - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*a_78d2)(x14, y13, x2, y2, g_02d2);
    (*a_78d2)(x14, y13, x3, y3, g_02d2);
    (*a_78d2)(x14, y13, x4, y4, g_02d2);
    (*a_78d2)(x2, y2, x3, y3, g_02d2);
    (*a_78d2)(x2, y2, x4, y4, g_02d2);
    av787at = ssegk;
    at767ct = soffn;
}

/* F_0FEAD @ 0F38:0B2D (linear 0x0FEAD, 18 bytes) */

void F_0FEAD(void)
{
    F_11324(g_0452);
    gz8249 = 0;
}

/* F_0FEBF @ 0F38:0B3F (linear 0x0FEBF, 18 bytes) */

void F_0FEBF(void)
{
    F_11324(g_palette);
    gz8249 = 1;
}

/* F_0FED1 @ 0F38:0B51 (linear 0x0FED1, 159 bytes) */

void F_0FED1(int ticks)
{
    long end;
    char key;

    end = g_tick + ticks;
    while (end > g_tick) {
        if (g_4736) {
            g_4736 = 0;
            break;
        }
        if (g_05f2) {
            F_16D57();
            if (g_05ee)
                break;
        }
        if (g_471d) {
            F_02DF4();
            if (g_471c)
                break;
        }
        key = F_02DB6();
        if (key == 0x4d) {
            if (g_58d0) {
                F_00D99();
                g_58d0 = 0;
            } else {
                g_58d0 = 1;
                F_00DD8();
            }
        } else if (key == 0xd || key == 0x1b)
            break;
    }
}

/* F_0FF70 @ 0F38:0BF0 (linear 0x0FF70, 370 bytes) */

void F_0FF70(void)
{
    int saved;

    saved = g_04c2;
    if (r_vid.mode == -2 || r_vid.mode == -6)
        (*bl78be)();
    g_04c2 = 200;
    if (r_vid.mode != -2 && r_vid.mode != -6)
        F_001F2(0);
    F_119B8(3, 0x28, 0x28, 0);
    F_158DE(3);
    (*m_present)();
    r_vid.f16 = 0;
    F_118D4(1, -1);
    F_158DE(1);
    F_0FED1(400);
    (*m_present)();
    if (r_vid.mode != -2 && r_vid.mode != -6)
        F_001F2(1);
    (*s_blit)(r_vid.front->bits.p, r_vid.back->bits.p);
    F_118D4(4, 0);
    F_158DE(4);
    F_0FED1(400);
    if (r_vid.mode == -2 || r_vid.mode == -6)
        F_11324(g_02d4);
    (*s_blit)(r_vid.front->bits.p, r_vid.back->bits.p);
    F_0FED1(400);
    g_04c2 = saved;
    F_1169A(0, 0, 320, 200, 0);
    (*s_blit)(r_vid.front->bits.p, r_vid.back->bits.p);
}

/* F_100E2 @ 0F38:0D62 (linear 0x100E2, 91 bytes) */

void F_100E2(void)
{
    F_03AB8(0x17bb);
    (*bl78be)();
    F_120FE(-4);
    r_vid.f16 = 1;
    F_1169A(0, 0, 320, 200, 0);
    F_065AF(1);
    F_10B7C();
    (*m_present)();
    F_03A83(300);
}

/* F_1013D @ 0F38:0DBD (linear 0x1013D, 406 bytes) */

void F_1013D(int mode)
{
    switch (mode) {
    case 0:
        if (!g_02ea) {
            g_02ea = r_vid.front;
            g_02e6 = r_vid.back;
        }
        g_004c = 1;
        break;
    case 1:
        r_vid.front = g_02ea;
        r_vid.back = g_02e6;
        if (r_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            crtc_set_split(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        r_vid.f16 = 1;
        F_113A2(0xbe);
        break;
    case 2:
        if (r_vid.mode == -2) {
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        r_vid.f16 = 1;
        break;
    case 3:
        r_vid.back = r_vid.front = g_02ea;
        r_vid.f16 = 0;
        F_113A2(r_vid.mode == -2 ? 0x5a : 0xbe);
        if (bw8aca)
            F_11593(g_49de);
        break;
    case 4:
        r_vid.front = g_02ea;
        r_vid.back = g_02e6;
        if (r_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            crtc_set_split(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        r_vid.f16 = 1;
        (*m_present)();
        break;
    }
}

/* F_102D3 @ 0F38:0F53 (linear 0x102D3, 321 bytes) */

void F_102D3(void)
{
    int i6;
    struct Bitmap far *bm;
    struct Blt pkt;

    if (r_vid.mode != -2) {
        pkt.w = 0xb8;
        pkt.h = 0x1a;
        pkt.sx = pkt.sy = 0;
        pkt.dx = 0x88;
        for (i6 = 0; i6 < 3; i6++) {
            bm = F_15BBE(g_58e8[i6], 1);
            if (r_vid.mode != -2 || i6 < 2) {
                g_6c28gzz[i6] = g82b4s;
                g_6c22[i6] = g_0352[i6];
            } else {
                g_6c28gzz[i6] = bm7678;
                g_6c22[i6] = g_0352[i6] + 0x66;
            }
            pkt.dy = g_6c22[i6];
            (*b_78a2)(bm, g_6c28gzz[i6], &pkt, 1);
            F_1941E(1);
        }
    } else {
        for (i6 = 0; i6 < 3; i6++)
            g_6c28gzz[i6] = F_15BBE(g_58e8[i6], 1);
    }
}

/* F_10414 @ 0F38:1094 (linear 0x10414, 156 bytes) */

void F_10414(int n)
{
    int x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    seg = av787at;
    off = at767ct;
    av787at = r_vid.mode == -2 ? 0xa200 : r_vid.mode == -4 ? 0xb000 : 0xb800;
    at767ct = 0;
    F_28840();
    x = g_031e[n][0];
    y2 = g_031e[n][1] + 100;
    ymax = g_clip_ymax;
    ymin2 = g_clip_ymin;
    g_clip_ymax = 190;
    g_clip_ymin = 100;
    F_29F0F(x, y2);
    g_clip_ymax = ymax;
    g_clip_ymin = ymin2;
    F_28860();
    av787at = seg;
    at767ct = off;
}

/* F_104B0 @ 0F38:1130 (linear 0x104B0, 455 bytes) */

void F_104B0(int a, int b, struct Bitmap far *dst)
{
    struct Blt pkt1;
    int sel, x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    sel = (a != 0) + (a > 0);
    if (sel < 3 && sel != g_0350) {
        pkt1.w = 0xb8;
        pkt1.h = 0x1a;
        if (r_vid.mode != -2) {
            pkt1.sx = 0x88;
            pkt1.sy = g_6c22[sel];
        } else
            pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x50;
        pkt1.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x3f;
        (*b_78a2)(g_6c28gzz[sel], dst, &pkt1, 1);
        g_0350 = sel;
    }
    if (b != 100) {
        x = g_031e[b][0];
        y2 = (r_vid.mode == -2 ? 0 : 100) + g_031e[b][1];
        F_0F38A(x - 2, y2 - 2, 5, 5, 1);
    }
    x = g_031e[a][0];
    y2 = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + g_031e[a][1];
    F_0F418(x - 2, y2 - 2, 5, 5, 1);
    if (g_58e4 == 0) {
        seg = av787at;
        off = at767ct;
        av787at = r_vid.mode == -2 ? 0xa000 : r_vid.mode == -4 ? 0xb000 : 0xb800;
        at767ct = 0;
        F_28840();
        if (r_vid.mode != -2) {
            ymax = g_clip_ymax;
            ymin2 = g_clip_ymin;
            g_clip_ymax = 190;
            g_clip_ymin = 100;
        }
        F_29F0F(x, y2);
        if (r_vid.mode != -2) {
            g_clip_ymax = ymax;
            g_clip_ymin = ymin2;
        }
        F_28860();
        av787at = seg;
        at767ct = off;
    }
}

/* F_10677 @ 0F38:12F7 (linear 0x10677, 507 bytes)

   Draws the car sprite at the top of the screen and, for each of the g_6c34->n points of the
   track, the wheel sprite (the position comes from the (x, y) pair, the source rectangle from
   g_038c). F_10872 is the per-frame update that draws the same pieces. */

void F_10677(int a, int b)
{
    struct Blt pk5;
    struct Bitmap far *cm2;
    struct Blt p2;
    struct Blt p3;
    int x;
    struct Bitmap far *bm19;
    int i;
    struct Bitmap far *bm28;
    int y;

    g_6c34 = &g_036e[a ? 0 : b == 3 ? 1 : 2];
    bm19 = F_15BBE(g_6c34->id1, 2);
    pk5.w = 0x30;
    pk5.h = 0x2b;
    pk5.sx = pk5.sy = 0;
    pk5.dx = 0x108;
    pk5.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x17;
    (*b_78a2)(bm19, g_58e4 ? g755cu : bm7678, &pk5, 1);
    bm28 = F_15BBE(g_6c34->id2, 2);
    p3.w = p2.w = (pk5.w = bm28->width) + 8;
    p3.h = p2.h = pk5.h = bm28->height;
    p3.sx = p3.sy = 0;
    p2.dx = p2.dy = 0;
    pk5.sx = pk5.sy = pk5.dy = 0;
    for (i = 0; i < g_6c34->n; i++) {
        x = g_6c34->pt[i * 2];
        y = g_6c34->pt[i * 2 + 1];
        p2.sx = x & 0xfff8;
        p2.sy = y;
        cm2 = bitmap_grab(bm19, &p2, 1);
        p3.dx = g_038c[i].a;
        p3.dy = g_038c[i].b;
        (*b_78a2)(cm2, g82b4s, &p3, 1);
        pk5.dx = x - p2.sx;
        bitmap_draw_masked(bm28, cm2, &pk5, 0);
        p3.dy += 0x13;
        (*b_78a2)(cm2, g82b4s, &p3, 1);
        F_1941E(1);
    }
    g_6c36 = -1;
    F_19438(2);
}

/* F_10872 @ 0F38:14F2 (linear 0x10872, 334 bytes) */

void F_10872(struct Bitmap far *dst, struct Eng *e)
{
    struct Blt16 pkt1;
    int x, idx3;

    if (e->f24 == 0)
        idx3 = e->f18;
    else if (e->f18 == 0)
        idx3 = 0;
    else
        idx3 = e->f24 - 1;
    pkt1.h = g_6c34->h;
    if (g_6c36 >= 0) {
        pkt1.sx = g_038c[g_6c36].a;
        pkt1.sy = g_038c[g_6c36].b;
        x = g_6c34->pt[g_6c36 * 2] & 0xfff8;
        pkt1.dx = x + 0x108;
        pkt1.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[g_6c36 * 2 + 1] + 0x17;
        pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
        (*b_78a2)(g82b4s, dst, (struct Blt *)&pkt1, 1);
    }
    pkt1.sx = g_038c[idx3].a;
    pkt1.sy = g_038c[idx3].b + 0x13;
    x = g_6c34->pt[idx3 * 2] & 0xfff8;
    pkt1.dx = x + 0x108;
    pkt1.dy = (r_vid.mode == -2 || g_58e4 ? 0 : 100) + g_6c34->pt[idx3 * 2 + 1] + 0x17;
    pkt1.w = 0x30 - x > 0x20 ? 0x20 : 0x30 - x;
    (*b_78a2)(g82b4s, dst, (struct Blt *)&pkt1, 1);
    g_6c36 = idx3;
    e->f1a = 0;
}

/* F_109C0 @ 0F38:1640 (linear 0x109C0, 64 bytes) */

void F_109C0(char far *p, int c, unsigned n)
{
    char far *q;

    for (q = p; n; n--)
        *q++ = c;
}

/* F_10A00 @ 0F38:1680 (linear 0x10A00, 253 bytes)

   Clips the rectangle in to the destination bitmap, then to the
   source bitmap; the result goes to out. Returns 1 if anything is left. The x values of in
   are shifted down by r_vid.shift. */

int F_10A00(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out)
{
    int t1, t2;
    int w1, h;
    int sx, sy, dx, dy;

    sx = in->sx >> r_vid.shift;
    sy = in->sy;
    w1 = in->w >> r_vid.shift;
    h = in->h;
    if (sx + w1 > dst->pitch)
        w1 = dst->pitch - sx;
    if (sy + h > dst->height)
        h = dst->height - sy;
    dx = in->dx >> r_vid.shift;
    dy = in->dy;
    if (dy < 0) {
        h += dy;
        sy -= dy;
        dy = 0;
    }
    if ((t1 = src->height) < dy + h)
        h = t1 - dy;
    if (dx < 0) {
        w1 += dx;
        sx -= dx;
        dx = 0;
    }
    if ((t2 = src->pitch) < dx + w1)
        w1 = t2 - dx;
    out->sx = sx;
    out->sy = sy;
    out->dx = dx;
    out->dy = dy;
    out->w = w1;
    out->h = h;
    return out->w > 0 && out->h > 0;
}

/* F_10AFD @ 0F38:177D (linear 0x10AFD, 75 bytes) */

void F_10AFD(int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (r_vid.mode != -4) {
            while (inp(0x3da) & 8)
                ;
            while (!(inp(0x3da) & 8))
                ;
        } else {
            while (!(inp(0x3ba) & 0x80))
                ;
            while (inp(0x3ba) & 0x80)
                ;
        }
    }
}

/* F_10B48 @ 0F38:17C8 (linear 0x10B48, 52 bytes) */

int F_10B48(char *s)
{
    int w;

    for (w = 0; *s; s++)
        w += g_font[*s - 32].w;
    return w;
}
