/* F_164A2: speaker on: program the tone for 25000 and set bits 0 and 1 of port 61h */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_164A2(void)
{
    F_16455(25000L);
    outp(0x61, inp(0x61) | 3);
    g_47cf = 1;
}
