/* F_1CEE7: copy a 16K video page, and on Hercules also its fourth bank */

#include <memory.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_1CEE7(char far *src, char far *dst)
{
    movedata(FP_SEG(src), FP_OFF(src), FP_SEG(dst), FP_OFF(dst), 0x4000);
    if (g_vid.mode == -4 && (g_vid.mode == -4 ? 0xb000 : 0xb800) == FP_SEG(dst))
        movedata(g_vid.mode == -4 ? 0xb200 : 0xba00, 0, 0xb400, 0, 0x2000);
}
