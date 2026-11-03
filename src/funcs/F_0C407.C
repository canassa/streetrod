/* F_0C407 @ 0000:C407 (linear 0x0C407, 144 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_0C407(void)
{
    if (g_53e6 == 0) {
        if ((g_7e8a - g_7d30) * 8 + g_7e8c <= g_7d32)
            g_8632 = 1;
        else
            g_8632 = 0;
        g_7644 = g_82bc / g_78e0.fc0;
        g_8bc4 = g_78e0.fc0 / 6.068827333333334;
        g_82c8 = (int)((g_7d30 - 200) * 100.0 / (g_7e8a - 200)) > 99 ? 99 : (int)((g_7d30 - 200) * 100.0 / (g_7e8a - 200));
        g_53e6 = 1;
    }
}
