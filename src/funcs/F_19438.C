/* F_19438 @ 0F38:A0B8 (linear 0x19438, 66 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_19438(int n)
{
    if (n == 0)
        g_8b02[g_7d80 = 60] = g_78de;
    else {
        g_7d80 += n;
        if (g_7d80 == 60)
            g_8b02[g_7d80] = g_78de;
        else
            g_8b02[g_7d80] = g_8b02[g_7d80 + 1] - 1;
    }
}
