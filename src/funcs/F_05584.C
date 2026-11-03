/* F_05584 @ 0000:5584 (linear 0x05584, 293 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

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
