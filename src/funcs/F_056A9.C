/* F_056A9 @ 0000:56A9 (linear 0x056A9, 99 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03614(int reset);
static void near F_05584(void);

static void near F_056A9(void)
{
    F_03614(1);
    g_7eaa = 750L;
    if (g_0610)
        g_7eaa = 10000L;
    g_7eae = 10;
    F_26FE1(100);
    F_05584();
    g_05fc = 0L;
    g_0604 = g_0600;
    g_4970 = g_4972 = g_4974 = g_4976 = 0;
}
