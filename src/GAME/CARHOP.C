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

static struct CarhopSheet g_carhopSheets[2] = {{PIC_CARHOP_WALK_RIGHT, 6, 11}, {PIC_CARHOP_WALK_LEFT, 6, 11}};
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

    id5 = g_carhopSheets[n].pictureId;
    cols8 = g_carhopSheets[n].columns;
    cells = g_carhopSheets[n].cellCount;
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

    di_pic_size(PIC_CARHOP_SILHOUETTES, &w, &h, &x);
    me_set_bitmap(*pbm, g_pageBmps[2]->bits.p + BACKDROP_ROWS * g_pageBmps[2]->pitch, 0L, w, h,
                  r_vid.mode == VID_EGA ? VID_EGA : BITMAP_WORK);
    if (r_vid.mode == VID_EGA) {
        ea_rect(r_vid.front->bits.p, (*pbm)->bits.p - 0xaa0, (*pbm)->pitch, (*pbm)->height, 0, 0x44, EGA_PITCH);
    } else {
        pu_grab(0, 0x44, w, h, *pbm);
    }
}

void ch_skate(enum CarhopMode mode, BOOL carPresent)
{
    struct Blt patchBlt;
    struct Bitmap far *patchedBg;
    struct Blt carCopyBlt;
    struct Sprite *carhopSprite;
    int carhopX;
    struct Bitmap far *captureStrip;
    struct Rect *ignoredRect;
    struct Bitmap sceneHeader;
    BOOL stepActive;
    struct Seq moves[4];
    long nextTick;
    struct Bitmap far *carCutout;
    struct Rect overlayArea;
    struct Bitmap far *foreground, far *frame;
    struct Bitmap far *imagePtrs[13];
    struct Seq *motionPtr;
    struct Bitmap far *canvas;
    int stepNum;
    struct Rect carClipRect;
    int shift;

    captureStrip = &sceneHeader;
    ma_release();
    grab_strip(&captureStrip);
    if (mode == CARHOP_LEAVE) {
        carCopyBlt.w = 0x30;
        carCopyBlt.h = 0x29;
        carCopyBlt.dx = 0xc8;
        carCopyBlt.dy = carCopyBlt.sx = carCopyBlt.sy = 0;
        (*g_videoOps.copy)(g_carhopBg, captureStrip, &carCopyBlt, 1);
    }
    if (carPresent) {
        if (sp_car_clip(0, 0x44, captureStrip->width, captureStrip->height, &carClipRect)) {
            carCopyBlt.w = carClipRect.x2 - carClipRect.x1;
            carCopyBlt.h = carClipRect.y2 - carClipRect.y1 + 1;
            carCopyBlt.sx = carClipRect.x1 - g_picXDraw;
            carCopyBlt.sy = carClipRect.y1 - g_sprDrawY;
            carCutout = bm_grab(g_autoSprImagePtr, &carCopyBlt, ARENA_HIGH_STACK);
            carCopyBlt.sx = 0;
            carCopyBlt.sy = 0;
            carCopyBlt.dx = carClipRect.x1;
            carCopyBlt.dy = captureStrip->height - carCutout->height;
            (*g_videoOps.maskedBlit)(carCutout, captureStrip, &carCopyBlt, 1);
        } else
            carCutout = 0;
    }
    cp_backdrop(captureStrip, &imagePtrs[0], PIC_CARHOP_SILHOUETTES, &canvas, 0x29, 0x29, &ignoredRect, &ignoredRect);
    if (mode == CARHOP_ENTER) {
        foreground = ar_bm(0x30, 0x29, 1, ARENA_HIGH_STACK);
        patchBlt.w = 0x30;
        patchBlt.h = 0x29;
        patchBlt.sx = patchBlt.sy = 0;
        patchBlt.dx = patchBlt.dy = 0;
        (*g_videoOps.copy)(g_carhopBg, foreground, &patchBlt, 1);
        patchBlt.sx = 0xc8;
        (*g_videoOps.copy)(imagePtrs[0], foreground, &patchBlt, 2);
    }
    if (carPresent && carCutout)
        bm_and_mask(carCutout, imagePtrs[0], &carCopyBlt);
    overlayArea.x1 = overlayArea.y1 = 0;
    overlayArea.x2 = imagePtrs[0]->width - 1;
    overlayArea.y2 = imagePtrs[0]->height - 1;
    cut_sheet(&imagePtrs[1], mode);
    frame = ar_bm(0x30, 0x29, 1, ARENA_HIGH_STACK);
    carhopSprite =
        sp_setup(mode == CARHOP_ENTER ? -40 : 200, 0x6c, &frame, 0, 1, canvas, imagePtrs[0], &overlayArea, 0);
    motionPtr = mode == CARHOP_ENTER ? g_carhopInSeq : g_carhopOutSeq;
    for (stepNum = 0; stepNum < 4; stepNum++)
        moves[stepNum] = *motionPtr++;
    carhopSprite->motionStep = moves;
    if (mode == CARHOP_LEAVE) {
        carhopSprite->page->rect = g_carhopRect;
        carhopSprite->page->link->rect = g_carhopRect;
    }
    g_spriteBottomCut = 0;
    for (stepNum = 0; stepNum < 0x54; stepNum++) {
        carhopX = carhopSprite->x;
        shift = carhopX & ~r_vid.xByteMask;
        bm_shift_right(imagePtrs[g_carhopFrames[stepNum]], shift, frame);
        nextTick = g_tick + 2;
        stepActive = (*g_videoOps.spriteStep)(carhopSprite);
        while (nextTick > g_tick)
            ;
    }
    stepActive = (*g_videoOps.spriteStep)(carhopSprite);
    g_carhopRect = carhopSprite->page->rect;
    if (mode == CARHOP_LEAVE) {
        cp_patch_bg(g_carhopBg, 0, 0, 0x30, 0x29, 0xc8, 0x44, 0xa3, 2);
    } else {
        patchedBg = ar_bm(frame->width, 0x29, 1, ARENA_HIGH_STACK);
        patchBlt.w = frame->width;
        patchBlt.h = 0x29;
        patchBlt.sx = patchBlt.sy = 0;
        patchBlt.dx = patchBlt.dy = 0;
        (*g_videoOps.copy)(g_carhopBg, patchedBg, &patchBlt, 1);
        (*g_videoOps.maskedBlit)(frame, patchedBg, &patchBlt, 1);
        (*g_videoOps.maskedBlit)(foreground, patchedBg, &patchBlt, 0);
        cp_patch_bg(patchedBg, 0, 0, patchedBg->width, 0x29, 0xc8, 0x44, 0xa3, 2);
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
