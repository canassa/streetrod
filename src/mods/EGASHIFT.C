#include <stdlib.h>
#include <conio.h>
#include "STRUCTS.H"
#include "EGASHIFT.H"

#pragma intrinsic(_rotl, _rotr, outp)

static void near shift_row_bits(unsigned char *p, int n, int len);
static void near write_plane_row(unsigned char far *dst, unsigned char *src, int w, int off, int shift);

/* EGA/VGA register writes (graphics controller, sequencer). Written as braced blocks: the
   block boundary is visible in egashift_blit's code; every other pair in the file (and the 38
   such pairs of seven other files) compiles the same either way. */
#define GCREG(i, v)  { outp(0x3ce, (i)); outp(0x3cf, (v)); }
#define SEQREG(i, v) { outp(0x3c4, (i)); outp(0x3c5, (v)); }

unsigned char g_5c16[8] = {128, 192, 224, 240, 248, 252, 254, 255};   /* DS:5C16 */
int g_5c1e = 0;   /* DS:5C1E */

/* shift_row_bits @ 21A0:1EA2 (linear 0x238A2, 235 bytes)

   Shift a row of bytes right (n > 0) or left (n < 0) by n bits, carrying between bytes */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near shift_row_bits(unsigned char *p, int n, int len)
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

/* write_plane_row @ 21A0:1F8D (linear 0x2398D, 686 bytes)

   Write one 4 plane row (40 bytes per plane) to planar VGA memory, shifted and edge masked */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near write_plane_row(unsigned char far *dst, unsigned char *src, int w, int off, int shift)
{
    unsigned char *a, *b, *c, *d;
    int e;
    unsigned char f;
    unsigned char g;
    int h;

    if (shift) {
        for (e = 0, h = (w + 7) / 8; e < 4; e++)
            shift_row_bits(src + e * 40, shift, h);
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
        GCREG(8, g);
        SEQREG(2, 1);
        f = *dst;
        *dst = *a++ & g;
        SEQREG(2, 2);
        f = *dst;
        *dst = *b++ & g;
        SEQREG(2, 4);
        f = *dst;
        *dst = *c++ & g;
        SEQREG(2, 8);
        f = *dst;
        *dst++ = *d++ & g;
        w -= 8 - off;
    }
    GCREG(8, 0xff);
    if (w > 7) {
        int j;

        j = -w / -8;
        w += -j << 3;
        do {
            SEQREG(2, 1);
            f = *dst;
            *dst = *a++;
            SEQREG(2, 2);
            f = *dst;
            *dst = *b++;
            SEQREG(2, 4);
            f = *dst;
            *dst = *c++;
            SEQREG(2, 8);
            f = *dst;
            *dst = *d++;
            dst++;
        } while (--j != 0);
    }
    if (w > 0) {
        outp(0x3ce, 8);
        g = g_5c16[w - 1];
        outp(0x3cf, g);
        SEQREG(2, 1);
        f = *dst;
        *dst = *a & g;
        SEQREG(2, 2);
        f = *dst;
        *dst = *b & g;
        SEQREG(2, 4);
        f = *dst;
        *dst = *c & g;
        SEQREG(2, 8);
        f = *dst;
        *dst++ = *d & g;
    }
    GCREG(8, 0xff);
    SEQREG(2, 0xf);
}

/* egashift_blit @ 21A0:223B (linear 0x23C3B, 383 bytes)

   Copy a rectangle of a planar VGA bitmap to another, via a per-row 4 plane buffer */
/* Local names are placeholders chosen to reproduce the original stack layout. */
/* The braces of GCREG(5, 0) are load-bearing: a block ends the basic block that holds the
   r->sx / 8 CSE in si, so /Ol does not give it a stack slot (the original has none). */

void egashift_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r)
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
    GCREG(5, 0);
    j = r->dx & 7;
    h = j - (r->sx & 7);
    for (e = 0; e < f; e++) {
        for (g = 0; g < p; g++) {
            for (i = 0; i < 4; i++)
                c[i][g] = *(a + g + i * d);
        }
        write_plane_row(b, (unsigned char *)c, r->w, j, h);
        a += bm1->pitch;
        b += bm2->pitch;
    }
}
