/* F_03FD4 @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03FD4(struct Node2 *p)
{
    p->next = g_7eb4;
    p->b = -1;
    g_7eb4 = p;
    g_7eb6--;
}
