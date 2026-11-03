/* F_00F0B @ 0000:0F0B (linear 0x00F0B, 172 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_00F0B(int n)
{
    register int i;

    i = g_0572[n].nxt;
    if (i >= 0)
        g_0572[i].prv = g_0572[n].prv;
    else {
        i = g_0572[n].prv;
        g_0566 = i;
        if (i < 0)
            g_056c = 0x7fffffffL;
        else
            g_056c = g_0572[i].key;
    }
    i = g_0572[n].prv;
    if (i >= 0)
        g_0572[i].nxt = g_0572[n].nxt;
    else
        g_0568 = g_0572[n].nxt;
    g_0572[n].nxt = g_056a;
    g_056a = n;
}
