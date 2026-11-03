/* F_16943: starts one of two sounds through F_1628A; `on` selects which */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16943(int on)
{
    if (on) {
        F_1628A(g_2f93_00b0, g_587e);
        F_1633D(1, 0, 8, 5, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}
