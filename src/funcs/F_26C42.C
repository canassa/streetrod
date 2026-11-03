/* F_26C42 (near, builds a terrain hill section) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26C42(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 6;
    for (i = 0; i < 38; i++) {
        if (g_6e00 < 0x442)
            g_ter56f0[g_6e00++] = g_5c4c[i];
        else
            break;
    }
    g_6e00 += 6;
    for (i = start; i < g_6e00; i++)
        g_ter4570[i] = g_6dfe;
}
