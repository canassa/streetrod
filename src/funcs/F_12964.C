/* F_12964: position (*px, *py) for the current item: looked up per slot, with an offset added in one mode */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1287B(void);
static int near F_1289C(int *px, int *py);

void F_12964(int *px, int *py)
{
    int slot;
    int item;

    switch (g_82ae) {
    default:
        slot = F_1289C(px, py);
        *px += g_076a[(slot += g_82b2) * 2];
        *py += g_076a[slot * 2 + 1];
        break;
    case 0:
        *px = g_070e[(slot = F_1287B()) * 2];
        *py = g_070e[slot * 2 + 1];
        break;
    case 1:
        item = g_8290[1];
        slot = (item == 0x438 || item == 0x437) ? 0 : 2;
        if (g_82b2 == 1)
            slot++;
        *px = g_0732[slot * 2];
        *py = g_0732[slot * 2 + 1];
        break;
    }
}
