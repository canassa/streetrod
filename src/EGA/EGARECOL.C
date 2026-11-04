#include <conio.h>
#include "EGARECOL.H"
#include "CARPIC.H"
#include "PC.H"

/* Fill a rectangle in a planar VGA bitmap using the bit mask register */

void er_rect(struct Bitmap far *bm, int mode, char color, int x1, int y1, int x2, int y2)
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
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, (GC_READ_MODE1 | GC_WRITE_MODE2));
    outp(GC_INDEX, GC_COLOR_COMPARE);
    outp(GC_DATA, mode);
    outp(SEQ_INDEX, SEQ_MAP_MASK);
    outp(SEQ_DATA, SEQ_ALL_PLANES);
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    g = y2 - f;
    a = bm->bits.p + bm->pitch * f + (d >> 3);
    l = bm->pitch;
    for (; g != 0; g--, a += l) {
        b = a;
        if (e) {
            j = *b & e;
            if (j) {
                outp(GC_DATA, j);
                *b = color;
            }
            b++;
        }
        h = k;
        while (h != 0) {
            j = *b;
            if (j) {
                outp(GC_DATA, j);
                *b = color;
            }
            h--;
            b++;
        }
        if (o) {
            j = *b & o;
            if (j) {
                outp(GC_DATA, j);
                *b = color;
            }
        }
    }
    outp(GC_INDEX, GC_BIT_MASK);
    outp(GC_DATA, GC_MASK_ALL);
    outp(GC_INDEX, GC_MODE);
    outp(GC_DATA, GC_WRITE_MODE0);
}
