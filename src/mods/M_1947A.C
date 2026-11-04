#include "STRUCTS.H"
#include "M_1947A.H"
#include "M_00000.H"
#include "M_10B7C.H"
#include "M_174A6.H"

/* F_1947A: intersect rectangles a and b into out, snapped to the r_vid.mask pixel mask; 0 if they do not overlap */


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
    out->x1 &= r_vid.mask;
    out->x2 += ~r_vid.mask;
    out->x2 &= r_vid.mask;
    return out;
}

/* F_19518: clip the rectangle (x, y, w, h) against the screen window; result in out */


struct Rect *F_19518(int x, int y, int w, int h, struct Rect *out)
{
    struct Rect rect, scr;

    scr.x1 = ac825et;
    scr.x2 = bu825a->width + ac825et - 1;
    scr.y1 = aw8260y;
    scr.y2 = aw8260y + l_8262 - 1;
    rect.x1 = x;
    rect.x2 = x + w - 1;
    rect.y1 = y;
    rect.y2 = y + h - 1;
    return F_1947A(&scr, &rect, out);
}
