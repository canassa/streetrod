/* F_04FFA @ 0000:4FFA (linear 0x04FFA, 69 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_04FFA(int a, int b, int c)
{
    long saved;

    saved = g_05cc;
    F_11DE2(a, b, c);
    F_03A83(g_05dc);
    F_11E5F();
    g_05cc = saved;
}
