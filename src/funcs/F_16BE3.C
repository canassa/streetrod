/* F_16BE3: sound effect, three repeats of a noise burst, a click and a blip */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_164E9(int a, int b, int c, int d);
static void near F_167E4(int a, int b, int c);
static void near F_16830(int a, int b, int c, int d, int e);
static void near F_1687C(int a, int b, int c, int d, int e);

void F_16BE3(void)
{
    register int i;
    int count;

    if (g_47ce) {
        count = 3;
        for (i = count; i; i--) {
            F_164A2();
            F_164E9(3000, 8000, 60, 10);
            F_16830(200, random_int(30) + 18, 15, 5, 2);
            F_167E4(random_int(300) + 40, 2, 30);
            F_1687C(16, random_int(100) + 50, 15, 5, 4);
        }
        F_164BD();
    }
}
