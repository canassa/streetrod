/* F_23262 @ 21A0:1862: copy a rectangle between planar VGA bitmaps, plane by plane */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(inp, outp)

void F_23262(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *rect, int flags)
{
    int p;                /* source bitmap height */
    char far *a;          /* destination pointer */
    char far *b;          /* row copy: source */
    int c;                /* last plane + 1 */
    int d;                /* width */
    int e;                /* destination plane size */
    int f;                /* both planes are flat */
    int g;                /* source plane size */
    struct Blt h;
    int i;                /* source pitch */
    int j;                /* height */
    int k;                /* plane write mask */
    int L;
    int l;                /* current plane */
    int M;
    int N;
    char far *m;          /* source pointer */
    char far *n;          /* row copy: destination */
    int o;                /* first plane */

    N = (flags & 2) && dst->alt.p && src->alt.p;
    if (!F_10A00(dst, src, rect, &h))
        return;
    if (flags & 1) {
        m = dst->bits.p;
        a = src->bits.p;
        o = 0;
    } else {
        m = dst->alt.p;
        a = src->alt.p;
        o = 4;
    }
    c = N ? 5 : 4;
    M = (dst->flags == -2) ? 40 : dst->pitch;
    g = (dst->flags == -2) ? 0 : dst->height * M;
    m += h.sy * M + h.sx;
    p = src->height;
    i = (src->flags == -2) ? 40 : src->pitch;
    e = (src->flags == -2) ? 0 : p * i;
    a += h.dy * i + h.dx;
    d = h.w;
    j = h.h;
    if (src->flags == -2 && dst->flags == -2) {
        F_24E78(m, a, d, j, 0, 0, 40);
        return;
    }
    f = e == 0 || g == 0;
    if (f) {
        outp(0x3ce, 4);
        outp(0x3c4, 2);
    }
    k = 1;
    for (l = o; l < c; l++, k <<= 1, m += g, a += e) {
        if (f) {
            if (g == 0)
                outp(0x3cf, l);
            if (e == 0)
                outp(0x3c5, k);
        }
        if (d == i && M == d) {
            movedata(FP_SEG(m), FP_OFF(m), FP_SEG(a), FP_OFF(a), d * j);
        } else {
            L = 0;
            b = m;
            n = a;
            for (; L < j; L++) {
                movedata(FP_SEG(b), FP_OFF(b), FP_SEG(n), FP_OFF(n), d);
                b += M;
                n += i;
            }
        }
    }
    if (f) {
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
    }
}
