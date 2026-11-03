/* F_27BBF @ 2645:176F (linear 0x27BBF, 66 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_27BBF(int a, int b, int c)
{
    int u, v;

    b += a * c >> 2;
    v = g_7d36 - b;
    u = v << 2;
    if (a == 0)
        a++;
    u = u / a;
    return 0xa0 - u;
}
