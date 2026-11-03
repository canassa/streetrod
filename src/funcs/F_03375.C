/* score_index @ 0000:3375 (linear 0x03375, 42 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near score_index(void)
{
    int a;
    int b;

    F_064A3(&a, &b);
    return F_06476() * 24 + a + 12;
}
