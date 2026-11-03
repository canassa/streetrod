/* F_09AF4 @ 0000:9AF4 (linear 0x09AF4, 209 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_09A27(int, int, int);
static void near F_092A4(int, int, int, int, int, int *);
static void near F_09A83(int *, int *, int *);

static int near F_09AF4(int n)
{
    int list[24], sel[24], kinds[24], choice;

    F_11421(-1, "", 0);
    F_09A83(list, sel, kinds);
    choice = F_142CF(0x24, 1, kinds, list, 0, 0);
    switch (choice) {
    case -4:
        return 0;
    case -3:
        if (g_7f86) {
            F_11421(-1, "", 0);
            F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
        }
        choice = F_14C8B();
        F_09A27(sel[choice - 1], n, 0);
        return 1;
    case 0x3ea:
        F_03AB8(0x198c);
        return 0;
    }
}
