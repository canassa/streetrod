#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03614(int reset)
{
    struct Node2 *p2;
    struct Node *r;
    int i;

    for (i = 0, p2 = g_7eb4 = g_83b0; i < 16; i++) {
        p2->next = p2 + 1;
        p2->b = -1;
        p2++;
    }
    p2--;
    p2->next = 0;
    for (i = 0, r = g_7eba = g_866a; i < 140; i++) {
        r->next = r + 1;
        r->idx = -1;
        r++;
    }
    r--;
    r->next = 0;
    g_7eb6 = g_7ebc = 0;
    if (reset) {
        g_7eb0 = 0;
        g_7eb2 = 0;
        g_7eb8 = 0;
        g_7eaa = 0;
        g_7eae = 10;
    }
}
