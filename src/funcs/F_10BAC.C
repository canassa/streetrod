/* F_10BAC @ 0F38:182C (linear 0x10BAC, 45 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_10BAC(void)
{
    int i;
    unsigned char save;

    for (i = 0; i < 17; i++) {
        save = g_palette[i];
        g_palette[i] = g_palette[i + 34];
        g_palette[i + 34] = save;
    }
}
