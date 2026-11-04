#include <conio.h>
#include <dos.h>
#include "STRUCTS.H"
#include "CARPIC.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARHOP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "EGABLIT.H"
#include "EGALATCH.H"
#include "GAME.H"
#include "GARAGE.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MASKOR.H"
#include "MEM.H"
#include "PC.H"
#include "POPUP.H"
#include "RACE.H"
#include "SPRITE.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Rect g_lastCarRect;                 /* DS:6CBC */
static void near blit_planes(char far *src, char far *dst, char far *mask, int w, int h, int n, int step);
static void near make_bands(struct Band *A, struct Bitmap far **B, int n, int y2, int h, char far *mask, char c);
static void near make_frames(struct Bitmap far **B, int n, int y2, int h, int color);
static void near build_pump_strip(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap g_bandRecordsSprArray[12];
struct Bitmap far *g_lastPic;
struct Band g_carSpriteStrips[4];
struct Bitmap g_carBackdropHdrBmp;
int g_carVisibleHtValue;

int g_delayScale = 1;   /* DS:58E2 */
int g_dashBuffered = 0;   /* DS:58E4 */
int g_autoTrans = 0;   /* DS:58E6 */
int g_wheelPicIds[3] = {1001, 1002, 1000};   /* DS:58E8 */
int g_driveRebuild = 0;   /* DS:58EE */
struct Cmd g_animScripts[44] = {
    {ANIM_NOP_1, 0, -1, -1},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_BLIT, BOX_KEEP, -1, -1},
    {ANIM_SET_SIZE, 0, 8, 1},
    {ANIM_SET_BOX, BOX_SET_DESTINATION, 48, 111},
    {ANIM_HOOK, 0, 250, -1},
    {ANIM_WAIT, ANIM_WAIT_ONLY, 10, 7},
    {ANIM_HOOK, 0, 0, -1},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 10, 7},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_BLIT_MASKED, BOX_KEEP, -1, -1},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_NOP_3, 0, -1, -1},
    {ANIM_LOAD_IMAGE, 0, 1233, 2},
    {ANIM_SET_SIZE, 0, 48, 64},
    {ANIM_SET_BOX, BOX_SET_DESTINATION, 200, 81},
    {ANIM_TILE_CAR, 0, -1, -1},
    {ANIM_WAIT, ANIM_WAIT_ONLY, 16, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 0, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 16, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 48, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 16, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 96, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 20, 0},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_HOOK, 0, -1, -1},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_STOP, 0, -1, -1},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_JUMP, 0, -1, -1},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_LOAD_IMAGE, 0, -1, -1},
    {ANIM_LOAD_IMAGE, 0, 1075, 2},
    {ANIM_SET_SIZE, 0, 16, 94},
    {ANIM_SET_BOX, BOX_SET_DESTINATION, 160, 52},
    {ANIM_WAIT, ANIM_WAIT_ONLY, 4, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 0, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 4, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 16, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 4, 0},
    {ANIM_BLIT, BOX_SET_SOURCE, 32, 0},
    {ANIM_WAIT, ANIM_WAIT_COPY_TO_FRONT, 4, 36},
    {ANIM_STOP, 0, 0, 0},
    {ANIM_STOP, 0, 0, 0}
};   /* DS:58F0 */
int g_animScriptStart[9] = {8, 0, 2, 12, 10, 25, 27, 29, 31};   /* DS:59F8 */
struct Bitmap far *g_backdrop = (struct Bitmap far *)&g_carBackdropHdrBmp;   /* DS:5A0A */
int g_carMoving = 0;   /* DS:5A0E */
struct Rect g_overlayRectA = {0, 0, 80, 0};   /* DS:5A10 */
struct Rect g_overlayRectB = {88, 0, 136, 0};   /* DS:5A18 */
int g_backdropX = 0;   /* DS:5A20 */
int g_backdropW = 0;   /* DS:5A22 */
int g_carPassBy = 0;   /* DS:5A24 */

/* blit_planes @ 0F38:8126 (linear 0x174A6, 116 bytes)

   Blit one 4-plane EGA image row set, one plane at a time via the sequencer map mask */

