/* F_15C07: load every picture of the zero terminated list of picture numbers (see F_1563A). Does
   nothing when g_0280 is set. Returns 1 */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_1563A(int n, int load, long dst);

int F_15C07(int *list)
{
    int n;

    if (g_0280)
        return 1;
    while (n = *list++)
        F_1563A(n, 1, 0L);
    return 1;
}
