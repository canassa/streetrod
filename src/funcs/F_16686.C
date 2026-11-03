/* F_16686: same as F_16612 with the table walked backwards: square wave on the speaker: for each of count tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

static void near F_16686(int a, int *p, int count, int reps)
{
    int i;
    int j;

    for (i = 0; i < count; i++) {
        for (j = 0; j < reps; j++) {
            outp(0x61, inp(0x61) | 2);
            F_21962(*p);
            outp(0x61, inp(0x61) & 0xfd);
            F_21962(a - *p);
        }
        p--;
    }
}
