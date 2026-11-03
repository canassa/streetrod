#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

static void near F_2874C(struct Blt *b);
static int near F_2914C(int x1, int y1, int x2, int y2);
static int near F_2937B(int n);
static void near F_29170(int mode, int a, int b, int c, int d, int e, int f);
static void near F_2992A(int, int, int, int, int, int, int);
static void near F_29E2E(void);
static void near F_29E5D(void);
static void near F_29BFF(int, int, int, int, int, int, int);
static void near F_29EA7(void);
static void near F_29ED6(void);
static void near F_28781(struct Blt *b);
static void near F_287D1(int color, int x, int y1, int y2);
static void near F_29FC9(int x, int y, int n, int align, int clip, int mode);
static void near F_29F3B(int x, int y, int n, int f, int idx);
static void near F_28847(void);
static void near F_29F04(void);
static void near F_29F8A(int x, int y, int n, int f);
static void near F_2A11C(int x1, int x2, int y1, int y2, int n, int clip, int flags);
static void near F_2A32B(void);
static void near F_2A3DD(void);
static void near F_286D8(void);
static void near F_28DD2(void);
static void near F_29E8B(int n);
static void near F_2A213(void);
static void near F_2A28C(void);
static void near F_2A84D(void);

int g_5e14[5][4] = {{1003, 1004, 1057, 1091}, {1010, 1012, 1011, 1239}, {1010, 1247, 1248, 1092}, {1010, 1249, 1250, 1251}, {1010, 1253, 1252, 1254}};   /* DS:5E14 */
int g_5e3c = 0;   /* DS:5E3C */
int g_5e3e = 1;   /* DS:5E3E */
int g_5e40 = 2;   /* DS:5E40 */
struct Blt12 g_5e42 = {320, 82, 0, 18, 0, 18};   /* DS:5E42 */
int g_clip_xmin = 0;   /* DS:5E4E */
int g_clip_ymin = 18;   /* DS:5E50 */
int g_clip_ymax = 101;   /* DS:5E52 */
int g_clip_xmax = 319;   /* DS:5E54 */
unsigned char g_5e56[3][4] = {{87, 37, 34, 0}, {247, 117, 82, 0}, {117, 84, 33, 0}};   /* DS:5E56 */
int g_5e62[18] = {1258, 1237, 1236, 1238, 8, 6, 5, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0};   /* DS:5E62 */
int g_5e86 = 0;   /* DS:5E86 */
int g_5e88[5] = {0, 0, 0, 0, 0};   /* DS:5E88 */
int g_5e92 = 0;   /* DS:5E92 */
int g_5e94[4] = {0, 0, 0, 0};   /* DS:5E94 */
struct Ball g_5e9c[7] = {{256, 8, 8}, {240, 16, 13}, {208, 32, 19}, {168, 40, 24}, {120, 48, 30}, {64, 56, 37}, {0, 64, 43}};   /* DS:5E9C */

/* F_286D8 @ 2645:22D8 (linear 0x286D8, 116 bytes) */


static void near F_286D8(void)
{
    int t;

    if (g_vid.mode == -2) {
        t = g_5e3e;
        g_5e3e = g_5e40;
        g_5e40 = t;
        g_787a = (g_5e3e == 1) ? 0xa200 : 0xa400;
        F_2322E(g_5e40);
    } else {
        (*g_78a2)(g_8188, g_vid.front, &g_5e42, 1);
        g_787a = g_8188->bits.w.seg;
        g_767c = g_8188->bits.w.off;
    }
}

/* F_2874C @ 2645:22FC (linear 0x2874C, 53 bytes)

   Clips the vertical extent of a blit rectangle to the clip window. */


