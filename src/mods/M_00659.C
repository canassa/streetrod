#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>

static int near F_005C6(void);
static int near F_00226(int mode);
static void near F_036BE(void);
static void near F_00316(int mode);
static void near F_004F4(int mode);
static void near F_00659(void);
static void near F_0503F(void);

/* F_00659 @ 0000:0659 (linear 0x00659, 22 bytes) */


static void near F_00659(void)
{
    if (g_8acc < 4)
        g_82c6 = 1;
    else
        g_82c6 = 0;
}

int g_0280 = 0;   /* DS:0280 */
int g_0282 = 0;   /* DS:0282 */
int g_0284 = 0;   /* DS:0284 */
int g_0286 = 0;   /* DS:0286 */
long g_288 = 0L;   /* DS:0288 */
struct Bitmap far *g_028c = 0;   /* DS:028C */
struct Bitmap far *g_0290 = 0;   /* DS:0290 */
struct Bitmap far *g_0294 = 0;   /* DS:0294 */
int g_0298[7] = {0, 19, 31, 29, 0, 0, 30};   /* DS:0298 */
int g_02a6[7] = {0, 0, 8, 0, 0, 0, 9};   /* DS:02A6 */
int g_02b4[7] = {0, 2, 0, 2, 0, 0, 0};   /* DS:02B4 */
int g_02c2[7] = {0, 3, 1, 3, 0, 3, 1};   /* DS:02C2 */
int g_02d0 = 0;   /* DS:02D0 */

/* F_0066F @ 0000:066F (linear 0x0066F, 580 bytes) */



int F_0066F(int argc, char **argv)
{
    int e;
    int b;
    int c;
    int d;
    int a;

    g_8247 = 0;
    g_8acc = F_005C6();
    e = 2;
    b = 0;
    g_0262 = 1;
    g_8bce = 0;
    g_0612 = 0;
    for (c = 1; c < argc; c++) {
        argv++;
        if (memcmp(*argv, "1", 2) == 0)
            e = 1;
        else if (strcmpi(*argv, "nomouse") == 0)
            g_0262 = 0;
        else if (strcmpi(*argv, "demo") == 0)
            g_8bce = 1;
        else if (strcmpi(*argv, "auto") == 0)
            g_0612 = 1;
        else if (strcmpi(*argv, "nouemem") == 0)
            g_0280 = 1;
    }
    b = F_00226(b);
    F_036BE();
    F_19269();
    F_00316(b);
    F_004F4(b);
    F_10BD9(g_screens, &g_screen1);
    F_01111();
    if (g_vid.mode == -2 || g_vid.mode == -6)
        F_11324(g_palette);
    F_15CC3(e, b);
    F_00659();
    d = F_1552B(1);
    F_11EA8();
    F_16481();
    F_1013D(0);
    if (g_vid.mode == -3 && g_0250)
        g_755c = F_0376B(320, 100, 0, -1, 0);
    g_7564 = F_03709(g_vid.mode != -2 && g_vid.mode != -6 ? 16000 : 18000, 1);
    a = F_15C4A(g_4c20, 0);
    if (a == 3) {
        g_70e2 = 1;
        d = F_1552B(2);
        F_15EF9();
        if (g_vid.mode == -2)
            F_15C07(g_4c28);
        F_15C4A(g_4c28, 1);
    } else {
        g_70e2 = 0;
        F_15C95(g_4c20);
    }
    F_0503F();
    F_00FEA();
}
