/* F_09A27 @ 0000:9A27 (linear 0x09A27, 92 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_098BA(int);
static int near F_09556(void);
static void near F_092A4(int, int, int, int, int, int *);

static void near F_09A27(int n, int a, int mode)
{
    F_098BA(n);
    if (mode == 1)
        g_5a24 = 1;
    if (mode == 2 && g_7f82 == 0x15)
        g_5a24 = F_09556() == 1;
    F_092A4(a, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
}
