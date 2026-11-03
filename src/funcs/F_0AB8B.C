/* F_0AB8B @ 0000:AB8B (linear 0x0AB8B, 1210 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0A0F8(int, int);
static int near F_095F1(void);
static void near F_092A4(int, int, int, int, int, int *);
static int near F_09AF4(int);
static int near F_0984E(int, int);
static void near F_09A27(int, int, int);
static int near F_0C613(void);
static void near F_0A544(void);
static void near F_0A265(void);
static int near F_09D6D(int);
static void near F_0A416(void);

void F_0AB8B(void)
{
    int n;
    int rival;
    int choice;
    int flag;
    char class;
    int place;
    int status;

    n = 0;
    class = 4;
    flag = 0;
    F_11421(-1, "", 0);
    g_472a = 0x9e;
    g_472c = 0x80;
    F_118D4(0x445, -1);
    F_10B8C();
    F_11593(g_49dc);
    g_cursor.r.x2 = -1;
    F_120FE(1);
    (*g_present)();
    F_16381(0);
    if (random_int(100) < 25)
        F_0A416();
    F_120FE(-2);
    g_7f86 = 0;
    g_6c84 = -1;
    g_6c88 = 0;
    g_6ca2 = 1;
    g_0284 = 2;
    g_5ee = g_471c = g_4732 = 0;
    for (;;) {
        F_0A0F8(g_7f86, !g_6c84 & !g_6c88);
        F_13DE6(0x25);
        g_05cc = flag == 0 ? (unsigned)(class == 4 ? 140 : 280) + g_tick : g_tick + 5;
        flag = 0;
        rival = class == 4 ? 0 : F_095F1();
        F_06529(0);
        if (g_0282)
            return;
        while ((choice = F_01417(3000)) == 0)
            ;
        switch (choice) {
        case 1:
            g_7f32[0] = 0;
            F_11421(g_51ee, " ", g_8bc0);
            F_13F21(1);
            g_0284 = 1;
            F_08D26();
            return;
        case 1000:
            F_06529(0);
            if (g_0282)
                return;
            if (g_7f86 == 0)
                goto race;
            F_11421(-1, "", 0);
            F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
            n = 0;
            F_13F21(1);
            break;
        default:
out:
            F_13F21(1);
            break;
        case 4:
            if ((status = F_09AF4(class)) == 0)
                flag = 1;
            else
                n = 0;
            F_13F21(1);
            break;
        case 2:
            if (g_7f86 == 0) {
                g_05cc = g_tick;
race:
                F_09A27(F_0984E(rival, 0), class, 2);
                class = 1;
                F_13F21(1);
            } else {
                g_8256 = 1;
                F_11421(-1, "", 0);
                F_11421(g_51ee, "", g_8bc0);
test:
                while ((F_11421(-1, "", 0), F_09D6D(g_7d40))) {
                    g_7f32[0] = 0;
                    F_11421(g_51ee, " ", g_8bc0);
                    place = F_0C613();
                    (*g_78be)();
                    F_08D9C(-1);
                    if (g_8278 != 1)
                        g_4970++;
                    if (g_7d40) {
                        if (g_7676 == 0) {
                            g_4976 = 0;
                            g_817d = 0;
                            g_4972 -= 3;
                            g_4974 -= 2;
                        } else if (g_7ec0 == 0)
                            g_4976 = 1;
                        else {
                            F_16381(0);
                            F_13F21(1);
                            F_0A544();
                            g_0282 |= 2;
                            return;
                        }
                    }
                    if (place == 4) {
                        F_16381(0);
                        F_13F21(1);
                        F_0B08C();
                        F_120FE(-4);
                        g_0282 |= 4;
                        return;
                    }
                    if (g_7676 && g_8278 != 1) {
                        if (g_7ec0 == 0)
                            g_4972++;
                        else
                            g_4974++;
                    }
                    if (g_8278 != 1) {
                        g_7d84->fc++;
                        if (g_7676 == 0)
                            g_7d84->fd++;
                    }
                    if (place == 2 || place == 1) {
                        F_13F21(1);
                        g_0284 = place == 2 ? 0 : 1;
                        if (g_0284 == 1)
                            g_51ec = 1;
                        F_08D26();
                        g_51ec = 0;
                        return;
                    }
                    F_16381(0);
                    F_118D4(0x445, -1);
                    F_11593(g_49dc);
                    (*g_present)();
                    class = 4;
                    g_7f86 = 0;
                    g_6c84 = -1;
                    g_6c88 = 0;
                    n = 0;
                    if (g_4976 == 0)
                        goto out;
                    F_09A27(0x15, 4, 0);
                    goto test;
                }
                F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
                n = 0;
                F_13F21(1);
            }
            break;
        case 3:
            if (g_7f86 == 0)
                goto out;
            if (n > 0 || g_7d40) {
                F_03AB8(0x122c);
                F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
                n = 0;
            } else {
                F_0A265();
                n++;
                g_05fc += 0x111L;
            }
            F_13F21(1);
            break;
        case 5:
            if (g_6c84 == 0)
                g_6c88 = 1;
            F_13F21(1);
            break;
        }
    }
}
