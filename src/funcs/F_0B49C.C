/* F_0B49C @ 0000:B49C (linear 0x0B49C, 66 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0B3CA(int);

static void near F_0B49C(int id, int x, int y, int w, int h)
{
    struct Hot *p;
    p = &g_09f4[F_0B3CA(id)];
    p->x2 = (p->x1 = x) + w - 1;
    p->y2 = (p->y1 = y) + h - 1;
    p->f0 = 11;
}
