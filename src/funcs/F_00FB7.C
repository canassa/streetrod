/* F_00FB7 @ 0000:0FB7 (linear 0x00FB7, 51 bytes) */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

void F_00FB7(void)
{
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);
    _dos_setvect(8, g_7aaa);
    _enable();
    F_02DE2();
    g_05e0 = 0;
    g_0044 = 0;
}
