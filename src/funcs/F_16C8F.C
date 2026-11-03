/* F_16C8F: sound effect, two noise bursts each followed by a click run */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164E9(int a, int b, int c, int d);
static void near F_167E4(int a, int b, int c);

void F_16C8F(void)
{
    if (g_47ce) {
        F_164A2();
        F_164E9(2000, 8000, 100, 10);
        F_167E4(120, 2, 30);
        F_164A2();
        F_164E9(4000, 9000, 100, 10);
        F_167E4(220, 2, 20);
        F_164BD();
    }
}
