#include "GLOBALS.H"
#include "PROTOS.H"

void F_03A83(int a)
{
    g_05cc = a + g_tick;
    g_05d0 = 1;
    F_01417(30000);
    g_05cc = 0;
    g_05d0 = 0;
}
