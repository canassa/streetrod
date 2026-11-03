/* F_1A6E0: blit to a bitmap fetched by index, remember it, advance the clock */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1A6E0(struct Bitmap far *dst, int n, unsigned char x, unsigned char y)
{
    struct Bitmap far *p;

    p = F_15BBE(n, 2);
    F_1A69F(dst, p, x, y, 3);
    g_70a8 = p;
    F_19438(1);
}
