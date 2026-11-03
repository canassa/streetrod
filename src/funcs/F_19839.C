/* F_19839: subtract rectangle a from b along one axis into out; NULL when nothing is left */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Rect *F_19839(struct Rect *a, struct Rect *b, struct Rect *out, int horiz)
{
    *out = *b;
    if (horiz) {
        if (b->x1 <= a->x1 && b->x2 <= a->x2) {
            if (b->x2 >= a->x1) {
                if (b->x2 <= a->x2)
                    out->x2 = a->x1 - 1;
                else
                    out->x2 = b->x2;
            }
        } else {
            if (a->x2 >= b->x1)
                out->x1 = a->x2 + 1;
            if (b->x2 < a->x2)
                out->x2 = a->x2;
        }
    } else {
        if (b->y1 < a->y1) {
            if (b->y2 >= a->y1)
                out->y2 = a->y1 - 1;
        } else if (b->y1 > a->y1) {
            if (b->y1 <= a->y2)
                out->y1 = a->y2 + 1;
        }
    }
    if (out->x2 < out->x1 || out->y2 < out->y1) {
        out->x1 = out->y1 = 1000;
        out->x2 = out->y2 = -1;
        return 0;
    }
    return out;
}
