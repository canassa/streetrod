#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

struct Sheet g_5a26[2] = {{1142, 6, 11}, {1141, 6, 11}};   /* DS:5A26 */
char g_5a32[84] = {4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 11};   /* DS:5A32 */
struct Seq g_5a86[4] = {{78, 3, 0}, {3, 2, 0}, {3, 1, 0}, {-1, 0, 0}};   /* DS:5A86 */
struct Seq g_5a96[4] = {{4, -1, 0}, {3, -2, 0}, {77, -3, 0}, {-1, 0, 0}};   /* DS:5A96 */
char g_5aa6 = 0;   /* DS:5AA6 */

/* F_18BDC: cut a sheet bitmap into equal cells, one sub-bitmap per cell */


static void near F_18BDC(struct Bitmap far **out, int n)
{
    int id5;
    struct Blt r;
    int cells;
    int cols8;
    struct Bitmap far *sheet;
    int i;

    id5 = g_5a26[n].id;
    cols8 = g_5a26[n].cols;
    cells = g_5a26[n].n;
    sheet = F_15BBE(id5, 2);
    r.w = sheet->width / cols8;
    r.h = sheet->height >> 1;
    r.sx = 0;
    r.sy = 0;
    i = 0;
    for (; i < cells; i++, r.sx += r.w) {
        if (i == cols8) {
            r.sx = 0;
            r.sy = r.h;
        }
        out[i] = F_1A89E(sheet, &r, 2);
    }
}

/* F_18C8F: draw a bitmap into the work area, then copy it on to the front buffer */


static void near F_18C8F(struct Bitmap far **pbm)
{
    int w, x, h;

    F_15B7F(0x474, &w, &h, &x);
    F_03904(*pbm, g_731e->bits.p + 0x46 * g_731e->pitch, 0L, w, h, g_vid.mode == -2 ? -2 : -1);
    if (g_vid.mode == -2) {
        F_24E78(g_vid.front->bits.p, (*pbm)->bits.p - 0xaa0, (*pbm)->pitch, (*pbm)->height, 0, 0x44, 0x28);
    } else {
        F_11A72(0, 0x44, w, h, *pbm);
    }
}

/* F_18D59: build the sprite animation of a moving car body: the cells from the sheet, the pages, the 0x54 step run */


void F_18D59(int a, int b)
{
    struct Blt r2;
    struct Bitmap far *t;
    struct Sprite *sp2;
    struct Blt r3;
    struct Rect rca2;
    struct Rect rcb1;
    struct Bitmap far *bm27, far *fr;
    struct Seq trk7[4];
    long tm7;
    struct Bitmap far *clip;
    int okn;
    struct Bitmap hdrz;
    struct Bitmap far *bm9;
    struct Rect *prc3;
    int x, v3;
    struct Bitmap far *tab2[13];
    struct Seq *p9;
    struct Bitmap far *B7;
    int i;

    bm9 = &hdrz;
    F_193FE();
    F_18C8F(&bm9);
    if (a == 1) {
        r3.w = 0x30;
        r3.h = 0x29;
        r3.dx = 0xc8;
        r3.dy = r3.sx = r3.sy = 0;
        (*g_78a2)(g_6ccc, bm9, &r3, 1);
    }
    if (b) {
        if (F_19518(0, 0x44, bm9->width, bm9->height, &rca2)) {
            r3.w = rca2.x2 - rca2.x1;
            r3.h = rca2.y2 - rca2.y1 + 1;
            r3.sx = rca2.x1 - g_825e;
            r3.sy = rca2.y1 - g_8260;
            clip = F_1A89E(g_825a, &r3, 2);
            r3.sx = 0;
            r3.sy = 0;
            r3.dx = rca2.x1;
            r3.dy = bm9->height - clip->height;
            (*g_78aa)(clip, bm9, &r3, 1);
        } else
            clip = 0;
    }
    F_1783E(bm9, &tab2[0], 0x474, &B7, 0x29, 0x29, &prc3, &prc3);
    if (a == 0) {
        bm27 = F_1934E(0x30, 0x29, 1, 2);
        r2.w = 0x30;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*g_78a2)(g_6ccc, bm27, &r2, 1);
        r2.sx = 0xc8;
        (*g_78a2)(tab2[0], bm27, &r2, 2);
    }
    if (b && clip)
        F_1A79B(clip, tab2[0], &r3);
    rcb1.x1 = rcb1.y1 = 0;
    rcb1.x2 = tab2[0]->width - 1;
    rcb1.y2 = tab2[0]->height - 1;
    F_18BDC(&tab2[1], a);
    fr = F_1934E(0x30, 0x29, 1, 2);
    sp2 = F_195F3(a == 0 ? -40 : 200, 0x6c, &fr, 0, 1, B7, tab2[0], &rcb1, 0);
    p9 = a == 0 ? g_5a86 : g_5a96;
    for (i = 0; i < 4; i++)
        trk7[i] = *p9++;
    sp2->seq = trk7;
    if (a == 1) {
        sp2->page->r = g_6cc4;
        sp2->page->link->r = g_6cc4;
    }
    g_02d0 = 0;
    for (i = 0; i < 0x54; i++) {
        x = sp2->x;
        v3 = x & ~g_vid.mask;
        F_1A03F(tab2[g_5a32[i]], v3, fr);
        tm7 = g_tick + 2;
        okn = (*g_78c6)(sp2);
        while (tm7 > g_tick)
            ;
    }
    okn = (*g_78c6)(sp2);
    g_6cc4 = sp2->page->r;
    if (a == 1) {
        F_18ADB(g_6ccc, 0, 0, 0x30, 0x29, 0xc8, 0x44, 0xa3, 2);
    } else {
        t = F_1934E(fr->width, 0x29, 1, 2);
        r2.w = fr->width;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*g_78a2)(g_6ccc, t, &r2, 1);
        (*g_78aa)(fr, t, &r2, 1);
        (*g_78aa)(bm27, t, &r2, 0);
        F_18ADB(t, 0, 0, t->width, 0x29, 0xc8, 0x44, 0xa3, 2);
    }
    if (g_vid.mode != -2)
        (*g_78b6)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_193FE();
}
