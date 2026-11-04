#include "STRUCTS.H"
#include "M_0B3CA.H"
#include "GBSS.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_08D26.H"
#include "M_08EA8.H"
#include "M_0914C.H"
#include "M_15396.H"
#include "M_265BA.H"
#include "SLOTS.H"
#include <string.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Bitmap far *ab7526x[13];
int ap7d7cv;
int gz7d7e;
int g7d82p;
int g_8648;
struct Bitmap far *g8acew[13];
int cz8bd0y;

static void near F_0D1DE(int rpm, struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);
static int near F_0E218(int n);

/* F_0D624 @ 0000:D624 (linear 0x0D624, 89 bytes) */


static void near F_0D624(void)
{
    ao7d2ev.m7d32 += ao7d2ev.m7d34 = e_78e0.f0 ? F_0D544(e_78e0.f0) : 0;
    if (ao7d2ev.m7d32 >= 8)
        F_2834C();
    if (ao7d2ev.m7d30 >= g_6caa && g_53e6 && e_78e0.f0 <= 0) {
        if (s_8632)
            g79e2g.fbc |= 1;
        else
            e_78e0.fbc |= 1;
        F_02374();
    }
}

/* F_0D67D @ 0000:D67D (linear 0x0D67D, 789 bytes) */



static void near F_0D67D(void)
{
    int flag;
    int slope;
    int t;
    int dist;

    ar7e8av.m7e8e = g79e2g.f0 ? F_0D544(g79e2g.f0) : 0;
    if (g79e2g.f0 == 0)
        ar7e8av.m7e8e = 0;
    ar7e8av.m7e8c += ar7e8av.m7e8e;
    if (ar7e8av.m7e8c >= 8 && ar7e8av.m7e8e > 0) {
        ar7e8av.m7e8a += ar7e8av.m7e8c >> 3;
        ar7e8av.m7e8c &= 7;
    }
    if (ar7e8av.m7e8a >= g_6caa && g_53e6 && e_78e0.f0 <= 0) {
        if (s_8632 == 0)
            e_78e0.fbc |= 1;
        else
            g79e2g.fbc |= 1;
        F_02374();
        return;
    }
    flag = g_terrain3[ar7e8av.m7e8a + 4] & 0xe0;
    dist = ar7e8av.m7e8a - ao7d2ev.m7d30;
    slope = g_terrain2[ao7d2ev.m7d30 + 4] - g_terrain2[ao7d2ev.m7d30 + 5];
    if (dist > 12) {
        g_6cb6 = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        if (slope)
            g_54d4 = slope < 0;
        g_8648 = 1;
    } else if (dist > 6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1 : g_54d4 ? ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200 : ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
    } else if ((bu8acc < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else if (flag)
            g_6cb6 = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        else if (g_8648) {
            if (g_54d4)
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
            else
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
            g_8648 = 0;
            g_54d4 = !g_54d4;
        }
    } else if (dist > -4) {
        if (ar7e8av.m7e8e > ao7d2ev.m7d34) {
            if (ap7d7cv >= 9) {
                if ((t = ao7d2ev.m7d36 - 330) > g7d82p + 480) {
                    g_54d4 = 1;
                    g_6cb6 = t;
                    if (ao7d2ev.m7d36 - ar7e8av.m7e90 < 280) {
                        ar7e8av.m7e8a = ao7d2ev.m7d30 - 1;
                        F_0D1DE(e_78e0.f0, &g79e2g);
                    }
                } else {
                    if ((t = ao7d2ev.m7d36 + 430) < cz8bd0y - 30) {
                        g_54d4 = 0;
                        g_6cb6 = t + 200;
                        if (ar7e8av.m7e90 - ao7d2ev.m7d36 < 580) {
                            ar7e8av.m7e8a = ao7d2ev.m7d30 - 2;
                            F_0D1DE(e_78e0.f0, &g79e2g);
                        }
                    } else {
                        ar7e8av.m7e8a = ao7d2ev.m7d30 - 2;
                        F_0D1DE(e_78e0.f0, &g79e2g);
                    }
                }
            } else if (ap7d7cv == 0 || ap7d7cv == 2) {
                g_6cb6 = ao7d2ev.m7d36 > ar7e8av.m7e90 ? ao7d2ev.m7d36 - 330 : ao7d2ev.m7d36 + 530;
            }
        } else if (ap7d7cv == 0)
            g_6cb6 = ao7d2ev.m7d36 - 330;
    } else if (dist > -6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1 : g_54d4 ? ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200 : ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
    } else {
        g_6cb6 = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        g_8648 = 1;
    }
    if (ar7e8av.m7e90 > g_6cb6) {
        dist = (ar7e8av.m7e90 - g_6cb6) >> 3;
        if (ar7e8av.m7e90 - dist > g_6cb6)
            ar7e8av.m7e90 -= dist;
    } else {
        dist = (g_6cb6 - ar7e8av.m7e90) >> 3;
        if (dist + ar7e8av.m7e90 < g_6cb6)
            ar7e8av.m7e90 += dist;
    }
}

/* F_0D992 @ 0000:D992 (linear 0x0D992, 147 bytes) */



static void near F_0D992(void)
{
    int n1;
    int old;

    old = g79e2g.f0;
    g7d84o = &av7ff8[10];
    F_0E0E2(0x19, &g79e2g);
    g_6cb4 = g7d84o->f1;
    F_0D1DE(old, &g79e2g);
    F_08D9C(5);
    for (n1 = 0; n1 < 12; n1++)
        g8acew[n1] = ab7526x[n1];
    g_6cac = F_0E218(0x19);
    g_6cb2 = (int)(g_6cac * 0.6);
    g_6cb0 = (int)(g_6cac * 0.8);
}
