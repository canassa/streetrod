/* F_2BA48 @ 2645:55F8 (linear 0x2BA48, 120 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2BA48(unsigned char v, char *pa, char *pb)
{
    int m;
    int sh;

    *pa = *pb = 0;
    for (m = 0x80, sh = 6; m > 8; m >>= 1, sh -= 2)
        if (v & m)
            *pa |= 3 << sh;
    for (m = 8, sh = 6; m > 0; m >>= 1, sh -= 2)
        if (v & m)
            *pb |= 3 << sh;
}
