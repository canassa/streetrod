/* F_1CF40: dissolve one 80-byte-per-line CGA screen into another, five interleaved passes */

#include <memory.h>
#include <dos.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_1CF40(char far *dst, char far *src)
{
    char far *d;
    char far *s2;
    int far *e;
    long deadline;
    int pass;
    int y;
    int k;

    for (pass = 0; pass < 5; pass++) {
        deadline = g_tick + 5;
        for (y = g_5b48[pass]; y < 200; y += 5) {
            d = dst + (y >> 1) * 0x50;
            s2 = src + (y >> 1) * 0x50;
            if (y & 1) {
                d += 0x2000;
                s2 += 0x2000;
            }
            for (e = (int far *)d, k = 0x28; k; k--)
                *e++ = 0;
            movedata(FP_SEG(s2), FP_OFF(s2), FP_SEG(d), FP_OFF(d), 0x50);
            if ((y & 1) && g_vid.mode == -4) {
                d += 0x2000;
                for (e = (int far *)d, k = 0x28; k; k--)
                    *e++ = 0;
                movedata(FP_SEG(s2), FP_OFF(s2), FP_SEG(d), FP_OFF(d), 0x50);
            }
        }
        while (g_tick < deadline)
            ;
    }
}
