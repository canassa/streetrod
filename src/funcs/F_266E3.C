/* F_266E3 @ 2645:0293 (linear 0x266E3, 277 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_266E3(void)
{
    int i;

    for (i = g_6e00; i < g_6e00 + 50; i++)
        g_ter4570[i] = g_6dfe;
    g_ter33f0[g_6e00] &= 0x1801;
    g_ter3cb0[g_6e00] &= 0x1801;
    g_ter4e30[g_6e00] |= 0x400;
    g_6e00 += 3;
    g_ter4e30[g_6e00] |= 0x400;
    g_6e00 += 3;
    for (i = g_6dfe / g_6e02; i > g_6dfe / g_6e02 - 45; i--, g_6e00++) {
        if (g_6e00 < 0x442) {
            g_ter4e30[g_6e00] |= 8;
            g_ter4570[g_6e00] = i * g_6e02;
        } else
            break;
    }
    g_6dfe = i * g_6e02;
}
