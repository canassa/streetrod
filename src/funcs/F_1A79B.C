/* F_1A79B: and one bitmap rectangle into another */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1A79B(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in)
{
    struct Blt d;
    char far *a;
    char far *h;
    int b, c, e, f, g, i;

    if (F_10A00(src, dst, in, &d)) {
        c = d.w;
        e = d.h;
        b = src->pitch - c;
        i = dst->pitch - c;
        h = src->alt.p + src->pitch * d.sy + d.sx;
        a = dst->alt.p + dst->pitch * d.dy + d.dx;
        for (f = 0; f < e; f++) {
            for (g = 0; g < c; g++, a++, h++)
                *a &= *h;
            h += b;
            a += i;
        }
    }
}
