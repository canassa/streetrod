/* F_0C5AC @ 0000:C5AC (linear 0x0C5AC, 103 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static int near F_04386(struct Node *, int);

static void near F_0C5AC(int n)
{
    char *q;
    int i;

    if (g_7eb0) {
        g_7eb0->weight -= F_04386(g_7eb0->slot6, 0);
        g_7eb0->slot6 = 0;
        for (i = 0, q = g_7eb0->b14; i < 2; i++)
            *q++ = 0;
    }
    F_00E24();
    F_16BC1();
    F_04FFA(-n, -1, 0x49);
}
