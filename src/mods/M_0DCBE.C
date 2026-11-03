#include <math.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

/* F_0DCBE @ 0000:DCBE (linear 0x0DCBE, 1060 bytes)

   Fills the drive train e (gear ranges, ratio table, limits) for the car c. The first half
   is the rating computation of F_0E218. */


static void near F_0DCBE(struct Car *c, struct Eng *e)
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
    a3 = g_4808[qn[0]->idx].kind;
    bl = g_4808[qn[1]->idx].kind;
    c3 = g_4808[qn[2]->idx].kind;
    d = g_4808[n6k->idx].kind;
    e1 = g_4808[m->idx].kind;
    vl = ((c->f24 & 0x2000) || g_8df0[c->f2].f2 == 0) * 0.09
        + ((c->f24 & 0x8000) || g_8df0[c->f2].f8 == 0) * 0.05
        + ((c->f24 & 0x4000) || g_8df0[c->f2].f6 == 0) * 0.01
        + g_556e[g_7d86[c->f2].f2] * 0.1 / 88.0
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
    e->f34[5] = (int)((int)(0x82 - g_7d88[c->f2].kind) * 5.0 / 50.0 + g_5568[g_4808[m->idx].kind]);
    e->f1c = g_5560[d];
    e->f1a = 1;
}
