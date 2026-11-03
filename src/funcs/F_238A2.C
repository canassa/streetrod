/* F_238A2 @ 21A0:1EA2: shift a row of bytes right (n > 0) or left (n < 0) by n bits, carrying between bytes */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(_rotl, _rotr)

static void near F_238A2(unsigned char *p, int n, int len)
{
    unsigned char a;
    unsigned char b;
    int c;
    unsigned char d;
    int e;
    unsigned char f;

    if (n > 0) {
        f = g_5c16[n - 1];
        d = ~f;
        e = 0;
        a = 0;
        for (; e < len; e++) {
            c = _rotr(*p, n);
            b = (c >> 8) | c;
            *p++ = (b & d) | a;
            a = b & f;
        }
    } else if (n < 0) {
        p += len - 1;
        n = -n;
        f = g_5c16[7 - n];
        d = ~f;
        e = 0;
        a = 0;
        for (; e < len; e++) {
            c = _rotl(*p, n);
            b = (c >> 8) | c;
            *p-- = (b & f) | a;
            a = ~f & b;
        }
    }
}
