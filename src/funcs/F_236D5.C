/* F_236D5 @ 21A0:1CD5: blit between planar VGA bitmaps through one of two hand-written plane routines */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

void F_236D5(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag)
{
    int p;                /* bm2 height */
    char far *a;          /* destination pointer */
    int b;                /* width */
    int c;                /* bm2 plane size */
    int d;                /* bm1 pitch */
    struct Blt e;
    void (far *f)(char far *, unsigned, int, int, int, int, int, int);
    int g;                /* bm2 pitch */
    int h;                /* height */
    int i;                /* offset of the second bm1 plane */
    int j;                /* offset of bm1 */
    int k;                /* plane write mask */
    int l;                /* segment of bm1 */
    int m;                /* current plane */
    int n;                /* bm1 plane size */

    if (!F_10A00(bm1, bm2, r, &e))
        return;
    d = bm1->pitch;
    b = e.w;
    h = e.h;
    n = bm1->height * d;
    p = bm2->height;
    g = (bm2->flags == -2) ? 40 : bm2->pitch;
    c = (bm2->flags == -2) ? 0 : p * g;
    a = bm2->bits.p + e.dy * g + e.dx;
    l = bm1->bits.w.seg;
    j = bm1->bits.w.off + e.sx + e.sy * d;
    i = bm1->alt.w.off + e.sx + e.sy * d;
    f = flag ? F_25954 : F_2634A;
    if (c == 0) {
        outp(0x3ce, 4);
        outp(0x3c4, 2);
    }
    k = 1;
    m = 0;
    for (; k < 9; k <<= 1, m++, j += n, a += c) {
        if (c == 0) {
            outp(0x3cf, m);
            outp(0x3c5, k);
        }
        (*f)(a, l, j, i, d, b, h, g);
    }
    if (c == 0) {
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
    }
}
