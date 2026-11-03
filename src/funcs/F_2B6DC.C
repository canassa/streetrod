/* F_2B6DC @ 2645:528C (linear 0x2B6DC, 43 bytes)

   Counts the set bits of a byte. */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2B6DC(unsigned char b)
{
    int count;
    int bit;

    count = 0;
    for (bit = 0; bit < 8; bit++) {
        if (b & 0x80)
            count++;
        b <<= 1;
    }
    return count;
}
