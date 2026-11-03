/* F_2A3DD @ 2645:3F8D (linear 0x2A3DD, 1136 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(abs)

static void near F_28781(struct Blt *b);
static void near F_2874C(struct Blt *b);
static void near F_28847(void);

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
