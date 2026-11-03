/* F_03C1B @ 0000:3C1B (linear 0x03C1B, 35 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node * near F_03C1B(void)
{
    struct Node *p;

    p = g_7eba;
    g_7eba = p->next;
    p->next = 0;
    g_7ebc++;
    return p;
}
