/* F_0D031 @ 0000:D031 (linear 0x0D031, 429 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0CDAF(struct Eng *p);
static void near F_0CF5C(void);

static void near F_0D031(void)
{
    int one;
    struct Node *slot;

    one = 1;
    F_120FE(-4);
    F_120FE(1);
    F_00E24();
    F_0CDAF(&g_78e0);
    F_0CF5C();
    if (g_7ec0 == 1 && g_7d40 == 0) {
        if (g_7f88 = !random_int(4))
            g_755a = random_int(255) + 0x1fe;
    } else
        g_7f88 = 0;
    g_79a6 = 0;
    g_8aca = 1;
    F_08E2D();
    F_01B27();
    g_05cc = 0;
    F_00E24();
    F_11593(g_49dc);
    if (g_7d30 > 200) {
        if (random_int(3) > 0) {
            if (g_7eb0->f20 > 0)
                g_7eb0->f20 = g_7eb0->f20 < random_int(4) ? random_int(4) : g_7eb0->f20;
            else if (g_7eb0->f20 < 0)
                g_7eb0->f20 = -random_int(8) < g_7eb0->f20 ? -random_int(8) : g_7eb0->f20;
            else
                g_7eb0->f20 = random_int(8) - 4;
        }
        slot = g_7eb0->slot12;
        slot->b += (g_4808[slot->idx].kind == 0 ? (g_7ec0 == 1 ? 700 : 300) : (g_7ec0 == 1 ? 400 : 200)) + random_int(300);
        if (slot->b > 10000)
            slot->b = 10000;
    }
}
