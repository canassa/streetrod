/* F_00FEA @ 0000:0FEA (linear 0x00FEA, 113 bytes) */

#include <conio.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_00FEA(void)
{
    if (g_0610) {
        g_0610 = (g_0610 == 0x62) ? 0 : 1;
        F_1174E("Quit demo");
        F_1169A(0, 0, 320, 200, 0);
    }
    if (g_0044)
        F_00FB7();
    if (g_0048) {
        (*g_present)();
        F_0E610();
    }
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    exit(5);
}
