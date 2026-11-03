/* F_04036 @ 0000:4036 (linear 0x04036, 848 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static struct Node * near F_03C1B(void);
static struct Node2 * near F_03FB1(void);

static struct Car * near F_04036(int idx, int weight, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;
    unsigned wd2;

    car = (old == 0) ? (struct Car *)F_03FB1() : old;
    car->f2 = idx;
    car->weight = weight;
    car->f22 = 0x28;
    car->f5 = g_7d88[idx].kind;
    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    car->f4 = i;
    car->f1f = 1;
    car->f20 = random_int(13) - 8;
    if (old == 0)
        node = F_03C1B();
    else
        node = car->slot6;
    car->slot6 = node;
    {
    unsigned *q;

    q = &g_7d88[idx].f2;
    {
    unsigned wd;

    wd = *q;
    w = (wd >> 6) & 3;
    w = (kind = (wd & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    }
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[1];
    node->next = 0;
    if (old == 0)
        node = F_03C1B();
    else
        node = car->rack.s[0];
    car->rack.s[0] = node;
    w = (*q >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[0];
    node->next = 0;
    if (old == 0)
        node = F_03C1B();
    else
        node = car->slot12;
    car->slot12 = node;
    w = (*q >> 4 & 3) + 0x28;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[4];
    node->next = 0;
    }
    if (old == 0)
        node = F_03C1B();
    else
        node = car->rack.s[1];
    car->rack.s[1] = node;
    for (i = 3; i < 9; i++)
        car->b16[i] = 0;
    wd2 = g_7d88[idx].f2;
    w = (i = wd2 >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[3];
    node->next = 0;
    w = (wd2 >> 14 & 3) + 0x16;
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
        if (g_7eb0) {
            car->f26 = (struct Car *)g_7eb2;
            g_7eb2 = (int)car;
        } else
            g_7eb0 = car;
    }
    return car;
}
