#include <math.h>
#include "STRUCTS.H"
#include "M_0DCBE.H"
#include "M_02B9A.H"
#include "M_15396.H"
#include "M_06476.H"
#include "M_113A2.H"

/* F_0DCBE @ 0000:DCBE (linear 0x0DCBE, 1060 bytes)

   Fills the drive train e (gear ranges, ratio table, limits) for the car c. The first half
   is the rating computation of F_0E218. */

int g_54dc[4][5] = {{0, 25, 50, 100, 0}, {0, 25, 50, 100, 0}, {0, 25, 50, 75, 100}, {0, 30, 50, 80, 100}};   /* DS:54DC */
int g_5504[3][5] = {{500, 600, 300, 100, 70}, {600, 700, 350, 120, 90}, {600, 760, 380, 140, 90}};   /* DS:5504 */
int g_5522[3][2] = {{2900, 4500}, {2900, 5000}, {2500, 5200}};   /* DS:5522 */
int g_552e[3][5] = {{0, 0, 100, 0, 0}, {0, 50, 0, 100, 75}, {0, 0, 0, 50, 100}};   /* DS:552E */
int g_554c[3] = {0, 50, 100};   /* DS:554C */
int g_5552[4] = {0, 33, 66, 100};   /* DS:5552 */
int g_555a[3] = {4500, 5000, 5200};   /* DS:555A */
int g_5560[4] = {3, 3, 4, 4};   /* DS:5560 */
int g_5568[3] = {9, 8, 5};   /* DS:5568 */
int g_556e[9] = {40, 44, 48, 60, 64, 68, 80, 84, 88};   /* DS:556E */

void near F_0DCBE(struct Car *c, struct Eng *e)
{
    double w9, vl;
    struct Gear *gl;
    int a3, d;
    double xl;
    int c3, e1, v3, i, k;
    struct Node *m;
    int bl;
    struct Node **qn;
    struct Node *n6k;

    qn = c->rack.s;
    n6k = c->slot6;
    m = c->slot12;
    a3 = ((struct Row8 *)((char *)g_4806 + 2))[qn[0]->idx].kind;
    bl = ((struct Row8 *)((char *)g_4806 + 2))[qn[1]->idx].kind;
    c3 = ((struct Row8 *)((char *)g_4806 + 2))[qn[2]->idx].kind;
    d = ((struct Row8 *)((char *)g_4806 + 2))[n6k->idx].kind;
    e1 = ((struct Row8 *)((char *)g_4806 + 2))[m->idx].kind;
    vl = ((c->f24 & 0x2000) || w_8df0[c->f2].f2 == 0) * 0.09
        + ((c->f24 & 0x8000) || w_8df0[c->f2].f8 == 0) * 0.05
        + ((c->f24 & 0x4000) || w_8df0[c->f2].f6 == 0) * 0.01
        + g_556e[g7d86n[c->f2].f2] * 0.1 / 88.0
        + g_552e[c3][bl] * 0.15 / 100.0
        + g_554c[a3] * 0.25 / 100.0
        + g_5552[d] * 0.15 / 100.0
        + (1.0 - fabs(c->f20 / 8.0)) * 0.2;
    vl = (10000.0 - qn[0]->b) / 10000.0 * vl;
    vl = (d == 0 ? 0.8 : 1.0) * vl * 0.3 + 0.7;
    w9 = vl * 130.0;
    if (w9 > 130.0)
        w9 = 130.0;
    for (i = 0; i <= 4; i++) {
        xl = g_54dc[d][i] * w9 / 100.0;
        if (xl == 0.0)
            xl = 0.2;
        e->f34[i] = (int)(5700.0 / xl);
        gl = &e->gear[i];
        gl->r[0].f0 = 0;
        gl->r[1].f0 = gl->r[0].f2 = g_5522[a3][0];
        gl->r[2].f0 = gl->r[1].f2 = g_5522[a3][1];
        gl->r[2].f2 = 0x1644;
        v3 = (int)(g_5504[a3][i] * vl);
        gl->r[0].f4 = ((i == 1 && e1 == 2) ? 3 : 1) * ((i == 1 && e1 != 2) ? 2 : 1) * (v3 / (i ? 5 : 1));
        gl->r[1].f4 = v3;
        gl->r[2].f4 = v3 / (i ? 10 : 1);
        for (k = 0; k < 3; k++)
            gl->r[k].f6 = i ? v3 / (5 - i) : v3;
    }
    e->f20 = e->f24 = (d == 0);
    if (e->f20) {
        e->f26 = e->f28 = -1;
        e->f22 = gl->r[1].f2;
    }
    e->f1e = g_555a[a3];
    e->f34[5] = (int)((int)(0x82 - g7d86n[c->f2].f2) * 5.0 / 50.0 + g_5568[((struct Row8 *)((char *)g_4806 + 2))[m->idx].kind]);
    e->f1c = g_5560[d];
    e->f1a = 1;
}

/* F_0E0E2 @ 0000:E0E2 (linear 0x0E0E2, 310 bytes)

   Builds a struct Car from the spec n of g7d86n and hands it to F_0DCBE. */

void near F_0E0E2(int n, struct Eng *e)
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
    s = &g7d86n[n];
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

   Rating of the car spec n (the car in a_7eaap.m7eb0 when n is negative): builds the
   car the way F_0E0E2 does, then folds its parts into one number. */

int near F_0E218(int n)
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
        p6 = a_7eaap.m7eb0;
    else {
        car.f20 = 0;
        car.f2 = n;
        car.slot6 = &na;
        car.slot12 = &lon;
        s = &g7d86n[n];
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
    a3 = ((struct Row8 *)((char *)g_4806 + 2))[qn[0]->idx].kind;
    bl = ((struct Row8 *)((char *)g_4806 + 2))[qn[1]->idx].kind;
    c3 = ((struct Row8 *)((char *)g_4806 + 2))[qn[2]->idx].kind;
    d = ((struct Row8 *)((char *)g_4806 + 2))[n6k->idx].kind;
    e1 = ((struct Row8 *)((char *)g_4806 + 2))[m->idx].kind;
    vl = ((p6->f24 & 0x2000) || w_8df0[p6->f2].f2 == 0) * 0.09
        + ((p6->f24 & 0x8000) || w_8df0[p6->f2].f8 == 0) * 0.05
        + ((p6->f24 & 0x4000) || w_8df0[p6->f2].f6 == 0) * 0.01
        + g_556e[g7d86n[p6->f2].f2] * 0.1 / 88.0
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
