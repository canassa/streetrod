#include <conio.h>
#include "STRUCTS.H"
#include "EGARECOL.H"

#pragma intrinsic(outp)

/* F_22F08 @ 21A0:1508 (linear 0x22F08, 468 bytes)

   Fill a rectangle in a planar VGA bitmap using the bit mask register */

void F_22F08(struct Bitmap far *bm, int mode, char color,
             int x1, int y1, int x2, int y2)
{
    unsigned char e = 0xff;
    unsigned char o = 0xff;
    int d = x1;
    int f = y1;
    int c = (d + 7) & ~7;
    int m = x2 & ~7;
    char i;
    int k;
    int g;
    int h;
    unsigned char far *a;
    unsigned char far *b;
    unsigned char j;
    int l;

    i = bm->flags <= -1;
    if (x2 > bm->width)
        x2 = m = bm->width;
    if (bm->height < y2)
        y2 = bm->height;
    e >>= d - c + 8;
    o <<= m - x2 + 8;
    k = (c < m) ? (m - c) >> 3 : 0;
    if (k == 0 && d >= m) {
        g = e & o;
        if (g) {
            e = g;
            o = 0;
        }
    }
    outp(0x3ce, 5);
    outp(0x3cf, 10);
    outp(0x3ce, 2);
    outp(0x3cf, mode);
    outp(0x3c4, 2);
    outp(0x3c5, 15);
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    g = y2 - f;
    a = bm->bits.p + bm->pitch * f + (d >> 3);
    l = bm->pitch;
    for (; g != 0; g--, a += l) {
        b = a;
        if (e) {
            j = *b & e;
            if (j) {
                outp(0x3cf, j);
                *b = color;
            }
            b++;
        }
        h = k;
        while (h != 0) {
            j = *b;
            if (j) {
                outp(0x3cf, j);
                *b = color;
            }
            h--;
            b++;
        }
        if (o) {
            j = *b & o;
            if (j) {
                outp(0x3cf, j);
                *b = color;
            }
        }
    }
    outp(0x3ce, 8);
    outp(0x3cf, 0xff);
    outp(0x3ce, 5);
    outp(0x3cf, 0);
}
