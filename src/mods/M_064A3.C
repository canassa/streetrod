#include "STRUCTS.H"
#include "M_06476.H"
#include "M_00000.H"
#include "M_00D58.H"

/* F_064A3 @ 0000:64A3 (linear 0x064A3, 134 bytes) */


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

/* F_06529 @ 0000:6529 (linear 0x06529, 19 bytes) */


void F_06529(int n)
{
    if (F_06476() >= 91)
        g_0282 |= 1;
}
