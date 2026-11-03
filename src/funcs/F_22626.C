/* F_22626: clear one source row, then copy the same row from the second screen: five interlaced passes over the lines */
/* kl: index over the five line phases (name chosen for the stack layout). */

#include <dos.h>
#include <memory.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_22626(char far *a, char far *b)
{
    char far *p1;
    long dl5;
    char far *p23;
    int far *e;
    int row;
    int kl;
    int i;

    for (kl = 0; kl < 5; kl++) {
        dl5 = g_tick + 5;
        for (row = g_5bc6[kl]; row < 200; row += 5) {
            p1 = a + (((row & 3) << 13) + (row >> 2) * 160);
            p23 = b + (((row & 3) << 13) + (row >> 2) * 160);
            for (e = (int far *)p1, i = 80; i; i--)
                *e++ = 0;
            movedata(FP_SEG(p23), FP_OFF(p23), FP_SEG(p1), FP_OFF(p1), 160);
        }
        while (g_tick < dl5)
            ;
    }
}
