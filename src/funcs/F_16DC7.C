/* F_16DC7: mouse motion polling; accumulates mickeys and converts to pixel steps */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16DC7(void)
{
    int mx, my;
    int mag, neg;

    F_16D94(&mx, &my);
    g_5e6 += mx;
    g_5e8 += my;
    if (g_5e6 < 0) {
        mag = -g_5e6;
        neg = -1;
    } else {
        mag = g_5e6;
        neg = 0;
    }
    g_5e2 = mag >> g_5ea;
    if (g_5e2) {
        if (neg)
            g_5e2 = -g_5e2;
        g_5e6 = 0;
    }
    if (g_5e8 < 0) {
        mag = -g_5e8;
        neg = -1;
    } else {
        mag = g_5e8;
        neg = 0;
    }
    g_5e4 = mag >> g_5ec;
    if (g_5e4) {
        if (neg)
            g_5e4 = -g_5e4;
        g_5e8 = 0;
    }
    F_16D57();
    if (g_5ee) {
        if (g_4732 == 0) {
            g_4736 = 1;
            g_4738 = g_472a;
            g_473a = g_472c;
        }
        g_4732 |= 4;
    } else
        g_4732 &= ~4;
}
