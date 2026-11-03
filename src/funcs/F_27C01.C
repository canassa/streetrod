/* F_27C01 @ 2645:17B1 (linear 0x27C01, 60 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_27C01(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_7d38;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return u + 0x3c;
}
