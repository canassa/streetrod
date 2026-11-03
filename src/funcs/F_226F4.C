/* F_226F4: blit a rectangle between two bitmaps, offset by a scroll origin */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
static void near F_226F4(struct Rect *r, struct Bitmap far *a, struct Bitmap far *b, int horiz)
{
    struct Blt blt;
    blt.w = r->x2 - r->x1 + 1;
    blt.h = r->y2 - r->y1 + 1;
    blt.dx = r->x1;
    blt.dy = r->y1;
    if (horiz) {
        blt.sx = r->x1 - g_5a20;
        blt.sy = 0;
    } else {
        blt.sx = 0;
        blt.sy = blt.dy - g_5aa8;
    }
    F_21EAB(b, a, &blt, 1);
}
