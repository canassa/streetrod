/* F_1D8C4: blit the part of one bitmap that lies under a rectangle */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1D8C4(struct Rect *rect, struct Bitmap far *bm2, struct Bitmap far *bm1, int flag)
{
    struct Clip c;

    c.w = rect->x2 - rect->x1 + 1;
    c.h = rect->y2 - rect->y1 + 1;
    c.sx = c.dx = flag ? rect->x1 : 0;
    c.sy = c.dy = flag ? 0 : rect->y1;
    F_1D3C8(bm1, bm2, (struct Blt *)&c, 1);
}
