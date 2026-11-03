#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <memory.h>
#include <dos.h>

/* F_1A79B: and one bitmap rectangle into another */


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

/* F_1A882 @ 0F38:B502 (linear 0x1a882, 28 bytes): far memory copy */


void F_1A882(char far *src, char far *dst, unsigned n)
{
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), n);
}

/* F_1A89E: grab the w x h area at (sx, sy) of a bitmap into a new bitmap */


struct Bitmap far *F_1A89E(struct Bitmap far *bm, struct Blt *r, int mode)
{
    struct Blt a;
    int b, c, d, e;
    struct Bitmap far *f;

    b = r->w;
    d = r->h;
    c = r->sx;
    e = r->sy;
    f = F_1934E(b, d, bm->alt.p != 0L, mode);
    a.w = b;
    a.h = d;
    a.sx = c;
    a.sy = e;
    a.dx = a.dy = 0;
    (*g_78a2)(bm, f, &a, 3);
    return f;
}

/* F_1A92A: copy a screen rectangle between two screens, clipped to 320x200 */


void F_1A92A(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2)
{
    struct Blt pkt;

    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (x2 > 319)
        x2 = 319;
    if (y2 > 199)
        y2 = 199;
    pkt.w = ((x2 + ~g_vid.mask + 1) & g_vid.mask) - (x1 & g_vid.mask);
    pkt.h = y2 - y1 + 1;
    pkt.sx = pkt.dx = x1;
    pkt.sy = pkt.dy = y1;
    (*g_78a2)(dst, src, &pkt, 1);
}
