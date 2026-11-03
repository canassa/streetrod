/* F_07789 @ 0000:7789 (linear 0x07789, 46 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_07789(struct Node **slot)
{
    (*slot)->next = g_7eb8;
    g_7eb0->weight -= F_03C5A(g_7eb8 = *slot);
    *slot = 0;
}
