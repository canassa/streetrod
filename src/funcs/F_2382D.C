/* F_2382D @ 21A0:1E2D: copy a w by h block of planar VGA video memory using write mode 1 (latch copy) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

void F_2382D(char far *src, int w, int h, char far *dst, int pitch)
{
    char far *p;
    int row;
    int col;

    if (h >= 1) {
        outp(0x3ce, 5);
        outp(0x3cf, 1);
        for (row = 0; row < h; row++, src += pitch) {
            for (col = 0, p = src; col < w; col++)
                *dst++ = *p++;
        }
        outp(0x3ce, 5);
        outp(0x3cf, 0);
    }
}
