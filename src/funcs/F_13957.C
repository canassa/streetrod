/* F_13957: draw a frame around the rectangle (x, y, w, h) of bitmap s, one pixel outside it:
   two vertical lines and two horizontal ones */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_13957(struct Bitmap far *s, int x, int y, int w, int h)
{
    int x1;
    int x2;
    int y1;
    int y2;

    x1 = x - 1;
    x2 = x + w;
    y1 = y - 1;
    y2 = y + h;
    F_1371C(s, x1, y1, h + 2);
    F_1377F(s, x, y2, w);
    F_1371C(s, x2, y1, h + 2);
    F_1377F(s, x, y1, w);
}
