/* F_16AC1 @ 0F38:7741 (linear 0x16AC1, 25 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16AC1(int on)
{
    if (on)
        g_58da = 1;
    else
        g_58da = 0;
}
