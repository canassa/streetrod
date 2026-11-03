/* F_12909: screen position (*px, *py) of the current slot, by game mode g_82ae */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1287B(void);
static int near F_1289C(int *px, int *py);

void F_12909(int *px, int *py)
{
    int n;

    switch (g_82ae) {
    default:
        F_1289C(px, py);
        break;
    case 0:
        *px = g_0702[n = F_1287B()][0];
        *py = g_0702[n][1];
        break;
    case 1:
        *px = 0x38;
        *py = 0x4c;
        break;
    }
}