static void near blit_planes(char far *src, char far *dst, char far *mask, int w, int h, int n, int step)
{
    int plane, bit1;

    for (bit1 = 1, plane = 0; bit1 < 9; bit1 <<= 1, plane++, src += step) {
        outp(SEQ_INDEX, SEQ_MAP_MASK);
        outp(SEQ_DATA, bit1);
        outp(GC_INDEX, GC_READ_MAP);
        outp(GC_DATA, plane);
        mr_blit(dst, FP_SEG(src), FP_OFF(src), FP_OFF(mask), n, w, h, w);
    }
    outp(SEQ_INDEX, SEQ_MAP_MASK);
    outp(SEQ_DATA, SEQ_ALL_PLANES);
}

/* make_bands @ 0F38:819A (linear 0x1751A, 412 bytes)

   For each of n bitmaps, lay a band of h rows into the scratch bitmap g_pageBmps[2], fill or copy it,
   then blit the bitmap through the EGA planes */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near make_bands(struct Band *A, struct Bitmap far **B, int n, int y2, int h,
    char far *mask, char c)
{
    int i;
    int j;
    char far *base;

    base = g_pageBmps[2]->bits.p + BACKDROP_ROWS * g_pageBmps[2]->pitch;
    for (i = 0; i < n; i++) {
        A[i].p = base;
        A[i].y = B[i]->height - y2 - h;
        A[i].h = h;
        base += B[i]->pitch * h;
        if (mask)
            eb_latch_pack(mask, B[i]->pitch, h, A[i].p, EGA_PITCH);
        else {
            outp(GC_INDEX, GC_MODE);
            outp(GC_DATA, GC_WRITE_MODE2);
            for (j = 0; j < B[i]->pitch * h; j++)
                A[i].p[j] = c;
            outp(GC_INDEX, GC_MODE);
            outp(GC_DATA, GC_WRITE_MODE0);
        }
        j = A[i].y * B[i]->pitch;
        blit_planes(B[i]->bits.p + j, A[i].p, B[i]->alt.p + j, B[i]->pitch, h, B[i]->pitch,
            B[i]->height * B[i]->pitch);
    }
}

/* make_frames @ 0F38:8336 (linear 0x176B6, 392 bytes)

   Copy n bitmaps into the table g_bandRecordsSprArray, each filled with a colour pattern and h rows high, in
   space cut from the scratch bitmap g_pageBmps[2] */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near make_frames(struct Bitmap far **B, int n, int y2, int h, int color)
{
    struct Blt pkt1;
    struct Bitmap far *bm1;
    int v;
    int i;
    char far *base2;
    int H5;
    int maxw;

    base2 = g_pageBmps[2]->bits.p + BACKDROP_ROWS * g_pageBmps[2]->pitch;
    H5 = B[0]->height - y2 - h;
    v = (r_vid.mode == VID_TANDY ? color << 4 : color << 2 | color << 4 | color << 6) | color;
    maxw = B[0]->width;
    for (i = 1; i < n; i++)
        if (B[i]->width > maxw)
            maxw = B[i]->width;
    bm1 = ar_bm(maxw, h, 0, ARENA_HIGH_STACK);
    pkt1.w = maxw;
    pkt1.h = h;
    pkt1.sy = H5;
    i = pkt1.sx = pkt1.dx = pkt1.dy = 0;
    for (i = 0; i < n; i++) {
        g_bandRecordsSprArray[i] = *bm1;
        g_bandRecordsSprArray[i].bits.p = base2;
        base2 += bm1->planesize;
        da_fmemset(bm1->bits.p, v, bm1->planesize);
        (*g_videoOps.maskedBlit)(B[i], bm1, &pkt1, 1);
        bm_copy(bm1, &g_bandRecordsSprArray[i], 1);
    }
    ma_pop_high(1);
}

/* cp_backdrop @ 0F38:84BE (linear 0x1783E, 624 bytes)

   Make a copy of a bitmap header that views the rows below y0, plus a bitmap to hold them (*pa), a
   second one (*pb), and two clip rects */

