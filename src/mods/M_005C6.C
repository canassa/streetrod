#include "STRUCTS.H"
#include "M_00000.H"

/* F_005C6 @ 0000:05C6 (linear 0x005C6, 147 bytes) */


static int near F_005C6(void)
{
    long count;
    long prev;

    count = 0;
    prev = *(long far *)0x0000046cL;
    while (prev == *(long far *)0x0000046cL)
        ;
    prev = *(long far *)0x0000046cL;
    while (prev == *(long far *)0x0000046cL)
        count++;
    count /= 95;
    count /= 10;
    return count > 1 ? (int)count : 1;
}
