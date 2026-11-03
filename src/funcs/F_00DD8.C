/* F_00DD8 @ 0000:0DD8 (linear 0x00DD8, 76 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_00DD8(void)
{
    if (g_58d0 && !g_58d2->off && g_58d2->lc != g_58d2->l8) {
        g_58d2->off = 1;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2ba9;
        F_00D78();
    }
}
