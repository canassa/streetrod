/* F_234E9 @ 21A0:1AE9: masked blit between planar VGA bitmaps */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

void F_234E9(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3,
             struct Mask *mask, struct Blt *r)
{
    int p;                /* mask height */
    int a;                /* bm2 height */
    int b;                /* bm3 plane size */
    char far *c;          /* destination pointer */
    int d;                /* width */
    int e;                /* bm3 offset */
    int f;                /* bm2 plane size */
    int g;                /* bm1 pitch */
    int h;                /* bm3 pitch */
    struct Blt i;
    int j;                /* bm2 width */
    int K;                /* bm2 pitch */
    int k;                /* plane write mask */
    int L;                /* offset of the second bm1 plane */
    int l;                /* height */
    int M;                /* segment of bm1 */
    int m;                /* offset of bm1 */
    int N;                /* bm1 plane size */
    char far *n;          /* mask pointer */
    int o;                /* height below the mask */

    if (mask) {
        p = mask->h;
        n = mask->data + (r->sx >> 3);
        if (r->dx < 0)
            n += (7 - r->dx) >> 3;
    } else {
        p = 0;
        n = 0;
    }
    if (!F_10A00(bm1, bm2, r, &i))
        return;
    g = bm1->pitch;
    N = bm1->height * g;
    b = bm3->pitch * bm3->height;
    d = i.w;
    l = i.h;
    o = l - p;
    j = bm2->width;
    a = bm2->height;
    K = bm2->pitch;
    h = bm3->pitch;
    f = (bm2->flags == -2) ? 0 : a * K;
    c = bm2->bits.p + i.dy * K + i.dx;
    M = bm1->bits.w.seg;
    m = bm1->bits.w.off + i.sx + i.sy * g;
    L = bm1->alt.w.off + i.sx + i.sy * g;
    if (g == h)
        e = i.dy * h + bm3->bits.w.off;
    else
        e = i.sy * h + bm3->bits.w.off + i.dx;
    k = 1;
    do {
        if (f == 0) {
            outp(0x3c4, 2);
            outp(0x3c5, k);
        }
        F_24F4A(c, M, m, L, e, g, d, o, h, K);
        k <<= 1;
        c += f;
        m += N;
        e += b;
    } while (k < 9);
    if (f == 0) {
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
    }
    if (p > 0) {
        c += 40 * o;
        F_24EFB(n, d, p, c, g);
    }
}
