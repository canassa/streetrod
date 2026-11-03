/* F_04386 @ 0000:4386 (linear 0x04386, 57 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_04386(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->a;
    if (keep) {
        n->next = g_7eb8;
        g_7eb8 = n;
    } else
        F_03C3E(n);
    return r;
}
