/* F_001D1 @ 0000:01D1 (linear 0x001D1, 33 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_001D1(int ax)
{
    union REGS r;

    r.x.ax = ax;
    int86(0x10, &r, &r);
}
