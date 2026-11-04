#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

static void near F_0A35C(struct Bitmap far *, int, struct Bitmap far *);
static void near F_098BA(int);
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

/* F_0A416 @ 0000:A416 (linear 0x0A416, 302 bytes) */



static void near F_0A416(void)
{
    struct Bitmap far *pict;
    int going;
    int i, key;

    F_193FE();
    pict = F_15BBE(0x4c9, 2);
    i = 0;
    F_13DE6(0x18);
    g_05cc = g_tick + 10;
    going = 1;
    do {
        while ((key = F_01417(3000)) == 0)
            ;
        switch (key) {
        case -26:
            going = 0;
            break;
        case 1000:
            if (i >= 31)
                going = 0;
            else {
                F_10AFD(1);
                F_0A35C(pict, g_525a[1 + g_521e[i].frame], g_vid.front);
                g_05cc = g_tick + (int)g_521e[i].delay;
                i++;
            }
            break;
        }
    } while (going);
    F_10AFD(1);
    F_0A35C(pict, g_525a[1 + 1], g_vid.front);
    F_0A35C(pict, g_525a[1 + 1], g_vid.back);
    F_13F21(1);
    F_193FE();
}

/* F_0A544 @ 0000:A544 (linear 0x0A544, 1607 bytes) */



static void near F_0A544(void)
{
    struct Bitmap far *pic4;
    int h8;
    int r;
    int flag8;
    struct Bitmap far *pic15;
    struct Bitmap far *pic24;
    struct Blt blt;
    struct Cell *p3;
    int v30;
    struct Bitmap far *pic58;
    int v2a;
    int i2;
    int v32;
    int minx;
    int miny;
    int v38l;
    struct Bitmap far *pic3;
    struct Cell *p2m;
    int w8;
    int cntz;
    struct Blt blt2k;
    struct Bitmap far *pic6m;
    int savedl;

    flag8 = 0;
    F_120FE(-4);
    F_113A2(0xbe);
    F_119B8(9, 8, 0x14, 2);
    g_8240 = 0;
    savedl = g_04c2;
    g_04c2 = 0xc8;
    (*g_78b6)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_118D4(0x408, -1);
    g_04c2 = savedl;
    (*g_present)();
    F_03A83(0x2ee);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    g_0284 = 1;
    pic15 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic15);
    F_15C95(g_4c28);
    g_4f4c = 1;
    r = F_15C4A(g_5318, 0);
    r = F_15C4A(g_5328, 0);
    (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
    g_8240 = 0;
    (*g_present)();
    g_8176 = 0x4c1;
    F_098BA(0x15);
    F_181C8(4, g_7648, g_7d84->fe, g_7d84->ff, 1, 0x98, 0, g_5030[g_8272.m827e]);
    g_8176 = 0x4dc;
    pic24 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic24);
    pic3 = F_15BBE(6, 1);
    pic4 = F_1934E(0x50, 0x2c, 0, 3);
    p3 = g_526a;
    blt.w = g_526a[0].w;
    blt.h = g_526a[0].h;
    blt.sx = g_526a[0].sx;
    blt.sy = g_526a[0].sy;
    blt.dx = g_526a[0].x - 0x50;
    blt.dy = 0x2c - g_526a[0].h;
    (*g_78aa)(pic3, pic15, &blt, 1);
    { }
    blt.w = p3->w;
    blt.h = p3->h;
    blt.sx = p3->x - 0x50;
    blt.sy = 0x2c - p3->h;
    blt.dx = blt.sx;
    blt.dy = blt.sy;
    (*g_78a2)(pic15, pic24, &blt, 1);
    F_03A83(10);
    for (i2 = 1; i2 < 0x14; i2++) {
        if (i2 == 13)
            F_10AFD(0x14);
        p3 = &g_526a[i2];
        F_19ED0(pic24, pic4, 1);
        blt.w = p3->w;
        blt.h = p3->h;
        blt.sx = p3->sx;
        blt.sy = p3->sy;
        blt.dx = p3->x - 0x50;
        blt.dy = 0x2c - p3->h;
        (*g_78aa)(pic3, pic4, &blt, 1);
        blt.w = 0x50;
        blt.h = 0x2c;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0x78;
        blt.dy = 0x45;
        if (i2 < 0x13) {
            F_10AFD(10);
            (*g_78a2)(pic4, g_vid.front, &blt, 1);
        }
        if (i2 == 0x12)
            (*g_78a2)(pic4, g_vid.back, &blt, 1);
    }
    F_193D4();
    if (flag8 == 0) {
        pic58 = F_15BBE(7, 1);
        cntz = 9;
        if (g_vid.mode == -6) {
            F_1929A(10000);
            cntz--;
        }
        v32 = v38l = 0x28;
        v2a = v30 = -0x28;
        for (i2 = 0; i2 < cntz; i2++) {
            p2m = &g_52e2[i2];
            w8 = max((int)p2m->w + (int)p2m->x, v32 + v2a) - (minx = min((int)p2m->x, v32));
            h8 = max((int)p2m->h + (int)p2m->y, v38l + v2a);
            h8 -= miny = min((int)p2m->y, v38l);
            pic6m = F_1934E(w8, h8, 0, 2);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = minx;
            blt2k.sy = miny;
            blt2k.dx = 0;
            blt2k.dy = 0;
            (*g_78a2)(g_vid.back, pic6m, &blt2k, 1);
            blt2k.w = v2a = p2m->w;
            blt2k.h = v30 = p2m->h;
            blt2k.sx = p2m->sx;
            blt2k.sy = p2m->sy;
            blt2k.dx = (v32 = p2m->x) - minx;
            blt2k.dy = (v38l = p2m->y) - miny;
            (*g_78aa)(pic58, pic6m, &blt2k, 1);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = 0;
            blt2k.sy = 0;
            blt2k.dx = minx;
            blt2k.dy = miny;
            F_10AFD(8);
            (*g_78a2)(pic6m, g_vid.front, &blt2k, 1);
            F_19438(1);
        }
    }
    blt2k.sx = minx;
    blt2k.sy = miny;
    F_03A83(0x50);
    F_10AFD(1);
    (*g_78a2)(g_vid.back, g_vid.front, &blt2k, 1);
    F_10AFD(1);
    (*g_78a2)(pic4, g_vid.front, &blt, 1);
    (*g_78a2)(pic4, g_vid.back, &blt, 1);
    F_03A83(0x32);
    F_03AB8(0x183c);
    g_8240 = 0;
    F_1923C();
    F_15C95(g_5328);
    F_15C95(g_5318);
}

