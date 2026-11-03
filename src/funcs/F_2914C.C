/* F_2914C @ 2645:2CFC (linear 0x2914C, 36 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_2914C(int x1, int y1, int x2, int y2)
{
    if (y2 == y1)
        return 0;
    return ((x2 - x1) << 4) / (y2 - y1);
}
