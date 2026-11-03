/* F_11B6F: blit bitmap bm to the back buffer at (x, y), x rounded to the pixel grid g_vid.mask */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_11B6F(struct Bitmap far *bm, int x, int y)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x & g_vid.mask;
    pkt.dy = y;
    F_120FE(-3);
    (*g_78a2)(bm, g_vid.back, &pkt, 1);
    F_120FE(-1);
}
