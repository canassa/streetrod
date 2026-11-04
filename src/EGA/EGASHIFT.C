#include <stdlib.h>
#include <conio.h>
#include "EGASHIFT.H"
#include "ADS.H"
#include "CARPIC.H"
#include "PC.H"

static void near shift_row_bits(unsigned char *p, int n, int len);
static void near write_plane_row(unsigned char far *dst, unsigned char *src, int w, int off, int shift);

/* EGA/VGA register writes (graphics controller, sequencer). Written as braced blocks: the
   block boundary is visible in ex_blit's code; every other pair in the file (and the 38
   such pairs of seven other files) compiles the same either way. */
#define GCREG(i, v)                                                                                                    \
    {                                                                                                                  \
        outp(GC_INDEX, (i));                                                                                           \
        outp(GC_DATA, (v));                                                                                            \
    }
#define SEQREG(i, v)                                                                                                   \
    {                                                                                                                  \
        outp(SEQ_INDEX, (i));                                                                                          \
        outp(SEQ_DATA, (v));                                                                                           \
    }

unsigned char g_leftBitMasks[8] = {128, 192, 224, 240, 248, 252, 254, 255};
int g_egaGcFunction = 0;

/* Shift a row of bytes right (n > 0) or left (n < 0) by n bits, carrying between bytes */

static void near shift_row_bits(unsigned char *p, int n, int len)
{
    unsigned char a;
    unsigned char b;
    int c;
    unsigned char d;
    int e;
    unsigned char f;

    if (n > 0) {
        f = g_leftBitMasks[n - 1];
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
        f = g_leftBitMasks[7 - n];
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

/* Write one 4 plane row (40 bytes per plane) to planar VGA memory, shifted and edge masked */

static void near write_plane_row(unsigned char far *dst, unsigned char *src, int w, int off, int shift)
{
    unsigned char *a, *b, *c, *d;
    int e;
    unsigned char f;
    unsigned char g;
    int h;

    if (shift) {
        for (e = 0, h = (w + 7) / 8; e < 4; e++)
            shift_row_bits(src + e * EGA_PITCH, shift, h);
    }
    a = src;
    b = src + EGA_PITCH;
    c = src + 80;
    d = src + 120;
    if (off) {
        g = ~g_leftBitMasks[off - 1];
        e = off + w;
        if (e < 8)
            g &= g_leftBitMasks[e - 1];
        GCREG(GC_BIT_MASK, g);
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE0);
        f = *dst;
        *dst = *a++ & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE1);
        f = *dst;
        *dst = *b++ & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE2);
        f = *dst;
        *dst = *c++ & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE3);
        f = *dst;
        *dst++ = *d++ & g;
        w -= 8 - off;
    }
    GCREG(GC_BIT_MASK, GC_MASK_ALL);
    if (w > 7) {
        int j;

        j = -w / -8;
        w += -j << 3;
        do {
            SEQREG(SEQ_MAP_MASK, SEQ_PLANE0);
            f = *dst;
            *dst = *a++;
            SEQREG(SEQ_MAP_MASK, SEQ_PLANE1);
            f = *dst;
            *dst = *b++;
            SEQREG(SEQ_MAP_MASK, SEQ_PLANE2);
            f = *dst;
            *dst = *c++;
            SEQREG(SEQ_MAP_MASK, SEQ_PLANE3);
            f = *dst;
            *dst = *d++;
            dst++;
        } while (--j != 0);
    }
    if (w > 0) {
        outp(GC_INDEX, GC_BIT_MASK);
        g = g_leftBitMasks[w - 1];
        outp(GC_DATA, g);
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE0);
        f = *dst;
        *dst = *a & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE1);
        f = *dst;
        *dst = *b & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE2);
        f = *dst;
        *dst = *c & g;
        SEQREG(SEQ_MAP_MASK, SEQ_PLANE3);
        f = *dst;
        *dst++ = *d & g;
    }
    GCREG(GC_BIT_MASK, GC_MASK_ALL);
    SEQREG(SEQ_MAP_MASK, SEQ_ALL_PLANES);
}

/* Copy a rectangle of a planar VGA bitmap to another, via a per-row 4 plane buffer */

void ex_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r)
{
    int p;                /* bytes per row */
    unsigned char far *a; /* source row */
    unsigned char far *b; /* destination row */
    unsigned char c[4][EGA_PITCH];
    unsigned d; /* plane size in bytes */
    int e;
    int f; /* rows */
    int g; /* byte in row */
    int h; /* shift */
    int i; /* plane */
    int j; /* destination bit offset */

    a = bm1->bits.p + bm1->pitch * r->sy + r->sx / 8;
    b = bm2->bits.p + bm2->pitch * r->dy + r->dx / 8;
    p = (r->sx + r->w - 1) / 8 - r->sx / 8 + 1;
    f = r->h;
    d = bm1->planesize >> 2;
    GCREG(GC_MODE, GC_WRITE_MODE0);
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
