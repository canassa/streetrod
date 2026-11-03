/* F_29E8B @ 2645:3A3B (linear 0x29E8B, 28 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_29E2E(void);
static void near F_29E5D(void);

static void near F_29E8B(int n)
{
    if (*(int *)&g_8aca == 0) {
        F_29E2E();
        F_29E5D();
    }
    F_29398(n);
}
