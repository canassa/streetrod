/* F_167E4: noise: speaker off, then count times flip bit 1 of port 61h and wait for a random time
   (random_int(range) | bits) */

#include <conio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(inp, outp)

static void near F_167E4(int range, int bits, int count)
{
    int i;
    int delay;

    F_164BD();
    for (i = 0; i < count; i++) {
        outp(0x61, inp(0x61) ^ 2);
        delay = random_int(range) | bits;
        F_21962(delay);
    }
}
