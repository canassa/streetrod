/* F_0B045 @ 0000:B045 (linear 0x0B045, 71 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B045(int id, char c)
{
    struct Hot *h;

    h = g_20aa;
    if (id == 14)
        g_20aa[0].f4 = "Wanna quit ?" - g_239e;
    else
        h->f4 = "Back to the garage" - g_239e;
    h[1].f0 = id;
    h[2].f0 = id;
    h[3].f0 = id;
    h[4].f0 = id;
    *g_57e9 = c;
}
