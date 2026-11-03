/* F_21AAC: copy rows from a four-bank interleaved bitmap to a linear bitmap, one row per movedata */

#include <dos.h>
#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_21AAC(struct Bitmap far *src, struct Bitmap far *dst, struct Clip *blt)
{
    char far *basel;
    int end, pitch, x, w2, i, y;
    unsigned sg1[4];
    char far *s8;
    int off;

    x = blt->sx;
    y = blt->sy;
    end = blt->h + y;
    s8 = dst->bits.p + ((pitch = dst->pitch) * blt->dy + blt->dx);
    w2 = blt->w;
    basel = src->bits.p;
    off = (y >> 2) * 160;
    off += x;
    off += FP_OFF(basel);
    sg1[0] = FP_SEG(basel);
    sg1[1] = sg1[0] + 0x200;
    sg1[2] = sg1[1] + 0x200;
    sg1[3] = sg1[2] + 0x200;
    for (i = y; i < end; i++, s8 += pitch) {
        movedata(sg1[i & 3], off, FP_SEG(s8), FP_OFF(s8), w2);
        if ((i & 3) == 3)
            off += 160;
    }
}
