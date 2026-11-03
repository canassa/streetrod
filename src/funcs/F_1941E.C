/* F_1941E @ 0F38:A09E (linear 0x1941E, 26 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1941E(int n)
{
    if (n == 0)
        g_7b14 = 0;
    else
        g_7b14 -= n;
}
