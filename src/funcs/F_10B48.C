/* F_10B48 @ 0F38:17C8 (linear 0x10B48, 52 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

int F_10B48(char *s)
{
    int w;

    for (w = 0; *s; s++)
        w += g_font[*s - 32].w;
    return w;
}
