/* F_1A89E: grab the w x h area at (sx, sy) of a bitmap into a new bitmap */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Bitmap far *F_1A89E(struct Bitmap far *bm, struct Blt *r, int mode)
{
    struct Blt a;
    int b, c, d, e;
    struct Bitmap far *f;

    b = r->w;
    d = r->h;
    c = r->sx;
    e = r->sy;
    f = F_1934E(b, d, bm->alt.p != 0L, mode);
    a.w = b;
    a.h = d;
    a.sx = c;
    a.sy = e;
    a.dx = a.dy = 0;
    (*g_78a2)(bm, f, &a, 3);
    return f;
}
