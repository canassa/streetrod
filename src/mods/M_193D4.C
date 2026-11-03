#include "GLOBALS.H"
#include "PROTOS.H"

/* F_193D4 @ 0F38:A054 (linear 0x193d4, 42 bytes) */


void F_193D4(void)
{
    g_8b02[g_7b14 = 0] = g_8f00 = 0;
    g_8b02[g_7d80 = 60] = g_78de;
    g_0294 = 0;
}

/* F_193FE @ 0F38:A07E (linear 0x193fe, 32 bytes) */


void F_193FE(void)
{
    g_8b02[g_7b14 = 0] = g_8f00;
    g_8b02[g_7d80 = 60] = g_78de;
}

/* F_1941E @ 0F38:A09E (linear 0x1941E, 26 bytes) */


void F_1941E(int n)
{
    if (n == 0)
        g_7b14 = 0;
    else
        g_7b14 -= n;
}

/* F_19438 @ 0F38:A0B8 (linear 0x19438, 66 bytes) */


void F_19438(int n)
{
    if (n == 0)
        g_8b02[g_7d80 = 60] = g_78de;
    else {
        g_7d80 += n;
        if (g_7d80 == 60)
            g_8b02[g_7d80] = g_78de;
        else
            g_8b02[g_7d80] = g_8b02[g_7d80 + 1] - 1;
    }
}
