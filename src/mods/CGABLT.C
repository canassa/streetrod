#include <dos.h>
#include <memory.h>
#include "STRUCTS.H"
#include "CGABLT.H"
#include "CGADRV.H"
#include "MASKBG.H"
#include "MASKBLT.H"
#include "MASKOR.H"
#include "M_0F38A.H"

static void near F_1D0C8(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Clip *o);

/* F_1D0C8 @ 1C11:0FB8 (linear 0x1D0C8, 205 bytes)

   Copy a clipped rectangle from an interleaved (CGA) bitmap to a linear one */

static void near F_1D0C8(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Clip *o)
{
    unsigned seg1;
    int yend1;
    unsigned seg2;
    char far *sb;
    int x1;
    int y1;
    int dpitchn;
    char far *d7;
    int off;
    int w4;
    int y2;

    x1 = o->sx;
    y1 = o->sy;
    yend1 = o->h + y1;
    dpitchn = bm2->pitch;
    d7 = bm2->bits.p + (bm2->pitch * o->dy + o->dx);
    w4 = o->w;
    sb = bm1->bits.p;
    off = (y1 >> 1) * 0x50;
    off += x1;
    off += (unsigned)sb;
    seg1 = FP_SEG(sb);
    seg2 = seg1 + 0x200;
    for (y2 = y1; y2 < yend1; y2++, d7 += dpitchn) {
        if (y2 & 1) {
            movedata(seg2, off, FP_SEG(d7), FP_OFF(d7), w4);
            off += 0x50;
        } else
            movedata(seg1, off, FP_SEG(d7), FP_OFF(d7), w4);
    }
}

/* F_1D195 @ 1C11:1085 (linear 0x1D195, 205 bytes)

   Masked blit between three bitmaps, clipped, through the assembly blitter */

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

/* F_1D262 @ 1C11:1152 (linear 0x1D262, 358 bytes)

   Masked blit into a linear or CGA-interleaved bitmap through one of two assembly line routines */

void F_1D262(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag)
{
    int off05;
    char far *db;
    int w;
    int off1k;
    int pitch1;
    int h3;
    int pitch21;
    int rows0;
    void (far *f5)(char far *, unsigned, int, int, int, int, int, int);
    struct Blt ol;
    int rows1;
    int ak;
    int bitsz;
    int altk;
    int dy;
    int segn;
    int soff;

    if (!F_10A00(bm1, bm2, r, &ol))
        return;
    pitch1 = bm1->pitch;
    w = ol.w;
    h3 = ol.h;
    dy = ol.dy;
    pitch21 = bm2->pitch;
    db = bm2->bits.p;
    segn = bm1->bits.w.seg;
    bitsz = (soff = ol.sy * pitch1 + ol.sx) + bm1->bits.w.off;
    altk = bm1->alt.w.off + soff;
    f5 = flag ? F_25954 : F_2634A;
    if (bm2->flags >= -1) {
        (*f5)(db + (pitch21 * dy + ol.dx), segn, bitsz, altk, pitch1, w, h3, pitch21);
    } else {
        rows0 = rows1 = h3 >> 1;
        if (h3 & 1)
            rows0++;
        ak = (dy >> 1) * pitch21 + ol.dx;
        if (dy & 1) {
            off05 = ak + 0x2000;
            off1k = pitch21 + ak;
        } else {
            off05 = ak;
            off1k = ak + 0x2000;
        }
        (*f5)(db + off05, segn, bitsz, altk, pitch1 * 2, w, rows0, pitch21);
        bitsz += pitch1;
        altk += pitch1;
        (*f5)(db + off1k, segn, bitsz, altk, pitch1 * 2, w, rows1, pitch21);
    }
}

/* F_1D3C8 @ 1C11:12B8 (linear 0x1D3C8, 456 bytes)

   Clipped blit between bitmaps, picking the plain or CGA-interleaved copy routine */

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
