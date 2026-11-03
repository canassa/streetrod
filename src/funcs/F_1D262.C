/* F_1D262: masked blit into a linear or CGA-interleaved bitmap through one of two assembly line routines */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

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
