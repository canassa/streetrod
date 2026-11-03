/* F_10AFD @ 0F38:177D (linear 0x10AFD, 75 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#pragma intrinsic(inp)

void F_10AFD(int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (g_vid.mode != -4) {
            while (inp(0x3da) & 8)
                ;
            while (!(inp(0x3da) & 8))
                ;
        } else {
            while (!(inp(0x3ba) & 0x80))
                ;
            while (inp(0x3ba) & 0x80)
                ;
        }
    }
}
