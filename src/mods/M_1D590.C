#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1D590(unsigned char *p, int n, int cnt);
static void near F_1D683(char far *dst, unsigned char *src, int n, int ph);

/* F_1D590: shift a row of bytes by n pixel pairs (2 bits each), carrying the bits across bytes */


#pragma intrinsic(_rotl, _rotr)

unsigned char g_5b52[4] = {192, 240, 252, 255};   /* DS:5B52 */

static void near F_1D590(unsigned char *p, int n, int cnt)
{
    unsigned char carry;
    int r2;
    unsigned char t;
    int k;
    unsigned char nm;
    unsigned char m;

    if (n > 0) {
        m = g_5b52[n - 1];
        nm = ~m;
        for (k = 0, carry = 0; k < cnt; k++) {
            r2 = _rotr(*p, n << 1);
            t = (r2 >> 8) | r2;
            *p++ = (t & nm) | carry;
            carry = t & m;
        }
    } else if (n < 0) {
        p += cnt - 1;
        n = -n;
        m = g_5b52[3 - n];
        nm = ~m;
        for (k = 0, carry = 0; k < cnt; k++) {
            r2 = _rotl(*p, n << 1);
            t = (r2 >> 8) | r2;
            *p-- = (t & m) | carry;
            carry = t & nm;
        }
    }
}

/* F_1D683: copy n pixel pairs from src into a byte row, keeping the bits outside the range */


static void near F_1D683(char far *dst, unsigned char *src, int n, int ph)
{
    unsigned char *s;
    int e;
    unsigned char m;

    s = src;
    if (ph) {
        m = g_5b52[ph - 1];
        e = ph + n;
        if (e < 4)
            m |= ~g_5b52[e - 1];
        *dst &= m;
        *dst |= *s++ & ~m;
        dst++;
        n -= 4 - ph;
    }
    for (; n > 3; n -= 4)
        *dst++ = *s++;
    if (n > 0) {
        m = g_5b52[n - 1];
        *dst &= ~m;
        *dst |= *s & m;
    }
}

/* F_1D741: blit a rectangle between 4-pixel-per-byte bitmaps, shifting each row through a buffer */
/* Local names are placeholders chosen to reproduce the original stack layout. */



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
