/* F_26F0B @ 2645:0ABB (linear 0x26F0B, 214 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26F0B(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 4;
    g_ter33f0[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_ter4e30[g_6e00++] &= 0xff7;
    g_ter33f0[g_6e00++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_ter4e30[g_6e00] |= 0x20;
        g_ter4e30[g_6e00++] &= 0xff7;
    }
    g_ter4e30[g_6e00++] &= 0xff7;
    g_6e00 += 2;
    for (i = start; i < g_6e00; i++)
        g_ter4570[i] = g_6dfe;
}
