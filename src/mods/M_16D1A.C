#include <dos.h>
#include "STRUCTS.H"
#include "M_16D1A.H"
#include "M_00D58.H"
#include "M_02B9A.H"

/* F_16D1A @ 0F38:799A (linear 0x16D1A, 61 bytes)

   Mouse driver present? Returns the INT 33h reset result (0 if the vector is empty) */

int F_16D1A(void)
{
    union REGS r;

    if (*(long far *)0xccL == 0L)
        return 0;
    r.h.ah = 0;
    r.h.al = 0;
    int86(0x33, &r, &r);
    return r.x.ax;
}

/* F_16D57 @ 0F38:79D7 (linear 0x16D57, 61 bytes)

   Mouse function 3, buttons: bl bit 0 and bit 1 stored in two globals */

void F_16D57(void)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 3;
    int86(0x33, &r, &r);
    g_05ee = r.h.bl & 1;
    g_05f0 = r.h.bl & 2;
}

/* F_16D94 @ 0F38:7A14 (linear 0x16D94, 51 bytes)

   Mouse function 0Bh, motion counters returned through two pointers */

void F_16D94(int *dx, int *dy)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 0x0b;
    int86(0x33, &r, &r);
    *dx = r.x.cx;
    *dy = r.x.dx;
}

/* F_16DC7 @ 0F38:7A47 (linear 0x16DC7, 218 bytes)

   Mouse motion polling; accumulates mickeys and converts to pixel steps */

void F_16DC7(void)
{
    int mx, my;
    int mag, neg;

    F_16D94(&mx, &my);
    g_05e6 += mx;
    g_05e8 += my;
    if (g_05e6 < 0) {
        mag = -g_05e6;
        neg = -1;
    } else {
        mag = g_05e6;
        neg = 0;
    }
    g_05e2 = mag >> g_05ea;
    if (g_05e2) {
        if (neg)
            g_05e2 = -g_05e2;
        g_05e6 = 0;
    }
    if (g_05e8 < 0) {
        mag = -g_05e8;
        neg = -1;
    } else {
        mag = g_05e8;
        neg = 0;
    }
    g_05e4 = mag >> g_05ec;
    if (g_05e4) {
        if (neg)
            g_05e4 = -g_05e4;
        g_05e8 = 0;
    }
    F_16D57();
    if (g_05ee) {
        if (g_4732 == 0) {
            g_4736 = 1;
            g_4738 = g_472a;
            g_473a = g_472c;
        }
        g_4732 |= 4;
    } else
        g_4732 &= ~4;
}
