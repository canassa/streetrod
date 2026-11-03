/* F_16AA8 @ 0F38:7728 (linear 0x16AA8, 25 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16AA8(int on)
{
    if (on)
        g_58dc = 1;
    else
        g_58dc = 0;
}
