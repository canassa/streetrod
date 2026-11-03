/* F_095F1 @ 0000:95F1 (linear 0x095F1, 85 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_095F1(void)
{
    int kind, v;

    if (g_7648 == 25)
        return 0;
    kind = F_09556();
    if (kind) {
        if (kind == 1)
            v = 5;
        else if (kind == 2)
            v = 8;
        else
            v = 12;
    } else {
        if (g_817d == 1)
            v = 10;
        else
            v = 50;
    }
    return v;
}
