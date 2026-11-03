#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include <malloc.h>

static void near F_03614(int reset);

/* data_segment @ 0000:36A8 (linear 0x036A8, 22 bytes) */


int data_segment(void)
{
    struct SREGS segs;

    segread(&segs);
    return segs.ds;
}

static void near F_036BE(void)
{
    g_64a6 = 0x800;
    g_6c60 = data_segment();
    F_03614(1);
}

void F_036D6(void far *p)
{
    _ffree(p);
}

/* out_of_memory @ 0000:36E9 (linear 0x036E9, 32 bytes) */


void out_of_memory(void)
{
    char *msg;

    msg = "Not enough memory to run Street Rod !";
    F_1174E(msg);
    F_00FEA();
}

char far *F_03709(unsigned n, int fatal)
{
    char far *p;

    if (p = _fmalloc(n)) {
        if (FP_SEG(p) == g_6c60 ||
            (g_vid.mode == -6 && (unsigned long)(p + n) >= 0x80000000UL))
            _ffree(p);
        else
            return p;
    }
    if (fatal)
        out_of_memory();
    else
        return 0;
}

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
