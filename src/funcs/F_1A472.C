/* F_1A472: draw a masked bitmap into another at pixel (x, y), pre-shifting it to the byte phase */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_1A472(struct Bitmap far *src, struct Bitmap far *dst, int x, int y)
{
    unsigned char far *sm;
    int dpitch4;
    int sh5;
    unsigned char far *spn;
    unsigned char far *dm;
    int pstep5;
    int dskip6;
    int off6;
    unsigned char far *sm1;
    int r;
    unsigned char far *dm1;
    int h;
    int c8;
    struct Bitmap far *t7;
    int spitch;
    int pn;
    unsigned char far *dpk;

    sh5 = x & ~g_vid.mask;
    if (sh5 > 0) {
        t7 = F_1934E(src->width + 8, src->height, src->alt.p != 0L, 2);
        F_1A03F(src, sh5, t7);
    } else
        t7 = src;
    dpitch4 = dst->pitch;
    spitch = t7->pitch;
    dskip6 = dpitch4 - spitch;
    pstep5 = (dst->height - t7->height) * dpitch4;
    h = t7->height;
    off6 = y * dpitch4 + (x >> g_vid.shift);
    dpk = (unsigned char far *)dst->bits.p + off6;
    dm1 = dm = (unsigned char far *)dst->alt.p + off6;
    spn = (unsigned char far *)t7->bits.p;
    sm1 = sm = (unsigned char far *)t7->alt.p;
    for (pn = 0; pn < g_vid.planes; pn++) {
        for (r = 0; r < h; r++) {
            for (c8 = 0; c8 < spitch; c8++)
                *dpk++ |= ~*sm1++ & *spn++ & *dm1++;
            dpk += dskip6;
            dm1 += dskip6;
        }
        dpk += pstep5;
        dm1 = dm;
        sm1 = sm;
    }
    if (!g_5aa6) {
        for (r = 0, dm1 = dm, sm1 = sm; r < h; r++) {
            for (c8 = 0; c8 < spitch; c8++)
                *dm1++ &= *sm1++;
            dm1 += dskip6;
        }
    }
    if (sh5)
        F_19438(1);
}
