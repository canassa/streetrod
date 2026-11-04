#include <dos.h>
#include "STRUCTS.H"
#include "M_16D1A.H"
#include "M_00D58.H"
#include "M_02B9A.H"

/* F_16D57: mouse function 3, buttons: bl bit 0 and bit 1 stored in two globals */


void F_16D57(void)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 3;
    int86(0x33, &r, &r);
    g_5ee = r.h.bl & 1;
    g_5f0 = r.h.bl & 2;
}

/* F_16D94: mouse function 0Bh, motion counters returned through two pointers */


void F_16D94(int *dx, int *dy)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 0x0b;
    int86(0x33, &r, &r);
    *dx = r.x.cx;
    *dy = r.x.dx;
}

/* F_16DC7: mouse motion polling; accumulates mickeys and converts to pixel steps */


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
