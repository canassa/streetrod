/* F_00E24 @ 0000:0E24 (linear 0x00E24, 231 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_00E24(void)
{
    int i;
    long deadline;

    g_0566 = g_0568 = -1;
    g_056a = 0;
    g_056c = 0x7fffffffL;
    g_0570 = g_0571 = 0;
    for (i = 0; i < 4; i++)
        g_0572[i].nxt = i + 1;
    g_05ba = -1;
    deadline = g_tick + 10;
    while (deadline > g_tick) {
        for (i = 0; F_02DB6(); i++)
            ;
        if (i)
            deadline = g_tick + 10;
    }
    g_05cc = 0;
    if (g_0610 == 0 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
    }
    g_4736 = 0;
    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_4732 = g_472e = g_472f = g_4730 = g_4731 = 0;
}
