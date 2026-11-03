/* F_1A882 @ 0F38:B502 (linear 0x1a882, 28 bytes): far memory copy */

#include <memory.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_1A882(char far *src, char far *dst, unsigned n)
{
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), n);
}
