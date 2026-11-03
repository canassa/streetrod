/* F_04940 @ 0000:4940 (linear 0x04940, 98 bytes) */

#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_04940(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_4fa4, "How about $%-d ?", *price);
    F_11DE2(0xff7, -1, -1);
    key = F_14294(0xd);
    F_11E5F();
    return key;
}
