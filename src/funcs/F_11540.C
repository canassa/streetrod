#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_11540(int n)
{
    int old;
    char buf[10];

    old = g_8256;
    g_8256 = 1;
    F_11421(g_8258, ltoa(g_7eaa += n, buf, 10), 0x30);
    g_82c0 = g_824c;
    g_8256 = old;
}
