/* F_1A734: draw the bitmap with index n onto dst at (x, y) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1A734(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode)
{
    struct Blt blt;
    struct Bitmap far *bm;

    bm = F_15BBE(n, 2);
    blt.w = bm->width;
    blt.h = blt.fc = bm->height;
    blt.sx = blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    F_1A2D0(bm, dst, &blt, mode);
    F_19438(1);
}
