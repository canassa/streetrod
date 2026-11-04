#include "GLOBALS.H"
#include "PROTOS.H"

/* F_193D4 @ 0F38:A054 (linear 0x193d4, 42 bytes) */


void F_193D4(void)
{
    c_8b02[g7b14o = 0] = o_8f00 = 0;
    c_8b02[g7d80q = 60] = bl78de;
    g_0294 = 0;
}

/* F_193FE @ 0F38:A07E (linear 0x193fe, 32 bytes) */


void F_193FE(void)
{
    c_8b02[g7b14o = 0] = o_8f00;
    c_8b02[g7d80q = 60] = bl78de;
}

/* F_1941E @ 0F38:A09E (linear 0x1941E, 26 bytes) */


void F_1941E(int n)
{
    if (n == 0)
        g7b14o = 0;
    else
        g7b14o -= n;
}

/* F_19438 @ 0F38:A0B8 (linear 0x19438, 66 bytes) */


void F_19438(int n)
{
    if (n == 0)
        c_8b02[g7d80q = 60] = bl78de;
    else {
        g7d80q += n;
        if (g7d80q == 60)
            c_8b02[g7d80q] = bl78de;
        else
            c_8b02[g7d80q] = c_8b02[g7d80q + 1] - 1;
    }
}
