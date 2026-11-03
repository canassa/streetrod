/* F_199DD: store step i of a sprite's motion sequence and terminate the sequence after it */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_199DD(struct Sprite *s, int i, int count, signed char dx, signed char dy)
{
    s->seq[i].count = count;
    s->seq[i].dx = dx;
    s->seq[i].dy = dy;
    s->seq[i + 1].count = -1;
    s->seq[i + 1].dx = 0;
    s->seq[i + 1].dy = 0;
}
