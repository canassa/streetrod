#include "GLOBALS.H"
#include "PROTOS.H"

/* F_0542E @ 0000:542E (linear 0x0542E, 342 bytes) */


void F_0542E(int k, int *list)
{
    int x, i, y, n;

    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    g_7ff8[k].fa = i;
    g_7ff8[k].fb = 0;
    g_7ff8[k].fc = 0;
    g_7ff8[k].fd = 0;
    g_7ff8[k].fe = 0;
    g_7ff8[k].ff = 0;
    g_7ff8[k].f10 = 0;
    n = list[0];
    do {
        i = random_int(n);
    } while (list[i + 1] == -1);
    g_7ff8[k].f8 = list[i + 1];
    list[i + 1] = -1;
    x = g_7ff8[k].f8;
    y = (g_7d88[x].kind >> 1) + 1;
    g_7ff8[k].fe = ((g_7d88[x].f2 >> 8 & 7) == 2) ? 8 : 0;
    g_7ff8[k].ff = (random_int(20) < y) ? random_int(9) : 0;
    if (g_8df0[x].f6 > 0 && g_8df0[x].f8 > 0)
        g_7ff8[k].fe |= (random_int(25) < y) ? 3 : 0;
    if (g_8df0[x].f2 > 0)
        g_7ff8[k].fe |= (random_int(30) < y) ? 4 : 0;
}

/* F_05584 @ 0000:5584 (linear 0x05584, 293 bytes) */


static void near F_05584(void)
{
    int nb, na1, nc, v, i, k;
    int cl[11], bm[11], an[11];

    i = na1 = nb = nc = 0;
    for (; i < 25; i++) {
        v = g_7d88[i].kind;
        if (v >= 6)
            cl[++nc] = i;
        else if (v < 3)
            an[++na1] = i;
        else
            bm[++nb] = i;
    }
    an[0] = na1;
    bm[0] = nb;
    cl[0] = nc;
    if (g_0610)
        an[0] = --na1;
    k = 0;
    for (i = 0; i < 7; i++)
        F_0542E(k++, an);
    for (i = 0; i < 7; i++)
        F_0542E(k++, bm);
    for (i = 0; i < 7; i++)
        F_0542E(k++, cl);
    g_817d = 0;
    if (g_0610) {
        an[0] = 1;
        an[1] = 0x18;
        F_0542E(g_4ff4, an);
        g_7ff8[g_4ff4].f6 = 0x629;
    }
}