static void near F_2874C(struct Blt *b)
{
    if (b->dy < g_clip_ymin) {
        b->h -= g_clip_ymin - b->dy;
        b->sy += g_clip_ymin - b->dy;
        b->dy = g_clip_ymin;
    }
    if (g_clip_ymax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clip_ymax;
}

/* F_28781 @ 2645:2331 (linear 0x28781, 80 bytes)

   Clips a blit rectangle to the clip window, vertically and then
   horizontally. */


static void near F_28781(struct Blt *b)
{
    if (b->dy < g_clip_ymin) {
        b->h -= g_clip_ymin - b->dy;
        b->sy += g_clip_ymin - b->dy;
        b->dy = g_clip_ymin;
    }
    if (g_clip_ymax < b->dy + b->h)
        b->h -= b->dy + b->h - g_clip_ymax;
    if (b->dx < g_clip_xmin) {
        b->w -= g_clip_xmin - b->dx;
        b->sx += g_clip_xmin - b->dx + 7;
        b->dx = g_clip_xmin;
    }
}

/* F_287D1 @ 2645:2381 (linear 0x287D1, 111 bytes)

   Draws a clipped vertical line (color, x, y1, y2). */


static void near F_287D1(int color, int x, int y1, int y2)
{
    if (y2 >= g_clip_ymin && y1 <= g_clip_ymax && x >= g_clip_xmin && x <= g_clip_xmax) {
        y1 = (y1 < g_clip_ymin) ? g_clip_ymin : y1;
        y1 = (y1 > g_clip_ymax) ? g_clip_ymax : y1;
        y2 = (y2 < g_clip_ymin) ? g_clip_ymin : y2;
        y2 = (y2 > g_clip_ymax) ? g_clip_ymax : y2;
        (*g_78d2)(x, y1, x, y2, color);
    }
}

/* F_28840 @ 2645:23F0 (linear 0x28840, 7 bytes) */


void F_28840(void)
{
    g_clip_ymin = 0;
}

/* F_28847 @ 2645:23F7 (linear 0x28847, 25 bytes)

   Sets the clip window to x 242..317, y 22..42. */


static void near F_28847(void)
{
    g_clip_xmin = 0xf2;
    g_clip_ymin = 0x16;
    g_clip_xmax = 0x13d;
    g_clip_ymax = 0x2a;
}

/* F_28860 @ 2645:2410 (linear 0x28860, 25 bytes)

   Sets the clip window to x 0..319, y 18..101. */


void F_28860(void)
{
    g_clip_xmin = 0;
    g_clip_ymin = 0x12;
    g_clip_ymax = 0x65;
    g_clip_xmax = 0x13f;
}

/* F_28879 @ 2645:2429 (linear 0x28879, 1369 bytes) */


void F_28879(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *(far *load)(struct Bitmap far *, unsigned char, unsigned char);
    struct Blt pkt1;
    int sel;
    struct Bitmap far *bm24;
    int il;
    int idx;

    F_193D4();
    bm1 = F_15BBE(0x3ed, 2);
    pkt1.w = bm1->width;
    pkt1.h = bm1->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_78a2)(bm1, g_8188, &pkt1, 1);
    F_19438(1);
    sel = *(int *)&g_8aca == 0 ? g_0284 : 0;
    bm24 = F_15BBE(g_5e62[sel], 2);
    pkt1.w = bm24->width;
    pkt1.h = bm24->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0x66;
    (*g_78a2)(bm24, g_82b4, &pkt1, 1);
    F_19438(1);
    if (*(int *)&g_8aca == 0) {
        bm24 = F_15BBE(0x4ed, 2);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (g_vid.mode == -2 ? 0x63 : 0) + 1;
        (*g_78a2)(bm24, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt1, 1);
        F_19438(1);
        bm24 = F_15BBE(0x4ee, 2);
        pkt1.w = bm24->width;
        pkt1.h = bm24->height;
        pkt1.dx = pkt1.sx = pkt1.sy = 0;
        pkt1.dy = (g_vid.mode == -2 ? 0x63 : 0) + 0x2c;
        (*g_78a2)(bm24, g_vid.mode == -2 ? g_7678 : g_82b4, &pkt1, 1);
        F_19438(1);
    }
    g_767e = F_15BBE(0x4f3, 1);
    if (*(int *)&g_8aca == 0) {
        g_764a = F_15BBE(0x3f0, 1);
        pkt1.w = g_764a->width;
        pkt1.h = g_764a->height;
        pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x2c;
        pkt1.dy = 0xc;
        (*g_78aa)(g_764a, g_767e, &pkt1, 1);
        F_1941E(1);
    }
    pkt1.w = g_767e->width;
    pkt1.h = g_767e->height;
    pkt1.dx = pkt1.sx = pkt1.sy = 0;
    pkt1.dy = 0xc8 - g_767e->height;
    (*g_78a2)(g_767e, g_8188, &pkt1, 1);
    F_1941E(1);
    g_72f4 = F_15BBE(0x3ee, 1);
    g_8bca = F_15BBE(0x3ef, 1);
    g_8184 = F_15BBE(0x3f1, 1);
    g_8b7e = F_15BBE(0x4db, 1);
    if (*(int *)&g_8aca) {
        idx = g_8df0[g_7648].kind;
        g_6e60 = F_15BBE(0x3f5, 1);
        g_7aae = g_6e60->height;
        g_8ace[0] = F_15BBE(g_5e14[idx][0], 1);
        g_8ace[1] = F_15BBE(g_5e14[idx][1], 1);
        g_8ace[2] = F_15BBE(g_5e14[idx][2], 2);
        load = g_vid.mode == -2 ? F_2B857 : g_vid.mode == -6 ? F_2BCD5 : F_2BAC0;
        for (il = 0; il < 3; il++)
            g_8ace[3 + il] = (*load)(g_8ace[2], g_5e56[0][il], g_5e56[0][il]);
        for (il = 0; il < 3; il++)
            g_8ace[6 + il] = (*load)(g_8ace[5], g_5e56[1][il], g_5e56[1][il]);
        g_8ace[9] = F_15BBE(g_5e14[idx][3], 1);
        for (il = 0; il < 3; il++)
            g_8ace[10 + il] = (*load)(g_8ace[9], g_5e56[2][il], g_5e56[2][il]);
        g_7526[0] = g_8ace[0];
        g_7526[1] = F_15BBE(0x4d9, 1);
        g_7526[2] = F_15BBE(0x4da, 2);
        g_7526[2] = g_7526[3] = (*load)(g_7526[2], g_5e56[0][0], g_5e56[0][0]);
        F_19438(1);
        for (il = 0; il < 3; il++)
            g_7526[4 + il] = g_7526[3];
        g_7526[9] = F_15BBE(0x4de, 1);
        for (il = 0; il < 3; il++)
            g_7526[10 + il] = (*load)(g_7526[9], g_5e56[2][il], g_5e56[2][il]);
    }
}

/* F_28DD2 @ 2645:2982 (linear 0x28DD2, 648 bytes) */


#pragma intrinsic(abs)


