/* F_1D741: blit a rectangle between 4-pixel-per-byte bitmaps, shifting each row through a buffer */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1D590(unsigned char *p, int n, int cnt);
static void near F_1D683(char far *dst, unsigned char *src, int n, int ph);

void F_1D741(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt)
{
    int n2;
    int w9;
    char far *c;
    unsigned char far *s;
    char far *d1;
    unsigned char buf9[80];
    char far *d2;
    int h1;
    int i;
    int sh;
    int sx;
    int off;
    int dx;
    int ph4;
    int sy;
    int dy;

    sx = blt->sx;
    sy = blt->sy;
    w9 = blt->w;
    h1 = blt->h;
    dx = blt->dx;
    dy = blt->dy;
    s = src->bits.p + src->pitch * sy + sx / 4;
    d1 = dst->bits.p + dst->pitch * (dy >> 1) + dx / 4;
    d2 = d1 + (dst->planesize >> 1);
    n2 = (sx + w9 - 1) / 4 - sx / 4 + 1;
    ph4 = dx & 3;
    sh = ph4 - (sx & 3);
    for (i = 0; i < h1; i++, s += src->pitch) {
        for (off = 0; off < n2; off++)
            buf9[off] = s[off];
        if ((i + dy) & 1) {
            c = d2;
            d2 += dst->pitch;
            d1 += dst->pitch;
        } else
            c = d1;
        if (sh)
            F_1D590(buf9, sh, n2);
        F_1D683(c, buf9, w9, ph4);
    }
}
