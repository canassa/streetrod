#include "GLOBALS.H"
#include "PROTOS.H"

char far *F_03967(unsigned size, char *pool)
{
    char far *p;
    int i;

    for (i = 0; i < 4; i++) {
        if (g_4c18[i] + size < 0x7d00) {
            p = g_4c14 + g_4c18[i] + 0x7d00;
            g_4c18[i] += size;
            *pool = i;
            return p;
        }
    }
    *pool = -1;
    return 0;
}
