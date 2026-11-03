/* F_03C3E @ 0000:3C3E (linear 0x03C3E, 28 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03C3E(struct Node *p)
{
    p->next = g_7eba;
    p->idx = -1;
    g_7eba = p;
    g_7ebc--;
}
