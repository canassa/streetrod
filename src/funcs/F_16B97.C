/* F_16B97: sound effect */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_16564(int a, int b, int c, int d, int e);

void F_16B97(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(5200, 40, 10, 1, 10);
        F_164BD();
    }
}