void cp_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb,
    int y1, int y0, struct Rect **pr1, struct Rect **pr2)
{
    struct Bitmap far *p2;
    struct Blt blt;
    int h, diff, flag, size, y2;
    struct Bitmap far *start;

    size = src->pitch;
    y2 = y0;
    h = y1 - y2;
    flag = mode != 0;
    start = (struct Bitmap far *)ar_alloc(0x30, ARENA_LOW_STACK);
    *start = *src;
    start->bits.w.off += h * size;
    start->height = y2;
    start->planesize = y2 * size * r_vid.planes;
    if (r_vid.mode == VID_TANDY) {
        if (g_backdropW == 0)
            g_backdropW = src->width;
        *pa = ar_bm(g_backdropW, y2, flag, ARENA_LOW_STACK);
        blt.w = g_backdropW;
        blt.h = y2;
        blt.sx = g_backdropX;
        blt.sy = blt.dx = blt.dy = 0;
        (*g_videoOps.copy)(start, *pa, &blt, 1);
    } else {
        *pa = ar_bm(src->width, y2, flag, ARENA_LOW_STACK);
        bm_copy(start, *pa, 1);
    }
    switch (r_vid.mode) {
    case VID_EGA:
        *pb = start;
        break;
    case VID_TANDY:
        *pb = 0;
        break;
    default:
        *pb = ar_bm(start->width, y2, 0, ARENA_LOW_STACK);
        if (g_scene == 6)
            bm_copy(start, *pb, 1);
        break;
    }
    *pr1 = 0;
    *pr2 = 0;
    if (flag) {
        p2 = di_bm(mode, ARENA_HIGH_STACK);
        if (r_vid.mode == VID_TANDY) {
            blt.sy = p2->height - y2;
            (*g_videoOps.copy)(p2, *pa, &blt, 2);
        } else {
            diff = p2->planesize / r_vid.planes - (*pa)->planesize / r_vid.planes;
            bm_far_copy(p2->alt.p + diff, (*pa)->alt.p, (*pa)->planesize / r_vid.planes);
        }
        ma_pop_high(1);
        *pr1 = &g_overlayRectA;
        (*pr1)->y2 = (*pa)->height - 1;
        *pr2 = &g_overlayRectB;
        (*pr2)->y1 = (*pa)->height - 15;
        (*pr2)->y2 = (*pa)->height - 1;
    }
}

/* build_pump_strip @ 0F38:872E (linear 0x17AAE, 312 bytes)

   Render a 16x70 strip into a scratch bitmap with two draw passes and blit it onto two bitmaps */
/* Local names are placeholders chosen to reproduce the original stack layout. */

static void near build_pump_strip(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2)
{
    char far *save1;
    struct Bitmap far *tmp1;
    struct Blt b;
    int t;

    g_picNewHeader = 0;
    tmp1 = ar_bm(0x10, BACKDROP_ROWS, 1, ARENA_HIGH_STACK);
    di_unpack_pic(id1, tmp1);
    save1 = tmp1->bits.p;
    tmp1->bits.p = 0;
    di_unpack_pic(id2, tmp1);
    tmp1->bits.p = save1;
    g_picNewHeader = 1;
    bm_copy(tmp1, dstbm, 1);
    b.h = g_autoSprImagePtr->height;
    b.sy = 0;
    b.dy = top - g_autoSprImagePtr->height - y + 1;
    t = x - (sx0 & 0xfff8);
    b.sx = (t < 0) ? 0 : t;
    b.w = (t + 16 > 16) ? 16 : t + 16;
    b.dx = (-t < 0) ? 0 : -t;
    (*g_videoOps.maskedBlit)(g_autoSprImagePtr, dstbm, &b, 1);
    b.w = 0x10;
    b.h = BACKDROP_ROWS;
    b.dx = b.dy = b.sx = b.sy = 0;
    (*g_videoOps.maskedBlit)(tmp1, dstbm, &b, 0);
    ma_pop_high(1);
}

/* cp_build @ 0F38:8866 (linear 0x17BE6, 1506 bytes)

   Load the bitmap of car part idx into out[0], paint its marks, and build the n frame copies
   out[1..] */

