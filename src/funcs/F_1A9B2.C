/* F_1A9B2: build the two scratch bitmaps for a shifted background strip and its 0x28-wide column */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static struct Bitmap far *near F_1A9B2(int n)
{
    struct Bitmap far *b;
    struct Blt blt;
    struct Bitmap far *a2;
    struct Bitmap far *c;

    a2 = F_1934E(g_6d30->width + 8, g_6d30->height, g_6d30->alt.p != 0L, 1);
    F_1A03F(g_6d30, n & ~g_vid.mask, a2);
    b = F_1934E(a2->width, a2->height, 0, 1);
    F_11A72((n + g_6da0) & g_vid.mask, 0x78, a2->width, a2->height, b);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_78aa)(a2, b, &blt, 1);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = 0;
    blt.dx = (n + g_6da0) & g_vid.mask;
    blt.dy = 0x78;
    (*g_78a2)(b, g_vid.back, &blt, 1);
    F_1941E(2);
    blt.w = 0x28;
    blt.h = g_6df6->height;
    blt.sx = n & g_vid.mask;
    blt.sy = blt.dx = 0;
    blt.dy = -(g_6df6->height + g_5aa8 - 0x95);
    c = F_1934E(0x28, 0x9c - g_5aa8, 1, 1);
    F_11A72(blt.sx + g_6da0, g_5aa8, 0x28, 0x9c - g_5aa8, c);
    F_109C0(c->alt.p, 0xff, c->planesize / g_vid.planes);
    (*g_78aa)(g_6df6, c, &blt, 1);
    (*g_78a2)(g_6df6, c, &blt, 2);
    return c;
}
