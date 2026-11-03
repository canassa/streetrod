/* F_1D195: masked blit between three bitmaps, clipped, through the assembly blitter */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1D195(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct Blt *r)
{
    char far *db;
    int soff;
    char far *sbz;
    char far *sak;
    struct Clip out7;
    int doff;
    char far *mb;

    sbz = bm1->bits.p;
    sak = bm1->alt.p;
    db = bm2->bits.p;
    mb = bm3->bits.p;
    if (F_10A00(bm1, bm2, r, (struct Blt *)&out7)) {
        soff = bm1->pitch * out7.sy + out7.sx;
        doff = bm2->pitch * out7.dy + out7.dx;
        F_24F4A(db + doff, FP_SEG(sbz), (unsigned)sbz + soff, (unsigned)sak + soff, (unsigned)mb + doff,
                bm1->pitch, out7.w, out7.h, bm3->pitch, bm2->pitch);
    }
}
