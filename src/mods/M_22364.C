#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <dos.h>

void (far *g_5b8a[15])() = {F_21EAB, F_221AC, F_21D58, F_22364, F_0E6C2, F_22609, F_2244F, F_0FEAD, F_0FEBF, F_22762, F_22626, F_19BFE, F_2487A, fill_span_clipped, F_24B79};   /* DS:5B8A */
int g_5bc6[5] = {0, 2, 4, 1, 3};   /* DS:5BC6 */

/* F_22364: render a glyph into a scratch buffer and copy it to the interleaved screen, 8 rows at most */
/* Local names are placeholders chosen to reproduce the original stack layout. */


void F_22364(struct Bitmap far *bm, int x, int y, int glyph)
{
    char buf3[0x500];
    int m3;
    int n2;
    unsigned char c1;
    unsigned char c2;
    int h;
    int ph;

    ph = x & 1;
    memset(buf3, 0, 0x500);
    c1 = x_colors.c[0];
    c2 = x_colors.c[1];
    n2 = F_24BE2(x, glyph, buf3, c1, c2);
    g824cd = x + n2 - ph;
    bq824e = y;
    if (n2 != ph) {
        m3 = (n2 >> 1) - ph;
        if (y + (h = 8) > bm->height)
            h = bm->height - y;
        F_24CAB(buf3, bm->bits.w.seg,
            (bm->planesize >> 2) * (y & 3) + (y >> 2) * 160 + bm->bits.w.off + (x >> 1),
            y, h, m3, ph, n2);
    }
}

/* F_2244F: replace one colour by another inside a rectangle of a 2-pixel-per-byte interlaced bitmap */
/* Local names are placeholders chosen to reproduce the original stack layout. */


void F_2244F(struct Bitmap far *bm, int from, int to, int x1, int y1, int x2, int y2)
{
    int pa2;
    unsigned char c;
    int mid;
    int row3;
    int xoddl;
    unsigned char lo1;
    unsigned char far *pl;
    unsigned char pair1;
    unsigned char hi2z;
    int tail3;
    unsigned char hi1k;
    int k2;
    int pbk;
    unsigned char lo2;
    unsigned char pair2;

    if (x1 < 0)
        x1 = 0;
    if (x2 < 0)
        x2 = 0;
    if (x2 > bm->width)
        x2 = bm->width;
    if (bm->height < y2)
        y2 = bm->height;
    lo2 = to & 0xf;
    hi2z = lo2 << 4;
    pair2 = lo2 | hi2z;
    lo1 = from & 0xf;
    hi1k = lo1 << 4;
    pair1 = lo1 | hi1k;
    tail3 = x2 & 1;
    mid = (x2 >> 1) - (x1 >> 1) - 1;
    xoddl = x1 & 1;
    if (xoddl == 0)
        mid++;
    pbk = 0x2000 - ((xoddl != 0) + mid);
    pa2 = 0xa0a0 - ((xoddl != 0) + mid);
    pl = (bm->planesize >> 2) * (y1 & 3) + bm->pitch * (y1 >> 2) + bm->bits.p + (x1 >> 1);
    for (row3 = y1; row3 < y2; row3++) {
        if (xoddl) {
            if ((*pl & 0xf) == lo1) {
                *pl &= 0xf0;
                *pl |= lo2;
                pl++;
            }
        }
        for (k2 = 0; k2 < mid; k2++) {
            if ((c = *pl) == pair1)
                *pl = pair2;
            else if ((c & 0xf) == lo1) {
                *pl &= 0xf0;
                *pl |= lo2;
            } else if ((c & 0xf0) == hi1k) {
                *pl &= 0xf;
                *pl |= hi2z;
            }
            pl++;
        }
        if (tail3) {
            if ((*pl & 0xf0) == hi1k) {
                *pl &= 0xf;
                *pl |= hi2z;
            }
        }
        pl += (row3 & 3) == 3 ? pa2 : pbk;
    }
}

/* F_22609: copy a 32K block (a full EGA/VGA-sized buffer) */


void F_22609(char far *src, char far *dst)
{
    F_1A882(src, dst, 0x8000);
}

/* F_22626: clear one source row, then copy the same row from the second screen: five interlaced passes over the lines */
/* kl: index over the five line phases (name chosen for the stack layout). */


void F_22626(char far *a, char far *b)
{
    char far *p1;
    long dl5;
    char far *p23;
    int far *e;
    int row;
    int kl;
    int i;

    for (kl = 0; kl < 5; kl++) {
        dl5 = g_tick + 5;
        for (row = g_5bc6[kl]; row < 200; row += 5) {
            p1 = a + (((row & 3) << 13) + (row >> 2) * 160);
            p23 = b + (((row & 3) << 13) + (row >> 2) * 160);
            for (e = (int far *)p1, i = 80; i; i--)
                *e++ = 0;
            movedata(FP_SEG(p23), FP_OFF(p23), FP_SEG(p1), FP_OFF(p1), 160);
        }
        while (g_tick < dl5)
            ;
    }
}
