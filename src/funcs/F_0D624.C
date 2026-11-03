/* F_0D624 @ 0000:D624 (linear 0x0D624, 89 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0D624(void)
{
    g_7d32 += g_7d34 = g_78e0.f0 ? F_0D544(g_78e0.f0) : 0;
    if (g_7d32 >= 8)
        F_2834C();
    if (g_7d30 >= g_6caa && g_53e6 && g_78e0.f0 <= 0) {
        if (g_8632)
            g_79e2.fbc |= 1;
        else
            g_78e0.fbc |= 1;
        F_02374();
    }
}
