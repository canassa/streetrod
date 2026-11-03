#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_1947A: intersect rectangles a and b into out, snapped to the g_vid.mask pixel mask; 0 if they do not overlap */


struct Rect *F_1947A(struct Rect *a, struct Rect *b, struct Rect *out)
{
    if (b->x2 < a->x1 || a->x2 < b->x1)
        return 0;
    out->x1 = (a->x1 < b->x1) ? b->x1 : a->x1;
    out->x2 = (a->x2 > b->x2) ? b->x2 : a->x2;
    if (a->y1 > b->y2 || a->y2 < b->y1)
        return 0;
    out->y1 = (a->y1 < b->y1) ? b->y1 : a->y1;
    out->y2 = (a->y2 > b->y2) ? b->y2 : a->y2;
    out->x1 &= g_vid.mask;
    out->x2 += ~g_vid.mask;
    out->x2 &= g_vid.mask;
    return out;
}

/* F_19518: clip the rectangle (x, y, w, h) against the screen window; result in out */


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
