/* F_049ED @ 0000:49ED (linear 0x049ED, 1501 bytes) */

#include <stdio.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_04940(unsigned *price);
static int near F_043BF(struct Car *c, int keep);

static int near F_049ED(void)
{
    struct Bitmap far *pict;
    struct Car *tbl[142];
    unsigned offer;
    unsigned price;
    int rx;
    int mid;
    struct Car *cur;
    int ticks;
    int ye;
    int n;
    int result;
    unsigned cash;
    int item;

    pict = 0;
    ticks = 0;
    item = 1;
    for (;; ticks++) {
        n = 0;
        if (g_7eb0) {
            n = 1;
            g_49e0[1] = g_7d86[(tbl[1] = g_7eb0)->f2].f6;
        }
        for (cur = g_7eb2; cur; cur = cur->f26) {
            n++;
            g_49e0[n] = g_7d86[(tbl[n] = cur)->f2].f6;
        }
        g_49e0[0] = n;
        switch (item = F_142CF(0xb, (n > item) ? item : n, 0, g_49e0, 0, 0)) {
        case 0x3ea:
            F_03AB8(ticks ? 0xee9 : 0xefa);
        case -3:
            return 0;
        case -1:
            g_4f48 = tbl[item = F_14C8B()];
            g_4f46 = g_7eb0;
            if (g_4f48 != g_4f46) {
                if (item == ((g_7eb0 != 0) + 1))
                    g_7eb2 = g_4f48->f26;
                else
                    tbl[item - 1]->f26 = g_4f48->f26;
                if (g_7eb0) {
                    g_7eb0->f26 = g_7eb2;
                    g_7eb2 = g_7eb0;
                }
                g_7eb0 = g_4f48;
            } else
                g_4f48 = 0;
            return 3;
        case -2:
            break;
        default:
            if (item > 0)
                g_49e0[0] = g_49e0[0];
        }
        cur = tbl[item = F_14C8B()];
        offer = price = (int)((random_int(20) + 75.0) * cur->weight / 100.0);
        strcpy(g_2990, g_2c02 + g_7d86[cur->f2].f6);
        g_29f8[0] = 0;
        pict = F_14090(12, &rx, &ye, 1);
        F_13DE6(12);
        do {
            if ((n = F_15127(&cash, g_29f8)) == -22)
                goto tail;
        } while (cash == 0);
        if ((unsigned)(cur->weight * 1.15) < cash) {
            F_03AB8(0xec0);
            goto tail;
        }
        if (cur->weight < cash) {
            F_03AB8(0x106e);
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
            if (cur->weight < cash) {
                F_03AB8(0x1078);
                goto tail;
            }
        }
        if (cash > offer) {
            offer = (random_int(5) + 85.0) * price / 100.0;
            if (cash > offer && F_04940(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if (cash > offer) {
            offer = (random_int(5) + 91.0) * price / 100.0;
            if (cash > offer && F_04940(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if (offer < cash * 0.9) {
            F_03AB8(0x1084);
            goto tail;
        }
        F_03AB8((cur->weight > cash * 2) ? 0x104b : 0x1091);
        g_05fc += 0x888L;
        F_11540(cash);
        if (item == 1 && g_7eb0) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
            F_120FE(-4);
            mid = g_7eb0->f2;
            result = F_049A2(g_7eb0);
            F_181C8(6, mid, result, (g_7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                    g_5030[g_4808[g_7eb0->slot12->idx].kind]);
            F_13F21(1);
            F_06AA6();
            F_13DE6(10);
            F_120FE(-2);
            F_043BF(g_7eb0, 0);
            g_7eb0 = 0;
        } else {
            if (item == ((g_7eb0 != 0) + 1))
                g_7eb2 = cur->f26;
            else
                tbl[item - 1]->f26 = cur->f26;
            F_043BF(cur, 0);
        }
tail:
        F_13F21(1);
        if (pict) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
        }
    }
}
