/* F_1D0C8: copy a clipped rectangle from an interleaved (CGA) bitmap to a linear one */

#include <dos.h>
#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1D0C8(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Clip *o)
{
    unsigned seg1;
    int yend1;
    unsigned seg2;
    char far *sb;
    int x1;
    int y1;
    int dpitchn;
    char far *d7;
    int off;
    int w4;
    int y2;

    x1 = o->sx;
    y1 = o->sy;
    yend1 = o->h + y1;
    dpitchn = bm2->pitch;
    d7 = bm2->bits.p + (bm2->pitch * o->dy + o->dx);
    w4 = o->w;
    sb = bm1->bits.p;
    off = (y1 >> 1) * 0x50;
    off += x1;
    off += (unsigned)sb;
    seg1 = FP_SEG(sb);
    seg2 = seg1 + 0x200;
    for (y2 = y1; y2 < yend1; y2++, d7 += dpitchn) {
        if (y2 & 1) {
            movedata(seg2, off, FP_SEG(d7), FP_OFF(d7), w4);
            off += 0x50;
        } else
            movedata(seg1, off, FP_SEG(d7), FP_OFF(d7), w4);
    }
}
