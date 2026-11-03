/* F_16B61: sound effect with a random start pitch */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_16564(int a, int b, int c, int d, int e);

void F_16B61(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(random_int(100) + 6200, 40, 10, 1, 10);
        F_164BD();
    }
}
