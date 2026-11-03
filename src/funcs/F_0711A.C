/* F_0711A @ 0000:711A (linear 0x0711a, 589 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0711A(unsigned mask)
{
    struct Bitmap far *bm1;
    int n9;
    char *msg;
    unsigned val5;
    int x;
    int r7;
    struct Model *p9;
    int y;
    int key;
    int cost;

    p9 = &g_8df0[g_7eb0->f2];
    if (!g_7eb0) {
        F_03AB8(0x0efa);
        return;
    }
    if (mask & 4) {
        if (p9->f2 == 0) {
            F_03AB8(0x1914);
            return;
        }
        msg = (g_7eb0->f24 & 0x2000) ? "Rebuild roof - $70" : "Chop roof - $70";
        cost = 70;
        val5 = 0x2220;
    }
    if (mask & 2) {
        if (p9->f6 == 0) {
            F_03AB8(0x1936);
            return;
        }
        msg = (g_7eb0->f24 & 0x4000) ? "Restore bumper - $15" : "Strip bumper - $15";
        cost = 15;
        val5 = 0x888;
    }
    if (mask & 1) {
        if (p9->f8 == 0) {
            F_03AB8(0x1936);
            return;
        }
        msg = (g_7eb0->f24 & 0x8000) ? "Restore bumper - $15" : "Strip bumper - $15";
        cost = 15;
        val5 = 0x888;
    }
    if ((long)cost > g_7eaa) {
        F_03AB8(0x1959);
        return;
    }
    bm1 = F_14090(34, &x, &y, 2);
    g_1e38 = msg - g_239e;
    F_13DE6(34);
    while ((key = F_01417(3000)) == 0)
        ;
    switch (key) {
    case -20:
        if (mask & 4)
            g_7eb0->f24 ^= 0x2000;
        if (mask & 2)
            g_7eb0->f24 ^= 0x4000;
        if (mask & 1)
            g_7eb0->f24 ^= 0x8000;
        F_120FE(-4);
        F_13F21(1);
        F_11BD3(bm1, x, y);
        F_19438(1);
        n9 = g_7eb0->f2;
        r7 = F_049A2(g_7eb0);
        F_181C8(8, n9, r7, (g_7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                g_5030[g_4808[g_7eb0->slot12->idx].kind]);
        F_16BC1();
        F_120FE(-2);
        g_05fc += val5;
        F_11540(-cost);
        break;
    case -22:
        F_13F21(1);
        F_11BD3(bm1, x, y);
        F_19438(1);
        break;
    }
}
