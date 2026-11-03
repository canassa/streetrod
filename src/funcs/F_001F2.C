/* F_001F2 @ 0000:01F2 (linear 0x001F2, 52 bytes) */

#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_001F2(unsigned char color)
{
    union REGS r;

    r.x.ax = 0x0b00;
    r.h.bh = 1;
    r.h.bl = color;
    if (g_vid.mode != -4)
        int86(0x10, &r, &r);
}
