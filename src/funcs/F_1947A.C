/* F_1947A: intersect rectangles a and b into out, snapped to the g_vid.mask pixel mask; 0 if they do not overlap */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

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
