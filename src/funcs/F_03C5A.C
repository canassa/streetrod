/* F_03C5A @ 0000:3C5A (linear 0x03C5A, 51 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_03C5A(struct Node *n)
{
    int r;

    if (g_4806[n->idx].f4 == 0 && g_4806[n->idx].f2 == 0)
        r = 100;
    else
        r = n->a;
    return r;
}
