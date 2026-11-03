/* F_14294 @ 0F38:4F14 (linear 0x14294, 59 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_14294(int a)
{
    int key;

    F_13DE6(a);
    while ((key = F_01417(0x7d00)) == 0)
        ;
    F_13F21(1);
    return key;
}
