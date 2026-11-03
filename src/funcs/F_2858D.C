/* F_2858D @ 2645:213D (linear 0x2858D, 272 bytes) */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(abs)

void F_2858D(void)
{
    if (g_78e0.f2 && g_78e0.f0 && abs(g_7d3c - *g_7560 - g_78e0.f2) < 200)
        g_7d3c -= (g_78e0.f2 >> 1) + 1;
    if (!g_8aca) {
        if (g_82c6)
            g_7d34 = 6;
        else
            g_7d34 = 4;
    }
    if (g_7d34 > 0 && g_7aae > 0)
        g_7aae -= 4;
    F_282EE();
    if (g_0610 == 0 && g_0612 == 0) {
        if (g_787c)
            g_7d36 += 25;
        else if (g_78e0.f0)
            g_7d36 -= (g_7d3c - *g_7560) * (g_7d34 + g_78ec) >> 2;
    }
    if (!g_8aca) {
        g_7d32 += g_7d34;
        if (g_7d32 >= 8)
            F_2834C();
        if ((g_ter33f0[g_7d30 + 7] & 1) || g_5e12) {
            g_5e12 = 1;
            F_28576(1);
        }
        if ((g_ter3cb0[g_7d30 + 6] & 1) || g_5e10) {
            F_28576(-1);
            g_5e10 = 1;
        }
    }
}
