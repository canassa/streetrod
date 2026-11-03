/* F_29F8A @ 2645:3B3A (linear 0x29F8A, 63 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_29F8A(int x, int y, int n, int f)
{
    n = 9 - n;
    if (n >= 8)
        n = 7;
    if (n < 0)
        n = 0;
    F_2905A(x, y, n, *g_6078[2], f);
}
