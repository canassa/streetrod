/* F_0B456 @ 0000:B456 (linear 0x0B456, 70 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0B3CA(int);

static void near F_0B456(void)
{
    int k;

    g_6ca6 = 0;
    g_6ca8 = 0;
    g_6ca4 = 0;
    for (k = 0; k < 6; k++)
        g_09f4[F_0B3CA(g_53b6[k])].f0 = 14;
}
