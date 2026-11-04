#include "GLOBALS.H"
#include "PROTOS.H"

/* F_03FB1 @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */


static struct Node2 * near F_03FB1(void)
{
    struct Node2 *p;

    p = g_7eaa.m7eb4;
    g_7eaa.m7eb4 = p->next;
    p->next = 0;
    g_7eaa.m7eb6++;
    return p;
}

/* F_03FD4 @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */


static void near F_03FD4(struct Node2 *p)
{
    p->next = g_7eaa.m7eb4;
    p->b = -1;
    g_7eaa.m7eb4 = p;
    g_7eaa.m7eb6--;
}

/* F_03FF0 @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */


static struct Node * near F_03FF0(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = F_03C1B();
    n->idx = idx;
    it = &g_4806[idx];
    n->b = g_4966[it->f4];
    n->a = a;
    n->next = g_7eaa.m7eb8;
    g_7eaa.m7eb8 = n;
    return n;
}
