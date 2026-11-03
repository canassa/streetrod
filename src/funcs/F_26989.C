/* F_26989 @ 2645:0539 (linear 0x26989, 401 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_26989(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 90; i++)
        g_ter4570[i] = g_6dfe;
    g_6e00++;
    g_ter33f0[g_6e00] &= 0x1801;
    g_ter3cb0[g_6e00] &= 0x1801;
    g_ter3cb0[g_6e00] |= 0x4000;
    g_6e00 += 6;
    g_ter3cb0[g_6e00] |= 0x4000;
    for (i = g_6dfe / g_6e02; i > g_6dfe / g_6e02 - 35; i--, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_ter4e30[g_6e00] |= 8;
            g_ter4570[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfe = i * g_6e02;
    for (; i < g_6dfe / g_6e02 + 48; i++, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_ter4e30[g_6e00] |= 8;
            g_ter4570[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfe = i * g_6e02;
}
