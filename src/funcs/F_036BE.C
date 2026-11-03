#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03614(int reset);

static void near F_036BE(void)
{
    g_64a6 = 0x800;
    g_6c60 = data_segment();
    F_03614(1);
}
