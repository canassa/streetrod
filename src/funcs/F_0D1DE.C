/* F_0D1DE @ 0000:D1DE (linear 0x0D1DE, 191 bytes)

   Steps p->f18 up from 1 until gear[f18].r[1].f2 exceeds f34[f18] * rpm or
   f18 reaches f1c. The four pointers the loop advances in memory are /Ol induction temps for the p->f18 indexing. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0D1DE(int rpm, struct Eng *p)
{
    struct Range *r;

    for (p->f18 = 1; ; p->f18++) {
        p->f40 = p->f34[p->f18];
        p->fe = p->f40 * rpm;
        if ((r = &p->gear[p->f18].r[1])->f2 > p->fe) {
            p->f42 = g_6cb8 = p->fe >= r->f0;
            if (g_6cb8 == 0)
                r = &p->gear[p->f18].r[0];
            break;
        }
        if (p->f18 == p->f1c) {
            r = &p->gear[p->f18].r[2];
            p->f42 = g_6cb8 = 2;
            break;
        }
    }
    p->f14 = r->f4;
    p->f0 = p->fe / p->f40;
}
