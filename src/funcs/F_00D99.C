/* F_00D99 @ 0000:0D99 (linear 0x00D99, 63 bytes) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_00D99(void)
{
    if (g_58d2->off) {
        g_58d2->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2bab;
        F_00D78();
    }
}
