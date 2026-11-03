/* F_0F7A5 @ 0F38:0425 (linear 0x0F7A5) */

#include <stdlib.h>
#pragma intrinsic(abs)
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0F38A(int x, int y, int w, int h, int kind);
static void near F_0F418(int x, int y, int w, int h, int kind);

int F_0F7A5(int cur, int old, int flag)
{
    int miny3, minx4, y1, x2, x11, y2;
    unsigned saveOff, saveSeg;

    if (g_02d2 < 0)
        g_02d2 = (g_vid.mode == -2 || g_vid.mode == -6) ? 11 : 3;
    saveSeg = g_787a;
    saveOff = g_767c;
    g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
    g_767c = 0;
    if (flag) {
        F_0F62E(old / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F38A(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    } else {
        F_0F4B7(old, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F38A(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    }
    if (flag) {
        F_0F62E(cur / 0x2b, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 3);
    } else {
        F_0F4B7(cur, &x11, &y1, &x2, &y2);
        minx4 = x11 > x2 ? x2 : x11;
        miny3 = y1 > y2 ? y2 : y1;
        F_0F418(minx4, miny3, abs(x11 - x2) + 1, abs(y1 - y2) + 1, 2);
    }
    (*g_78d2)(x11, y1, x2, y2, g_02d2);
    g_787a = saveSeg;
    g_767c = saveOff;
    return cur;
}
