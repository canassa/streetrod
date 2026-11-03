/* F_01B91 @ 0000:1B91 (linear 0x01B91, 29 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_01B91(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}