void cp_build(struct Bitmap far **out, int idx, int flags, int kk, int q, int *ids, int w, int n)
{
    int row8;
    struct Blt blt;
    struct Spr12 *p12;
    struct Pos6 *pp8;
    int h;
    int off;
    int left;
    struct Model *m2;
    int right1;
    struct Spr6 *p69;
    int j5;
    struct Bitmap far *bm;
    struct Bitmap far *bm2n;

    m2 = &g_models[idx];
    p12 = &g_layoutAutoSprArray[idx];
    p69 = &g_pointsModelService[idx];
    *out = di_bm(g7d86n[idx].bodyPic, ARENA_LOW_STACK);
    g_featurePos.gasCapX = p69->fuelCapX;
    g_featurePos.gasCapY = p69->fuelCapY;
    g_featurePos.rearGlassX = p69->rearGlassX;
    g_featurePos.rearGlassY = p69->rearGlassY;
    g_featurePos.hasRearGlass = p69->rearGlassX && p69->rearGlassY;
    g_featurePos.frontGlassX = p69->frontGlassX;
    g_featurePos.frontGlassY = p69->frontGlassY;
    m_8272.stickerSubX = ~r_vid.mask & p12->stickerX;
    m_8272.stickerX = r_vid.mask & p12->stickerX;
    m_8272.stickerY = p12->stickerY;
    if (flags & 2)
        bm_put_pic(*out, m2->frontBumperPic, 0, p12->frontBumperY);
    if (flags & 1)
        bm_put_pic(*out, m2->rearBumperPic, p12->rearBumperX, p12->rearBumperY);
    if (flags & 4) {
        bm_put_pic(*out, m2->roofPic, p12->roofX, 0);
        for (j5 = 0; j5 < g_lastPic->planesize / r_vid.planes; j5++)
            if (((unsigned char far *)g_lastPic->alt.p)[j5] != 0xff)
                break;
        row8 = j5 / g_lastPic->pitch;
    } else
        row8 = 0;
    if ((flags & 8) && idx != 0x19)
        bm_draw_pic(*out, m2->f4, p12->b5, p12->b6, 1);
    blt.w = g_stickerBg->width;
    blt.h = g_stickerBg->height;
    blt.sx = p12->stickerX;
    blt.sy = p12->stickerY;
    blt.dx = blt.dy = 0;
    (*g_videoOps.copy)(*out, g_stickerBg, &blt, 1);
    if (kk > 0) {
        pp8 = &a784als[kk - 1];
        bm_draw_pic(*out, pp8->id, p12->stickerX + pp8->dx, p12->stickerY + pp8->dy, 0);
    }
    if (q) {
        bm = di_bm(m_8272.overlayPic, ARENA_HIGH_STACK);
        bm_underlay(bm, *out, p12->b2, p12->b3 - bm->height + row8 + 1);
        ma_pop_high(1);
    }
    j5 = 1;
    for (j5 = 1; j5 < n; j5++)
        out[j5] = ar_bm((*out)->width + (r_vid.mode == VID_TANDY ? 0 : 8), (*out)->height,
            r_vid.mode != VID_TANDY, ARENA_LOW_STACK);
    if (n > 1) {
        switch (r_vid.mode) {
        case VID_EGA:
            for (j5 = 1; j5 < n; j5++)
                bm_shift_right(out[j5 - 1], 2, out[j5]);
            break;
        case VID_TANDY:
            for (j5 = 1; j5 < n; j5++) {
                bm_copy(out[0], out[j5], 1);
                out[j5]->alt.p = out[0]->alt.p;
            }
            break;
        default:
            bm_shift_right(out[0], 2, out[1]);
            bm_copy(out[0], out[2], 3);
            bm_copy(out[1], out[3], 3);
            break;
        }
    }
    if (ids) {
        for (j5 = 0; j5 < n; j5++) {
            bm = di_bm(ids[j5], ARENA_HIGH_STACK);
            h = out[j5]->height - bm->height;
            switch (r_vid.mode) {
            case VID_EGA:
                off = j5 * 2;
                break;
            case VID_TANDY:
                off = 0;
                g_keepMask = n - 1 > j5;
                break;
            default:
                off = (j5 & 1) << 1;
                break;
            }
            bm_underlay(bm, out[j5], p12->wheelX1 + off, h);
            bm_underlay(bm, out[j5], p12->wheelX2 + off, h);
            ma_pop_high(1);
        }
    }
    if (w) {
        bm2n = di_bm(0x49d, ARENA_HIGH_STACK);
        bm_opaque_span(*out, &left, &right1);
        h = (*out)->height - bm2n->height;
        for (j5 = 0; j5 < n; j5++) {
            switch (r_vid.mode) {
            case VID_EGA:
                off = j5 * 2;
                break;
            case VID_TANDY:
                off = 0;
                g_keepMask = n - 1 > j5;
                break;
            default:
                off = (j5 & 1) << 1;
                break;
            }
            bm_underlay(bm2n, out[j5], left + off, h);
            bm_underlay(bm2n, out[j5], right1 - bm2n->width + off + 1, h);
        }
        ma_pop_high(1);
    }
}

