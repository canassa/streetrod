/* F_00226 @ 0000:0226 (linear 0x00226, 240 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_0018B(void);
static int near F_00082(int mode);

static int near F_00226(int mode)
{
    int herc;
    int pick;

    herc = 0;
    F_0E62F();
    if (g_0252)
        g_0250 = 1;
    if (F_0E5C3() && (g_0250 == 1 || F_0E584())) {
        g_0250 = 0;
        herc = 1;
    }
    if (g_0250)
        mode = g_5af1 == 3 ? 2 : 6;
    else
        mode = 1;
    if (herc)
        mode = 4;
    if (F_24602("Tandy", (char far *)0xF0000000L, 0xffff) ||
        F_24602("TANDY", (char far *)0xF0000000L, 0xffff))
        mode = 5;
    if (F_0018B())
        mode = 3;
    pick = F_00082(mode);
    if (pick)
        mode = pick;
    if (mode == 1 && g_0250)
        return 6;
    if (mode == 3) {
        g_0254 = 1;
        return 2;
    }
    return mode;
}
