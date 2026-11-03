/* F_1934E: allocate a bitmap w x h from the arena (header plus pixel rows) and initialise it */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Bitmap far *F_1934E(int w, int h, int flags, int mode)
{
    int cols, extra, total;
    unsigned size;
    struct Bitmap far *mem;
    struct Bitmap far *bm;

    cols = ((w - 1) >> g_vid.shift) + 1;
    size = cols * h;
    extra = 0;
    if (flags)
        extra = size;
    total = extra + 0x30;
    if (!(flags & 2))
        total += size * g_vid.planes;
    bm = mem = (struct Bitmap far *)F_192AC(total, mode);
    bm->bits.p = (char far *)mem + 0x30;
    F_0376B(w, h, flags, -1, bm);
    return bm;
}