static void near F_28DD2(void)
{
    int r;
    int n8 = 0xf0;
    int m5 = 1000;
    struct Blt b;
    int v;
    int ll;
    int i;

    b.dy = 0x12;
    if (*(int *)&g_8aca) {
        v = F_27C01(n8, m5 * 2, g_7d3e);
        b.h = v - 0x1e;
        b.sy = 0x9a - b.h;
        if (g_7d3c > 0) {
            r = (g_7d3c * 2 % 320) & ~7;
            ll = 320 - r;
        } else {
            ll = (abs(g_7d3c) * 2 % 320) & ~7;
            r = 320 - ll;
        }
        b.w = ll;
        b.sx = 0;
        b.dx = r;
    } else {
        b.w = 320;
        b.sx = 0;
        b.sy = 0x66;
        b.dx = 0;
        b.h = 0x1d;
    }
    (*g_78a2)(g_8188, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    if (*(int *)&g_8aca) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_78a2)(g_8188, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    }
    b.h = *(int *)&g_8aca ? 0x14 : 0x10;
    b.sy = 0xa3;
    if (*(int *)&g_8aca) {
        if (g_7d3c > 0) {
            r = (g_7d3c * 4 % 320) & ~7;
            ll = 320 - r;
        } else {
            ll = (abs(g_7d3c) * 4 % 320) & ~7;
            r = 320 - ll;
        }
        b.w = ll;
        b.sx = 0;
        b.dx = r;
        b.dy = v - 0xc;
    } else {
        b.w = 320;
        b.sx = 0;
        b.dx = 0;
        b.dy = 0x2f;
    }
    F_2874C(&b);
    (*g_78a2)(g_8188, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    g_6e06 = 14;
    if (*(int *)&g_8aca) {
        b.w = r;
        b.sx = ll;
        b.dx = 0;
        (*g_78a2)(g_8188, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
        g_6e04 = v + 8;
        for (i = 14; i; i--) {
            if (g_7884[i] < g_6e04) {
                g_6e04 = g_7884[i];
                g_6e06 = i;
            }
        }
        (*g_78da)(g_6e04, 9);
    } else
        (*g_78da)(0x3f, 1);
}

/* F_2905A @ 2645:2C0A (linear 0x2905A, 242 bytes) */


void F_2905A(int x, int y, int first, struct List l, int dir)
{
    struct Run run;
    int px;
    int x2;
    int y1;
    int j;

    if (l.last > first) {
        run = l.items[first];
        if (dir == -1) {
            for (j = 0; j < run.n; j++) {
                px = x - run.parts[j].dx;
                x2 = px - run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_78d6)(x2, px, y1, run.parts[j].h);
            }
        } else {
            for (j = 0; j < run.n; j++) {
                px = run.parts[j].dx + x;
                x2 = px + run.parts[j].w;
                y1 = y + run.parts[j].dy;
                (*g_78d6)(px, x2, y1, run.parts[j].h);
            }
        }
    }
}

/* F_2914C @ 2645:2CFC (linear 0x2914C, 36 bytes) */


static int near F_2914C(int x1, int y1, int x2, int y2)
{
    if (y2 == y1)
        return 0;
    return ((x2 - x1) << 4) / (y2 - y1);
}

/* F_29170 @ 2645:2D20 (linear 0x29170, 523 bytes) */



static void near F_29170(int col, int a, int b, int c, int d, int e, int f)
{
    int mode;
    int dxa;
    int dxb;
    int racc6;
    int laccl;
    int xr5;
    int xlk;
    int ymin2;
    int y6;
    int ymax;

    if (c == d && f == e) {
        for (y6 = f; y6 <= d; y6++)
            (*g_78d6)(a, b, y6, col);
    } else {
        ymin2 = f > e ? e : f;
        ymax = c < d ? d : c;
        if (f < e && c > d) {
            mode = 2;
            xlk = a;
            racc6 = a << 4;
        } else if (e < f && d > c) {
            mode = 3;
            xr5 = b;
            xlk = b << 4;
        } else {
            if (f <= e && c <= d) {
                mode = 4;
                racc6 = a << 4;
            } else {
                mode = 5;
                racc6 = b << 4;
            }
            laccl = racc6;
        }
        dxa = F_2914C(a, f, b, e);
        dxb = F_2914C(a, c, b, d);
        y6 = ymin2;
        goto test;
    loop:
        switch (mode) {
        case 2:
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else if (y6 <= d)
                xr5 = b;
            else
                xr5 = (racc6 += dxb) >> 4;
            break;
        case 3:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            break;
        case 4:
            if (y6 <= c)
                xlk = a;
            else
                xlk = (laccl += dxb) >> 4;
            if (y6 < e)
                xr5 = (racc6 += dxa) >> 4;
            else
                xr5 = b;
            break;
        case 5:
            if (y6 < f)
                xlk = (laccl += dxa) >> 4;
            else
                xlk = a;
            if (y6 > d)
                xr5 = (racc6 += dxb) >> 4;
            else
                xr5 = b;
            break;
        }
        if (xlk > xr5)
            (*g_78d6)(xr5, xlk, y6, col);
        else
            (*g_78d6)(xlk, xr5, y6, col);
        y6++;
    test:
        if (y6 <= ymax && y6 <= g_clip_ymax)
            goto loop;
    }
}

/* F_2937B @ 2645:2F2B (linear 0x2937B, 29 bytes) */


static int near F_2937B(int n)
{
    if (n > 0)
        return 1;
    if (n < 0)
        return -1;
    return 0;
}

/* F_29398 @ 2645:2F48 (linear 0x29398, 773 bytes) */



void F_29398(int n)
{
    int xl;
    int ybot6;
    int xr28;
    int yk;
    int wid;
    int cnt;
    int m17;
    int dxr7;
    int t1;
    int xlsn;
    int xr2s6;
    int dxl2k;
    int dxlm;
    int flagk;
    int dxr26;
    int t2;
    int xl2;
    int last2;
    int xrl;
    int cx0k;
    int xrs;
    int xl2s6;
    int m2;

    xl = *g_6e08;
    xrl = *g_6e18;
    xlsn = xl << 4;
    xrs = xrl << 4;
    yk = *g_6e1a;
    ybot6 = *g_6e1e;
    dxlm = F_2914C(*g_6e10, ybot6, xl, yk);
    dxr7 = F_2914C(*g_6e1c, ybot6, xrl, yk);
    flagk = *g_8b82 & 1;
    if (flagk) {
        xl2 = *g_6e0a;
        xr28 = *g_6e0e;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = F_2914C(*g_6e12, ybot6, xl2, yk);
        dxr26 = F_2914C(*g_6e16, ybot6, xr28, yk);
    }
    if (*g_8b82 & 8) {
        wid = *g_6e28;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (F_2937B(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 101; cnt--, yk++) {
        if (flagk) {
            if (*g_8b82 & 4)
                break;
            if (*g_6e20 & 0x80) {
                m17 = xl2 + ((xl - xl2) >> 1);
                (*g_78d6)(xl2, m17, yk, 4);
                (*g_78d6)(m17, xl, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xl2, m17, yk, 2);
            } else {
                (*g_78d6)(xl2, xl, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xl2, xl, yk, 2);
            }
            if (*g_6e24 & 0x80) {
                m2 = xr28 - ((xr28 - xrl) >> 1);
                (*g_78d6)(m2, xr28, yk, 4);
                (*g_78d6)(xrl, m2, yk, 9);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(m2, xr28, yk, 2);
            } else {
                (*g_78d6)(xrl, xr28, yk, 4);
                if (cnt == last2 && n < 6)
                    (*g_78d6)(xrl, xr28, yk, 2);
            }
        } else {
            (*g_78d6)(xl, xrl, yk, (*g_8b82 & 0x10) ? 15 : 1);
        }
        if (*g_8b82 & 8) {
            t1 = cx0k;
            t2 = t1 + wid;
            (*g_78d6)(t1, t2, yk, 4);
        }
        xlsn += dxlm;
        xrs += dxr7;
        xl = xlsn >> 4;
        xrl = xrs >> 4;
        if (flagk) {
            xl2s6 += dxl2k;
            xr2s6 += dxr26;
            xl2 = xl2s6 >> 4;
            xr28 = xr2s6 >> 4;
        }
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
}

/* F_2969D @ 2645:324D (linear 0x2969D, 653 bytes) */



void F_2969D(void)
{
    int xl;
    int ybot6;
    int xr28;
    int yk;
    int wid;
    int cnt;
    int m17;
    int dxr7;
    int t1;
    int xlsn;
    int xr2s6;
    int dxl2k;
    int dxlm;
    int flagk;
    int dxr26;
    int t2;
    int xl2;
    int xrl;
    int cx0k;
    int xrs;
    int xl2s6;
    int m2;

    xl = *g_6e08;
    xrl = *g_6e18;
    xlsn = xl << 4;
    xrs = xrl << 4;
    yk = *g_6e1a;
    ybot6 = *g_6e1e;
    dxlm = F_2914C(*g_6e10, ybot6, xl, yk);
    dxr7 = F_2914C(*g_6e1c, ybot6, xrl, yk);
    flagk = *g_8b82 & 1;
    if (flagk) {
        xl2 = *g_6e0a;
        xr28 = *g_6e0e;
        xl2s6 = xl2 << 4;
        xr2s6 = xr28 << 4;
        dxl2k = F_2914C(*g_6e12, ybot6, xl2, yk);
        dxr26 = F_2914C(*g_6e16, ybot6, xr28, yk);
    }
    if (*g_8b82 & 8) {
        wid = *g_6e28;
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
    if (F_2937B(ybot6 - yk) < 0)
        return;
    for (cnt = ybot6 - yk; cnt && yk <= 42; cnt--, yk++) {
        if (flagk) {
            if (*g_8b82 & 4)
                break;
            if (*g_6e20 & 0x80) {
                m17 = xl2 + ((xl - xl2) >> 1);
                (*g_78d6)(xl2, m17, yk, 4);
                (*g_78d6)(m17, xl, yk, 9);
            } else {
                (*g_78d6)(xl2, xl, yk, 4);
            }
            if (*g_6e24 & 0x80) {
                m2 = xr28 - ((xr28 - xrl) >> 1);
                (*g_78d6)(m2, xr28, yk, 4);
                (*g_78d6)(xrl, m2, yk, 9);
            } else {
                (*g_78d6)(xrl, xr28, yk, 4);
            }
        } else {
            (*g_78d6)(242, xl, yk, 9);
            (*g_78d6)(xrl, 317, yk, 9);
            (*g_78d6)(xl, xrl, yk, 1);
        }
        if (*g_8b82 & 8) {
            t1 = cx0k;
            t2 = t1 + wid;
            (*g_78d6)(t1, t2, yk, 4);
        }
        xlsn += dxlm;
        xrs += dxr7;
        xl = xlsn >> 4;
        xrl = xrs >> 4;
        if (flagk) {
            xl2s6 += dxl2k;
            xr2s6 += dxr26;
            xl2 = xl2s6 >> 4;
            xr28 = xr2s6 >> 4;
        }
        cx0k = ((xl + xrl) >> 1) - (wid >> 1);
    }
}

/* F_2992A @ 2645:34DA (linear 0x2992A, 725 bytes) */



static void near F_2992A(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b8l;
    int b4;
    int t8l;
    int y;

    t8l = *g_8b82 & 1;
    if (rev) {
        a41 = g_6e20[0] & 4;
        b8l = g_6e20[0] & 8;
        b4 = g_6e20[-1] & 4;
        c10 = g_6e20[0] & 0x10;
    } else {
        a41 = g_6e24[0] & 4;
        b8l = g_6e24[0] & 8;
        b4 = g_6e24[-1] & 4;
        c10 = g_6e24[0] & 0x10;
    }
    if (!(*g_8b82 & 4)) {
        if (c10)
            F_29170(2, a, b, c, d, e, f);
        else
            F_29170(3, a, b, c, d, e, f);
        if (a41) {
            if (b8l) {
                F_29170(4, a, b, f, e, e - d + e, f - c + f);
            } else {
                F_29170(4, a, b, f, e, e - ((d - e) >> 1), f - ((c - f) >> 1));
            }
        }
        if (g_8b82[-1] & 4) {
            if (rev)
                (*g_78d6)(g_clip_xmin, a, c - 1, 4);
            else
                (*g_78d6)(b, g_clip_xmax, d - 1, 4);
            if (rev) {
                for (y = f; y < c - 1; y++)
                    (*g_78d6)(g_clip_xmin, a, y, 2);
            } else {
                for (y = e; y < d - 1; y++)
                    (*g_78d6)(b, g_clip_xmax, y, 2);
            }
        }
        if (a41 && !b4) {
            int x1;
            int x2;

            if (b8l) {
                if (rev) {
                    y = f - c + f;
                    if (y < f) {
                        x1 = a - b + a;
                        x2 = a - 1;
                        do
                            (*g_78d6)(x1, x2, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - d + e;
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            } else {
                if (rev) {
                    y = f - ((c - f) >> 1);
                    if (y < f) {
                        x2 = a - b + a;
                        x1 = a - 1;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - ((d - e) >> 1);
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            }
        }
    }
}

/* F_29BFF @ 2645:37AF (linear 0x29BFF, 559 bytes) */



static void near F_29BFF(int a, int b, int c, int d, int e, int f, int rev)
{
    int c10;
    int a41;
    int b4;
    int t8l;
    int y;

    if (t8l = *g_8b82 & 1) {
        if (rev) {
            a41 = g_6e20[0] & 4;
            b4 = g_6e20[1] & 4;
            c10 = g_6e20[0] & 0x10;
        } else {
            a41 = g_6e24[0] & 4;
            b4 = g_6e24[1] & 4;
            c10 = g_6e24[0] & 0x10;
        }
        if (!(*g_8b82 & 4)) {
            if (c10)
                F_29170(2, a, b, c, d, e, f);
            else
                F_29170(3, a, b, c, d, e, f);
            if (a41) {
                if (rev)
                    F_29170(4, a, b, f, e, e - ((d - e) >> 1), f - ((c - f) >> 1));
                else
                    F_29170(4, a, b, f, e, e - d + e, f - c + f);
            }
            if (g_8b82[1] & 4) {
                if (rev)
                    (*g_78d6)(g_clip_xmin, a, c - 1, 4);
                else
                    (*g_78d6)(b, g_clip_xmax, d - 1, 4);
                if (rev) {
                    for (y = f; y < c - 1; y++)
                        (*g_78d6)(g_clip_xmin, a, y, 2);
                } else {
                    for (y = e; y < d - 1; y++)
                        (*g_78d6)(b, g_clip_xmax, y, 2);
                }
            }
            if (a41 && !b4) {
                int x1;
                int x2;

                if (rev) {
                    y = f - ((c - f) >> 1);
                    if (y < f) {
                        x1 = a - b + a;
                        x2 = a - 1;
                        do
                            (*g_78d6)(x1, x2, y, 2);
                        while (++y < f);
                    }
                } else {
                    y = e - d + e;
                    if (y < e) {
                        x2 = b + 1;
                        x1 = b * 2 - a;
                        do
                            (*g_78d6)(x2, x1, y, 2);
                        while (++y < e);
                    }
                }
            }
        }
    }
}

/* F_29E2E @ (linear 0x29E2E) */



static void near F_29E2E(void)
{
    F_2992A(*g_6e12, *g_6e0a, *g_6e1e, *g_6e1a, *g_6e0c, *g_6e14, 1);
}

/* F_29E5D @ (linear 0x29E5D) */



static void near F_29E5D(void)
{
    F_2992A(*g_6e0e, *g_6e16, *g_6e1a, *g_6e1e, *g_6e14, *g_6e0c, 0);
}

/* F_29E8B @ 2645:3A3B (linear 0x29E8B, 28 bytes) */



static void near F_29E8B(int n)
{
    if (*(int *)&g_8aca == 0) {
        F_29E2E();
        F_29E5D();
    }
    F_29398(n);
}

/* F_29EA7 @ (linear 0x29EA7) */



static void near F_29EA7(void)
{
    F_29BFF(*g_6e12, *g_6e0a, *g_6e1e, *g_6e1a, *g_6e0c, *g_6e14, 1);
}

/* F_29ED6 @ (linear 0x29ED6) */



static void near F_29ED6(void)
{
    F_29BFF(*g_6e0e, *g_6e16, *g_6e1a, *g_6e1e, *g_6e14, *g_6e0c, 0);
}

/* F_29F04 @ 2645:3AB4 (linear 0x29F04, 11 bytes) */



static void near F_29F04(void)
{
    F_29EA7();
    F_29ED6();
    F_2969D();
}

/* F_29F0F @ 2645:3ABF (linear 0x29F0F, 44 bytes) */


void F_29F0F(int x, int y)
{
    F_26458();
    F_2905A(x, y, 0, *g_6078[20], 1);
    F_2646D();
}

/* F_29F3B @ 2645:3AEB (linear 0x29F3B, 79 bytes) */


static void near F_29F3B(int x, int y, int n, int f, int idx)
{
    n = 9 - n;
    if (n >= 7)
        n = 6;
    if (n < 0)
        n = 0;
    if (y > 0x65)
        y = 0x65;
    F_2905A(x, y, n, *g_6078[idx], f);
}

/* F_29F8A @ 2645:3B3A (linear 0x29F8A, 63 bytes) */


static void near F_29F8A(int x, int y, int n, int f)
{
    n = 9 - n;
    if (n >= 8)
        n = 7;
    if (n < 0)
        n = 0;
    F_2905A(x, y, n, *g_6078[2], f);
}

/* F_29FC9 @ 2645:3B79 (linear 0x29FC9, 339 bytes) */



static void near F_29FC9(int x, int y, int n, int align, int clip, int mode)
{
    struct Blt b;

    n = 9 - n;
    if (n > 6)
        n = 6;
    if (n < 0)
        n = 0;
    b.w = g_5e9c[n].w;
    b.h = g_5e9c[n].h;
    b.sx = g_5e9c[n].sx;
    switch (mode) {
    case 1:
        b.sy = 0x66;
        break;
    case 2:
        b.sy = (g_vid.mode == -2 ? 0x63 : 0) + 1;
        break;
    case 3:
        b.sy = (g_vid.mode == -2 ? 0x63 : 0) + 0x2c;
        break;
    }
    if (align == 1)
        b.dx = x - b.w;
    else if (align == 0)
        b.dx = x - (b.w >> 1);
    else
        b.dx = x;
    if (align == 0)
        b.dy = y - (b.h >> 1);
    else
        b.dy = y - b.h;
    if (clip)
        F_28781(&b);
    else
        F_2874C(&b);
    if (mode == 1)
        (*g_78a2)(g_82b4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    else
        (*g_78a2)(g_vid.mode == -2 ? g_7678 : g_82b4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}

/* F_2A11C @ 2645:3CCC (linear 0x2A11C, 247 bytes) */



static void near F_2A11C(int x1, int x2, int y1, int y2, int n, int clip, int flags)
{
    int j;

    for (j = *g_6e28 + 1; j; j--) {
        F_2646D();
        F_287D1(10, x1 - j, y1, y2);
        F_287D1(10, x2 + j, y1, y2);
        F_26458();
        (*g_78d6)(x1, x2, y1 + j - 1, 10);
    }
    if (flags & 0x1000)
        F_29F3B((x1 + x2) >> 1, y1, n, 1, 13);
    else if (flags & 0x2000) {
        F_2646D();
        F_29FC9((x1 + x2) >> 1, y1, n, 0, clip, 1);
        F_26458();
    } else if (*g_8b82 & 1) {
        F_2646D();
        F_29FC9((x1 + x2) >> 1, y1, n, 0, clip, (flags & 0x4000) ? 2 : 3);
        F_26458();
    } else
        F_29F3B((x1 + x2) >> 1, y1, n, 1, (flags & 0x4000) ? 12 : 14);
}

/* F_2A213 @ 2645:3DC3 (linear 0x2A213, 121 bytes) */


static void near F_2A213(void)
{
    struct Blt b;

    if (g_7aae > 0) {
        b.w = 0x18;
        b.h = g_7aae;
        if (g_7d66) {
            g_8228 = 1;
            b.sx = 0;
        } else
            b.sx = 0x18;
        b.sy = 0x20 - g_7aae;
        b.dx = 0x82;
        b.dy = 0x12;
        (*g_78aa)(g_6e60, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    }
}

/* F_2A28C @ 2645:3E3C (linear 0x2A28C, 159 bytes) */


static void near F_2A28C(void)
{
    struct Blt b;

    b.w = g_72f4->width;
    b.h = g_72f4->height;
    b.dx = b.sx = b.sy = 0;
    b.dy = 0x12;
    (*g_78aa)(g_72f4, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    b.w = g_8bca->width;
    b.h = g_8bca->height;
    b.sx = b.sy = 0;
    b.dx = 0x58;
    b.dy = 0x5f;
    (*g_78aa)(g_8bca, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}

/* F_2A32B @ 2645:3EDB (linear 0x2A32B, 178 bytes) */



static void near F_2A32B(void)
{
    struct Blt b;

    b.w = g_8184->width;
    b.h = g_8184->height;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = 0x12;
    (*g_78a2)(g_8184, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    b.w = g_8b7e->width;
    b.h = 2;
    b.sx = b.sy = 0;
    b.dx = 0xf0;
    b.dy = g_8646 - 2;
    F_2874C(&b);
    (*g_78a2)(g_8b7e, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
}

/* F_2A3DD @ 2645:3F8D (linear 0x2A3DD, 1136 bytes) */


#pragma intrinsic(abs)


static void near F_2A3DD(void)
{
    struct Blt b;
    int seln;
    int ra;
    int flag9;
    int z9;
    int a2;
    int xk;
    int rb;
    int t1;
    int rc;
    int rd;
    struct Bitmap far *bm;

    a2 = g_7e8a;
    xk = (a2 - g_7d30) * 8 + g_7e8c - g_7d32;
    z9 = g_7e90 - g_7d36;
    if (xk >= 0x78)
        seln = -1;
    else if (xk >= 0x10) {
        if ((seln = ((xk + 8) >> 4) + 2) > 8)
            seln = 8;
    } else if (xk < -7)
        seln = ((0x10 - xk) >> 4) + 8;
    else if (xk <= 8) {
        if (abs(z9) < 180) {
            if (g_7d7c <= 8)
                seln = 2;
            else
                seln = 9;
        } else
            seln = z9 < 0 ? 0 : 2;
    } else if (z9 < -180)
        seln = 1;
    else
        seln = 3;
    if (g_8acc >= 4 && seln != g_7d7c) {
        if (seln < 0) {
            if (g_7d7c == 8)
                return;
            seln = g_7d7c + 1;
        } else if (seln > 12) {
            if (g_7d7c == 12)
                return;
            seln = g_7d7c + 1;
        } else if (seln != g_6e2a) {
            if ((seln <= 8 && g_7d7c > 9) || (seln >= 9 && g_7d7c <= 8 && g_7d7c > 0))
                seln = g_7d7c - 1;
            else if (g_7d7c == 0)
                seln = seln < 9 ? 1 : 9;
            else if (g_7d7c == 9)
                seln = seln < 9 ? (z9 < 0 ? 0 : 2) : 10;
            else if (g_7d7c == 1)
                seln = seln < 1 ? 0 : 3;
            else if (g_7d7c == 3 && seln == 1)
                ;
            else if (seln > g_7d7c)
                seln = g_7d7c + 1;
            else
                seln = g_7d7c - 1;
        }
    }
    g_8bc2 = seln == 0 || (seln == 2 && xk <= 10);
    g_72f2 = seln == 2 && xk <= 10 && abs(z9) < 180 &&
        (g_8acc < 4 ? 100 : 180) > abs(z9) && g_7e8e < g_7d34;
    g_6e2a = g_7d7c;
    g_7d7c = seln;
    g_5ec6 = xk > 16 ? (xk + 8) & 15 : 0;
    t1 = xk;
    if (seln == 0)
        t1 += 16;
    if (t1 < -2 && seln == 2)
        t1 += 8;
    rc = g_7d3e - g_ter56f0[a2];
    rd = g_7d3c - g_ter4570[a2];
    if (seln < 9) {
        t1 += 4;
        ra = F_27BBF(t1, g_7e90, rd);
        rb = F_27C01(t1, 2000, rc);
    } else {
        t1 = -t1;
        ra = F_27C3D(t1, g_7e90, -rd);
        rb = F_27C78(t1, 2000, -rc);
    }
    if (seln == 0) {
        bm = g_8ace[seln];
        b.w = bm->width;
        b.h = bm->height;
        if ((b.sx = b.w - ra + 0x40) < 0)
            b.sx = 0;
        b.dx = 0;
        b.sy = 0;
        b.dy = rb;
    } else if (seln == 1) {
        bm = g_8ace[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - b.w;
        b.sy = 0;
        b.dy = rb - b.h + 0x11;
    } else {
        bm = g_8ace[seln];
        b.w = bm->width;
        b.h = bm->height;
        b.sx = 0;
        b.dx = ra - (b.w >> 1);
        b.sy = 0;
        b.dy = (seln < 9 ? (rb < g_6e04 ? g_6e04 : rb) : rb) - b.h;
        if (seln == 2 && b.dy + b.h >= 0x65)
            b.dy = 0x65 - b.h;
    }
    flag9 = 0;
    if (seln >= 9) {
        if (g_clip_xmin == 0) {
            flag9 = 1;
            F_28847();
        }
    } else if (g_clip_xmin != 0) {
        flag9 = 1;
        F_28860();
    }
    if (b.dx + b.w >= g_clip_xmin && b.dx < g_clip_xmax) {
        if (seln >= 9)
            F_28781(&b);
        else
            F_2874C(&b);
        (*g_78aa)(g_8ace[seln], g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
        if (flag9) {
            if (g_clip_xmin == 0)
                F_28847();
            else
                F_28860();
        }
    }
}

/* F_2A84D @ 2645:43FD (linear 0x2A84D, 1935 bytes) */



static void near F_2A84D(void)
{
    int w36;
    struct Blt pkt1;
    int w11;
    int i;
    int f3;
    int w2;

    g_6e08 = &g_7f30;
    g_6e18 = &g_8668;
    g_6e1a = &g_8646;
    g_6e0a = &g_70d0;
    g_6e0e = &g_70e0;
    g_6e0c = &g_8658;
    g_6e10 = &g_7f2e;
    g_6e1c = &g_8666;
    g_6e1e = &g_8644;
    g_6e14 = &g_8656;
    g_6e12 = &g_70ce;
    g_6e16 = &g_70de;
    g_6e28 = &g_5e92;
    g_6e20 = (int far *)(g_72ee - 8);
    g_6e24 = (int far *)(g_8634 - 8);
    g_8b82 = (int far *)(g_7d62 - 8);
    F_2646D();
    F_2A32B();
    F_26458();
    F_28847();
    for (i = 7; i; i--) {
        F_29F04();
        if (*g_8b82 & 4)
            goto next;
        if (*g_8b82 & 0xf000)
            F_2A11C(*g_6e08, *g_6e18, -((*g_6e1a - *g_6e0c) * 4 - *g_6e1a), *g_6e1a, i + 5, 1, *g_8b82 & 0xf000);
        if (i >= 6)
            goto next;
        f3 = *g_8b82 & 1;
        w11 = *g_6e20;
        w2 = *g_6e24;
        w36 = *g_8b82;
        if (w11 & 0x20) {
            if (f3)
                F_29F3B(*g_6e08, *g_6e1a, i + 5, -1, 0);
            else
                F_29F3B(*g_6e0a, *g_6e1a, i + 5, -1, 0);
        }
        if (w11 & 0x100) {
            if (f3)
                F_29F3B(*g_6e08, *g_6e1a, i + 5, -1, 4);
            else if (w11 & 0x200)
                F_29F3B(*g_6e0a - *g_6e08 + *g_6e0a, *g_6e1a, i + 5, -1, 4);
            else
                F_29F3B(*g_6e0a, *g_6e1a, i + 5, -1, 4);
        }
        if (w11 & 0x800)
            F_29F3B(*g_6e08, *g_6e1a, i + 5, 1, 3);
        if (w11 & 0x1000)
            F_29F3B(*g_6e08, *g_6e1a, i + 5, 1, 6);
        if (f3) {
            if (w11 & 0x40)
                F_29F8A(*g_6e08, *g_6e1a, i + 5, 1);
            if (w2 & 0x40)
                F_29F8A(*g_6e18, *g_6e1a, i + 5, -1);
        } else {
            if (w11 & 1)
                F_29F3B(*g_6e08, *g_6e1a, i + 5, 1, 1);
            if (w11 & 4)
                F_29F3B(*g_6e08, *g_6e1a, i + 5, -1, 7);
            if (w11 & 8)
                F_29F3B(*g_6e08, *g_6e1a, i + 5, -1, 8);
            if (w11 & 0x40)
                F_29F3B(-(((*g_6e08 - *g_6e0a) >> 1) - *g_6e0a), *g_6e1a, i + 5, -1, 10);
            if (w11 & 0x80)
                F_29F3B(*g_6e0a, *g_6e1a, i + 5, -1, 11);
            if (w2 & 1)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 1);
            if (w2 & 4)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 7);
            if (w2 & 8)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 8);
            if (w2 & 0x40)
                F_29F3B(*g_6e0e, *g_6e1a, i + 5, 1, 10);
            if (w2 & 0x80)
                F_29F3B(((*g_6e0e - *g_6e18) >> 1) + *g_6e0e, *g_6e1a, i + 5, 1, 11);
            if (w36 & 2)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 9);
        }
        if (w2 & 0x20) {
            if (f3)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0);
            else
                F_29F3B(*g_6e0e, *g_6e1a, i + 5, 1, 0);
        }
        if (w2 & 0x100) {
            if (f3)
                F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 4);
            else if (w2 & 0x200)
                F_29F3B((*g_6e0e << 1) - *g_6e18, *g_6e1a, i + 5, 1, 4);
            else
                F_29F3B(*g_6e0e, *g_6e1a, i + 5, 1, 4);
        }
        if (w2 & 0x2000)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0x12);
        if (w2 & 0x4000)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0x13);
        if (w2 & 0x800)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 3);
        if (w2 & 0x1000)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, -1, 6);
        if (w36 & 0x100)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 5);
        if (w36 & 0x200)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0x10);
        if (w36 & 0x400)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0x11);
        if (w36 & 0x800)
            F_29F3B(*g_6e18, *g_6e1a, i + 5, 1, 0xf);
    next:
        if (g_7d30 - g_7e8a == i && *(int *)&g_8aca) {
            F_2646D();
            F_2A3DD();
            F_26458();
        }
        g_6e10--;
        g_6e1c--;
        g_6e1e--;
        g_6e08--;
        g_6e18--;
        g_6e1a--;
        g_6e28--;
        g_6e20++;
        g_6e24++;
        g_8b82++;
        g_6e0a--;
        g_6e0e--;
        g_6e0c--;
        g_6e12--;
        g_6e16--;
        g_6e14--;
    }
    F_28860();
    F_287D1(2, 0xf0, 0x17, 0x2a);
    F_287D1(4, 0xf1, 0x17, 0x2a);
    F_287D1(4, 0x13e, 0x17, 0x2a);
    F_287D1(2, 0x13f, 0x17, 0x2a);
    pkt1.w = 0x20;
    pkt1.h = g_8184->height - 6;
    pkt1.sx = 0xf8;
    pkt1.sy = 0x16;
    pkt1.dx = 8;
    pkt1.dy = (g_vid.mode == -2 ? 0 : 0x64) + 3;
    (*g_78a2)(g_5e3e == 1 ? g_8188 : g_82b4, g_7678, &pkt1, 1);
}

/* F_2AFDC @ 2645:4B8C (linear 0x2AFDC, 1774 bytes) */



void F_2AFDC(void)
{
    int w36;
    int w11;
    int i;
    int f3;
    int w2;

    g_6e08 = &g_7314;
    g_6e18 = &g_7aea;
    g_6e1a = &g_78a0;
    g_6e0a = &g_7fa6;
    g_6e0e = &g_7ff4;
    g_6e0c = &g_7acc;
    g_6e10 = &g_7312;
    g_6e1c = &g_7ae8;
    g_6e1e = &g_789e;
    g_6e14 = &g_7aca;
    g_6e12 = &g_7fa4;
    g_6e16 = &g_7ff2;
    g_6e28 = &g_5e86;
    g_6e20 = (int far *)(g_72ee + 14);
    g_6e24 = (int far *)(g_8634 + 14);
    g_8b82 = (int far *)(g_7d62 + 14);
    F_28860();
    F_28DD2();
    F_26458();
    for (i = 14; i; i--) {
        if (i <= g_6e06)
            F_29E8B(i);
        if (*g_8b82 & 4)
            goto next;
        if (i >= 11)
            goto next;
        f3 = *g_8b82 & 1;
        w11 = *g_6e20;
        w2 = *g_6e24;
        w36 = *g_8b82;
        if (w11 & 0x20) {
            if (f3)
                F_29F3B(*g_6e08, *g_6e1a, i, -1, 0);
            else
                F_29F3B(*g_6e0a, *g_6e1a, i, -1, 0);
        }
        if (w11 & 0x100) {
            if (f3)
                F_29F3B(*g_6e08, *g_6e1a, i, -1, 4);
            else if (w11 & 0x200)
                F_29F3B(*g_6e0a - *g_6e08 + *g_6e0a, *g_6e1a, i, -1, 4);
            else
                F_29F3B(*g_6e0a, *g_6e1a, i, -1, 4);
        }
        if (w11 & 0x800)
            F_29F3B(*g_6e08, *g_6e1a, i, 1, 3);
        if (w11 & 0x1000)
            F_29F3B(*g_6e08, *g_6e1a, i, 1, 6);
        if (w11 & 1) {
            if (f3) {
                F_2646D();
                F_29FC9(*g_6e08, (*g_6e0c + *g_6e1a) >> 1, i, 1, 0, 1);
                F_26458();
            } else
                F_29F3B(*g_6e08, *g_6e1a, i, 1, 1);
        }
        if (f3) {
            if (w11 & 0x40)
                F_29F8A(*g_6e08, *g_6e1a, i, 1);
            if (w2 & 0x40)
                F_29F8A(*g_6e18, *g_6e1a, i, -1);
        } else {
            if (w11 & 4)
                F_29F3B(*g_6e08, *g_6e1a, i, -1, 7);
            if (w11 & 8)
                F_29F3B(*g_6e08, *g_6e1a, i, -1, 8);
            if (w11 & 0x40)
                F_29F3B(-(((*g_6e08 - *g_6e0a) >> 1) - *g_6e0a), *g_6e1a, i, -1, 10);
            if (w11 & 0x80)
                F_29F3B(*g_6e0a, *g_6e1a, i, -1, 11);
            if (w2 & 4)
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 7);
            if (w2 & 8)
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 8);
            if (w2 & 0x40)
                F_29F3B(*g_6e0e, *g_6e1a, i, 1, 10);
            if (w2 & 0x80)
                F_29F3B(((*g_6e0e - *g_6e18) >> 1) + *g_6e0e, *g_6e1a, i, 1, 11);
            if (w36 & 2)
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 9);
        }
        if (w2 & 0x20) {
            if (f3)
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 0);
            else
                F_29F3B(*g_6e0e, *g_6e1a, i, 1, 0);
        }
        if (w2 & 0x100) {
            if (f3)
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 4);
            else if (w2 & 0x200)
                F_29F3B((*g_6e0e << 1) - *g_6e18, *g_6e1a, i, 1, 4);
            else
                F_29F3B(*g_6e0e, *g_6e1a, i, 1, 4);
        }
        if (w2 & 0x2000)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 0x12);
        if (w2 & 0x4000)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 0x13);
        if (w2 & 0x800)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 3);
        if (w2 & 0x1000)
            F_29F3B(*g_6e18, *g_6e1a, i, -1, 6);
        if (w2 & 1) {
            if (f3) {
                F_2646D();
                F_29FC9(*g_6e18, (*g_6e0c + *g_6e1a) >> 1, i, -1, 0, 1);
                F_26458();
            } else
                F_29F3B(*g_6e18, *g_6e1a, i, 1, 1);
        }
        if (w36 & 0xf000)
            F_2A11C(*g_6e08, *g_6e18, -((*g_6e1a - *g_6e0c) * 4 - *g_6e1a), *g_6e1a, i, 0, w36 & 0xf000);
        if (w36 & 0x100)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 5);
        if (w36 & 0x800)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 0xf);
        if (w36 & 0x200)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 0x10);
        if (w36 & 0x400)
            F_29F3B(*g_6e18, *g_6e1a, i, 1, 0x11);
    next:
        if (g_7e8a - g_7d30 == i && *(int *)&g_8aca) {
            F_2646D();
            F_2A3DD();
            F_26458();
        }
        g_6e10--;
        g_6e1c--;
        g_6e1e--;
        g_6e08--;
        g_6e18--;
        g_6e1a--;
        g_6e28--;
        g_6e20--;
        g_6e24--;
        g_8b82--;
        g_6e0a--;
        g_6e0e--;
        g_6e0c--;
        g_6e12--;
        g_6e16--;
        g_6e14--;
    }
    if (g_7e8a == g_7d30 && *(int *)&g_8aca) {
        F_2646D();
        F_2A3DD();
        F_26458();
    }
    F_2A84D();
    F_2646D();
    F_2A28C();
    if (*(int *)&g_8aca)
        F_2A213();
    F_286D8();
}
