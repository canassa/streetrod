/* F_16CF3: sound effect, a falling sweep */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_16532(int start, int end, int step, int delay);

void F_16CF3(void)
{
    if (g_47ce) {
        F_164A2();
        F_16532(130, 90, 4, 1000);
        F_164BD();
    }
}
