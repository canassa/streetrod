/* F_21EAB: clipped blit between bitmaps, picking the plain, interleaved or planar copy routine */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_21AAC(struct Bitmap far *src, struct Bitmap far *dst, struct Clip *o);

void F_21EAB(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *r, int flags)
{
    int sp3;
    int f;
    int dp3;
    int skip;
    int dskip;
    int g8;
    struct Clip o;

    f = flags & 1;
    g8 = (flags & 2) && src->alt.p && dst->alt.p;
    if (!F_10A00(src, dst, r, (struct Blt *)&o))
        return;
    if (src->flags < -1 && dst->flags < -1)
        F_24DC7(src->bits.p, dst->bits.p, o.w, o.h, o.sx, o.sy, o.dx, o.dy);
    else if (src->flags < -1)
        F_21AAC(src, dst, &o);
    else {
        sp3 = src->pitch * o.sy + o.sx;
        skip = src->pitch - o.w;
        dskip = dst->pitch - o.w;
        if (dst->flags >= -1) {
            dp3 = dst->pitch * o.dy + o.dx;
            if (f)
                blit_rect(src->bits.p + sp3, dst->bits.p + dp3, o.w, o.h, skip, dskip);
            if (g8)
                blit_rect(src->alt.p + sp3, dst->alt.p + dp3, o.w, o.h, skip, dskip);
        } else {
            dp3 = dst->pitch * (o.dy >> 2) + ((o.dy & 3) << 13) + o.dx;
            F_24D84(src->bits.p + sp3, dst->bits.p + dp3, o.w, o.h, skip, dskip, o.dy);
        }
    }
}
