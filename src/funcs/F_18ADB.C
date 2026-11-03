/* F_18ADB: blit a clipped band of a bitmap through the blit hook, then the seven frame cells onto the front buffer */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_18ADB(struct Bitmap far *dst, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    int base7, hi1, lo7, i9;
    struct Blt r;
    int top;

    base7 = p7 - 0x45;
    top = g_5a0a->height + base7 - 1;
    lo7 = p6 < base7 ? base7 : p6;
    hi1 = p6 + p4 - 1 > top ? top : p6 + p4 - 1;
    if (lo7 <= hi1) {
        r.w = p3;
        r.h = hi1 - lo7 + 1;
        r.sx = p1;
        r.sy = p2 + lo7 - p6;
        r.dx = p5;
        r.dy = lo7 - base7;
        (*g_78a2)(dst, g_5a0a, &r, 1);
    }
    if (g_0284 == 6) {
        r.h = g_5a0a->height;
        r.sy = p7 - 0x45;
        r.dy = 0;
        for (i9 = 0; i9 < 7; i9++) {
            r.w = g_5338[i9].w;
            r.sx = r.dx = g_5338[i9].x;
            (*g_78a2)(g_vid.front, g_5a0a, &r, 1);
        }
    }
}
