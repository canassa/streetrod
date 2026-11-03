/* F_03FF0 @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node * near F_03FF0(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = F_03C1B();
    n->idx = idx;
    it = &g_4806[idx];
    n->b = g_4966[it->f4];
    n->a = a;
    n->next = g_7eb8;
    g_7eb8 = n;
    return n;
}
