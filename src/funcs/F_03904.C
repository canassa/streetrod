#include "GLOBALS.H"
#include "PROTOS.H"

void F_03904(struct Bitmap far *bm, char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;

    pitch = ((w - 1) >> g_vid.shift) + 1;
    size = pitch * h * g_vid.planes;
    bm->bits.p = bits;
    bm->alt.p = alt;
    bm->planesize = size;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = flags;
}
