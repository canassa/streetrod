/* F_2A213 @ 2645:3DC3 (linear 0x2A213, 121 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2A213(void)
{
    struct Blt b;

    if (g_7aae > 0) {
        b.w = 0x18;
        b.h = g_7aae;
        if (g_7d66) {
            g_8228 = 1;
            b.sx = 0;
        } else
            b.sx = 0x18;
        b.sy = 0x20 - g_7aae;
        b.dx = 0x82;
        b.dy = 0x12;
        (*g_78aa)(g_6e60, g_5e3e == 1 ? g_8188 : g_82b4, &b, 1);
    }
}
