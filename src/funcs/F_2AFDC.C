/* F_2AFDC @ 2645:4B8C (linear 0x2AFDC, 1774 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_286D8(void);
static void near F_28DD2(void);
static void near F_29E8B(int n);
static void near F_29F3B(int x, int y, int n, int f, int idx);
static void near F_29F8A(int x, int y, int n, int f);
static void near F_29FC9(int x, int y, int n, int align, int clip, int mode);
static void near F_2A11C(int x1, int x2, int y1, int y2, int n, int clip, int flags);
static void near F_2A213(void);
static void near F_2A28C(void);
static void near F_2A3DD(void);
static void near F_2A84D(void);

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
