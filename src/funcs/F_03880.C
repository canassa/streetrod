#include "GLOBALS.H"
#include "PROTOS.H"

struct Bitmap far *F_03880(char far *bits, char far *alt, int w, int h, int flags)
{
    int pitch;
    int size;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> g_vid.shift) + 1;
    size = pitch * h * g_vid.planes;
    bm = (struct Bitmap far *)F_03709(0x30, 0);
    if (bm == 0)
        return 0;
    bm->bits.p = bits;
    bm->alt.p = alt;
    bm->planesize = size;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = flags;
    return bm;
}
