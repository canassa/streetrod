/* F_100E2 @ 0F38:0D62 (linear 0x100E2, 91 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_100E2(void)
{
    F_03AB8(0x17bb);
    (*g_78be)();
    F_120FE(-4);
    g_vid.f16 = 1;
    F_1169A(0, 0, 320, 200, 0);
    F_065AF(1);
    F_10B7C();
    (*g_present)();
    F_03A83(300);
}
