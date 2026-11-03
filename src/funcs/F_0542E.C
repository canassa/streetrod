/* F_0542E @ 0000:542E (linear 0x0542E, 342 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

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
