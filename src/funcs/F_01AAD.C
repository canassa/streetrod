/* F_01AAD @ 0000:1AAD (linear 0x01AAD, 122 bytes) */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

void F_01AAD(void)
{
    g_060a = 0;
    _dos_setvect(8, F_0238C);
    if (!g_0610 && g_05f2) {
        F_16DC7();
        g_5e6 = g_5e8 = 0;
        g_5ea = 1;
        g_5ec = 6;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
    g_78e0.f2 = g_78e0.f4 = 0;
    g_0609 = 0;
    outp(0x42, 0x1e);
    outp(0x42, 0);
    if (g_47ce) {
        outp(0x61, inp(0x61) | 3);
        g_47cf = 1;
    }
}
