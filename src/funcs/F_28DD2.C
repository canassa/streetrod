/* F_28DD2 @ 2645:2982 (linear 0x28DD2, 648 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(abs)

static void near F_2874C(struct Blt *b);

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
