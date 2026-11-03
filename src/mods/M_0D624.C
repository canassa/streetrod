#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <string.h>

static void near F_0D1DE(int rpm, struct Eng *p);
static void near F_0E0E2(int n, struct Eng *p);
static int near F_0E218(int n);

/* F_0D624 @ 0000:D624 (linear 0x0D624, 89 bytes) */


static void near F_0D624(void)
{
    g_7d32 += g_7d34 = g_78e0.f0 ? F_0D544(g_78e0.f0) : 0;
    if (g_7d32 >= 8)
        F_2834C();
    if (g_7d30 >= g_6caa && g_53e6 && g_78e0.f0 <= 0) {
        if (g_8632)
            g_79e2.fbc |= 1;
        else
            g_78e0.fbc |= 1;
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

    g_7e8e = g_79e2.f0 ? F_0D544(g_79e2.f0) : 0;
    if (g_79e2.f0 == 0)
        g_7e8e = 0;
    g_7e8c += g_7e8e;
    if (g_7e8c >= 8 && g_7e8e > 0) {
        g_7e8a += g_7e8c >> 3;
        g_7e8c &= 7;
    }
    if (g_7e8a >= g_6caa && g_53e6 && g_78e0.f0 <= 0) {
        if (g_8632 == 0)
            g_78e0.fbc |= 1;
        else
            g_79e2.fbc |= 1;
        F_02374();
        return;
    }
    flag = g_ter4e30[g_7e8a + 4] & 0xe0;
    dist = g_7e8a - g_7d30;
    slope = g_ter4570[g_7d30 + 4] - g_ter4570[g_7d30 + 5];
    if (dist > 12) {
        g_6cb6 = (g_70c0 + g_7d2e) >> 1;
        if (slope)
            g_54d4 = slope < 0;
        g_8648 = 1;
    } else if (dist > 6) {
        g_7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (g_70c0 + g_7d2e) >> 1 : g_54d4 ? ((g_70c0 + g_7d2e) >> 1) - 200 : ((g_70c0 + g_7d2e) >> 1) + 300;
    } else if ((g_8acc < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else if (flag)
            g_6cb6 = (g_70c0 + g_7d2e) >> 1;
        else if (g_8648) {
            if (g_54d4)
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) + 300;
            else
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) - 200;
            g_8648 = 0;
            g_54d4 = !g_54d4;
        }
    } else if (dist > -4) {
        if (g_7e8e > g_7d34) {
            if (g_7d7c >= 9) {
                if ((t = g_7d36 - 330) > g_7d82 + 480) {
                    g_54d4 = 1;
                    g_6cb6 = t;
                    if (g_7d36 - g_7e90 < 280) {
                        g_7e8a = g_7d30 - 1;
                        F_0D1DE(g_78e0.f0, &g_79e2);
                    }
                } else {
                    if ((t = g_7d36 + 430) < g_8bd0 - 30) {
                        g_54d4 = 0;
                        g_6cb6 = t + 200;
                        if (g_7e90 - g_7d36 < 580) {
                            g_7e8a = g_7d30 - 2;
                            F_0D1DE(g_78e0.f0, &g_79e2);
                        }
                    } else {
                        g_7e8a = g_7d30 - 2;
                        F_0D1DE(g_78e0.f0, &g_79e2);
                    }
                }
            } else if (g_7d7c == 0 || g_7d7c == 2) {
                g_6cb6 = g_7d36 > g_7e90 ? g_7d36 - 330 : g_7d36 + 530;
            }
        } else if (g_7d7c == 0)
            g_6cb6 = g_7d36 - 330;
    } else if (dist > -6) {
        g_7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) - 200;
                g_54d4 = 1;
            } else {
                g_6cb6 = ((g_70c0 + g_7d2e) >> 1) + 300;
                g_54d4 = 0;
            }
        } else
            g_6cb6 = flag ? (g_70c0 + g_7d2e) >> 1 : g_54d4 ? ((g_70c0 + g_7d2e) >> 1) - 200 : ((g_70c0 + g_7d2e) >> 1) + 300;
    } else {
        g_6cb6 = (g_70c0 + g_7d2e) >> 1;
        g_8648 = 1;
    }
    if (g_7e90 > g_6cb6) {
        dist = (g_7e90 - g_6cb6) >> 3;
        if (g_7e90 - dist > g_6cb6)
            g_7e90 -= dist;
    } else {
        dist = (g_6cb6 - g_7e90) >> 3;
        if (dist + g_7e90 < g_6cb6)
            g_7e90 += dist;
    }
}

/* F_0D992 @ 0000:D992 (linear 0x0D992, 147 bytes) */



static void near F_0D992(void)
{
    int n1;
    int old;

    old = g_79e2.f0;
    g_7d84 = &g_7ff8[10];
    F_0E0E2(0x19, &g_79e2);
    g_6cb4 = g_7d84->f1;
    F_0D1DE(old, &g_79e2);
    F_08D9C(5);
    for (n1 = 0; n1 < 12; n1++)
        g_8ace[n1] = g_7526[n1];
    g_6cac = F_0E218(0x19);
    g_6cb2 = (int)(g_6cac * 0.6);
    g_6cb0 = (int)(g_6cac * 0.8);
}
