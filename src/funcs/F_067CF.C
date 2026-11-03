/* F_067CF @ 0000:67CF (linear 0x067CF, 88 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_067CF(int a)
{
    if (g_vid.mode == -2 || g_vid.mode == -6) {
        if (g_8249) {
            F_112CB(6, g_50c2[a][0]);
            F_112CB(7, g_50c2[a][1]);
        }
        g_palette[6] = g_50c2[a][0];
        g_palette[7] = g_50c2[a][1];
    }
}
