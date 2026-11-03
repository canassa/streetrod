/* F_19ED0: copy a bitmap into another through the blit hook */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19ED0(struct Bitmap far *src, struct Bitmap far *dst, int e)
{
    struct Blt pkt;

    F_0376B(src->width, src->height, dst->alt.p ? 1 : 0, -1, dst);
    pkt.w = src->width;
    pkt.h = src->height;
    pkt.dx = pkt.dy = pkt.sx = pkt.sy = 0;
    (*g_78a2)(src, dst, &pkt, e);
}
