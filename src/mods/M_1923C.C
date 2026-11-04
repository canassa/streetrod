#include "STRUCTS.H"
#include "M_1923C.H"
#include "GBSS.H"
#include "M_00000.H"
#include "M_036A8.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int bl78de;
int g7b14o;
int g7d80q;
unsigned c_8b02[61];
int o_8f00;

/* F_1923C @ 0F38:9EBC (linear 0x1923c, 45 bytes) */


void F_1923C(void)
{
    c_8b02[g7b14o = 0] = o_8f00 = 0;
    c_8b02[g7d80q = 60] = bl78de = -536;
    g_0294 = 0;
}

/* F_19269 @ 0F38:9EE9 (linear 0x19269, 49 bytes) */


void F_19269(void)
{
    if (r_vid.mode == -6)
        g_6cd0 = (char far *)0x80000000L;
    else
        g_6cd0 = F_03709(0xfde8, 1);
    F_1923C();
}
