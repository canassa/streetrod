#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"
#include <math.h>

static void near F_0DCBE(struct Car *c, struct Eng *e);

/* F_0E0E2 @ 0000:E0E2 (linear 0x0E0E2, 310 bytes)

   Builds a struct Car from the spec n of g_7d86 and hands it to F_0DCBE. */



static void near F_0E0E2(int n, struct Eng *e)
{
    struct Car car;
    struct Node nd[5];
    struct Node na, lo;
    struct Spec *s;
    int t, i, j;

    car.f20 = 0;
    car.f2 = n;
    car.slot6 = &na;
    car.slot12 = &lo;
    s = &g_7d86[n];
    car.rack.s[3] = car.rack.s[4] = 0;
    t = ((s->f4 >> 14) & 3) + 0x16;
    for (i = g_5126[(s->f4 >> 11) & 7] + 1; i >= 0; i--) {
        car.rack.s[i] = &nd[i];
        nd[i].b = g_496a;
        nd[i].idx = t;
    }
    i = (s->f4 >> 6) & 3;
    na.idx = (t = (s->f4 & 0xf) >> 1) + (i == 3) + i * 3 + 9;
    lo.idx = ((s->f4 >> 4) & 3) + 0x28;
    nd[0].idx = ((s->f4 >> 8) & 7) * 3 + t;
    nd[1].idx = (j = (s->f4 >> 11) & 7) * 3 + t + 0x19;
    F_0DCBE(&car, e);
}

/* F_0E218 @ 0000:E218 (linear 0x0E218, 849 bytes)

   Rating of the car spec n (the car in g_7eb0 when n is negative): builds the
   car the way F_0E0E2 does, then folds its parts into one number. */


static int near F_0E218(int n)
{
    double w9, vl;
    struct Spec *s;
    int a3, t, d, c3;
    struct Car *p6;
    int e1;
    struct Car car;
    int i;
    struct Node lon;
    int j;
    struct Node *m;
    int bl;
    struct Node **qn;
    struct Node ndm[5];
    struct Node *n6k;
    struct Node na;

    if (n < 0)
        p6 = g_7eb0;
    else {
        car.f20 = 0;
        car.f2 = n;
        car.slot6 = &na;
        car.slot12 = &lon;
        s = &g_7d86[n];
        car.rack.s[3] = car.rack.s[4] = 0;
        t = ((s->f4 >> 14) & 3) + 0x16;
        for (i = g_5126[(s->f4 >> 11) & 7] + 1; i >= 0; i--) {
            car.rack.s[i] = &ndm[i];
            ndm[i].b = g_496a;
            ndm[i].idx = t;
        }
        i = (s->f4 >> 6) & 3;
        na.idx = (t = (s->f4 & 0xf) >> 1) + (i == 3) + i * 3 + 9;
        lon.idx = ((s->f4 >> 4) & 3) + 0x28;
        ndm[0].idx = ((s->f4 >> 8) & 7) * 3 + t;
        ndm[1].idx = (j = (s->f4 >> 11) & 7) * 3 + t + 0x19;
        p6 = &car;
    }
    qn = p6->rack.s;
    n6k = p6->slot6;
    m = p6->slot12;
    a3 = g_4808[qn[0]->idx].kind;
    bl = g_4808[qn[1]->idx].kind;
    c3 = g_4808[qn[2]->idx].kind;
    d = g_4808[n6k->idx].kind;
    e1 = g_4808[m->idx].kind;
    vl = ((p6->f24 & 0x2000) || g_8df0[p6->f2].f2 == 0) * 0.09
        + ((p6->f24 & 0x8000) || g_8df0[p6->f2].f8 == 0) * 0.05
        + ((p6->f24 & 0x4000) || g_8df0[p6->f2].f6 == 0) * 0.01
        + g_556e[g_7d86[p6->f2].f2] * 0.1 / 88.0
        + g_552e[c3][bl] * 0.15 / 100.0
        + g_554c[a3] * 0.25 / 100.0
        + g_5552[d] * 0.15 / 100.0
        + (1.0 - fabs(p6->f20 / 8.0)) * 0.2;
    vl = (10000.0 - qn[0]->b) / 10000.0 * vl;
    vl = (d == 0 ? 0.8 : 1.0) * vl * 0.3 + 0.7;
    w9 = vl * 130.0;
    if (w9 > 130.0)
        w9 = 130.0;
    return (int)w9;
}
