/* F_16BC1: sound effect */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_167E4(int a, int b, int c);

void F_16BC1(void)
{
    if (g_47ce) {
        F_164A2();
        F_167E4(200, 10, 60);
        F_164BD();
    }
}
