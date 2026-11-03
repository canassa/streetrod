/* F_26B1A @ 2645:06CA (linear 0x26B1A, 147 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26B1A(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 20;
    for (i = 0; i < 22; i++) {
        if (g_6e00 < 0x442)
            g_ter56f0[g_6e00++] = g_5c20[i];
        else
            break;
    }
    g_6e00 += 8;
    for (i = start; i < g_6e00; i++)
        g_ter4570[i] = g_6dfe;
}
