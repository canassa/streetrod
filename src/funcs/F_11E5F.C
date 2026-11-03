/* F_11E5F: remove the pop-up window: put back the saved background */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_11E5F(void)
{
    if (g_04fa) {
        F_11BD3(g_04fa, g_6c40->x, g_6c40->y);
        F_19438(1);
        g_04fa = 0;
    }
    g_05de = 0;
}
