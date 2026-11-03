/* F_19DB7: advance a sprite one tick along its motion sequence */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19DB7(struct Sprite *s)
{
    int n;

    g_8b7c = s->x;
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