/* cp_show_car @ 0F38:8E48 (linear 0x181C8, 2323 bytes) */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void cp_show_car(enum CarAnimKind kind, int idx, int flags, int kk, int q, int bot, int mode2, int *ids)
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

    top7 = bot - (BACKDROP_ROWS - 1);
    cu_ctl(CURSOR_HIDE_NOW);
again:
    n2 = (kind == CAR_SHOW_BACK_PAGE || kind == CAR_SHOW_SCREEN || kind == CAR_SHOW_BACKDROP) ? 1 : 4;
    di_pic_size(g7d86n[idx].bodyPic, &w1, &h7, &x7);
    g_spriteBottomCut = g_carBottomCuts[g_scene];
    tmpn = (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? g_carBg4[g_scene] : g_carBg16[g_scene];
    hh8 = (kind == CAR_SHOW_BACK_PAGE || kind == CAR_SHOW_SCREEN || kind == CAR_SHOW_BACKDROP) ? h7 - g_spriteBottomCut : g_carBandHeights[g_scene];
    x0 = ((SCREEN_WIDTH - w1) / 2 + 4) & 0xfff8;
    right = kind == CAR_DRIVE_OUT ? SCREEN_WIDTH - x0 : x0 + w1;
    if (g_carPassBy != 2) {
        if (kind == CAR_PULL_IN) {
            g_4f4a = 1;
            if (r_vid.mode == VID_EGA) {
                me_set_bitmap(g_backdrop, g_pageBmps[2]->bits.p, 0, SCREEN_WIDTH, BACKDROP_ROWS - g_spriteBottomCut, VID_EGA);
                ea_rect(r_vid.back->bits.p, g_backdrop->bits.p - EGA_PITCH * top7, EGA_PITCH, g_backdrop->height, 0, top7, EGA_PITCH);
            } else {
                me_set_bitmap(g_backdrop, g_pageBmps[2]->bits.p, 0, SCREEN_WIDTH, BACKDROP_ROWS - g_spriteBottomCut, -1);
                pu_grab(0, top7, SCREEN_WIDTH, g_backdrop->height, g_backdrop);
            }
        }
        if (kind == CAR_PULL_IN) {
            ma_clear();
            if (g_scene == 3) {
                di_pic_size(0x449, &w2, &h2, &x2);
                g_pumpWithNozzle = ar_bm(w2, h2, 0, ARENA_LOW_BASE);
                g_pumpNoNozzle = ar_bm(w2, h2, 0, ARENA_LOW_BASE);
            }
            if (g_scene == 2 || g_scene == 6)
                ch_save_bg();
        } else
            ma_release();
        if (!g_stickerBg)
            g_stickerBg = ar_bm(0x40, 0x19, 0, ARENA_LOW_BASE);
        cp_build(bms6, idx, flags, kk, q, ids, 1, n2);
        if (r_vid.mode == VID_TANDY && kind != CAR_SHOW_BACK_PAGE && kind != CAR_SHOW_SCREEN && kind != CAR_SHOW_BACKDROP) {
            make_frames(bms6, n2, g_spriteBottomCut, hh8, tmpn);
            bms6[1] = bms6[2] = bms6[3] = bms6[0];
            ma_pop_low(3);
        }
        if (kind != CAR_SHOW_BACK_PAGE && kind != CAR_SHOW_SCREEN && kind != CAR_SHOW_BACKDROP) {
            if (g_carPassBy != 1) {
                g_backdropW = right;
                g_backdropX = kind == CAR_DRIVE_OUT ? x0 : 0;
            } else {
                g_backdropW = SCREEN_WIDTH;
                g_backdropX = 0;
            }
            cp_backdrop(g_backdrop, &pan, mode2 ? 0x448 : 0, &pb8, BACKDROP_ROWS, h7, &pr16, &pr26);
        }
    }
    ac825et = x0;
    aw8260y = bot - bms6[0]->height + 1;
    g_carVisibleHtValue = bms6[0]->height - g_spriteBottomCut;
    if (g_featurePos.frontGlassX > 0) {
        g_featurePos.frontGlassX += x0;
        g_featurePos.frontGlassY += aw8260y;
        g_featurePos.rearGlassX += x0;
        g_featurePos.rearGlassY += aw8260y;
    }
    g_featurePos.gasCapX += x0;
    g_featurePos.gasCapY += aw8260y;
    m_8272.stickerX += x0;
    m_8272.stickerY += aw8260y;
    ang2 = (kind == CAR_DRIVE_IN || kind == CAR_PULL_IN) ? x0 - right : x0;
    if (g_carPassBy != 2) {
        if (r_vid.mode == VID_EGA) {
            if (kind == CAR_SHOW_BACK_PAGE || kind == CAR_SHOW_BACKDROP)
                mask3 = g_backdrop->bits.p + ((g_backdrop->height - hh8) * g_backdrop->pitch + x0 / 8);
            else
                mask3 = 0;
            make_bands(g_carSpriteStrips, bms6, n2, g_spriteBottomCut, hh8, mask3, tmpn);
        }
        if (!((r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) && r_vid.mode != VID_TANDY)) {
            if (kind == CAR_SHOW_BACK_PAGE || kind == CAR_SHOW_SCREEN || kind == CAR_SHOW_BACKDROP) {
                bm48 = ar_bm(w1, h7 - g_spriteBottomCut, 0, ARENA_LOW_STACK);
                if (kind != CAR_SHOW_SCREEN) {
                    blt.w = w1;
                    blt.h = h7 - g_spriteBottomCut;
                    blt.sx = x0;
                    blt.sy = g_backdrop->height - h7;
                    blt.dx = blt.dy = 0;
                    (*g_videoOps.copy)(g_backdrop, bm48, &blt, 1);
                } else
                    pu_grab(x0, aw8260y, bm48->width, bm48->height, bm48);
                blt.w = w1;
                blt.h = blt.fc = h7 - g_spriteBottomCut;
                blt.dx = blt.dy = blt.sx = blt.sy = 0;
                (*g_videoOps.maskedBlit)(bms6[0], bm48, &blt, 1);
            } else if (r_vid.mode != VID_TANDY)
                make_frames(bms6, n2, g_spriteBottomCut, hh8, tmpn);
        }
        if (!((r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) && r_vid.mode != VID_TANDY)) {
            blt.w = w1;
            blt.h = h7 - g_spriteBottomCut;
            blt.sx = blt.sy = 0;
            blt.dx = x0;
            blt.dy = bot - h7 + 1;
        }
        if (kind == CAR_SHOW_BACK_PAGE) {
            if (r_vid.mode == VID_EGA)
                ea_to_page(g_carSpriteStrips[0].p, bms6[0]->pitch, bms6[0]->height - g_spriteBottomCut,
                    r_vid.back->bits.p + ((bot - h7) * EGA_PITCH + (x0 >> 3)) + EGA_PITCH, bms6[0]->pitch);
            else
                (*g_videoOps.copy)(bm48, r_vid.back, &blt, 1);
            (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
            goto done;
        } else if (kind == CAR_SHOW_SCREEN || kind == CAR_SHOW_BACKDROP) {
            if (r_vid.mode == VID_EGA)
                ea_to_page(g_carSpriteStrips[0].p, bms6[0]->pitch, h7 - g_spriteBottomCut,
                    r_vid.front->bits.p + ((bot - h7) * EGA_PITCH + (x0 >> 3)) + EGA_PITCH, bms6[0]->pitch);
            else
                (*g_videoOps.copy)(bm48, r_vid.front, &blt, 1);
            bm_copy_box(r_vid.front, r_vid.back, x0, bot - h7 + 1, x0 + w1 - 1, bot);
            goto done;
        }
    }
    spr3 = sp_setup(ang2, bot, bms6, (char (*)[8])g_carSpriteStrips, n2, pb8, pan, pr16, pr26);
    sp_plan_drive(spr3, kind, right);
    if (kind == CAR_BACK_OUT)
        spr3->page->rect = g_lastCarRect;
    if (kind == CAR_PULL_IN || kind == CAR_DRIVE_IN || kind == CAR_DRIVE_OUT || kind == CAR_BACK_OUT) {
        g_carAnim = 1;
        g_carMoving = 1;
        if (g_carPassBy != 2)
            sy_drive_start();
        while ((*g_videoOps.spriteStep)(spr3))
            ;
        if (g_carPassBy != 1)
            sy_drive_stop();
        g_autoSprImagePtr = g_spritePic;
        g_carMoving = 0;
        g_carAnim = 0;
        g_lastCarRect = spr3->page->rect;
    }
    if (g_scene == 3 && kind == CAR_PULL_IN) {
        scratch = ar_bm(0x18, 7, 0, ARENA_HIGH_STACK);
        blt.w = 0x10;
        blt.h = scratch->height;
        blt.sx = g_featurePos.gasCapX - ac825et;
        blt.sy = g_featurePos.gasCapY - aw8260y;
        blt.dx = blt.dy = 0;
        (*g_videoOps.copy)(bms6[0], scratch, &blt, 1);
        bm_draw_pic(bms6[0], 0x472, (char)blt.sx, (char)blt.sy, 0);
        build_pump_strip(0x40, 0x64, g_xCoordSprite, bot, g_pumpWithNozzle, 0x4c8, 0x4ca);
        build_pump_strip(0x40, 0x64, g_xCoordSprite, bot, g_pumpNoNozzle, 0x449, 0x44a);
        blt.dx = blt.sx;
        blt.dy = blt.sy;
        blt.sx = blt.sy = 0;
        (*g_videoOps.copy)(scratch, bms6[0], &blt, 1);
    }
    da_set_view(2);
    if (r_vid.mode != VID_EGA)
        (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    if (g_carPassBy == 1) {
        kind = CAR_DRIVE_OUT;
        g_carPassBy = 2;
        goto again;
    }
done:
    g_spriteBottomCut = g_backdropW = g_backdropX = 0;
    ma_release();
    g_carPassBy = 0;
    cu_ctl(CURSOR_SHOW_NOW);
}

/* cp_patch_bg @ 0F38:975B (linear 0x18ADB, 256 bytes)

   Blit a clipped band of a bitmap through the blit hook, then the seven frame cells onto the front
   buffer */

void cp_patch_bg(struct Bitmap far *dst, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
    int base7, hi1, lo7, i9;
    struct Blt r;
    int top;

    base7 = p7 - (BACKDROP_ROWS - 1);
    top = g_backdrop->height + base7 - 1;
    lo7 = p6 < base7 ? base7 : p6;
    hi1 = p6 + p4 - 1 > top ? top : p6 + p4 - 1;
    if (lo7 <= hi1) {
        r.w = p3;
        r.h = hi1 - lo7 + 1;
        r.sx = p1;
        r.sy = p2 + lo7 - p6;
        r.dx = p5;
        r.dy = lo7 - base7;
        (*g_videoOps.copy)(dst, g_backdrop, &r, 1);
    }
    if (g_scene == 6) {
        r.h = g_backdrop->height;
        r.sy = p7 - (BACKDROP_ROWS - 1);
        r.dy = 0;
        for (i9 = 0; i9 < 7; i9++) {
            r.w = g_jailBars[i9].w;
            r.sx = r.dx = g_jailBars[i9].x;
            (*g_videoOps.copy)(r_vid.front, g_backdrop, &r, 1);
        }
    }
}
