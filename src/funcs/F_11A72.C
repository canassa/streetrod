/* F_11A72: copy the rectangle (x, y, w, h) of the back buffer into pict new bitmap and return it.
   The x range is rounded to the pixel grid g_vid.mask. src, when given, is the bitmap to reuse. */

#include "GLOBALS.H"
#include "PROTOS.H"

struct Bitmap far *F_11A72(int x, int y, int w, int h, struct Bitmap far *src)
{
    struct Bitmap far *pict;
    int x2;
    struct Blt blit;
    int ye;
    int color;

    x2 = (x + w + ~g_vid.mask & g_vid.mask) > 320 ? 320 : (x + w + ~g_vid.mask & g_vid.mask);
    ye = y + h > 200 ? 200 : y + h;
    x &= g_vid.mask;
    if (src) {
        color = src->alt.p != 0;
        pict = F_0376B(x2 - x, ye - y, color, -1, src);
    } else
        pict = F_1934E(x2 - x, ye - y, 0, 2);
    blit.w = pict->width;
    blit.h = pict->height;
    blit.sx = x;
    blit.sy = y;
    blit.dx = blit.dy = 0;
    F_120FE(-3);
    (*g_78a2)(g_vid.back, pict, &blit, 1);
    F_120FE(-1);
    return pict;
}
