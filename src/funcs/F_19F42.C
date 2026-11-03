/* F_19F42: find the first and last set pixel column of a bitmap */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19F42(struct Bitmap far *bm, int *left, int *right)
{
    int a, f, d;
    unsigned char c, e, b;
    char far *g, far *h;

    a = bm->pitch;
    f = bm->height;
    h = (g = bm->alt.p) + a - 1;
    d = 0;
    c = e = 0xff;
    for (; d < f; d++) {
        c &= *g;
        e &= *h;
        g += a;
        h += a;
    }
    for (d = 0, b = 0x80; d < 8; d++, b >>= 1)
        if (!(c & b))
            break;
    *left = d / g_vid.bpp;
    for (d = 0, b = 1; d < 8; d++, b <<= 1)
        if (!(e & b))
            break;
    *right = (bm->pitch << g_vid.shift) - 1;
    *right -= d / g_vid.bpp;
}
