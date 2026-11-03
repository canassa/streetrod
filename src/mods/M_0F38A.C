#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <stdlib.h>
#include <math.h>
#include <conio.h>

static void near F_0F418(int x, int y, int w, int h, int kind);
static void near F_0F38A(int x, int y, int w, int h, int kind);

int g_02d2 = -1;   /* DS:02D2 */
char g_02d4[18] = {0, 8, 7, 15, 14, 1, 9, 11, 0, 2, 4, 12, 13, 10, 5, 6, 0, 0};   /* DS:02D4 */
struct Bitmap far *g_02e6 = 0;   /* DS:02E6 */
struct Bitmap far *g_02ea = 0;   /* DS:02EA */
signed char g_02ee[24][2] = {{87, 69}, {88, 64}, {90, 58}, {91, 54}, {93, 49}, {95, 45}, {98, 40}, {101, 36}, {104, 33}, {108, 29}, {111, 26}, {114, 23}, {118, 20}, {121, 19}, {125, 16}, {-127, 14}, {-122, 12}, {-117, 11}, {-113, 9}, {-108, 8}, {-104, 7}, {-99, 7}, {-95, 6}, {-91, 6}};   /* DS:02EE */
unsigned char g_031e[25][2] = {{170, 6}, {175, 6}, {179, 6}, {183, 7}, {188, 7}, {192, 8}, {197, 9}, {201, 11}, {206, 12}, {211, 14}, {215, 16}, {219, 19}, {222, 20}, {226, 23}, {229, 26}, {232, 29}, {236, 33}, {239, 36}, {242, 40}, {245, 45}, {247, 49}, {249, 54}, {250, 58}, {252, 64}, {253, 69}};   /* DS:031E */
int g_0350 = 0;   /* DS:0350 */
int g_0352[3] = {148, 174, 73};   /* DS:0352 */
signed char g_0358[11][2] = {{14, 17}, {14, 28}, {13, 12}, {13, 2}, {13, 20}, {22, 2}, {13, 12}, {13, 2}, {13, 20}, {22, 2}, {22, 20}};   /* DS:0358 */
struct Track g_036e[3] = {{2, 1229, 1232, 8, g_0358}, {4, 1227, 1230, 19, g_0358[2]}, {5, 1228, 1231, 19, g_0358[6]}};   /* DS:036E */
struct Pair g_038c[5] = {{0, 162}, {32, 162}, {64, 162}, {96, 162}, {264, 102}};   /* DS:038C */

/* F_0F38A @ 0F38:000A (linear 0x0F38A, 142 bytes) */


static void near F_0F38A(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = 0x108;
    pkt.sy = (g_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    pkt.dx = x & 0xfff8;
    pkt.dy = y;
    (*g_78a2)(g_vid.mode == -2 ? g_7678 : g_82b4, g_7678, &pkt, 1);
}

/* F_0F418 @ 0E9F:0A28 (linear 0x0F418, 159 bytes) */


static void near F_0F418(int x, int y, int w, int h, int kind)
{
    struct Blt pkt;

    pkt.w = (w & 0xfff8) + 0x10;
    pkt.h = h;
    pkt.sx = x & 0xfff8;
    pkt.sy = y;
    pkt.dx = 0x108;
    pkt.dy = (g_vid.mode == -2 ? 100 : 0) + (kind == 1 ? 5 : kind == 2 ? 12 : 35);
    (*g_78a2)(g_58e4 ? g_755c : g_7678, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt, 1);
}

/* F_0F4B7 @ 0F38:0137 (linear 0x0F4B7, 155 bytes) */


void F_0F4B7(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > 0x82)
        i = 0x82;
    *x1 = g_2e47_0000[0][i][0] + 0xc4;
    *y1 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[0][i][1] + 0x2a;
    *x2 = g_2e47_0000[1][i][0] + 0xc4;
    *y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[1][i][1] + 0x2a;
}

/* F_0F552 @ 0F38:01D2 (linear 0x0F552, 220 bytes) */

#pragma intrinsic(abs)


void F_0F552(int i)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = (g_vid.mode == -2 || g_vid.mode == -6) ? 11 : 3;
    F_0F4B7(i, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    if (g_58e4 == 0) {
        saveSeg = g_787a;
        saveOff = g_767c;
        g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
        g_767c = 0;
        (*g_78d2)(x11, y1, x2, y2, color);
        g_787a = saveSeg;
        g_767c = saveOff;
    }
}

/* F_0F62E @ 0E9F:0C3E (linear 0x0F62E, 155 bytes) */


