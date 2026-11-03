#include "GLOBALS.H"
#include "PROTOS.H"

void F_10D6C(int n)
{
    struct Obj *o;

    o = &g_7568[g_58f0[g_59f8[n]].a];
    if (o->img)
        o->img = 0;
    if (o->fc)
        o->fc = 0;
    g_58f0[g_59f8[n]].a = -1;
    o->id = -1;
    if (o->f4)
        F_019BC(n);
}
