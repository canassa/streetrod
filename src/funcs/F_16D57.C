/* F_16D57: mouse function 3, buttons: bl bit 0 and bit 1 stored in two globals */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_16D57(void)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = 3;
    int86(0x33, &r, &r);
    g_5ee = r.h.bl & 1;
    g_5f0 = r.h.bl & 2;
}
