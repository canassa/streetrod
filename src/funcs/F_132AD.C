/* F_132AD: redraw all nine slot pictures (a and b give each slot's mode and sub-mode) into the
   back buffer over the g_vid.f8 strip, then the g_82ac sprite, and an optional label when flag is set.
   The screen shadow g_vid.f16 is switched off meanwhile and the area is copied to the front afterwards. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_132AD(int *a, int *b, int flag)
{
    int prev2;
    int prev;
    int id;
    int i;
    int shadow;
    struct Blt blt;

    prev = g_82ae;
    prev2 = g_82b2;
    shadow = g_vid.f16;
    g_vid.f16 = 0;
    blt.sx = 0;
    blt.dx = 0x60;
    blt.sy = 0;
    blt.dy = 9;
    blt.w = g_vid.f8->width;
    blt.h = blt.fc = g_vid.f8->height;
    (*g_78a2)(g_vid.f8, g_vid.back, &blt, 1);
    for (i = 0; i < 9; i++) {
        if (id = g_829a[i]) {
            g_82ae = a[i];
            g_82b2 = b[i];
            F_12A17(id);
        }
    }
    if (g_82ac)
        F_1309C();
    if (flag) {
        F_119B8(0x4c6, 0x60, 0x94, -1);
        blt.h = 0xaf;
    }
    if (g_vid.f16 = shadow)
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + blt.w - 1, blt.dy + blt.h - 1);
    g_82ae = prev;
    g_82b2 = prev2;
}
