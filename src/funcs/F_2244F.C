/* F_2244F: replace one colour by another inside a rectangle of a 2-pixel-per-byte interlaced bitmap */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

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
