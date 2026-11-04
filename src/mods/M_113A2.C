#include "STRUCTS.H"
#include "M_113A2.H"
#include "M_00000.H"
#include "M_10B7C.H"
#include "M_13682.H"
#include "M_19ED0.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int o_8250;
int au8252x;
int as8254y;
int aq8256z;

char *g_msgs[7] = {"0", "1", "2", "3", "4", "N", "D"};   /* DS:0482 */

void F_113A2(int y)
{
    au8252x = y + 1;
    as8254y = 0xa0;
    o_8250 = 0x58;
    aq8256z = 0;
    x_colors = bustyle->colors;
    F_13682(r_vid.back, 0, y, 320, 10);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, 0, y, 319, y + 9);
}
