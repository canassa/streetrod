/* F_16A2F: starts one of two sounds through F_1628A; `on` selects which */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_16A2F(int on)
{
    if (on)
        F_1628A(g_2f93_0050, g_587e);
    else
        F_1628A(g_2e6b_0000, g_5858);
}
