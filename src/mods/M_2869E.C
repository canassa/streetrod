#include "GLOBALS.H"
#include "PROTOS.H"

/* F_2869E @ 2645:224E (linear 0x2869E, 58 bytes) */


static void near F_2869E(void)
{
    if (g_vid.mode == -2) {
        g_787a = (g_5e3e == 1) ? 0xa200 : 0xa400;
        F_2322E(g_5e40);
    } else {
        g_787a = g_8188->bits.w.seg;
        g_767c = g_8188->bits.w.off;
    }
}
