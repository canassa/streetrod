/* F_22077: shift a run of packed nibbles by one nibble, forwards or backwards */

#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

#pragma intrinsic(_rotr, _rotl)

static void near F_22077(unsigned char *p, int dir, int n)
{
    unsigned char carry;
    unsigned char c;
    unsigned rot;
    int i;

    if (dir > 0) {
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotr(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p++ = (c & 0x0f) | carry;
            carry = c & 0xf0;
        }
    } else if (dir < 0) {
        p += n - 1;
        for (i = 0, carry = 0; i < n; i++) {
            rot = _rotl(*p, 4);
            c = (unsigned char)((rot >> 8) | rot);
            *p-- = (c & 0xf0) | carry;
            carry = c & 0x0f;
        }
    }
}
