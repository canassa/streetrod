#include "STRUCTS.H"
#include "M_039C0.H"
#include "M_02B9A.H"
#include "M_06476.H"
#include "M_113A2.H"
#include "M_15236.H"
#include "M_15396.H"


/* F_04036 @ 0000:4036 (linear 0x04036, 848 bytes) */



struct Car * near F_04036(int idx, int weight, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;

    car = (old == 0) ? (struct Car *)F_03FB1() : old;
    car->f2 = idx;
    car->weight = weight;
    car->f22 = 0x28;
    car->f5 = g7d86n[idx].f2;
    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    car->f4 = i;
    car->f1f = 1;
    car->f20 = random_int(13) - 8;
    car->slot6 = node = (old == 0) ? F_03C1B() : car->slot6;
    w = (g7d86n[idx].f4 >> 6) & 3;
    w = (kind = (g7d86n[idx].f4 & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[1];
    node->next = 0;
    car->rack.s[0] = node = (old == 0) ? F_03C1B() : car->rack.s[0];
    w = (g7d86n[idx].f4 >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[0];
    node->next = 0;
    car->slot12 = node = (old == 0) ? F_03C1B() : car->slot12;
    w = (g7d86n[idx].f4 >> 4 & 3) + 0x28;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[4];
    node->next = 0;
    car->rack.s[1] = node = (old == 0) ? F_03C1B() : car->rack.s[1];
    for (i = 3; i < 9; i++)
        car->b16[i] = 0;
    w = (i = g7d86n[idx].f4 >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[3];
    node->next = 0;
    w = (g7d86n[idx].f4 >> 14 & 3) + 0x16;
    i = g_5126[i] - 1;
    for (j = 2; j > i; j--)
        car->rack.s[j + 2] = 0;
    for (; i >= 0; i--) {
        car->rack.s[i + 2] = node = (old == 0) ? F_03C1B() : car->rack.s[i + 2];
        node->idx = w;
        node->a = g_4806[w].f0;
        node->b = g_4966[2];
        node->next = 0;
        car->b16[2 * i + 3] = 3;
        car->b16[2 * i + 4] = 3;
    }
    for (i = 0; i < 3; i++)
        car->b16[i] = 3;
    for (i = 0; i < 2; i++)
        car->b14[i] = 3;
    car->f24 &= 0xff00;
    car->f24 &= 0xe0ff;
    car->f24 &= 0xdfff;
    car->f24 &= 0xbfff;
    car->f24 &= 0x7fff;
    if (old == 0) {
        if (a_7eaap.m7eb0) {
            car->f26 = (struct Car *)a_7eaap.m7eb2;
            a_7eaap.m7eb2 = (int)car;
        } else
            a_7eaap.m7eb0 = car;
    }
    return car;
}

/* F_04386 @ 0000:4386 (linear 0x04386, 57 bytes) */


int near F_04386(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->a;
    if (keep) {
        n->next = a_7eaap.m7eb8;
        a_7eaap.m7eb8 = n;
    } else
        F_03C3E(n);
    return r;
}

/* F_043BF @ 0000:43BF (linear 0x043BF, 108 bytes) */



int near F_043BF(struct Car *c, int keep)
{
    register struct Node **p2;
    int r;
    register int i;

    c->weight -= F_04386(c->slot6, keep);
    r = 5;
    for (p2 = c->rack.s, i = r; i; p2++, i--)
        c->weight -= F_04386(*p2, keep);
    F_04386(c->slot12, 0);
    r = c->weight;
    if (r < 0)
        r = 0;
    F_03FD4((struct Node2 *)c);
    return r;
}
