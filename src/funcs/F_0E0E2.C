/* F_0E0E2 @ 0000:E0E2 (linear 0x0E0E2, 310 bytes)

   Builds a struct Car from the spec n of g_7d86 and hands it to F_0DCBE. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0DCBE(struct Car *c, struct Eng *e);

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
