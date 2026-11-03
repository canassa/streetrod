/* F_1A03F: shift a bitmap (pixels and mask) right by n pixels into a new, wider bitmap */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Bitmap far *F_1A03F(struct Bitmap far *src, int n, struct Bitmap far *dst)
{
    unsigned char acc1;
    int pitch;
    struct Bitmap far *out;
    unsigned char far *sp24;
    unsigned char acc2;
    int h3;
    unsigned char far *sp8;
    unsigned char far *dp25;
    int k;
    int compl;
    int i21;
    int extra8;
    unsigned char far *dp8;
    int shl5;
    int j2;
    int mask;

    shl5 = (n * g_vid.bpp) & 7;
    compl = 8 - shl5;
    mask = 0xff >> compl;
    h3 = src->height;
    pitch = src->pitch;
    if (n <= 0) {
        F_19ED0(src, dst, src->alt.p ? 3 : 1);
        return dst;
    }
    acc1 = 0xff;
    sp8 = (unsigned char far *)src->alt.p + pitch - 1;
    for (j2 = 0; j2 < h3; j2++) {
        acc1 &= *sp8;
        sp8 += pitch;
    }
    extra8 = (acc1 & mask) == mask ? 0 : 1 << g_vid.shift;
    out = F_0376B(src->width + extra8, src->height, src->alt.p ? 1 : 0, -1, dst);
    dp8 = (unsigned char far *)out->bits.p;
    sp8 = (unsigned char far *)src->bits.p;
    for (k = 0; k < h3 * g_vid.planes; k++) {
        for (i21 = 0, acc1 = 0; i21 < pitch; i21++) {
            *dp8++ = (*sp8 >> shl5) | (acc1 << compl);
            acc1 = mask & *sp8;
            sp8++;
        }
        if (extra8)
            *dp8++ = acc1 << compl;
    }
    if (src->alt.p) {
        dp25 = (unsigned char far *)out->alt.p;
        sp24 = (unsigned char far *)src->alt.p;
        for (k = 0; k < h3; k++) {
            for (i21 = 0, acc2 = 0xff; i21 < pitch; i21++) {
                *dp25++ = (*sp24 >> shl5) | (acc2 << compl);
                acc2 = mask & *sp24;
                sp24++;
            }
            if (extra8)
                *dp25++ = (acc2 << compl) | (0xff >> shl5);
        }
    }
    return out;
}
