/* F_0C497 @ 0000:C497 (linear 0x0C497, 149 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdio.h>
#include <string.h>

static void near F_0C497(void)
{
    if (g_7676) {
        sprintf(g_2b3f, "Your``winning``time %2.1f sec.,`avg. speed %2d mph", g_8bc4, g_7644);
        strcpy(g_2b72, "Congratulations !");
    } else {
        sprintf(g_2b3f, "In %2.1f sec you covered only %2d%%", g_8bc4, g_82c8);
        sprintf(g_2b72, "of the distance. Your avg. speed %2d mph.", g_7644);
    }
    F_13DE6(0x21);
    F_13F21(1);
}
