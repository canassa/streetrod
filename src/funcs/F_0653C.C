/* F_0653C @ 0000:653C (linear 0x0653C, 115 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0653C(int v, int *out)
{
    int r;

    if (v < 15) {
        r = 0;
        *out = v + 16;
    } else if (v < 46) {
        r = 1;
        *out = v - 14;
    } else if (v < 77) {
        r = 2;
        *out = v - 45;
    } else if (v < 91) {
        r = 3;
        *out = v - 76;
    } else {
        r = 3;
        *out = 15;
    }
    return r;
}
