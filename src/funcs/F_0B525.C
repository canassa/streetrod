/* F_0B525 @ 0000:B525 (linear 0x0B525, 45 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B456(void);
static void near F_0B49C(int, int, int, int, int);
static void near F_0B425(void);

static void near F_0B525(void)
{
    F_0B456();
    F_0B49C(4, g_826e - 10, g_8270 - 5, 18, 12);
    F_0B425();
    g_6ca4 = 1;
}
