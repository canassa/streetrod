/* F_2212C: copy n packed nibbles to a far buffer, starting at the low nibble when odd */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_2212C(char far *dst, char *src, int n, int odd)
{
    unsigned char *s;

    s = src;
    if (odd) {
        *dst &= 0xf0;
        *dst |= *s++ & 0x0f;
        dst++;
        n--;
    }
    for (; n > 1; n -= 2)
        *dst++ = *s++;
    if (n > 0) {
        *dst &= 0x0f;
        *dst |= *s & 0xf0;
    }
}
