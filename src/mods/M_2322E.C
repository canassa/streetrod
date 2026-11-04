#include <conio.h>
#include "STRUCTS.H"
#include "M_2322E.H"

#pragma intrinsic(inp, outp)

/* F_2322E @ 21A0:182E (linear 0x2322E, 52 bytes)

   Set the CRTC start address and wait for a vertical retrace */

void F_2322E(int n)
{
    outp(0x3d4, 0x0c);
    outp(0x3d5, n << 5);
    outp(0x3d4, 0x0d);
    outp(0x3d5, 0);
    while (inp(0x3da) & 8)
        ;
    while (!(inp(0x3da) & 8))
        ;
}
