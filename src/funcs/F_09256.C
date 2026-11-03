/* F_09256 @ 0000:9256 (linear 0x09256, 78 bytes) */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_09256(char *s)
{
    int i;

    while ((i = strlen(s) - 1) >= 0) {
        if (s[i] != ' ')
            s[i] = 0;
        else
            break;
    }
    if (i >= 0)
        s[i] = 0;
}
