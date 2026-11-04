#include "GLOBALS.H"
#include "PROTOS.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Node2 as83b0[16];
struct Node aj866au[140];

static void near F_03614(int reset)
{
    struct Node2 *p2;
    struct Node *r;
    int i;

    for (i = 0, p2 = a_7eaap.m7eb4 = as83b0; i < 16; i++) {
        p2->next = p2 + 1;
        p2->b = -1;
        p2++;
    }
    p2--;
    p2->next = 0;
    for (i = 0, r = a_7eaap.m7eba = aj866au; i < 140; i++) {
        r->next = r + 1;
        r->idx = -1;
        r++;
    }
    r--;
    r->next = 0;
    a_7eaap.m7eb6 = a_7eaap.m7ebc = 0;
    if (reset) {
        a_7eaap.m7eb0 = 0;
        a_7eaap.m7eb2 = 0;
        a_7eaap.m7eb8 = 0;
        a_7eaap.m7eaa = 0;
        a_7eaap.m7eae = 10;
    }
}
