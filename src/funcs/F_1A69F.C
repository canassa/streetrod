/* F_1A69F: blit all of bm to dst at (x, y) through the blit hook */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1A69F(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x;
    pkt.dy = y;
    (*g_78a2)(bm, dst, &pkt, mode);
}
