#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

/* F_16D1A: mouse driver present? Returns the INT 33h reset result (0 if the vector is empty) */


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
