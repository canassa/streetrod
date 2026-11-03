/* F_27C3D @ 2645:17ED (linear 0x27C3D, 59 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_27C3D(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_7d36 - b;
    if (a == 0)
        a++;
    u = v / a;
    return 0x118 - u;
}
