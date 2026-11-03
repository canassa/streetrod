/* F_077B7 @ 0000:77B7 (linear 0x077B7, 73 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node * near F_077B7(struct Node *n)
{
    struct Node *p1, *q;

    for (p1 = g_7eb8, q = 0; p1;)
        if (p1 != n)
            p1 = (q = p1)->next;
        else
            break;
    if (q == 0)
        g_7eb8 = p1->next;
    else
        q->next = p1->next;
    return p1;
}
