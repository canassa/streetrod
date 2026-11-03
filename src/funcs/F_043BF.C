/* F_043BF @ 0000:43BF (linear 0x043BF, 108 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_04386(struct Node *n, int keep);

static int near F_043BF(struct Car *c, int keep)
{
    register struct Node **p2;
    int r;
    register int i;

    c->weight -= F_04386(c->slot6, keep);
    r = 5;
    for (p2 = c->rack.s, i = r; i; p2++, i--)
        c->weight -= F_04386(*p2, keep);
    F_04386(c->slot12, 0);
    r = c->weight;
    if (r < 0)
        r = 0;
    F_03FD4((struct Node2 *)c);
    return r;
}
