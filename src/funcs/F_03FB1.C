/* F_03FB1 @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node2 * near F_03FB1(void)
{
    struct Node2 *p;

    p = g_7eb4;
    g_7eb4 = p->next;
    p->next = 0;
    g_7eb6++;
    return p;
}
