#include "STRUCTS.H"
#include "MARKS.H"
#include "ARENA.H"
#include "MAIN.H"

/* marks_clear @ 0F38:A054 (linear 0x193D4, 42 bytes) */

void marks_clear(void)
{
    c_8b02[g7b14o = 0] = o_8f00 = 0;
    c_8b02[g7d80q = 60] = bl78de;
    g_0294 = 0;
}

/* marks_release @ 0F38:A07E (linear 0x193FE, 32 bytes) */

void marks_release(void)
{
    c_8b02[g7b14o = 0] = o_8f00;
    c_8b02[g7d80q = 60] = bl78de;
}

/* marks_pop_low @ 0F38:A09E (linear 0x1941E, 26 bytes) */

void marks_pop_low(int n)
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
