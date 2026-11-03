/* F_079EB @ 0000:79EB (linear 0x079EB, 185 bytes) */

#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_079EB(void)
{
    int ids[143];
    char vals[144];

    if (g_7eb0 == 0) {
        F_03AB8(0x0efa);
        return;
    }
    F_078F1(ids, vals);
    sprintf(g_511e[3] + 9, " %3i mph", F_07EE6(g_7eb0, 1) ? F_0E218(-1) : 0);
    sprintf(g_511e[1] + 5, " %2i gallons", g_7eb0->f22 / 10);
    switch (F_142CF(0x26, 1, ids, g_49e0, vals, g_511e)) {
    case -3:
        return;
    case 0x3ea:
        F_03AB8(0x0f4e);
        return;
    }
}
