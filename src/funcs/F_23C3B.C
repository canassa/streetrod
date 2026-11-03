/* F_23C3B @ 21A0:223B: copy a rectangle of a planar VGA bitmap to another, via a per-row 4 plane buffer */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

#pragma intrinsic(outp)

static void near F_2398D(unsigned char far *dst, unsigned char *src, int w, int off, int shift);

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

    {
        register int xb = r->sx / 8;

        a = bm1->bits.p + bm1->pitch * r->sy + xb;
        b = bm2->bits.p + bm2->pitch * r->dy + r->dx / 8;
        p = (r->sx + r->w - 1) / 8 - xb + 1;
    }
    f = r->h;
    d = bm1->planesize >> 2;
    outp(0x3ce, 5);
    outp(0x3cf, 0);
    j = r->dx & 7;
    h = j - (r->sx & 7);
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
