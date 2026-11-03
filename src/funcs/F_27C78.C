/* F_27C78 @ 2645:1828 (linear 0x27C78, 53 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_27C78(int a, int b, int c)
{
    int u, v;

    b += a * c;
    v = b - g_7d38;
    if (a == 0)
        a++;
    u = v / a;
    return u + 0x24;
}
