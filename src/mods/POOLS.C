#include "STRUCTS.H"
#include "POOLS.H"
#include "SCREEN.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Node2 as83b0[16];
struct Node aj866au[140];

/* pools_init @ 0000:3614 (linear 0x03614, 148 bytes) */

void near pools_init(int reset)
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
