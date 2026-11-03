/* F_2398D @ 21A0:1F8D: write one 4 plane row (40 bytes per plane) to planar VGA memory, shifted and edge masked */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

static void near F_238A2(unsigned char *p, int n, int len);

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
