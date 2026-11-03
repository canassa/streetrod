/* F_049A2 @ 0000:49A2 (linear 0x049A2, 75 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_049A2(struct Car *c)
{
    int r;

    r = 0;
    if (c->f24 & 0x2000)
        r |= 4;
    if (c->f24 & 0x4000)
        r |= 2;
    if (c->f24 & 0x8000)
        r |= 1;
    if (g_4806[c->rack.s[0]->idx].f2 == 2)
        r |= 8;
    return r;
}
