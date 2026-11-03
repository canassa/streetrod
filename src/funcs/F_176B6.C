/* F_176B6: copy n bitmaps into the table g_6e68, each filled with a colour pattern and h rows high, in space cut from the scratch bitmap g_731e */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_176B6(struct Bitmap far **B, int n, int y2, int h, int color)
{
    struct Blt pkt1;
    struct Bitmap far *bm1;
    int v;
    int i;
    char far *base2;
    int H5;
    int maxw;

    base2 = g_731e->bits.p + 0x46 * g_731e->pitch;
    H5 = B[0]->height - y2 - h;
    v = (g_vid.mode == -6 ? color << 4 : color << 2 | color << 4 | color << 6) | color;
    maxw = B[0]->width;
    for (i = 1; i < n; i++)
        if (B[i]->width > maxw)
            maxw = B[i]->width;
    bm1 = F_1934E(maxw, h, 0, 2);
    pkt1.w = maxw;
    pkt1.h = h;
    pkt1.sy = H5;
    i = pkt1.sx = pkt1.dx = pkt1.dy = 0;
    for (i = 0; i < n; i++) {
        g_6e68[i] = *bm1;
        g_6e68[i].bits.p = base2;
        base2 += bm1->planesize;
        F_109C0(bm1->bits.p, v, bm1->planesize);
        (*g_78aa)(B[i], bm1, &pkt1, 1);
        F_19ED0(bm1, &g_6e68[i], 1);
    }
    F_19438(1);
}
