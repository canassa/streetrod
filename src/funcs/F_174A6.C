/* F_174A6: blit one 4-plane EGA image row set, one plane at a time via the sequencer map mask */

#include <conio.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(outp)

static void near F_174A6(char far *src, char far *dst, char far *mask, int w, int h, int n, int step)
{
    int plane, bit1;

    for (bit1 = 1, plane = 0; bit1 < 9; bit1 <<= 1, plane++, src += step) {
        outp(0x3c4, 2);
        outp(0x3c5, bit1);
        outp(0x3ce, 4);
        outp(0x3cf, plane);
        F_25954(dst, FP_SEG(src), FP_OFF(src), FP_OFF(mask), n, w, h, w);
    }
    outp(0x3c4, 2);
    outp(0x3c5, 0xf);
}
