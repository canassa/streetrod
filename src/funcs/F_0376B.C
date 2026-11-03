#include "GLOBALS.H"
#include "PROTOS.H"

struct Bitmap far *F_0376B(int w, int h, int flags, int c, struct Bitmap far *mem)
{
    int pitch;
    int extra6;
    int hdrl;
    unsigned size;
    unsigned planesize;
    struct Bitmap far *bm;

    pitch = ((w - 1) >> g_vid.shift) + 1;
    size = pitch * h;
    planesize = size * g_vid.planes;
    hdrl = 0x30;
    extra6 = 0;
    if (flags)
        extra6 = size;
    if (!(flags & 2))
        hdrl += planesize;
    if (mem == 0) {
        bm = (struct Bitmap far *)F_03709(hdrl + extra6, 0);
        if (bm == 0)
            return 0;
        bm->bits.p = (char far *)bm + 0x30;
    } else {
        bm = mem;
    }
    if (flags & 2) {
        if (bm->bits.p) {
            bm->alt.p = bm->bits.p;
            bm->bits.p = 0;
        } else {
            bm->alt.p = (char far *)bm + 0x30;
        }
    } else {
        bm->alt.p = flags ? bm->bits.p + planesize : 0;
    }
    bm->planesize = planesize;
    bm->width = w;
    bm->height = h;
    bm->pitch = pitch;
    bm->flags = c;
    return bm;
}
