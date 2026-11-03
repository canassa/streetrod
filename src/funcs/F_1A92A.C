/* F_1A92A: copy a screen rectangle between two screens, clipped to 320x200 */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1A92A(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2)
{
    struct Blt pkt;

    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (x2 > 319)
        x2 = 319;
    if (y2 > 199)
        y2 = 199;
    pkt.w = ((x2 + ~g_vid.mask + 1) & g_vid.mask) - (x1 & g_vid.mask);
    pkt.h = y2 - y1 + 1;
    pkt.sx = pkt.dx = x1;
    pkt.sy = pkt.dy = y1;
    (*g_78a2)(dst, src, &pkt, 1);
}
