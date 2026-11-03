/* F_16A5A: starts one of two sounds through F_1628A; `on` selects which */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16A5A(int on)
{
    if (on) {
        F_1628A(g_2f93_007c, g_587e);
        F_1633D(1, 1, 8, 14, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}
