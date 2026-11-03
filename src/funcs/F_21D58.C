/* F_21D58: blit between bitmaps through the blitter selected by flag, in banks when the destination is interleaved */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_21B7C(int x, int x0, int y, int *b, int *a);

void F_21D58(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in, int flag)
{
    union FarPtr bits_b;
    int pa6;
    int w;
    void (far *fn4)(char far *, unsigned, int, int, int, int, int, int);
    struct Blt clip;
    int h;
    int pb9;
    int k;
    int off2;
    int al[4];
    int bl[4];
    int off3;
    int off04;
    int dy2;
    int seg;

    if (!F_10A00(src, dst, in, &clip))
        return;
    pa6 = src->pitch;
    w = clip.w;
    h = clip.h;
    dy2 = clip.dy;
    pb9 = dst->pitch;
    bits_b = dst->bits;
    seg = src->bits.w.seg;
    off3 = (off04 = clip.sy * pa6 + clip.sx) + src->bits.w.off;
    off2 = src->alt.w.off + off04;
    fn4 = flag ? F_25954 : F_2634A;
    if (dst->flags >= -1) {
        (*fn4)(bits_b.p + pb9 * dy2 + clip.dx, seg, off3, off2, pa6, w, h, pb9);
    } else {
        F_21B7C(clip.h, clip.dx, dy2, bl, al);
        for (k = 0; k < 4; k++) {
            (*fn4)(bits_b.p + bl[k], seg, off3, off2, pa6 * 4, w, al[k], 0xa0);
            off3 += pa6;
            off2 += pa6;
        }
    }
}
