/* F_21C0B: masked blit between interleaved-bank bitmaps, one F_24F4A call per bank */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_21B7C(int x, int x0, int y, int *b, int *a);

void F_21C0B(struct Bitmap far *dst, struct Bitmap far *src, struct Bitmap far *mask, struct Blt *in)
{
    int seg2;
    int a[4];
    int k6;
    union FarPtr alt_a;
    struct Clip clip9;
    int p1;
    int p2o;
    int pcn;
    union FarPtr bits_cm;
    int pa;
    unsigned off6;
    int wz;
    union FarPtr bits_bn;
    union FarPtr bits_a;
    int b[4];
    unsigned di;

    bits_a = dst->bits;
    alt_a = dst->alt;
    bits_bn = src->bits;
    bits_cm = mask->bits;
    if (!F_10A00(dst, src, in, (struct Blt *)&clip9))
        return;
    pa = dst->pitch;
    wz = clip9.w;
    pcn = mask->pitch;
    seg2 = bits_a.w.seg;
    off6 = clip9.sy * pa + clip9.sx;
    p1 = bits_a.w.off + off6;
    p2o = alt_a.w.off + off6;
    di = (in->my * pcn + (in->mx >> 1)) + bits_cm.w.off;
    F_21B7C(clip9.h, clip9.dx, clip9.dy, b, a);
    for (k6 = 0; k6 < 4; k6++) {
        F_24F4A(bits_bn.p + b[k6], seg2, p1, p2o, di, pa * 4, wz, a[k6], pcn * 4, 0xa0);
        p1 += pa;
        p2o += pa;
        di += pcn;
    }
}
