#include "GLOBALS.H"
#include "PROTOS.H"

/* F_1923C @ 0F38:9EBC (linear 0x1923c, 45 bytes) */


void F_1923C(void)
{
    g_8b02[g_7b14 = 0] = g_8f00 = 0;
    g_8b02[g_7d80 = 60] = g_78de = -536;
    g_0294 = 0;
}

/* F_19269 @ 0F38:9EE9 (linear 0x19269, 49 bytes) */


void F_19269(void)
{
    if (g_vid.mode == -6)
        g_6cd0 = (char far *)0x80000000L;
    else
        g_6cd0 = F_03709(0xfde8, 1);
    F_1923C();
}
