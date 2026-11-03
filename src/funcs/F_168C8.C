/* F_168C8 @ 0F38:7548 (linear 0x168C8, 43 bytes)

   Starts one of two sounds through F_1628A; `on` selects which. */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_168C8(int on)
{
    if (on)
        F_1628A(g_2f93_00c0, g_587e);
    else
        F_1628A(g_2e6b_0000, g_5858);
}
