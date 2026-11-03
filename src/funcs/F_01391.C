/* F_01391 @ 0000:1391 (linear 0x01391, 134 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

int F_01391(int keep)
{
    int i;
    struct KeyMap *p;

    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_472e = g_472f = g_4730 = g_4731 = 0;
    i = g_064e;
    if (i) {
        if (keep) {
            for (p = g_0650; p->key; p++)
                if (p->val >= 0x20)
                    g_4766[p->key] = p->val;
        } else {
            for (p = g_0650; p->key; p++)
                g_4766[p->key] = p->val;
            g_064e = 0;
        }
    }
    return i;
}