struct Btn g_5338[7] = {{&g_6e64, 0, 0, 16, 149, 40, 5}, {&g_6e64, 0, 0, 16, 149, 96, 5}, {&g_6e64, 0, 0, 16, 149, 152, 5}, {&g_6e64, 0, 0, 16, 149, 208, 5}, {&g_6e64, 0, 0, 16, 149, 264, 5}, {&g_82b8, 0, 0, 32, 149, 0, 5}, {&g_82b8, 32, 0, 32, 149, 288, 5}};   /* DS:5338 */
struct Pair g_539a[6] = {{32, 8}, {56, 40}, {112, 40}, {168, 40}, {224, 40}, {280, 8}};   /* DS:539A */

/* F_0AB8B @ 0000:AB8B (linear 0x0AB8B, 1210 bytes) */



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
            F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_8272.m827e]);
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
                    if (g_8272.m8278 != 1)
                        g_4970++;
                    if (g_7d40) {
                        if (g_7676 == 0) {
                            g_4976 = 0;
                            g_817d = 0;
                            g_4972 -= 3;
                            g_4974 -= 2;
                        } else if (g_7eaa.m7ec0 == 0)
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
                    if (g_7676 && g_8272.m8278 != 1) {
                        if (g_7eaa.m7ec0 == 0)
                            g_4972++;
                        else
                            g_4974++;
                    }
                    if (g_8272.m8278 != 1) {
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
                F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_8272.m827e]);
                n = 0;
                F_13F21(1);
            }
            break;
        case 3:
            if (g_7f86 == 0)
                goto out;
            if (n > 0 || g_7d40) {
                F_03AB8(0x122c);
                F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_8272.m827e]);
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
