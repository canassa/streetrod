/* F_09524 @ 0000:9524 (linear 0x09524, 50 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_09524(int n)
{
    int v;

    v = g_7d88[n].kind;
    if (v < 3)
        return 0;
    if (v >= 7)
        return 2;
    return 1;
}
