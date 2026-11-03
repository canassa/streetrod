/* F_1D05B: BIOS video function 5, select the active display page */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_1D05B(int page)
{
    union REGS r;

    r.h.ah = 5;
    r.h.al = page;
    int86(0x10, &r, &r);
}
