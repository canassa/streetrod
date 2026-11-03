/* F_07741 @ 0000:7741 (linear 0x07741, 72 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_07741(struct Node *n, struct Node **slot)
{
    if (*slot) {
        (*slot)->next = g_7eb8;
        g_7eb0->weight -= F_03C5A(g_7eb8 = *slot);
    }
    *slot = n;
    n->next = 0;
    g_7eb0->weight += F_03C5A(n);
}
