/* F_13682: fill the rectangle (x, y, w, h) of bitmap s with colour g_8244, clipped to its size,
   one horizontal line at a time through the line routine g_78d2 */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_13682(struct Bitmap far *s, int x, int y, int w, int h)
{
    int col;
    unsigned oldseg;
    int j;
    unsigned oldoff;
    int x2;

    if (x + w > s->width)
        w = s->width - x;
    if (y + h > s->height)
        h = s->height - y;
    oldseg = g_787a;
    oldoff = g_767c;
    g_787a = s->bits.w.seg;
    g_767c = s->bits.w.off;
    col = g_8244;
    x2 = x + w - 1;
    for (j = 0; j < h; j++)
        (*g_78d2)(x, y + j, x2, y + j, col);
    g_787a = oldseg;
    g_767c = oldoff;
}
