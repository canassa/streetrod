/* F_14090: copy the screen area around hot spot id (see F_13F9A, which updates *px and *py to the
   box corner) into a bitmap of the given mode and return it. The x range is rounded to g_vid.mask */

#include "GLOBALS.H"
#include "PROTOS.H"

struct Bitmap far *F_14090(int id, int *px, int *py, int mode)
{
    int x;
    int y1;
    int x2;
    struct Bitmap far *bits9;
    int h3;
    int w4;
    int y2;

    F_13F9A(id, px, py, &w4, &h3);
    x = *px;
    y1 = *py;
    x2 = (x + w4 + ~g_vid.mask & g_vid.mask) > 320 ? 320 : (x + w4 + ~g_vid.mask & g_vid.mask);
    y2 = y1 + h3 > 200 ? 200 : y1 + h3;
    x &= g_vid.mask;
    bits9 = F_1934E(x2 - x, y2 - y1, 0, mode);
    return F_11A72(*px, y1, w4, h3, bits9);
}
