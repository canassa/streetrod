/* F_26D6A @ 2645:091A (linear 0x26D6A, 214 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26D6A(void)
{
    int i, start;

    start = g_6e00;
    g_6e00 += 3;
    g_ter3cb0[g_6e00] |= 0x800;
    g_6e00 += 4;
    g_ter4e30[g_6e00++] &= 0xff7;
    g_ter3cb0[g_6e00++] |= 0x1000;
    for (i = 0; i < 10; i++) {
        g_ter4e30[g_6e00] |= 0x40;
        g_ter4e30[g_6e00++] &= 0xff7;
    }
    g_ter4e30[g_6e00++] &= 0xff7;
    g_6e00 += 3;
    for (i = start; i < g_6e00; i++)
        g_ter4570[i] = g_6dfe;
}
