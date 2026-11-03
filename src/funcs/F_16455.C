/* F_16455 @ 0F38:70D5: program PIT channel 2 for a tone of the given frequency */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

void F_16455(long freq)
{
    union Word div;

    div.w = (int)(1193180L / freq);
    outp(0x42, div.b[0]);
    outp(0x42, div.b[1]);
}
