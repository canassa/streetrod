/* F_17AAE: render a 16x70 strip into a scratch bitmap with two draw passes and blit it onto two bitmaps */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_17AAE(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2)
{
    char far *save1;
    struct Bitmap far *tmp1;
    struct Blt b;
    int t;

    g_4f0e = 0;
    tmp1 = F_1934E(0x10, 0x46, 1, 2);
    F_1592E(id1, tmp1);
    save1 = tmp1->bits.p;
    tmp1->bits.p = 0;
    F_1592E(id2, tmp1);
    tmp1->bits.p = save1;
    g_4f0e = 1;
    F_19ED0(tmp1, dstbm, 1);
    b.h = g_825a->height;
    b.sy = 0;
    b.dy = top - g_825a->height - y + 1;
    t = x - (sx0 & 0xfff8);
    b.sx = (t < 0) ? 0 : t;
    b.w = (t + 16 > 16) ? 16 : t + 16;
    b.dx = (-t < 0) ? 0 : -t;
    (*g_78aa)(g_825a, dstbm, &b, 1);
    b.w = 0x10;
    b.h = 0x46;
    b.dx = b.dy = b.sx = b.sy = 0;
    (*g_78aa)(tmp1, dstbm, &b, 0);
    F_19438(1);
}