void F_0F62E(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > 0x82)
        i = 0x82;
    *x1 = g_2e47_0000[0][i][0] + 0x91;
    *y1 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[0][i][1] + 0x2a;
    *x2 = g_2e47_0000[1][i][0] + 0x91;
    *y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[1][i][1] + 0x2a;
}

/* F_0F6C9 @ 0F38:0349 (linear 0x0F6C9, 220 bytes) */

#pragma intrinsic(abs)


void F_0F6C9(int v)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = g_vid.mode == -2 ? 11 : 3;
    F_0F62E(v / 0x2b, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    if (g_58e4 == 0) {
        saveSeg = g_787a;
        saveOff = g_767c;
        g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
        g_767c = 0;
        (*g_78d2)(x11, y1, x2, y2, color);
        g_787a = saveSeg;
        g_767c = saveOff;
    }
}

/* F_0F7A5 @ 0F38:0425 (linear 0x0F7A5) */

#pragma intrinsic(abs)


int F_0F7A5(int cur, int old, int flag)
{
    int miny3, minx4, y1, x2, x11, y2;
    unsigned saveOff, saveSeg;

    if (g_02d2 < 0)
        g_02d2 = (g_vid.mode == -2 || g_vid.mode == -6) ? 11 : 3;
    saveSeg = g_787a;
    saveOff = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
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
    (*g_78d2)(x11, y1, x2, y2, g_02d2);
    g_787a = saveSeg;
    g_767c = saveOff;
    return cur;
}

/* F_0F972 @ 0F38:05F2 (linear 0x0F972, 1339 bytes)

   Draws the clock: the hour hand (width 5 or 8) and the minute hand (width 4 or 8), each a
   diamond of five lines around the face centre, from the time F_064A3 returns. */

#pragma intrinsic(abs, cos, sin)

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
        g_02d2 = (g_vid.mode == -2 || g_vid.mode == -6) ? 11 : 3;
    ssegk = g_787a;
    soffn = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
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
    y13 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 13.0) + 0x91;
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 13.0) + 0x2a;
    x3 = (int)(cos((al + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((al - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((al - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_78d2)(x14, y13, x2, y2, g_02d2);
    (*g_78d2)(x14, y13, x3, y3, g_02d2);
    (*g_78d2)(x14, y13, x4, y4, g_02d2);
    (*g_78d2)(x2, y2, x3, y3, g_02d2);
    (*g_78d2)(x2, y2, x4, y4, g_02d2);
    a2 = 0x5a - 6 * m;
    rk = abs(a2 - a2 / 0x5a * 0x5a);
    if (rk < 10 || rk > 0x50)
        w6 = 8;
    else
        w6 = 4;
    rad5 = a2 * pi3 / 180.0;
    x14 = (int)(cos(rad5) * 5.0) + 0x91;
    y13 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 5.0) + 0x2a;
    x2 = (int)(cos(rad5) * 18.0) + 0x91;
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin(rad5) * 18.0) + 0x2a;
    x3 = (int)(cos((a2 + w6) * pi3 / 180.0) * 9.0) + 0x91;
    y3 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 + w6) * pi3 / 180.0) * 9.0) + 0x2a;
    x4 = (int)(cos((a2 - w6) * pi3 / 180.0) * 9.0) + 0x91;
    y4 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - (int)(sin((a2 - w6) * pi3 / 180.0) * 9.0) + 0x2a;
    (*g_78d2)(x14, y13, x2, y2, g_02d2);
    (*g_78d2)(x14, y13, x3, y3, g_02d2);
    (*g_78d2)(x14, y13, x4, y4, g_02d2);
    (*g_78d2)(x2, y2, x3, y3, g_02d2);
    (*g_78d2)(x2, y2, x4, y4, g_02d2);
    g_787a = ssegk;
    g_767c = soffn;
}

void F_0FEAD(void)
{
    F_11324(g_0452);
    g_8249 = 0;
}

void F_0FEBF(void)
{
    F_11324(g_palette);
    g_8249 = 1;
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
            if (g_5ee)
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
    if (g_vid.mode == -2 || g_vid.mode == -6)
        (*g_78be)();
    g_04c2 = 200;
    if (g_vid.mode != -2 && g_vid.mode != -6)
        F_001F2(0);
    F_119B8(3, 0x28, 0x28, 0);
    F_158DE(3);
    (*g_present)();
    g_vid.f16 = 0;
    F_118D4(1, -1);
    F_158DE(1);
    F_0FED1(400);
    (*g_present)();
    if (g_vid.mode != -2 && g_vid.mode != -6)
        F_001F2(1);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_118D4(4, 0);
    F_158DE(4);
    F_0FED1(400);
    if (g_vid.mode == -2 || g_vid.mode == -6)
        F_11324(g_02d4);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_0FED1(400);
    g_04c2 = saved;
    F_1169A(0, 0, 320, 200, 0);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
}

