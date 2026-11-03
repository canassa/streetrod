/* F_01B27 @ 0000:1B27 (linear 0x01B27, 106 bytes) */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_01B27(void)
{
    _dos_setvect(8, F_01FE2);
    g_58dc = 0;
    g_58da = 0;
    g_5e0e = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    if (!g_0610 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 1;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
}
