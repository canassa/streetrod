/* F_1592E: unpack picture number id into the bitmap bm (a new one when bm is 0). The picture is
   loaded to the work area g_7564 first; its data is RLE coded (F_0E926) and expanded for the video
   mode in use into the planes bits and alt of the bitmap. Returns the bitmap */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

static int near F_1563A(int n, int load, char far *dst);
static int near F_153D7(int n);

struct Bitmap far *F_1592E(int id, struct Bitmap far *bm)
{
    int num;
    char far *src;
    unsigned char colors16[16];
    struct Slot far *cur;
    int j;
    int res;
    struct Bitmap far *pic;
    struct Slot far *sl;

    F_120FE(-4);
    res = F_1563A(id, 0, g_7564);
    cur = sl = &g_slots[F_153D7(id)];
    if (g_4f0e)
        pic = F_0376B(cur->f0, cur->f2, cur->f6, 1, bm);
    else
        pic = bm;
    for (j = 0; j < 16; j++)
        colors16[j] = cur->pal[j];
    if (sl->f1e != 0xff) {
        outp(0x3ce, 4);
        outp(0x3cf, sl->f1e);
    }
    src = sl->ptr.p + 1;
    num = cur->f1c - 1;
    if (g_vid.mode == -2) {
        if (pic->bits.p == 0) {
            F_0E926(src, pic->alt.p, num, colors16, 16);
        } else {
            F_0E926(src, pic->bits.p, num, colors16, 16);
            if (pic->alt.p)
                F_26401(pic->bits.p, pic->alt.p, pic->planesize);
        }
    } else if (g_vid.mode == -6) {
        if (pic->bits.p == 0) {
            F_24740(src, pic->alt.p, pic->planesize, colors16);
        } else {
            F_2467A(src, pic->bits.p, pic->planesize, colors16);
            if (pic->alt.p)
                F_24849(pic->bits.p, pic->alt.p, pic->planesize);
        }
    } else {
        F_0E926(src, pic->bits.p == 0 ? pic->alt.p : pic->bits.p, num, colors16, 16);
    }
    if (sl->f1e != 0xff) {
        outp(0x3c4, 2);
        outp(0x3c5, 0xf);
    }
    if (res == 2)
        sl->ptr.p = 0;
    F_120FE(-2);
    return pic;
}
