/* F_16D94: mouse function 0Bh, motion counters returned through two pointers */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_16D94(int *dx, int *dy)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 0x0b;
    int86(0x33, &r, &r);
    *dx = r.x.cx;
    *dy = r.x.dx;
}
