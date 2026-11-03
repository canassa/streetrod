/* F_0B59B @ 0000:B59B (linear 0x0B59B, 124 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0B456(void);
static void near F_0B49C(int, int, int, int, int);
static void near F_0B425(void);

static void near F_0B59B(void)
{
    F_0B456();
    if (g_8264)
        F_0B49C(6, g_826a, g_826c, 20, 19);
    F_0B49C(5, g_8266, g_8268, 22, 19);
    F_0B49C(1, 0x3e, 0xa5, 0x35, 18);
    F_0B49C(3, 0xc4, 0x5a, 16, 14);
    F_0B425();
    g_6ca6 = 1;
}
