#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0D29D(void);
static void near F_0D624(void);
static void near F_0D67D(void);
static void near F_0D992(void);

/* F_0DA25 @ 0000:DA25 (linear 0x0DA25, 664 bytes) */



void F_0DA25(void)
{
    int a1, b, c1, d;

    if (g_78e0.f24) {
        if (g_78e0.f24 != g_8630)
            F_11669((g_8630 = g_78e0.f24) + 4);
    } else if (g_78e0.f18 != g_8630)
        F_11669(g_8630 = g_78e0.f18);
    if (g_78ea && g_7d30 > g_78ea)
        g_787c = 1;
    if (g_58da == 0) {
        if (abs(g_78e0.f4) >= 12)
            g_58da = 1;
    } else if (abs(g_78e0.f4) < 10)
        g_58da = 0;
    F_0D29D();
    switch (g_7f88) {
    case 1:
        if (g_7d30 >= g_755a) {
            if (g_7e8a + 5 <= g_7d30) {
                g_7f88++;
                F_0D992();
            } else
                g_7f88 = 0;
        }
        break;
    case 2:
        if (g_7e8a + 15 >= g_7d30) {
            g_7f88++;
            F_16AA8(1);
        }
        break;
    case 3:
        if (g_7e8a - 2 > g_7d30) {
            g_78e0.fbc |= 0x41;
            F_02374();
            return;
        }
        break;
    }
    a1 = g_7d30 * 8 + g_7d32;
    b = g_7e8a * 8 + g_7e8c;
    c1 = g_7d36;
    d = g_7e90;
    F_0D624();
    F_0D67D();
    if (g_7d30 >= g_6caa || g_7e8a >= g_6caa)
        F_0C407();
    if (g_78e0.fbc || g_79e2.fbc) {
        if (g_7f88 >= 2) {
            if (g_78e0.fbc)
                g_78e0.fbc |= 0x41;
            F_16AA8(0);
        }
        F_02374();
        return;
    }
    if (g_8bc2 == 0)
        return;
    if (g_0610)
        return;
    if ((g_7eaa.m7ec0 == 0 ? 0x17c : 0x3fc) <= g_7d30)
        return;
    if (g_72f2 && g_7f88 <= 1) {
        g_78e0.fbc |= 3;
        F_02374();
        return;
    }
    if (g_7d7c == 0 && g_7d34 > 1) {
        g_54d8++;
        if (g_78e0.f2 < 0) {
            if (g_terrain2[g_7d30] - g_terrain2[g_7d30 + 1] >= 0 || ((g_70c0 + g_7d2e) >> 1) >= g_7d36) {
                F_16C8F();
                if (g_54d2 == 0) {
                    g_7e8a--;
                    g_54d2 = 1;
                } else
                    g_54d2 = 0;
            }
            return;
        }
        if (g_7d7e && g_54d8 > 3 && g_7f88 < 2) {
            if (random_int((g_7d84->f2 >> 2) + 2) == 0) {
                F_16C8F();
                F_16C8F();
                g_7d36 += 300;
                if (g_78e0.f0 > 20)
                    g_54da = 2;
                g_7d7e = 0;
                g_7d3c += random_int(2) ? 15 : -15;
                F_16C8F();
            }
            g_54d8 = 0;
        }
    } else if (g_7d7c)
        g_54d8 = 0;
}
