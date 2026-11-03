/* F_1D3C8: clipped blit between bitmaps, picking the plain or CGA-interleaved copy routine */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1D0C8(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Clip *o);

void F_1D3C8(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flags)
{
    int sp3;
    int f;
    int dp3;
    int skip;
    int dskip;
    int g8;
    struct Clip o;

    f = flags & 1;
    g8 = (flags & 2) && bm1->alt.p && bm2->alt.p ? 1 : 0;
    if (F_10A00(bm1, bm2, r, (struct Blt *)&o)) {
        if (bm1->flags < -1 && bm2->flags < -1)
            F_1C1D4(bm1->bits.p, bm2->bits.p, o.w, o.h, o.sx, o.sy, o.dx, o.dy);
        else if (bm1->flags < -1)
            F_1D0C8(bm1, bm2, &o);
        else {
            sp3 = bm1->pitch * o.sy + o.sx;
            skip = bm1->pitch - o.w;
            dskip = bm2->pitch - o.w;
            if (bm2->flags >= -1) {
                dp3 = bm2->pitch * o.dy + o.dx;
                if (f)
                    blit_rect(bm1->bits.p + sp3, bm2->bits.p + dp3, o.w, o.h, skip, dskip);
                if (g8)
                    blit_rect(bm1->alt.p + sp3, bm2->alt.p + dp3, o.w, o.h, skip, dskip);
            } else {
                dp3 = bm2->pitch * (o.dy >> 1) + o.dx;
                if (o.dy & 1)
                    dp3 += 0x2000;
                F_1C14C(bm1->bits.p + sp3, bm2->bits.p + dp3, o.w, o.h, skip, dskip, o.dy);
            }
        }
    }
}
