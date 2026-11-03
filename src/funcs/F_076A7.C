/* F_076A7 @ 0000:76A7 (linear 0x076A7, 154 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_076A7(int t, struct Node **list, char *buf, int start)
{
    struct Node *p;
    struct Item *ent7;
    int n;

    n = start;
    p = g_7eb8;
    while (p) {
        if ((ent7 = &g_4806[p->idx])->f4 == t) {
            n++;
            list[n] = p;
            g_49e0[n] = ent7->f6;
            if (p->b != -128)
                buf[n] = p->b / 100;
            else
                buf[n] = 0x80;
        }
        p = p->next;
    }
    g_49e0[0] = n;
}
