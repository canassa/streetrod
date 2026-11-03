/* F_181C8: draw a pop-up picture (car part, kind 1..8) with its frame sprite, saving and restoring the screen behind it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_1751A(struct Band *A, struct Bitmap far **B, int n, int y2, int h, char far *mask, char c);
static void near F_176B6(struct Bitmap far **B, int n, int y2, int h, int color);
static void near F_17AAE(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2);

void F_181C8(int kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids)
{
    int n2;
    struct Blt blt;
    struct Bitmap far *bms6[4];
    int ang2;
    int w1;
    struct Bitmap far *scratch;
    int hh8;
    struct Sprite *spr3;
    int x0;
    struct Rect *pr16;
    int w2;
    int x2;
    struct Bitmap far *pb8;
    int top7;
    struct Rect *pr26;
    int h2;
    struct Bitmap far *bm48;
    int right;
    int tmpn;
    int h7;
    char far *mask3;
    int x7;
    struct Bitmap far *pan;

    top7 = bot - 0x45;
    F_120FE(-4);
again:
    n2 = (kind == 3 || kind == 7 || kind == 8) ? 1 : 4;
    F_15B7F(g_7d8e[idx].id, &w1, &h7, &x7);
    g_02d0 = g_02a6[g_0284];
    tmpn = (g_vid.mode != -2 && g_vid.mode != -6) ? g_02b4[g_0284] : g_02c2[g_0284];
    hh8 = (kind == 3 || kind == 7 || kind == 8) ? h7 - g_02d0 : g_0298[g_0284];
    x0 = ((0x140 - w1) / 2 + 4) & 0xfff8;
    right = kind == 2 ? 0x140 - x0 : x0 + w1;
    if (g_5a24 != 2) {
        if (kind == 4) {
            g_4f4a = 1;
            if (g_vid.mode == -2) {
                F_03904(g_5a0a, g_731e->bits.p, 0, 0x140, 0x46 - g_02d0, -2);
                F_24E78(g_vid.back->bits.p, g_5a0a->bits.p - 0x28 * top7, 0x28, g_5a0a->height, 0, top7, 0x28);
            } else {
                F_03904(g_5a0a, g_731e->bits.p, 0, 0x140, 0x46 - g_02d0, -1);
                F_11A72(0, top7, 0x140, g_5a0a->height, g_5a0a);
            }
        }
        if (kind == 4) {
            F_193D4();
            if (g_0284 == 3) {
                F_15B7F(0x449, &w2, &h2, &x2);
                g_028c = F_1934E(w2, h2, 0, 0);
                g_0290 = F_1934E(w2, h2, 0, 0);
            }
            if (g_0284 == 2 || g_0284 == 6)
                F_191DD();
        } else
            F_193FE();
        if (!g_0294)
            g_0294 = F_1934E(0x40, 0x19, 0, 0);
        F_17BE6(bms6, idx, flags, kk, q, ids, 1, n2);
        if (g_vid.mode == -6 && kind != 3 && kind != 7 && kind != 8) {
            F_176B6(bms6, n2, g_02d0, hh8, tmpn);
            bms6[1] = bms6[2] = bms6[3] = bms6[0];
            F_1941E(3);
        }
        if (kind != 3 && kind != 7 && kind != 8) {
            if (g_5a24 != 1) {
                g_5a22 = right;
                g_5a20 = kind == 2 ? x0 : 0;
            } else {
                g_5a22 = 0x140;
                g_5a20 = 0;
            }
            F_1783E(g_5a0a, &pan, mode2 ? 0x448 : 0, &pb8, 0x46, h7, &pr16, &pr26);
        }
    }
    g_825e = x0;
    g_8260 = bot - bms6[0]->height + 1;
    g_8262 = bms6[0]->height - g_02d0;
    if (g_8266 > 0) {
        g_8266 += x0;
        g_8268 += g_8260;
        g_826a += x0;
        g_826c += g_8260;
    }
    g_826e += x0;
    g_8270 += g_8260;
    g_8272 += x0;
    g_8274 += g_8260;
    ang2 = (kind == 1 || kind == 4) ? x0 - right : x0;
    if (g_5a24 != 2) {
        if (g_vid.mode == -2) {
            if (kind == 3 || kind == 8)
                mask3 = g_5a0a->bits.p + ((g_5a0a->height - hh8) * g_5a0a->pitch + x0 / 8);
            else
                mask3 = 0;
            F_1751A(g_7ec2, bms6, n2, g_02d0, hh8, mask3, tmpn);
        }
        if (!((g_vid.mode == -2 || g_vid.mode == -6) && g_vid.mode != -6)) {
            if (kind == 3 || kind == 7 || kind == 8) {
                bm48 = F_1934E(w1, h7 - g_02d0, 0, 1);
                if (kind != 7) {
                    blt.w = w1;
                    blt.h = h7 - g_02d0;
                    blt.sx = x0;
                    blt.sy = g_5a0a->height - h7;
                    blt.dx = blt.dy = 0;
                    (*g_78a2)(g_5a0a, bm48, &blt, 1);
                } else
                    F_11A72(x0, g_8260, bm48->width, bm48->height, bm48);
                blt.w = w1;
                blt.h = blt.fc = h7 - g_02d0;
                blt.dx = blt.dy = blt.sx = blt.sy = 0;
                (*g_78aa)(bms6[0], bm48, &blt, 1);
            } else if (g_vid.mode != -6)
                F_176B6(bms6, n2, g_02d0, hh8, tmpn);
        }
        if (!((g_vid.mode == -2 || g_vid.mode == -6) && g_vid.mode != -6)) {
            blt.w = w1;
            blt.h = h7 - g_02d0;
            blt.sx = blt.sy = 0;
            blt.dx = x0;
            blt.dy = bot - h7 + 1;
        }
        if (kind == 3) {
            if (g_vid.mode == -2)
                F_24EFB(g_7ec2[0].p, bms6[0]->pitch, bms6[0]->height - g_02d0,
                    g_vid.back->bits.p + ((bot - h7) * 0x28 + (x0 >> 3)) + 0x28, bms6[0]->pitch);
            else
                (*g_78a2)(bm48, g_vid.back, &blt, 1);
            (*g_78b6)(g_vid.back->bits.p, g_vid.front->bits.p);
            goto done;
        } else if (kind == 7 || kind == 8) {
            if (g_vid.mode == -2)
                F_24EFB(g_7ec2[0].p, bms6[0]->pitch, h7 - g_02d0,
                    g_vid.front->bits.p + ((bot - h7) * 0x28 + (x0 >> 3)) + 0x28, bms6[0]->pitch);
            else
                (*g_78a2)(bm48, g_vid.front, &blt, 1);
            F_1A92A(g_vid.front, g_vid.back, x0, bot - h7 + 1, x0 + w1 - 1, bot);
            goto done;
        }
    }
    spr3 = F_195F3(ang2, bot, bms6, (char (*)[8])g_7ec2, n2, pb8, pan, pr16, pr26);
    F_19B6E(spr3, kind, right);
    if (kind == 6)
        spr3->page->r = g_6cbc;
    if (kind == 4 || kind == 1 || kind == 2 || kind == 6) {
        g_0286 = 1;
        g_5a0e = 1;
        if (g_5a24 != 2)
            F_01AAD();
        while ((*g_78c6)(spr3))
            ;
        if (g_5a24 != 1)
            F_01B27();
        g_825a = (struct Bitmap far *)g_288;
        g_5a0e = 0;
        g_0286 = 0;
        g_6cbc = spr3->page->r;
    }
    if (g_0284 == 3 && kind == 4) {
        scratch = F_1934E(0x18, 7, 0, 2);
        blt.w = 0x10;
        blt.h = scratch->height;
        blt.sx = g_826e - g_825e;
        blt.sy = g_8270 - g_8260;
        blt.dx = blt.dy = 0;
        (*g_78a2)(bms6[0], scratch, &blt, 1);
        F_1A734(bms6[0], 0x472, (char)blt.sx, (char)blt.sy, 0);
        F_17AAE(0x40, 0x64, g_8b7c, bot, g_028c, 0x4c8, 0x4ca);
        F_17AAE(0x40, 0x64, g_8b7c, bot, g_0290, 0x449, 0x44a);
        blt.dx = blt.sx;
        blt.dy = blt.sy;
        blt.sx = blt.sy = 0;
        (*g_78a2)(scratch, bms6[0], &blt, 1);
    }
    F_1013D(2);
    if (g_vid.mode != -2)
        (*g_78b6)(g_vid.front->bits.p, g_vid.back->bits.p);
    if (g_5a24 == 1) {
        kind = 2;
        g_5a24 = 2;
        goto again;
    }
done:
    g_02d0 = g_5a22 = g_5a20 = 0;
    F_193FE();
    g_5a24 = 0;
    F_120FE(-2);
}
