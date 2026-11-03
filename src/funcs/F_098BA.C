/* F_098BA @ 0000:98BA (linear 0x098BA, 365 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

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
