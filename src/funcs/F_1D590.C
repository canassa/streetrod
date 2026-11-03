/* F_1D590: shift a row of bytes by n pixel pairs (2 bits each), carrying the bits across bytes */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(_rotl, _rotr)

static void near F_1D590(unsigned char *p, int n, int cnt)
{
    unsigned char carry;
    int r2;
    unsigned char t;
    int k;
    unsigned char nm;
    unsigned char m;

    if (n > 0) {
        m = g_5b52[n - 1];
        nm = ~m;
        for (k = 0, carry = 0; k < cnt; k++) {
            r2 = _rotr(*p, n << 1);
            t = (r2 >> 8) | r2;
            *p++ = (t & nm) | carry;
            carry = t & m;
        }
    } else if (n < 0) {
        p += cnt - 1;
        n = -n;
        m = g_5b52[3 - n];
        nm = ~m;
        for (k = 0, carry = 0; k < cnt; k++) {
            r2 = _rotl(*p, n << 1);
            t = (r2 >> 8) | r2;
            *p-- = (t & m) | carry;
            carry = t & nm;
        }
    }
}
