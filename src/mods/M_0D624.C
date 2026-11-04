#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <string.h>

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Bitmap far *ab7526x[13];
int aa7d34l;
int a_7d36m;
int ap7d7cv;
int gz7d7e;
int g7d82p;
int an7e8ex;
int g_8648;
struct Bitmap far *g8acew[13];
int cz8bd0y;

static void near F_0D1DE(int rpm, struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);
static int near F_0E218(int n);

/* F_0D624 @ 0000:D624 (linear 0x0D624, 89 bytes) */


static void near F_0D624(void)
{
    bp7d32 += aa7d34l = e_78e0.f0 ? F_0D544(e_78e0.f0) : 0;
    if (bp7d32 >= 8)
        F_2834C();
    if (ac7d30z >= g_6caa && g_53e6 && e_78e0.f0 <= 0) {
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

    an7e8ex = g79e2g.f0 ? F_0D544(g79e2g.f0) : 0;
    if (g79e2g.f0 == 0)
        an7e8ex = 0;
    a_7e8c += an7e8ex;
    if (a_7e8c >= 8 && an7e8ex > 0) {
        ar7e8av += a_7e8c >> 3;
        a_7e8c &= 7;
    }
    if (ar7e8av >= g_6caa && g_53e6 && e_78e0.f0 <= 0) {
        if (s_8632 == 0)
            e_78e0.fbc |= 1;
        else
            g79e2g.fbc |= 1;
        F_02374();
        return;
    }
    flag = g_terrain3[ar7e8av + 4] & 0xe0;
    dist = ar7e8av - ac7d30z;
    slope = g_terrain2[ac7d30z + 4] - g_terrain2[ac7d30z + 5];
    if (dist > 12) {
        g_6cb6 = (al70c0x + ao7d2ev) >> 1;
        if (slope)
            g_54d4 = slope < 0;
        g_8648 = 1;
    } else if (dist > 6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (al70c0x + ao7d2ev) >> 1 : g_54d4 ? ((al70c0x + ao7d2ev) >> 1) - 200 : ((al70c0x + ao7d2ev) >> 1) + 300;
    } else if ((bu8acc < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) + 300;
                g_54d4 = 0;
            }
        } else if (flag)
            g_6cb6 = (al70c0x + ao7d2ev) >> 1;
        else if (g_8648) {
            if (g_54d4)
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) + 300;
            else
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) - 200;
            g_8648 = 0;
            g_54d4 = !g_54d4;
        }
    } else if (dist > -4) {
        if (an7e8ex > aa7d34l) {
            if (ap7d7cv >= 9) {
                if ((t = a_7d36m - 330) > g7d82p + 480) {
                    g_54d4 = 1;
                    g_6cb6 = t;
                    if (a_7d36m - ar7e90 < 280) {
                        ar7e8av = ac7d30z - 1;
                        F_0D1DE(e_78e0.f0, &g79e2g);
                    }
                } else {
                    if ((t = a_7d36m + 430) < cz8bd0y - 30) {
                        g_54d4 = 0;
                        g_6cb6 = t + 200;
                        if (ar7e90 - a_7d36m < 580) {
                            ar7e8av = ac7d30z - 2;
                            F_0D1DE(e_78e0.f0, &g79e2g);
                        }
                    } else {
                        ar7e8av = ac7d30z - 2;
                        F_0D1DE(e_78e0.f0, &g79e2g);
                    }
                }
            } else if (ap7d7cv == 0 || ap7d7cv == 2) {
                g_6cb6 = a_7d36m > ar7e90 ? a_7d36m - 330 : a_7d36m + 530;
            }
        } else if (ap7d7cv == 0)
            g_6cb6 = a_7d36m - 330;
    } else if (dist > -6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((al70c0x + ao7d2ev) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (al70c0x + ao7d2ev) >> 1 : g_54d4 ? ((al70c0x + ao7d2ev) >> 1) - 200 : ((al70c0x + ao7d2ev) >> 1) + 300;
    } else {
        g_6cb6 = (al70c0x + ao7d2ev) >> 1;
        g_8648 = 1;
    }
    if (ar7e90 > g_6cb6) {
        dist = (ar7e90 - g_6cb6) >> 3;
        if (ar7e90 - dist > g_6cb6)
            ar7e90 -= dist;
    } else {
        dist = (g_6cb6 - ar7e90) >> 3;
        if (dist + ar7e90 < g_6cb6)
            ar7e90 += dist;
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
