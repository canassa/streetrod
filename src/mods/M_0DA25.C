#include <stdlib.h>
#include "STRUCTS.H"
#include "M_0B3CA.H"
#include "GBSS.H"
#include "M_00D58.H"
#include "M_08EA8.H"
#include "M_0914C.H"
#include "M_113A2.H"
#include "M_15236.H"
#include "M_163BE.H"
#include "SLOTS.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int by72f2;
int ay8bc2z;

static void near F_0D29D(void);
static void near F_0D624(void);
static void near F_0D67D(void);
static void near F_0D992(void);

/* F_0DA25 @ 0000:DA25 (linear 0x0DA25, 664 bytes) */



void F_0DA25(void)
{
    int a1, b, c1, d;

    if (e_78e0.f24) {
        if (e_78e0.f24 != bz8630z)
            F_11669((bz8630z = e_78e0.f24) + 4);
    } else if (e_78e0.f18 != bz8630z)
        F_11669(bz8630z = e_78e0.f18);
    if (e_78e0.fa && ao7d2ev.m7d30 > e_78e0.fa)
        at787cu = 1;
    if (g_58da == 0) {
        if (abs(e_78e0.f4) >= 12)
            g_58da = 1;
    } else if (abs(e_78e0.f4) < 10)
        g_58da = 0;
    F_0D29D();
    switch (a_7f88d) {
    case 1:
        if (ao7d2ev.m7d30 >= g755av) {
            if (ar7e8av.m7e8a + 5 <= ao7d2ev.m7d30) {
                a_7f88d++;
                F_0D992();
            } else
                a_7f88d = 0;
        }
        break;
    case 2:
        if (ar7e8av.m7e8a + 15 >= ao7d2ev.m7d30) {
            a_7f88d++;
            F_16AA8(1);
        }
        break;
    case 3:
        if (ar7e8av.m7e8a - 2 > ao7d2ev.m7d30) {
            e_78e0.fbc |= 0x41;
            F_02374();
            return;
        }
        break;
    }
    a1 = ao7d2ev.m7d30 * 8 + ao7d2ev.m7d32;
    b = ar7e8av.m7e8a * 8 + ar7e8av.m7e8c;
    c1 = ao7d2ev.m7d36;
    d = ar7e8av.m7e90;
    F_0D624();
    F_0D67D();
    if (ao7d2ev.m7d30 >= g_6caa || ar7e8av.m7e8a >= g_6caa)
        F_0C407();
    if (e_78e0.fbc || g79e2g.fbc) {
        if (a_7f88d >= 2) {
            if (e_78e0.fbc)
                e_78e0.fbc |= 0x41;
            F_16AA8(0);
        }
        F_02374();
        return;
    }
    if (ay8bc2z == 0)
        return;
    if (g_0610)
        return;
    if ((a_7eaap.m7ec0 == 0 ? 0x17c : 0x3fc) <= ao7d2ev.m7d30)
        return;
    if (by72f2 && a_7f88d <= 1) {
        e_78e0.fbc |= 3;
        F_02374();
        return;
    }
    if (ap7d7cv == 0 && ao7d2ev.m7d34 > 1) {
        g_54d8++;
        if (e_78e0.f2 < 0) {
            if (g_terrain2[ao7d2ev.m7d30] - g_terrain2[ao7d2ev.m7d30 + 1] >= 0 || ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) >= ao7d2ev.m7d36) {
                F_16C8F();
                if (g_54d2 == 0) {
                    ar7e8av.m7e8a--;
                    g_54d2 = 1;
                } else
                    g_54d2 = 0;
            }
            return;
        }
        if (gz7d7e && g_54d8 > 3 && a_7f88d < 2) {
            if (random_int((g7d84o->f2 >> 2) + 2) == 0) {
                F_16C8F();
                F_16C8F();
                ao7d2ev.m7d36 += 300;
                if (e_78e0.f0 > 20)
                    g_54da = 2;
                gz7d7e = 0;
                ao7d2ev.m7d3c += random_int(2) ? 15 : -15;
                F_16C8F();
            }
            g_54d8 = 0;
        }
    } else if (ap7d7cv)
        g_54d8 = 0;
}
