#include "STRUCTS.H"
#include "M_039C0.H"
#include "M_02B9A.H"
#include "M_113A2.H"

/* F_03FB1 @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */


struct Node2 * near F_03FB1(void)
{
    struct Node2 *p;

    p = a_7eaap.m7eb4;
    a_7eaap.m7eb4 = p->next;
    p->next = 0;
    a_7eaap.m7eb6++;
    return p;
}

/* F_03FD4 @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */


static void near F_03FD4(struct Node2 *p)
{
    p->next = a_7eaap.m7eb4;
    p->b = -1;
    a_7eaap.m7eb4 = p;
    a_7eaap.m7eb6--;
}

/* F_03FF0 @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */


struct Node * near F_03FF0(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = F_03C1B();
    n->idx = idx;
    it = &g_4806[idx];
    n->b = g_4966[it->f4];
    n->a = a;
    n->next = a_7eaap.m7eb8;
    a_7eaap.m7eb8 = n;
    return n;
}
