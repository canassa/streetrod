/* F_19925: rotate the ring of four frames of f1 sprite and pick the one for the current heading */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_19925(struct Sprite *s)
{
    struct Frame *f1, *f2, *f3, *f4;
    int idx;

    s->x = g_8b7c;
    f4 = (f3 = (f2 = (f1 = s->frame)->nxt[0])->nxt[0])->nxt[0];
    f1->nxt[0] = f4;
    f2->nxt[0] = f1;
    f3->nxt[0] = f2;
    f4->nxt[0] = f3;
    for (idx = 0; idx < 4; idx++)
        g_77c0[idx].f2 = g_77c0[idx].f0->nxt[0];
    for (idx = 0; idx < 4; idx++)
        g_77c0[idx].f4 = g_77c0[idx].f2->nxt[1];
    switch (g_8b7c & 7) {
    case 0:
        break;
    case 6:
        s->frame = f4;
        return;
    case 4:
        s->frame = f3;
        return;
    case 2:
        s->frame = f2;
    }
}
