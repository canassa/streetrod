/* F_064A3 @ 0000:64A3 (linear 0x064A3, 134 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_064A3(int *a, int *b)
{
    long t;
    int r;

    t = g_05fc + g_0600 - g_0604;
    if (t > 0x91908L)
        t = 0x91908L;
    r = (int)(t % 6552);
    *a = (unsigned)r / 546;
    *b = (long)((unsigned long)(unsigned)(r - 546 * *a) * 60) / 546;
}
