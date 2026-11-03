/* F_26E40 @ 2645:09F0 (linear 0x26E40, 203 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26E40(void)
{
    int i, start;

    start = g_6e00;
    g_ter4e30[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_ter33f0[g_6e00] &= 0x1801;
    g_ter3cb0[g_6e00] &= 0x1801;
    g_ter4e30[g_6e00] |= 0x800;
    g_6e00 += 6;
    for (i = 0; i < 20; i++) {
        g_ter4e30[g_6e00] |= 0x80;
        g_ter4e30[g_6e00++] &= 0xff7;
    }
    g_ter4e30[g_6e00] &= 0x0fff;
    for (i = start; i < g_6e00; i++)
        g_ter4570[i] = g_6dfe;
}
