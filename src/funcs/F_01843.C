/* F_01843 @ 0000:1843 (linear 0x01843, 327 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_01843(int delay, int pri)
{
    register int i, cur3;
    int prev;
    long when, key;

    g_0571 = 1;
    i = g_056a;
    if (i < 0) {
        g_0571 = 0;
        return -1;
    }
    cur3 = g_0568;
    prev = -1;
    when = g_tick + delay;
    if (delay) {
        for (;;) {
            if (cur3 >= 0) {
                if ((key = g_0572[cur3].key) > when) {
                    cur3 = g_0572[prev = cur3].nxt;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_0572[cur3].f8 >= pri)
                        break;
                    cur3 = g_0572[prev = cur3].nxt;
                    if (cur3 < 0 || g_0572[cur3].key != key)
                        break;
                }
            }
            break;
        }
    }
    g_0572[i].key = when;
    g_0572[i].f8 = pri;
    g_056a = g_0572[i].nxt;
    g_0572[i].nxt = cur3;
    if (cur3 < 0) {
        g_0566 = i;
        g_056c = when;
    } else
        g_0572[cur3].prv = i;
    g_0572[i].prv = prev;
    if (prev < 0)
        g_0568 = i;
    else
        g_0572[prev].nxt = i;
    g_0571 = 0;
    return i;
}
