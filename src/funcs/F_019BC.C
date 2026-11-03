/* F_019BC @ 0000:19BC (linear 0x019BC, 73 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_019BC(int n)
{
    struct Link *p;
    int i;

    for (i = g_0566; i >= 0; i = g_0572[i].nxt)
        if ((p = g_0572 + i)->fa == 1 && p->fc == n)
            p->fa = 0;
}
