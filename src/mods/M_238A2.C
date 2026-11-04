#include <stdlib.h>
#include "STRUCTS.H"
#include "M_238A2.H"
#include <conio.h>

static void near F_238A2(unsigned char *p, int n, int len);
static void near F_2398D(unsigned char far *dst, unsigned char *src, int w, int off, int shift);

/* F_238A2 @ 21A0:1EA2: shift a row of bytes right (n > 0) or left (n < 0) by n bits, carrying between bytes */
/* Local names are placeholders chosen to reproduce the original stack layout. */


#pragma intrinsic(_rotl, _rotr)

unsigned char g_5c16[8] = {128, 192, 224, 240, 248, 252, 254, 255};   /* DS:5C16 */
int g_5c1e = 0;   /* DS:5C1E */

static void near F_238A2(unsigned char *p, int n, int len)
{
    unsigned char a;
    unsigned char b;
    int c;
    unsigned char d;
    int e;
    unsigned char f;

    if (n > 0) {
        f = g_5c16[n - 1];
        d = ~f;
        e = 0;
        a = 0;
        for (; e < len; e++) {
            c = _rotr(*p, n);
            b = (c >> 8) | c;
            *p++ = (b & d) | a;
            a = b & f;
        }
    } else if (n < 0) {
        p += len - 1;
        n = -n;
        f = g_5c16[7 - n];
        d = ~f;
        e = 0;
        a = 0;
        for (; e < len; e++) {
            c = _rotl(*p, n);
            b = (c >> 8) | c;
            *p-- = (b & f) | a;
            a = ~f & b;
        }
    }
}

/* F_2398D @ 21A0:1F8D: write one 4 plane row (40 bytes per plane) to planar VGA memory, shifted and edge masked */
/* Local names are placeholders chosen to reproduce the original stack layout. */


#pragma intrinsic(outp)


static void near F_2398D(unsigned char far *dst, unsigned char *src, int w, int off, int shift)
{
    unsigned char *a, *b, *c, *d;
    int e;
    unsigned char f;
    unsigned char g;
    int h;

    if (shift) {
        for (e = 0, h = (w + 7) / 8; e < 4; e++)
            F_238A2(src + e * 40, shift, h);
    }
    a = src;
    b = src + 40;
    c = src + 80;
    d = src + 120;
    if (off) {
        g = ~g_5c16[off - 1];
        e = off + w;
        if (e < 8)
            g &= g_5c16[e - 1];
        outp(0x3ce, 8);
        outp(0x3cf, g);
        outp(0x3c4, 2);
        outp(0x3c5, 1);
        f = *dst;
        *dst = *a++ & g;
        outp(0x3c4, 2);
        outp(0x3c5, 2);
        f = *dst;
        *dst = *b++ & g;
        outp(0x3c4, 2);
        outp(0x3c5, 4);
        f = *dst;
        *dst = *c++ & g;
        outp(0x3c4, 2);
        outp(0x3c5, 8);
        f = *dst;
        *dst++ = *d++ & g;
        w -= 8 - off;
    }
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    if (w > 7) {
        int j;

        j = -w / -8;
        w += -j << 3;
        do {
            outp(0x3c4, 2);
            outp(0x3c5, 1);
            f = *dst;
            *dst = *a++;
            outp(0x3c4, 2);
            outp(0x3c5, 2);
            f = *dst;
            *dst = *b++;
            outp(0x3c4, 2);
            outp(0x3c5, 4);
            f = *dst;
            *dst = *c++;
            outp(0x3c4, 2);
            outp(0x3c5, 8);
            f = *dst;
            *dst = *d++;
            dst++;
        } while (--j != 0);
    }
    if (w > 0) {
        outp(0x3ce, 8);
        g = g_5c16[w - 1];
        outp(0x3cf, g);
        outp(0x3c4, 2);
        outp(0x3c5, 1);
        f = *dst;
        *dst = *a & g;
        outp(0x3c4, 2);
        outp(0x3c5, 2);
        f = *dst;
        *dst = *b & g;
        outp(0x3c4, 2);
        outp(0x3c5, 4);
        f = *dst;
        *dst = *c & g;
        outp(0x3c4, 2);
        outp(0x3c5, 8);
        f = *dst;
        *dst++ = *d & g;
    }
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    outp(0x3c4, 2);
    outp(0x3c5, 0xf);
}

/* F_23C3B @ 21A0:223B: copy a rectangle of a planar VGA bitmap to another, via a per-row 4 plane buffer */
/* Local names are placeholders chosen to reproduce the original stack layout. */
/* The empty if before the loop is load-bearing: it leaves no code but ends the basic block that
   holds the r->sx / 8 CSE in si, so /Ol does not give it a stack slot (the original has none).
   The condition is a guess; a disabled debug check on the row width (c[] rows hold 40 bytes). */


#pragma intrinsic(outp)


void F_23C3B(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r)
{
    int p;                 /* bytes per row */
    unsigned char far *a;  /* source row */
    unsigned char far *b;  /* destination row */
    unsigned char c[4][40];
    unsigned d;            /* plane size in bytes */
    int e;
    int f;                 /* rows */
    int g;                 /* byte in row */
    int h;                 /* shift */
    int i;                 /* plane */
    int j;                 /* destination bit offset */

    a = bm1->bits.p + bm1->pitch * r->sy + r->sx / 8;
    b = bm2->bits.p + bm2->pitch * r->dy + r->dx / 8;
    p = (r->sx + r->w - 1) / 8 - r->sx / 8 + 1;
    f = r->h;
    d = bm1->planesize >> 2;
    outp(0x3ce, 5);
    outp(0x3cf, 0);
    j = r->dx & 7;
    h = j - (r->sx & 7);
    if (p > 40)
        ;
    for (e = 0; e < f; e++) {
        for (g = 0; g < p; g++) {
            for (i = 0; i < 4; i++)
                c[i][g] = *(a + g + i * d);
        }
        F_2398D(b, (unsigned char *)c, r->w, j, h);
        a += bm1->pitch;
        b += bm2->pitch;
    }
}
