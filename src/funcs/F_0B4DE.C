/* F_0B4DE @ 0000:B4DE (linear 0x0B4DE, 71 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B456(void);
static void near F_0B49C(int, int, int, int, int);
static void near F_0B425(void);

static void near F_0B4DE(void)
{
    F_0B456();
    F_0B49C(2, 0x3f, 100, 12, 23);
    F_0B49C(1, 0x54, 0xa8, 0x1f, 15);
    F_0B425();
    g_6ca6 = 1;
    g_6ca8 = 1;
}
