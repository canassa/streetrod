/* F_2A84D @ 2645:43FD (linear 0x2A84D, 1935 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_287D1(int color, int x, int y1, int y2);
static void near F_28847(void);
static void near F_29F04(void);
static void near F_29F3B(int x, int y, int n, int f, int idx);
static void near F_29F8A(int x, int y, int n, int f);
static void near F_2A11C(int x1, int x2, int y1, int y2, int n, int clip, int flags);
static void near F_2A32B(void);
static void near F_2A3DD(void);

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
