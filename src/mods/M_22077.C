#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_22077(unsigned char *p, int dir, int n);
static void near F_2212C(char far *dst, char *src, int n, int odd);

/* F_22077: shift a run of packed nibbles by one nibble, forwards or backwards */


#pragma intrinsic(_rotr, _rotl)

static void near F_22077(unsigned char *p, int dir, int n)
{
    unsigned char carry;
    unsigned char c;
    unsigned rot;
    int i;

    if (dir > 0) {
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotr(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p++ = (c & 0x0f) | carry;
            carry = c & 0xf0;
        }
    } else if (dir < 0) {
        p += n - 1;
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotl(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p-- = (c & 0xf0) | carry;
            carry = c & 0x0f;
        }
    }
}

/* F_2212C: copy n packed nibbles to a far buffer, starting at the low nibble when odd */


static void near F_2212C(char far *dst, char *src, int n, int odd)
{
    unsigned char *s;

    s = src;
    if (odd) {
        *dst &= 0xf0;
        *dst |= *s++ & 0x0f;
        dst++;
        n--;
    }
    for (; n > 1; n -= 2)
        *dst++ = *s++;
    if (n > 0) {
        *dst &= 0x0f;
        *dst |= *s & 0xf0;
    }
}

/* F_221AC: copy rows between 2-pixel-per-byte bitmaps, shifting each row through a buffer by a nibble */
/* Local names are placeholders chosen to reproduce the original stack layout. */



void F_221AC(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt)
{
    int n2;
    int w9;
    char far *dn[4];
    unsigned char buf6[160];
    char far *c;
    unsigned char far *s;
    int h1;
    int i;
    int sh;
    int sx;
    int off;
    int dx;
    int k1;
    int ph4;
    int sy;
    int off2;
    int dy;
    sx = blt->sx;
    sy = blt->sy;
    w9 = blt->w;
    h1 = blt->h;
    dx = blt->dx;
    dy = blt->dy;
    s = src->bits.p + src->pitch * sy + (sx >> 1);
    dn[0] = dst->bits.p;
    dn[1] = dn[0] + (dst->planesize >> 2);
    dn[2] = dn[1] + (dst->planesize >> 2);
    dn[3] = dn[2] + (dst->planesize >> 2);
    off2 = dst->pitch * (dy >> 2) + (dx >> 1);
    n2 = ((sx + w9 - 1) >> 1) - (sx >> 1) + 1;
    ph4 = dx & 1;
    sh = ph4 - (sx & 1);
    for (i = 0; i < h1; i++, s += src->pitch) {
        for (off = 0; off < n2; off++)
            buf6[off] = s[off];
        k1 = (i + dy) & 3;
        c = dn[k1] + off2;
        if (k1 == 3)
            off2 += dst->pitch;
        if (sh)
            F_22077(buf6, sh, n2);
        F_2212C(c, buf6, w9, ph4);
    }
}
