/* F_1371C: vertical line of height h at (x, y) in bitmap s with colour g_colors.c[3], clipped to the bitmap */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1371C(struct Bitmap far *s, int x, int y, int h)
{
    unsigned oldseg;
    unsigned oldoff;

    if (h + y > s->height)
        h = s->height - y;
    oldseg = g_787a;
    oldoff = g_767c;
    g_787a = s->bits.w.seg;
    g_767c = s->bits.w.off;
    (*g_78d2)(x, y, x, y + h - 1, g_colors.c[3]);
    g_787a = oldseg;
    g_767c = oldoff;
}
