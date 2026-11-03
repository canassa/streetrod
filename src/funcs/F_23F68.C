/* F_23F68 @ 21A0:2568: copy a rectangle between two planar bitmaps with F_24E78 */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_23F68(struct Rect *r, struct Bitmap far *dst, struct Bitmap far *src, int rows)
{
    int off = rows ? 40 * r->y1 : r->x1 >> 3;

    F_24E78(src->bits.p - off, dst->bits.p, (r->x2 >> 3) - (r->x1 >> 3) + 1,
            r->y2 - r->y1 + 1, r->x1 >> 3, r->y1, dst->pitch);
}
