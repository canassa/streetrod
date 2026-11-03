/* F_286D8 @ 2645:22D8 (linear 0x286D8, 116 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_286D8(void)
{
    int t;

    if (g_vid.mode == -2) {
        t = g_5e3e;
        g_5e3e = g_5e40;
        g_5e40 = t;
        g_787a = (g_5e3e == 1) ? 0xa200 : 0xa400;
        F_2322E(g_5e40);
    } else {
        (*g_78a2)(g_8188, g_vid.front, &g_5e42, 1);
        g_787a = g_8188->bits.w.seg;
        g_767c = g_8188->bits.w.off;
    }
}
