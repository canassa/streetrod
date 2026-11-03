/* F_19E42: clip a span [pos, pos+len) against a sprite's frame, masked to g_vid.mask */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

int F_19E42(int pos, int len, struct Sprite *s, struct Blt16 *o)
{
    int a, b, c, d;

    b = s->x & g_vid.mask;
    a = ((struct Bitmap far *)s->frame->pos)->width;
    if (pos + len <= b || b + a < pos)
        return 0;
    c = (b < pos) ? pos : b;
    d = (b + a > pos + len) ? pos + len : b + a;
    d = (d + ~g_vid.mask) & g_vid.mask;
    o->sx = (c - b < 0) ? 0 : c - b;
    o->dx = c;
    o->w = d - c;
    return 1;
}
