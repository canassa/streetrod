/* F_284BB @ 2645:206B (linear 0x284BB, 169 bytes)

   Returns 1 when the car at the current terrain index would hit something. */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_284BB(void)
{
    g_5e0e = 0;
    if (g_7d82 - 0x104 < g_7d36 - 0x1e && g_7d36 + 0x46 < g_8bd0 + 0x104) {
        if (g_7d82 > g_7d36 - 0x1e) {
            if ((g_ter33f0[g_7d30] & 0x1000) || (g_ter33f0[g_7d30 - 1] & 0x1000))
                return 1;
            if (g_ter4e30[g_7d30 + 1] & 0xf000)
                return 1;
            g_5e0e = 1;
        } else if (g_7d36 + 0x46 > g_8bd0) {
            if ((g_ter3cb0[g_7d30] & 0x1000) || (g_ter3cb0[g_7d30 - 1] & 0x1000))
                return 1;
            if (g_ter4e30[g_7d30 + 1] & 0xf002)
                return 1;
            g_5e0e = 1;
        }
        return 0;
    }
    return 1;
}
