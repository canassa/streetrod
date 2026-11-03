/* F_19518: clip the rectangle (x, y, w, h) against the screen window; result in out */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Rect *F_19518(int x, int y, int w, int h, struct Rect *out)
{
    struct Rect rect, scr;

    scr.x1 = g_825e;
    scr.x2 = g_825a->width + g_825e - 1;
    scr.y1 = g_8260;
    scr.y2 = g_8260 + g_8262 - 1;
    rect.x1 = x;
    rect.x2 = x + w - 1;
    rect.y1 = y;
    rect.y2 = y + h - 1;
    return F_1947A(&scr, &rect, out);
}
