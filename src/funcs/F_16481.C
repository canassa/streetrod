/* F_16481 @ 0F38:7101: set up the PIT for sound (channel 2, mode 3) and program a tone (divisor from 25000) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

void F_16481(void)
{
    g_58e2 = F_163BE(10);
    outp(0x43, 0xb6);
    F_16455(25000L);
}
