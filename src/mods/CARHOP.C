#include "STRUCTS.H"
#include "CARHOP.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "DASH.H"
#include "DISK.H"
#include "EGALATCH.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "POPUP.H"
#include "SPRITE.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Rect g_carhopRect;                 /* DS:6CC4 */
static struct Bitmap far *g_carhopBg;          /* DS:6CCC */

struct Sheet g_carhopSheets[2] = {{1142, 6, 11}, {1141, 6, 11}};   /* DS:5A26 */
char g_carhopFrames[84] = {4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 11};   /* DS:5A32 */
struct Seq g_carhopInSeq[4] = {{78, 3, 0}, {3, 2, 0}, {3, 1, 0}, {-1, 0, 0}};   /* DS:5A86 */
struct Seq g_carhopOutSeq[4] = {{4, -1, 0}, {3, -2, 0}, {77, -3, 0}, {-1, 0, 0}};   /* DS:5A96 */
char g_5aa6 = 0;   /* DS:5AA6 */

/* cut_sheet @ 0F38:985C (linear 0x18BDC, 179 bytes)

   Cut a sheet bitmap into equal cells, one sub-bitmap per cell */

static void near cut_sheet(struct Bitmap far **out, int n)
{
    int id5;
    struct Blt r;
    int cells;
    int cols8;
    struct Bitmap far *sheet;
    int i;

    id5 = g_carhopSheets[n].id;
    cols8 = g_carhopSheets[n].cols;
    cells = g_carhopSheets[n].n;
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
        out[i] = bitmap_grab(sheet, &r, 2);
    }
}

/* grab_strip @ 0F38:990F (linear 0x18C8F, 202 bytes)

   Draw a bitmap into the work area, then copy it on to the front buffer */

static void near grab_strip(struct Bitmap far **pbm)
{
    int w, x, h;

    disk_pic_size(0x474, &w, &h, &x);
    mem_set_bitmap(*pbm, bp7316[2]->bits.p + 0x46 * bp7316[2]->pitch, 0L, w, h, r_vid.mode == -2 ? -2 : -1);
    if (r_vid.mode == -2) {
        egalatch_rect(r_vid.front->bits.p, (*pbm)->bits.p - 0xaa0, (*pbm)->pitch, (*pbm)->height, 0, 0x44, 0x28);
    } else {
        F_11A72(0, 0x44, w, h, *pbm);
    }
}

/* carhop_walk @ 0F38:99D9 (linear 0x18D59, 1156 bytes)

   Build the sprite animation of a moving car body: the cells from the sheet, the pages, the 0x54
   step run */

void carhop_walk(int a, int b)
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
    marks_release();
    grab_strip(&bm9);
    if (a == 1) {
        r3.w = 0x30;
        r3.h = 0x29;
        r3.dx = 0xc8;
        r3.dy = r3.sx = r3.sy = 0;
        (*b_78a2)(g_carhopBg, bm9, &r3, 1);
    }
    if (b) {
        if (sprite_car_clip(0, 0x44, bm9->width, bm9->height, &rca2)) {
            r3.w = rca2.x2 - rca2.x1;
            r3.h = rca2.y2 - rca2.y1 + 1;
            r3.sx = rca2.x1 - ac825et;
            r3.sy = rca2.y1 - aw8260y;
            clip = bitmap_grab(bu825a, &r3, 2);
            r3.sx = 0;
            r3.sy = 0;
            r3.dx = rca2.x1;
            r3.dy = bm9->height - clip->height;
            (*ab78aat)(clip, bm9, &r3, 1);
        } else
            clip = 0;
    }
    carpic_backdrop(bm9, &tab2[0], 0x474, &B7, 0x29, 0x29, &prc3, &prc3);
    if (a == 0) {
        bm27 = F_1934E(0x30, 0x29, 1, 2);
        r2.w = 0x30;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*b_78a2)(g_carhopBg, bm27, &r2, 1);
        r2.sx = 0xc8;
        (*b_78a2)(tab2[0], bm27, &r2, 2);
    }
    if (b && clip)
        bitmap_and_mask(clip, tab2[0], &r3);
    rcb1.x1 = rcb1.y1 = 0;
    rcb1.x2 = tab2[0]->width - 1;
    rcb1.y2 = tab2[0]->height - 1;
    cut_sheet(&tab2[1], a);
    fr = F_1934E(0x30, 0x29, 1, 2);
    sp2 = sprite_setup(a == 0 ? -40 : 200, 0x6c, &fr, 0, 1, B7, tab2[0], &rcb1, 0);
    p9 = a == 0 ? g_carhopInSeq : g_carhopOutSeq;
    for (i = 0; i < 4; i++)
        trk7[i] = *p9++;
    sp2->seq = trk7;
    if (a == 1) {
        sp2->page->r = g_carhopRect;
        sp2->page->link->r = g_carhopRect;
    }
    g_02d0 = 0;
    for (i = 0; i < 0x54; i++) {
        x = sp2->x;
        v3 = x & ~r_vid.mask;
        bitmap_shift_right(tab2[g_carhopFrames[i]], v3, fr);
        tm7 = g_tick + 2;
        okn = (*g78c6c)(sp2);
        while (tm7 > g_tick)
            ;
    }
    okn = (*g78c6c)(sp2);
    g_carhopRect = sp2->page->r;
    if (a == 1) {
        carpic_patch_bg(g_carhopBg, 0, 0, 0x30, 0x29, 0xc8, 0x44, 0xa3, 2);
    } else {
        t = F_1934E(fr->width, 0x29, 1, 2);
        r2.w = fr->width;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*b_78a2)(g_carhopBg, t, &r2, 1);
        (*ab78aat)(fr, t, &r2, 1);
        (*ab78aat)(bm27, t, &r2, 0);
        carpic_patch_bg(t, 0, 0, t->width, 0x29, 0xc8, 0x44, 0xa3, 2);
    }
    if (r_vid.mode != -2)
        (*dz78b6)(r_vid.front->bits.p, r_vid.back->bits.p);
    marks_release();
}

/* carhop_save_bg @ 0F38:9E5D (linear 0x191DD, 94 bytes)

   Allocate the 0x30 x 0x29 bitmap g_carhopBg and clear a 200 x 68 rectangle of it */

void carhop_save_bg(void)
{
    struct Blt pkt;

    g_carhopBg = F_1934E(0x30, 0x29, 0, 0);
    pkt.w = g_carhopBg->width;
    pkt.h = g_carhopBg->height;
    pkt.sx = 200;
    pkt.sy = 68;
    pkt.dx = pkt.dy = 0;
    (*b_78a2)(r_vid.back, g_carhopBg, &pkt, 1);
}
