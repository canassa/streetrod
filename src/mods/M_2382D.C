#include <conio.h>
#include "STRUCTS.H"
#include "M_23262.H"

/* F_2382D @ 21A0:1E2D: copy a w by h block of planar VGA video memory using write mode 1 (latch copy) */


#pragma intrinsic(outp)

/* The file is compiled /Oals, but this copy loop is a plain /Ol-off loop in the
   shipped code (byte moves through memory variables). Under /Ol MSC turns the
   inner loop into rep movsw, and every for/while/do form gets registers and
   loop temps. A word move would also break the write-mode-1 latch copy (the
   second byte's latches are written to both bytes), a likely reason the
   programmer switched the loop optimizer off here. */
#pragma loop_opt(off)

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
