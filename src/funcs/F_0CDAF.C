/* F_0CDAF @ 0000:CDAF (linear 0x0CDAF, 429 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static int near F_0E218(int n);

static void near F_0CDAF(struct Eng *p)
{
    struct Node *car;
    int t;

    _disable();
    if (g_4746 == 0 && g_4748 == 0) {
        g_4746 = g_472a;
        g_4748 = g_472c;
    }
    g_472a = g_472c = g_4730 = g_4731 = 0;
    _enable();
    g_47cd = 0;
    p->f0 = p->f18 = p->f42 = p->f2 = p->f4 = 0;
    p->fbc = p->fbe = p->fc0 = 0;
    if (p->f20) {
        p->gear[2].r[1].f2 = p->gear[2].r[2].f0 = p->f1e;
        p->f24 = 1;
    }
    p->f28 = p->f26 = -1;
    p->fe = 0x5dc;
    p->f40 = p->f2a;
    p->f14 = p->gear[0].r[0].f4;
    p->f16 = p->gear[0].r[0].f6;
    p->f12 = p->gear[0].r[0].f2;
    p->f10 = p->gear[0].r[0].f0;
    g_5d6 = 0;
    g_05d8 = g_05da = 0;
    g_8630 = -1;
    g_6caa = g_7ec0 == 0 ? 0x17c : 0x3fc;
    car = g_7eb0->slot12;
    p->fc = g_54b2[g_4808[car->idx].kind][car->b / 2000];
    if (car->b < 8000)
        p->fa = 0;
    else {
        if (car->b + random_int(2000) <= 9500)
            p->fa = 0;
        else {
            t = (g_6caa - 200) >> 1;
            p->fa = random_int(t >> 1) + t + 200;
        }
        goto nx;
    }
nx:
    g_787c = 0;
    g_54d0 = 0;
    g_6cac = F_0E218(g_7648);
    g_6cb2 = (int)(g_6cac * 0.7);
    g_6cb0 = (int)(g_6cac * 0.8);
    g_6cae = g_8acc < 4 ? 10 : 16;
    g_54d2 = 0;
    g_53e6 = 0;
}
