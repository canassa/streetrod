/* F_22364: render a glyph into a scratch buffer and copy it to the interleaved screen, 8 rows at most */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_22364(struct Bitmap far *bm, int x, int y, int glyph)
{
    char buf3[0x500];
    int m3;
    int n2;
    unsigned char c1;
    unsigned char c2;
    int h;
    int ph;

    ph = x & 1;
    memset(buf3, 0, 0x500);
    c1 = g_colors.c[0];
    c2 = g_colors.c[1];
    n2 = F_24BE2(x, glyph, buf3, c1, c2);
    g_824c = x + n2 - ph;
    g_824e = y;
    if (n2 != ph) {
        m3 = (n2 >> 1) - ph;
        if (y + (h = 8) > bm->height)
            h = bm->height - y;
        F_24CAB(buf3, bm->bits.w.seg,
            (bm->planesize >> 2) * (y & 3) + (y >> 2) * 160 + bm->bits.w.off + (x >> 1),
            y, h, m3, ph, n2);
    }
}