/* F_100E2 @ 0F38:0D62 (linear 0x100E2, 91 bytes) */


void F_100E2(void)
{
    F_03AB8(0x17bb);
    (*g_78be)();
    F_120FE(-4);
    g_vid.f16 = 1;
    F_1169A(0, 0, 320, 200, 0);
    F_065AF(1);
    F_10B7C();
    (*g_present)();
    F_03A83(300);
}

/* F_1013D @ 0F38:0DBD (linear 0x1013D, 406 bytes) */


#pragma intrinsic(outp)

void F_1013D(int mode)
{
    switch (mode) {
    case 0:
        if (!g_02ea) {
            g_02ea = g_vid.front;
            g_02e6 = g_vid.back;
        }
        g_004c = 1;
        break;
    case 1:
        g_vid.front = g_02ea;
        g_vid.back = g_02e6;
        if (g_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            F_21A14(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        F_113A2(0xbe);
        break;
    case 2:
        if (g_vid.mode == -2) {
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        break;
    case 3:
        g_vid.back = g_vid.front = g_02ea;
        g_vid.f16 = 0;
        F_113A2(g_vid.mode == -2 ? 0x5a : 0xbe);
        if (g_8aca)
            F_11593(g_49de);
        break;
    case 4:
        g_vid.front = g_02ea;
        g_vid.back = g_02e6;
        if (g_vid.mode == -2) {
            outp(0x3d4, 0x13);
            outp(0x3d5, 0x14);
            F_21A14(-1);
            outp(0x3d4, 0xc);
            outp(0x3d5, 0);
            outp(0x3d4, 0xd);
            outp(0x3d5, 0);
        }
        g_vid.f16 = 1;
        (*g_present)();
        break;
    }
}

/* F_102D3 @ 0F38:0F53 (linear 0x102D3, 321 bytes) */


void F_102D3(void)
{
    int i6;
    struct Bitmap far *bm;
    struct Blt pkt;

    if (g_vid.mode != -2) {
        pkt.w = 0xb8;
        pkt.h = 0x1a;
        pkt.sx = pkt.sy = 0;
        pkt.dx = 0x88;
        for (i6 = 0; i6 < 3; i6++) {
            bm = F_15BBE(g_58e8[i6], 1);
            if (g_vid.mode != -2 || i6 < 2) {
                g_6c28[i6] = g_82b4;
                g_6c22[i6] = g_0352[i6];
            } else {
                g_6c28[i6] = g_7678;
                g_6c22[i6] = g_0352[i6] + 0x66;
            }
            pkt.dy = g_6c22[i6];
            (*g_78a2)(bm, g_6c28[i6], &pkt, 1);
            F_1941E(1);
        }
    } else {
        for (i6 = 0; i6 < 3; i6++)
            g_6c28[i6] = F_15BBE(g_58e8[i6], 1);
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

    seg = g_787a;
    off = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa200 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
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
    g_787a = seg;
    g_767c = off;
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
        if (g_vid.mode != -2) {
            pkt1.sx = 0x88;
            pkt1.sy = g_6c22[sel];
        } else
            pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x50;
        pkt1.dy = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x3f;
        (*g_78a2)(g_6c28[sel], dst, &pkt1, 1);
        g_0350 = sel;
    }
    if (b != 100) {
        x = g_031e[b][0];
        y2 = (g_vid.mode == -2 ? 0 : 100) + g_031e[b][1];
        F_0F38A(x - 2, y2 - 2, 5, 5, 1);
    }
    x = g_031e[a][0];
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + g_031e[a][1];
    F_0F418(x - 2, y2 - 2, 5, 5, 1);
    if (g_58e4 == 0) {
        seg = g_787a;
        off = g_767c;
        g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
        g_767c = 0;
        F_28840();
        if (g_vid.mode != -2) {
            ymax = g_clip_ymax;
            ymin2 = g_clip_ymin;
            g_clip_ymax = 190;
            g_clip_ymin = 100;
        }
        F_29F0F(x, y2);
        if (g_vid.mode != -2) {
            g_clip_ymax = ymax;
            g_clip_ymin = ymin2;
        }
        F_28860();
        g_787a = seg;
        g_767c = off;
    }
}
