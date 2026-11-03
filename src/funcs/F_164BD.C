/* F_164BD: speaker off: clear bits 0 and 1 of port 61h */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_164BD(void)
{
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
}
