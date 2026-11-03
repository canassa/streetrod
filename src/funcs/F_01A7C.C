/* F_01A7C @ 0000:1A7C (linear 0x01A7C, 49 bytes) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_01A7C(void)
{
    g_58d2->off = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    g_58d2->dirty = 1;
    g_58d2->lc = g_58d2->cur = g_58d2->l8;
}
