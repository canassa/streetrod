/* F_27B6D @ 2645:171D (linear 0x27B6D, 82 bytes)

   Points the terrain pointers at entry n of each terrain array. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_27B6D(int n)
{
    g_72ee = &g_ter33f0[n];
    g_8634 = &g_ter3cb0[n];
    g_7d62 = &g_ter4e30[n];
    g_7880 = &g_ter56f0[n];
    g_7560 = &g_ter4570[n];
}
