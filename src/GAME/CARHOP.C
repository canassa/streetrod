#include "CARHOP.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "DASH.H"
#include "DISK.H"
#include "EGALATCH.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "PC.H"
#include "POPUP.H"
#include "SPRITE.H"
#include "SYS.H"
#include "VIDEO.H"

static struct Rect g_carhopRect;
static struct Bitmap far *g_carhopBg;

static struct Sheet g_carhopSheets[2] = {{1142, 6, 11}, {1141, 6, 11}};
static char g_carhopFrames[84] = {4,  4,  4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 9, 10, 10, 1, 1,  2,  2,
                                  3,  3,  4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,  9,  9, 10, 10, 1,
                                  1,  2,  2, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8,  8,  9, 9,  9,  9,
                                  10, 10, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,  8,  8, 9,  9,  11};
static struct Seq g_carhopInSeq[4] = {{78, 3, 0}, {3, 2, 0}, {3, 1, 0}, {-1, 0, 0}};
static struct Seq g_carhopOutSeq[4] = {{4, -1, 0}, {3, -2, 0}, {77, -3, 0}, {-1, 0, 0}};
char g_keepMask = 0;

/* Cut a sheet bitmap into equal cells, one sub-bitmap per cell */

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
    sheet = di_bm(id5, ARENA_HIGH_STACK);
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
        out[i] = bm_grab(sheet, &r, ARENA_HIGH_STACK);
    }
}

/* Draw a bitmap into the work area, then copy it on to the front buffer */

static void near grab_strip(struct Bitmap far **pbm)
{
    int w, x, h;

    di_pic_size(0x474, &w, &h, &x);
    me_set_bitmap(*pbm, g_pageBmps[2]->bits.p + BACKDROP_ROWS * g_pageBmps[2]->pitch, 0L, w, h,
                  r_vid.mode == VID_EGA ? VID_EGA : -1);
    if (r_vid.mode == VID_EGA) {
        ea_rect(r_vid.front->bits.p, (*pbm)->bits.p - 0xaa0, (*pbm)->pitch, (*pbm)->height, 0, 0x44, EGA_PITCH);
    } else {
        pu_grab(0, 0x44, w, h, *pbm);
    }
}

/* Build the sprite animation of a moving car body: the cells from the sheet, the pages, the 0x54
   step run */

void ch_skate(enum CarhopMode a, int b)
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
    ma_release();
    grab_strip(&bm9);
    if (a == CARHOP_LEAVE) {
        r3.w = 0x30;
        r3.h = 0x29;
        r3.dx = 0xc8;
        r3.dy = r3.sx = r3.sy = 0;
        (*g_videoOps.copy)(g_carhopBg, bm9, &r3, 1);
    }
    if (b) {
        if (sp_car_clip(0, 0x44, bm9->width, bm9->height, &rca2)) {
            r3.w = rca2.x2 - rca2.x1;
            r3.h = rca2.y2 - rca2.y1 + 1;
            r3.sx = rca2.x1 - g_picXDraw;
            r3.sy = rca2.y1 - g_sprDrawY;
            clip = bm_grab(g_autoSprImagePtr, &r3, ARENA_HIGH_STACK);
            r3.sx = 0;
            r3.sy = 0;
            r3.dx = rca2.x1;
            r3.dy = bm9->height - clip->height;
            (*g_videoOps.maskedBlit)(clip, bm9, &r3, 1);
        } else
            clip = 0;
    }
    cp_backdrop(bm9, &tab2[0], 0x474, &B7, 0x29, 0x29, &prc3, &prc3);
    if (a == CARHOP_ENTER) {
        bm27 = ar_bm(0x30, 0x29, 1, ARENA_HIGH_STACK);
        r2.w = 0x30;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*g_videoOps.copy)(g_carhopBg, bm27, &r2, 1);
        r2.sx = 0xc8;
        (*g_videoOps.copy)(tab2[0], bm27, &r2, 2);
    }
    if (b && clip)
        bm_and_mask(clip, tab2[0], &r3);
    rcb1.x1 = rcb1.y1 = 0;
    rcb1.x2 = tab2[0]->width - 1;
    rcb1.y2 = tab2[0]->height - 1;
    cut_sheet(&tab2[1], a);
    fr = ar_bm(0x30, 0x29, 1, ARENA_HIGH_STACK);
    sp2 = sp_setup(a == CARHOP_ENTER ? -40 : 200, 0x6c, &fr, 0, 1, B7, tab2[0], &rcb1, 0);
    p9 = a == CARHOP_ENTER ? g_carhopInSeq : g_carhopOutSeq;
    for (i = 0; i < 4; i++)
        trk7[i] = *p9++;
    sp2->seq = trk7;
    if (a == CARHOP_LEAVE) {
        sp2->page->rect = g_carhopRect;
        sp2->page->link->rect = g_carhopRect;
    }
    g_spriteBottomCut = 0;
    for (i = 0; i < 0x54; i++) {
        x = sp2->x;
        v3 = x & ~r_vid.mask;
        bm_shift_right(tab2[g_carhopFrames[i]], v3, fr);
        tm7 = g_tick + 2;
        okn = (*g_videoOps.spriteStep)(sp2);
        while (tm7 > g_tick)
            ;
    }
    okn = (*g_videoOps.spriteStep)(sp2);
    g_carhopRect = sp2->page->rect;
    if (a == CARHOP_LEAVE) {
        cp_patch_bg(g_carhopBg, 0, 0, 0x30, 0x29, 0xc8, 0x44, 0xa3, 2);
    } else {
        t = ar_bm(fr->width, 0x29, 1, ARENA_HIGH_STACK);
        r2.w = fr->width;
        r2.h = 0x29;
        r2.sx = r2.sy = 0;
        r2.dx = r2.dy = 0;
        (*g_videoOps.copy)(g_carhopBg, t, &r2, 1);
        (*g_videoOps.maskedBlit)(fr, t, &r2, 1);
        (*g_videoOps.maskedBlit)(bm27, t, &r2, 0);
        cp_patch_bg(t, 0, 0, t->width, 0x29, 0xc8, 0x44, 0xa3, 2);
    }
    if (r_vid.mode != VID_EGA)
        (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    ma_release();
}

/* Allocate the 0x30 x 0x29 bitmap g_carhopBg and clear a 200 x 68 rectangle of it */

void ch_save_bg(void)
{
    struct Blt pkt;

    g_carhopBg = ar_bm(0x30, 0x29, 0, ARENA_LOW_BASE);
    pkt.w = g_carhopBg->width;
    pkt.h = g_carhopBg->height;
    pkt.sx = 200;
    pkt.sy = 68;
    pkt.dx = pkt.dy = 0;
    (*g_videoOps.copy)(r_vid.back, g_carhopBg, &pkt, 1);
}
