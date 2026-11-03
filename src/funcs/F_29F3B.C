/* F_29F3B @ 2645:3AEB (linear 0x29F3B, 79 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_29F3B(int x, int y, int n, int f, int idx)
{
    n = 9 - n;
    if (n >= 7)
        n = 6;
    if (n < 0)
        n = 0;
    if (y > 0x65)
        y = 0x65;
    F_2905A(x, y, n, *g_6078[idx], f);
}
