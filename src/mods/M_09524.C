#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

static int near F_0E218(int n);
static void near F_09646(int);
static void near F_098BA(int);
static int near F_09556(void);
static void near F_092A4(int, int, int, int, int, int *);

/* F_09524 @ 0000:9524 (linear 0x09524, 50 bytes) */


static int near F_09524(int n)
{
    int v;

    v = g_7d88[n].kind;
    if (v < 3)
        return 0;
    if (v >= 7)
        return 2;
    return 1;
}

/* F_09556 @ 0000:9556 (linear 0x09556, 155 bytes) */


static int near F_09556(void)
{
    int k, any, total;

    any = 0;
    for (k = 0; k < 21; k++)
        if (g_8000[k].f3 != -1)
            any = 1;
    if (any == 0)
        return 0;
    total = g_4972 + g_4974;
    if (total < 3)
        return 1;
    if (total >= 7 && g_4972 < 6 && g_4974 < 6)
        return 3;
    if (total < 7 && g_4972 < 6 && g_4974 < 6)
        return 2;
    if (g_4972 < 6)
        return 4;
    if (g_4974 < 6)
        return 5;
    if (total >= (g_4970 >> 1))
        return 0;
    return 3;
}

/* F_095F1 @ 0000:95F1 (linear 0x095F1, 85 bytes) */


static int near F_095F1(void)
{
    int kind, v;

    if (g_7648 == 25)
        return 0;
    kind = F_09556();
    if (kind) {
        if (kind == 1)
            v = 5;
        else if (kind == 2)
            v = 8;
        else
            v = 12;
    } else {
        if (g_817d == 1)
            v = 10;
        else
            v = 50;
    }
    return v;
}

/* F_09646 @ 0000:9646 (linear 0x09646, 520 bytes) */


#pragma intrinsic(abs)


static void near F_09646(int flag)
{
    int result;
    struct Spec *item;
    struct Cand map[21];
    int m, found, k, low;

    for (k = 0; k < 21; k++) {
        item = &g_7d86[g_8000[k].f0];
        map[k].dr = abs(F_0E218(g_8000[k].f0) - F_0E218(-1));
        map[k].dk = abs(g_7d88[g_8000[k].f0].kind - g_7d88[g_7eb0->f2].kind);
        map[k].taken = (g_8000[k].f3 != -1) ? 0 : 1;
    }
    g_6c86 = 0;
    found = 1;
    do {
        if ((flag ? 21 : 9) <= g_6c86)
            break;
        found = 0;
        low = 0x7d00;
        for (m = 0; m < 21; m++)
            if (map[m].taken == 0 && map[m].dr < low) {
                result = m;
                low = map[m].dr;
                found = 1;
            }
        if (found) {
            g_6c8a[g_6c86++] = result;
            map[result].taken = 1;
        }
    } while (found);
    m = 0;
    k = g_6c86;
    for (; flag == 0 && k > 6; k--)
        if (map[k - 1].dk > 1)
            g_6c8a[k - 1] = g_6c8a[m++];
    if (g_0610) {
        g_6c8a[g_6c86++] = g_4ff4;
        g_6c8a[g_6c86++] = g_4ff4;
        if (g_6ca2) {
            g_6c8a[(g_6c86 = 1) - 1] = g_4ff4;
        }
        g_6ca2 = 0;
    }
}

/* F_0984E @ 0000:984E (linear 0x0984E, 108 bytes) */



static int near F_0984E(int chance, int arg)
{
    int pick;

    if (random_int(100) < chance)
        return 0x15;
    F_09646(arg);
    if (g_6c86 == 0)
        return 0x15;
    pick = g_6c8a[random_int(g_6c86)];
    if (pick == g_51f8)
        pick = g_6c8a[random_int(g_6c86)];
    g_51f8 = pick;
    return pick;
}

/* F_098BA @ 0000:98BA (linear 0x098BA, 365 bytes) */


static void near F_098BA(int n)
{
    int skill;

    g_7f82 = n;
    g_7d84 = &g_7ff8[n];
    g_7d7a = &g_7d86[g_7648 = g_7d84->f8];
    if (n != 0x15) {
        g_7d40 = 0;
        g_7d84->fb = 1;
        skill = (g_7d7a->f2 >> 1) + 1;
        if (g_7d84->ff == 0 && g_7d84->fc > 5)
            g_7d84->ff = random_int(20) < skill ? random_int(9) : 0;
        else if (g_7d84->ff > 0 && g_7d84->fc > 10)
            g_7d84->ff = random_int(40) < skill ? random_int(9) : g_7d84->ff;
        if (g_8df0[g_7648].f6 > 0 && g_8df0[g_7648].f8 > 0 && g_7d84->fc > 8)
            g_7d84->fe |= random_int(25) < skill ? 3 : g_7d84->fe;
        if (g_8df0[g_7648].f2 > 0 && g_7d84->fc > 8)
            g_7d84->fe |= random_int(30) < skill ? 4 : g_7d84->fe;
    } else
        g_7d40 = 1;
    g_827a = g_7d7a->f8;
    g_827e = (g_7d7a->f4 >> 4) & 3;
    g_827c = g_7d84->fa;
    F_067CF(g_827c);
    g_8280 = g_7d84->f4;
}

/* F_09A27 @ 0000:9A27 (linear 0x09A27, 92 bytes) */



static void near F_09A27(int n, int a, int mode)
{
    F_098BA(n);
    if (mode == 1)
        g_5a24 = 1;
    if (mode == 2 && g_7f82 == 0x15)
        g_5a24 = F_09556() == 1;
    F_092A4(a, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
}

/* F_09A83 @ 0000:9A83 (linear 0x09A83, 113 bytes) */



static void near F_09A83(int *ids, int *slots, int *kinds)
{
    int i, n;

    if (F_09556() == 0)
        g_817d = 1;
    else
        g_817d = 0;
    for (i = 0, n = 0; i <= 21; i++) {
        if (g_7ff8[i].fb == 1) {
            slots[n] = i;
            ids[++n] = g_7ff8[i].f6;
            kinds[n] = 7;
        }
    }
    ids[0] = n;
}
