/* F_012DE @ 0000:12DE (linear 0x012DE, 179 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_012DE(int n)
{
    struct KeyMap *p;
    int num;
    struct KeyMap *q;
    char c;

    num = 1;
    if (g_0610)
        g_47cc = 0;
    g_6c4c = n;
    F_01391(0);
    g_064e = n;
    p = g_579f[n];
    q = g_0650;
    for (; p->key; p++, q++, num++) {
        if (num >= 30)
            break;
        q->val = g_4766[q->key = c = p->key];
        g_4766[c] = p->val;
    }
    p = g_57f3;
    q->val = g_4766[q->key = c = p->key];
    g_4766[c] = p->val;
    q[1].key = 0;
}
