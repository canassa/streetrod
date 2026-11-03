/* F_19269 @ 0F38:9EE9 (linear 0x19269, 49 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_19269(void)
{
    if (g_vid.mode == -6)
        g_6cd0 = (char far *)0x80000000L;
    else
        g_6cd0 = F_03709(0xfde8, 1);
    F_1923C();
}
