/* F_0F4B7 @ 0F38:0137 (linear 0x0F4B7, 155 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_0F4B7(int i, int *x1, int *y1, int *x2, int *y2)
{
    if (i < 0)
        i = 0;
    else if (i > 0x82)
        i = 0x82;
    *x1 = g_2e47_0000[0][i][0] + 0xc4;
    *y1 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[0][i][1] + 0x2a;
    *x2 = g_2e47_0000[1][i][0] + 0xc4;
    *y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) - g_2e47_0000[1][i][1] + 0x2a;
}
