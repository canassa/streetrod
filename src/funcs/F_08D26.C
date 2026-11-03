/* F_08D26 @ 0000:8D26 (linear 0x08D26, 118 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_08D26(void)
{
    g_47ee = g_5ee = g_471c = g_4732 = 0;
    if (g_0284) {
        if (g_7eb0->f22 > 2)
            g_7eb0->f22 -= 2;
        else
            g_7eb0->f22 = 0;
        (*g_78be)();
        if (g_47ee == 0) {
            F_120FE(-4);
            g_58e6 = !g_4806[g_7eb0->slot6->idx].f2;
            F_0CDAF(&g_78e0);
            g_79a6 = 0;
            g_8aca = 0;
            F_08E2D();
        }
    }
}
