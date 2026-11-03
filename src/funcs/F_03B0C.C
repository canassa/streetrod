#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_03B0C(void)
{
    if (g_7eb6 > 15) {
        F_03AB8(0x0e27);
        return 1;
    }
    if (g_7ebc > 130) {
        F_03AB8(0x0e3f);
        return 1;
    }
    return 0;
}
