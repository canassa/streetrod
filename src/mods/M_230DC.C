#include <conio.h>
#include <dos.h>
#include <memory.h>
#include "STRUCTS.H"
#include "M_230DC.H"

/* F_230DC @ 21A0:16DC: copy 8000 bytes between planar VGA buffers using write mode 1 (latch copy) */


#pragma intrinsic(outp)

int g_5c0c[5] = {0, 80, 160, 40, 120};   /* DS:5C0C */

void F_230DC(char far *src, char far *dst)
{
    outp(0x3ce, 5);
    outp(0x3cf, 1);
    outp(0x3c4, 2);
    outp(0x3c5, 15);
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), 8000);
    outp(0x3ce, 5);
    outp(0x3cf, 0);
}
