/* F_0B552 @ 0000:B552 (linear 0x0B552, 73 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B456(void);
static void near F_0B49C(int, int, int, int, int);
static void near F_0B425(void);

static void near F_0B552(void)
{
    F_0B456();
    F_0B49C(2, 0x34, 100, 16, 23);
    F_0B49C(4, g_826e - 1, g_8270 - 1, 10, 9);
    F_0B425();
    g_6ca8 = 1;
    g_6ca4 = 1;
}
