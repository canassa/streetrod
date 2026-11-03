/* F_16B3A: sound effect, only when sound is on */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_16564(int a, int b, int c, int d, int e);

void F_16B3A(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(3000, 10, 10, 60, 9);
        F_164BD();
    }
}
