/* F_0C52C @ 0000:C52C (linear 0x0C52C, 128 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static int near F_04386(struct Node *, int);

static void near F_0C52C(int n)
{
    struct Node **p;
    char *q;
    int i;

    if (g_7eb0) {
        for (i = 0, p = g_7eb0->rack.s; i < 5; i++, p++) {
            g_7eb0->weight -= F_04386(*p, 0);
            *p = 0;
        }
        for (i = 0, q = g_7eb0->b16; i < 9; i++)
            *q++ = 0;
        g_7eb0->f1f = 0;
    }
    F_00E24();
    F_16BC1();
    F_04FFA(-n, -1, 0x49);
}
