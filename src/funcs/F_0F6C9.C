/* F_0F6C9 @ 0F38:0349 (linear 0x0F6C9, 220 bytes) */

#include <stdlib.h>
#pragma intrinsic(abs)
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0F418(int x, int y, int w, int h, int kind);

void F_0F6C9(int v)
{
    int miny3, minx4, y1, x2, x11, y2, color;
    unsigned saveOff, saveSeg;

    color = g_vid.mode == -2 ? 11 : 3;
    F_0F62E(v / 0x2b, &x11, &y1, &x2, &y2);
    minx4 = x11 > x2 ? x2 : x11;
    miny3 = y1 > y2 ? y2 : y1;
    F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    if (g_58e4 == 0) {
        saveSeg = g_787a;
        saveOff = g_767c;
        g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
        g_767c = 0;
        (*g_78d2)(x11, y1, x2, y2, color);
        g_787a = saveSeg;
        g_767c = saveOff;
    }
}
