#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_19DB7: advance a sprite one tick along its motion sequence */


void F_19DB7(struct Sprite *s)
{
    int n;

    z_8b7c = s->x;
    g_288 = s->frame->pos;
    s->seq->count--;
    if (s->seq->count == 0)
        s->seq++;
    if (s->seq->count != -1) {
        s->x += s->seq->dx;
        s->y += s->seq->dy;
        n = s->seq->dx;
        if (n < 0)
            n = -n;
        if (n)
            s->frame = s->frame->nxt[n >> 2];
    }
}

/* F_19E42: clip a span [pos, pos+len) against a sprite's frame, masked to r_vid.mask */


int F_19E42(int pos, int len, struct Sprite *s, struct Blt16 *o)
{
    int a, b, c, d;

    b = s->x & r_vid.mask;
    a = ((struct Bitmap far *)s->frame->pos)->width;
    if (pos + len <= b || b + a < pos)
        return 0;
    c = (b < pos) ? pos : b;
    d = (b + a > pos + len) ? pos + len : b + a;
    d = (d + ~r_vid.mask) & r_vid.mask;
    o->sx = (c - b < 0) ? 0 : c - b;
    o->dx = c;
    o->w = d - c;
    return 1;
}
