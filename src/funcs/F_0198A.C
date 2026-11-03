/* F_0198A @ 0000:198A (linear 0x0198A, 50 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_0198A(int fc, int delay)
{
    int n;

    n = F_01843(delay, g_8ef6);
    g_0572[n].fa = 1;
    g_0572[n].fc = fc;
}
