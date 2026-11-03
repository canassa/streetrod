/* F_16381 @ 0F38:7001: toggle between two states (calls F_00D99 on entering, F_00DD8 on leaving) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16381(int on)
{
    if (on) {
        if (g_58d0) {
            F_00D99();
            g_58d0 = 0;
            g_58d8 = 1;
        } else
            g_58d8 = 0;
    } else {
        if (g_58d8) {
            g_58d0 = 1;
            F_00DD8();
            g_58d8 = 0;
        }
    }
}
