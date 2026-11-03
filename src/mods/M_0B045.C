#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0B045(int id, char c);
static int near F_095F1(void);
static int near F_0984E(int chance, int arg);
static void near F_09A27(int n, int a, int mode);
static void near F_0A416(void);

/* F_0B045 @ 0000:B045 (linear 0x0B045, 71 bytes) */


static void near F_0B045(int id, char c)
{
    struct Hot *h;

    h = g_20aa;
    if (id == 14)
        g_20aa[0].f4 = "Wanna quit ?" - g_239e;
    else
        h->f4 = "Back to the garage" - g_239e;
    h[1].f0 = id;
    h[2].f0 = id;
    h[3].f0 = id;
    h[4].f0 = id;
    *g_57e9 = c;
}

/* F_0B08C @ 0000:B08C (linear 0x0B08C, 829 bytes) */



void F_0B08C(void)
{
    struct Bitmap far *pict;
    struct Blt blt;
    char mode;
    int i;
    int flag;
    int ch;
    int who;
    int old;

    old = 0;
    mode = 4;
    flag = 0;
    g_vid.f16 = 0;
    F_120FE(-3);
    F_193D4();
    g_82b8 = F_15BBE(0x4e7, 1);
    g_6e64 = F_15BBE(0x4e9, 1);
    F_118D4(0x445, -1);
    F_10B8C();
    pict = F_15BBE(0x4e8, 2);
    blt.w = pict->width;
    blt.h = 5;
    blt.dx = blt.dy = blt.sx = blt.sy = 0;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    blt.h = pict->height - 5;
    blt.sy = 5;
    blt.dy = 190 - blt.h;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    for (i = 0; i < 7; i++) {
        blt.w = g_5338[i].w;
        blt.h = g_5338[i].h;
        blt.sx = g_5338[i].sx;
        blt.sy = g_5338[i].sy;
        blt.dx = g_5338[i].x;
        blt.dy = g_5338[i].y;
        (*g_78a2)(*g_5338[i].src, g_vid.back, &blt, 1);
    }
    F_11421(-1, "", 0);
    F_11593(g_49dc);
    (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
    g_vid.f16 = 1;
    (*g_present)();
    g_7f86 = 0;
    g_6c84 = -1;
    g_6c88 = 0;
    F_120FE(1);
    F_120FE(-1);
    g_0284 = 6;
    F_0B045(14, 16);
    for (;;) {
        F_13DE6(0x25);
        if (flag == 0)
            g_05cc = (mode == 4 ? 140u : 280u) + g_tick;
        else
            g_05cc = g_tick + 5;
        flag = 0;
        who = mode == 4 ? 0 : F_095F1();
        while ((ch = F_01417(3000)) == 0)
            ;
        switch (ch) {
        case 1:
            F_13F21(1);
            F_0B045(11, 34);
            g_0284 = 1;
            return;
        case 1000:
            if (random_int(100) < 6) {
                F_120FE(-4);
                F_11421(-1, "", 0);
                F_0A416();
                F_120FE(-2);
            }
            F_09A27(F_0984E(who, 1), mode, 1);
            mode = 1;
            g_7f32[0] = 0;
            F_11421(g_51ee, " ", g_8bc0);
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            F_13F21(0);
        }
    }
}
