/* F_15B7F: fields of slot picture number n: *w, *h and the byte *x */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_153D7(int n);

void F_15B7F(int n, int *w, int *h, int *x)
{
    n = F_153D7(n);
    *w = g_slots[n].f0;
    *h = g_slots[n].f2;
    *x = g_slots[n].f6;
}
