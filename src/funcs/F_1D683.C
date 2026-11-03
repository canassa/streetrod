/* F_1D683: copy n pixel pairs from src into a byte row, keeping the bits outside the range */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_1D683(char far *dst, unsigned char *src, int n, int ph)
{
    unsigned char *s;
    int e;
    unsigned char m;

    s = src;
    if (ph) {
        m = g_5b52[ph - 1];
        e = ph + n;
        if (e < 4)
            m |= ~g_5b52[e - 1];
        *dst &= m;
        *dst |= *s++ & ~m;
        dst++;
        n -= 4 - ph;
    }
    for (; n > 3; n -= 4)
        *dst++ = *s++;
    if (n > 0) {
        m = g_5b52[n - 1];
        *dst &= ~m;
        *dst |= *s & m;
    }
}
