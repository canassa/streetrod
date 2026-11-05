#include <conio.h>
#include <dos.h>
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
#include "STRUCTS.H"
#include "SYS.H"
#include "VIDEO.H"

static struct Rect g_lastCarRect;
static void near blit_planes(char far *src, char far *dst, char far *mask, int w, int h, int n, int step);
static void near make_bands(struct Band *A, struct Bitmap far **B, int n, int y2, int h, char far *mask, char c);
static void near make_frames(struct Bitmap far **B, int n, int y2, int h, int color);
static void near build_pump_strip(int x, int y, int sx0, int top, struct Bitmap far *dstbm, int id1, int id2);

struct Bitmap g_bandRecordsSprArray[12];
struct Bitmap far *g_lastPic;
struct Band g_carSpriteStrips[4];
struct Bitmap g_carBackdropHdrBmp;
int g_carVisibleHtValue;

int g_delayScale = 1;
int g_dashBuffered = 0;
int g_autoTrans = 0;
int g_wheelPicIds[3] = {PIC_STEERING_WHEEL_CENTER, PIC_STEERING_WHEEL_LEFT, PIC_STEERING_WHEEL_RIGHT};
int g_driveRebuild = 0;
struct AnimCommand g_animScripts[44] = {{ANIM_NOP_1, 0, -1, -1},
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
                                        {ANIM_LOAD_IMAGE, 0, PIC_GAS_ATTENDANT, 2},
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
                                        {ANIM_LOAD_IMAGE, 0, PIC_FUEL_PUMP_DIGITS, 2},
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
                                        {ANIM_STOP, 0, 0, 0}};
int g_animScriptStart[9] = {8, 0, 2, 12, 10, 25, 27, 29, 31};
static struct Bitmap far *g_backdrop = (struct Bitmap far *)&g_carBackdropHdrBmp;
int g_carMoving = 0;
static struct Rect g_overlayRectA = {0, 0, 80, 0};
static struct Rect g_overlayRectB = {88, 0, 136, 0};
int g_backdropX = 0;
static int g_backdropW = 0;
enum PassByPhase g_carPassPhase = PASS_BY_NONE;

/* Blit one 4-plane EGA image row set, one plane at a time via the sequencer map mask */

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

/* For each of n bitmaps, lay a band of h rows into the scratch bitmap g_pageBmps[2], fill or copy it,
   then blit the bitmap through the EGA planes */

static void near make_bands(struct Band *A, struct Bitmap far **B, int n, int y2, int h, char far *mask, char c)
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
        blit_planes(B[i]->bits.p + j, A[i].p, B[i]->mask.p + j, B[i]->pitch, h, B[i]->pitch,
                    B[i]->height * B[i]->pitch);
    }
}

/* Copy n bitmaps into the table g_bandRecordsSprArray, each filled with a colour pattern and h rows high, in
   space cut from the scratch bitmap g_pageBmps[2] */

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
        base2 += bm1->pixelBytes;
        da_fmemset(bm1->bits.p, v, bm1->pixelBytes);
        (*g_videoOps.maskedBlit)(B[i], bm1, &pkt1, 1);
        bm_copy(bm1, &g_bandRecordsSprArray[i], 1);
    }
    ma_pop_high(1);
}

/* Make a copy of a bitmap header that views the rows below y0, plus a bitmap to hold them (*pa), a
   second one (*pb), and two clip rects */

void cp_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb, int y1, int y0,
                 struct Rect **pr1, struct Rect **pr2)
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
    start->pixelBytes = y2 * size * r_vid.planes;
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
        if (g_scene == SCENE_JAIL)
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
            diff = p2->pixelBytes / r_vid.planes - (*pa)->pixelBytes / r_vid.planes;
            bm_far_copy(p2->mask.p + diff, (*pa)->mask.p, (*pa)->pixelBytes / r_vid.planes);
        }
        ma_pop_high(1);
        *pr1 = &g_overlayRectA;
        (*pr1)->y2 = (*pa)->height - 1;
        *pr2 = &g_overlayRectB;
        (*pr2)->y1 = (*pa)->height - 15;
        (*pr2)->y2 = (*pa)->height - 1;
    }
}

/* Render a 16x70 strip into a scratch bitmap with two draw passes and blit it onto two bitmaps */

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

void cp_build(struct Bitmap far **frames, int specIndex, enum BodyMod bodyMods, int sticker, BOOL showPortrait,
              int *tirePics, BOOL showShadow, int frameCount)
{
    struct Model *bodyModel;
    int right;
    struct Spr6 *servicePoints;
    int imageFrame;
    struct Bitmap far *pic;
    struct Bitmap far *carShadow;
    int framePixelShift;
    int left;
    struct Pos6 *decal;
    int underY;
    struct Spr12 *carLayout;
    struct Blt blt;
    int roofCut;

    bodyModel = &g_models[specIndex];
    carLayout = &g_layoutAutoSprArray[specIndex];
    servicePoints = &g_pointsModelService[specIndex];
    *frames = di_bm(g_carData[specIndex].bodyPic, ARENA_LOW_STACK);
    g_featurePos.gasCapX = servicePoints->fuelCapX;
    g_featurePos.gasCapY = servicePoints->fuelCapY;
    g_featurePos.rearGlassX = servicePoints->rearGlassX;
    g_featurePos.rearGlassY = servicePoints->rearGlassY;
    g_featurePos.hasRearGlass = servicePoints->rearGlassX && servicePoints->rearGlassY;
    g_featurePos.frontGlassX = servicePoints->frontGlassX;
    g_featurePos.frontGlassY = servicePoints->frontGlassY;
    g_carDraw.stickerSubX = ~r_vid.mask & carLayout->stickerX;
    g_carDraw.stickerX = r_vid.mask & carLayout->stickerX;
    g_carDraw.stickerY = carLayout->stickerY;
    if (bodyMods & BODY_REAR_BUMPER)
        bm_put_pic(*frames, bodyModel->rearBumperRemovalPic, 0, carLayout->rearBumperY);
    if (bodyMods & BODY_FRONT_BUMPER)
        bm_put_pic(*frames, bodyModel->frontBumperRemovalPic, carLayout->frontBumperX, carLayout->frontBumperY);
    if (bodyMods & BODY_ROOF) {
        bm_put_pic(*frames, bodyModel->choppedRoofPic, carLayout->roofX, 0);
        for (imageFrame = 0; imageFrame < g_lastPic->pixelBytes / r_vid.planes; imageFrame++)
            if (((unsigned char far *)g_lastPic->mask.p)[imageFrame] != 0xff)
                break;
        roofCut = imageFrame / g_lastPic->pitch;
    } else
        roofCut = 0;
    if ((bodyMods & BODY_LARGE_ENGINE) && specIndex != SPEC_KING)
        bm_draw_pic(*frames, bodyModel->engineOverlayPic, carLayout->b5, carLayout->b6, 1);
    blt.w = g_stickerBg->width;
    blt.h = g_stickerBg->height;
    blt.sx = carLayout->stickerX;
    blt.sy = carLayout->stickerY;
    blt.dx = blt.dy = 0;
    (*g_videoOps.copy)(*frames, g_stickerBg, &blt, 1);
    if (sticker > 0) {
        decal = &g_imageDecalLayout[sticker - 1];
        bm_draw_pic(*frames, decal->id, carLayout->stickerX + decal->dx, carLayout->stickerY + decal->dy, 0);
    }
    if (showPortrait) {
        pic = di_bm(g_carDraw.portraitPic, ARENA_HIGH_STACK);
        bm_underlay(pic, *frames, carLayout->b2, carLayout->b3 - pic->height + roofCut + 1);
        ma_pop_high(1);
    }
    imageFrame = 1;
    for (imageFrame = 1; imageFrame < frameCount; imageFrame++)
        frames[imageFrame] = ar_bm((*frames)->width + (r_vid.mode == VID_TANDY ? 0 : 8), (*frames)->height,
                                   r_vid.mode != VID_TANDY, ARENA_LOW_STACK);
    if (frameCount > 1) {
        switch (r_vid.mode) {
        case VID_EGA:
            for (imageFrame = 1; imageFrame < frameCount; imageFrame++)
                bm_shift_right(frames[imageFrame - 1], 2, frames[imageFrame]);
            break;
        case VID_TANDY:
            for (imageFrame = 1; imageFrame < frameCount; imageFrame++) {
                bm_copy(frames[0], frames[imageFrame], 1);
                frames[imageFrame]->mask.p = frames[0]->mask.p;
            }
            break;
        default:
            bm_shift_right(frames[0], 2, frames[1]);
            bm_copy(frames[0], frames[2], 3);
            bm_copy(frames[1], frames[3], 3);
            break;
        }
    }
    if (tirePics) {
        for (imageFrame = 0; imageFrame < frameCount; imageFrame++) {
            pic = di_bm(tirePics[imageFrame], ARENA_HIGH_STACK);
            underY = frames[imageFrame]->height - pic->height;
            switch (r_vid.mode) {
            case VID_EGA:
                framePixelShift = imageFrame * 2;
                break;
            case VID_TANDY:
                framePixelShift = 0;
                g_keepMask = frameCount - 1 > imageFrame;
                break;
            default:
                framePixelShift = (imageFrame & 1) << 1;
                break;
            }
            bm_underlay(pic, frames[imageFrame], carLayout->wheelX1 + framePixelShift, underY);
            bm_underlay(pic, frames[imageFrame], carLayout->wheelX2 + framePixelShift, underY);
            ma_pop_high(1);
        }
    }
    if (showShadow) {
        carShadow = di_bm(PIC_CAR_SHADOW, ARENA_HIGH_STACK);
        bm_opaque_span(*frames, &left, &right);
        underY = (*frames)->height - carShadow->height;
        for (imageFrame = 0; imageFrame < frameCount; imageFrame++) {
            switch (r_vid.mode) {
            case VID_EGA:
                framePixelShift = imageFrame * 2;
                break;
            case VID_TANDY:
                framePixelShift = 0;
                g_keepMask = frameCount - 1 > imageFrame;
                break;
            default:
                framePixelShift = (imageFrame & 1) << 1;
                break;
            }
            bm_underlay(carShadow, frames[imageFrame], left + framePixelShift, underY);
            bm_underlay(carShadow, frames[imageFrame], right - carShadow->width + framePixelShift + 1, underY);
        }
        ma_pop_high(1);
    }
}


void cp_show_car(enum CarAnimKind animation, int specIndex, enum BodyMod bodyMods, int sticker, BOOL showPortrait,
                 int carBottom, BOOL useGasMask, int *tirePics)
{
    int backgroundColor;
    int carHeight;
    char far *bandBackgroundData;
    int carPicFlags;
    struct Bitmap far *savedBg;
    int driveSpan;
    struct Bitmap far *canvas;
    int pumpBitmapFlags;
    struct Bitmap far *workBg;
    int backdropTop;
    struct Rect *rightForeground;
    int pumpPicHeight;
    struct Rect *leftForeground;
    int pumpImageWidth;
    struct Bitmap far *carFrames[4];
    int spriteXPixels;
    int imageWidth;
    struct Bitmap far *capSave;
    int bandHeightPixels;
    struct Sprite *carSpritePtr;
    int centeredX;
    struct Blt blt;
    int frameCnt;

    backdropTop = carBottom - (BACKDROP_ROWS - 1);
    cu_ctl(CURSOR_HIDE_NOW);
again:
    frameCnt =
        (animation == CAR_SHOW_BACK_PAGE || animation == CAR_SHOW_SCREEN || animation == CAR_SHOW_BACKDROP) ? 1 : 4;
    di_pic_size(g_carData[specIndex].bodyPic, &imageWidth, &carHeight, &carPicFlags);
    g_spriteBottomCut = g_carBottomCuts[g_scene];
    backgroundColor = (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? g_carBg4[g_scene] : g_carBg16[g_scene];
    bandHeightPixels =
        (animation == CAR_SHOW_BACK_PAGE || animation == CAR_SHOW_SCREEN || animation == CAR_SHOW_BACKDROP)
            ? carHeight - g_spriteBottomCut
            : g_carBandHeights[g_scene];
    centeredX = ((SCREEN_WIDTH - imageWidth) / 2 + 4) & 0xfff8;
    driveSpan = animation == CAR_DRIVE_OUT ? SCREEN_WIDTH - centeredX : centeredX + imageWidth;
    if (g_carPassPhase != PASS_BY_DEPARTING) {
        if (animation == CAR_PULL_IN) {
            g_carPulledIn = 1;
            if (r_vid.mode == VID_EGA) {
                me_set_bitmap(g_backdrop, g_pageBmps[2]->bits.p, 0, SCREEN_WIDTH, BACKDROP_ROWS - g_spriteBottomCut,
                              VID_EGA);
                ea_rect(r_vid.back->bits.p, g_backdrop->bits.p - EGA_PITCH * backdropTop, EGA_PITCH, g_backdrop->height,
                        0, backdropTop, EGA_PITCH);
            } else {
                me_set_bitmap(g_backdrop, g_pageBmps[2]->bits.p, 0, SCREEN_WIDTH, BACKDROP_ROWS - g_spriteBottomCut,
                              BITMAP_WORK);
                pu_grab(0, backdropTop, SCREEN_WIDTH, g_backdrop->height, g_backdrop);
            }
        }
        if (animation == CAR_PULL_IN) {
            ma_clear();
            if (g_scene == SCENE_GAS_STATION) {
                di_pic_size(PIC_PUMP_WITHOUT_NOZZLE, &pumpImageWidth, &pumpPicHeight, &pumpBitmapFlags);
                g_pumpWithNozzle = ar_bm(pumpImageWidth, pumpPicHeight, 0, ARENA_LOW_BASE);
                g_pumpNoNozzle = ar_bm(pumpImageWidth, pumpPicHeight, 0, ARENA_LOW_BASE);
            }
            if (g_scene == SCENE_BOBS || g_scene == SCENE_JAIL)
                ch_save_bg();
        } else
            ma_release();
        if (!g_stickerBg)
            g_stickerBg = ar_bm(0x40, 0x19, 0, ARENA_LOW_BASE);
        cp_build(carFrames, specIndex, bodyMods, sticker, showPortrait, tirePics, TRUE, frameCnt);
        if (r_vid.mode == VID_TANDY && animation != CAR_SHOW_BACK_PAGE && animation != CAR_SHOW_SCREEN &&
            animation != CAR_SHOW_BACKDROP) {
            make_frames(carFrames, frameCnt, g_spriteBottomCut, bandHeightPixels, backgroundColor);
            carFrames[1] = carFrames[2] = carFrames[3] = carFrames[0];
            ma_pop_low(3);
        }
        if (animation != CAR_SHOW_BACK_PAGE && animation != CAR_SHOW_SCREEN && animation != CAR_SHOW_BACKDROP) {
            if (g_carPassPhase != PASS_BY_ARRIVING) {
                g_backdropW = driveSpan;
                g_backdropX = animation == CAR_DRIVE_OUT ? centeredX : 0;
            } else {
                g_backdropW = SCREEN_WIDTH;
                g_backdropX = 0;
            }
            cp_backdrop(g_backdrop, &savedBg, useGasMask ? PIC_GAS_STATION_MASK : 0, &workBg, BACKDROP_ROWS, carHeight,
                        &leftForeground, &rightForeground);
        }
    }
    g_picXDraw = centeredX;
    g_sprDrawY = carBottom - carFrames[0]->height + 1;
    g_carVisibleHtValue = carFrames[0]->height - g_spriteBottomCut;
    if (g_featurePos.frontGlassX > 0) {
        g_featurePos.frontGlassX += centeredX;
        g_featurePos.frontGlassY += g_sprDrawY;
        g_featurePos.rearGlassX += centeredX;
        g_featurePos.rearGlassY += g_sprDrawY;
    }
    g_featurePos.gasCapX += centeredX;
    g_featurePos.gasCapY += g_sprDrawY;
    g_carDraw.stickerX += centeredX;
    g_carDraw.stickerY += g_sprDrawY;
    spriteXPixels = (animation == CAR_DRIVE_IN || animation == CAR_PULL_IN) ? centeredX - driveSpan : centeredX;
    if (g_carPassPhase != PASS_BY_DEPARTING) {
        if (r_vid.mode == VID_EGA) {
            if (animation == CAR_SHOW_BACK_PAGE || animation == CAR_SHOW_BACKDROP)
                bandBackgroundData =
                    g_backdrop->bits.p + ((g_backdrop->height - bandHeightPixels) * g_backdrop->pitch + centeredX / 8);
            else
                bandBackgroundData = 0;
            make_bands(g_carSpriteStrips, carFrames, frameCnt, g_spriteBottomCut, bandHeightPixels, bandBackgroundData,
                       backgroundColor);
        }
        if (!((r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) && r_vid.mode != VID_TANDY)) {
            if (animation == CAR_SHOW_BACK_PAGE || animation == CAR_SHOW_SCREEN || animation == CAR_SHOW_BACKDROP) {
                canvas = ar_bm(imageWidth, carHeight - g_spriteBottomCut, 0, ARENA_LOW_STACK);
                if (animation != CAR_SHOW_SCREEN) {
                    blt.w = imageWidth;
                    blt.h = carHeight - g_spriteBottomCut;
                    blt.sx = centeredX;
                    blt.sy = g_backdrop->height - carHeight;
                    blt.dx = blt.dy = 0;
                    (*g_videoOps.copy)(g_backdrop, canvas, &blt, 1);
                } else
                    pu_grab(centeredX, g_sprDrawY, canvas->width, canvas->height, canvas);
                blt.w = imageWidth;
                blt.h = blt.unused1 = carHeight - g_spriteBottomCut;
                blt.dx = blt.dy = blt.sx = blt.sy = 0;
                (*g_videoOps.maskedBlit)(carFrames[0], canvas, &blt, 1);
            } else if (r_vid.mode != VID_TANDY)
                make_frames(carFrames, frameCnt, g_spriteBottomCut, bandHeightPixels, backgroundColor);
        }
        if (!((r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) && r_vid.mode != VID_TANDY)) {
            blt.w = imageWidth;
            blt.h = carHeight - g_spriteBottomCut;
            blt.sx = blt.sy = 0;
            blt.dx = centeredX;
            blt.dy = carBottom - carHeight + 1;
        }
        if (animation == CAR_SHOW_BACK_PAGE) {
            if (r_vid.mode == VID_EGA)
                ea_to_page(g_carSpriteStrips[0].p, carFrames[0]->pitch, carFrames[0]->height - g_spriteBottomCut,
                           r_vid.back->bits.p + ((carBottom - carHeight) * EGA_PITCH + (centeredX >> 3)) + EGA_PITCH,
                           carFrames[0]->pitch);
            else
                (*g_videoOps.copy)(canvas, r_vid.back, &blt, 1);
            (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
            goto done;
        } else if (animation == CAR_SHOW_SCREEN || animation == CAR_SHOW_BACKDROP) {
            if (r_vid.mode == VID_EGA)
                ea_to_page(g_carSpriteStrips[0].p, carFrames[0]->pitch, carHeight - g_spriteBottomCut,
                           r_vid.front->bits.p + ((carBottom - carHeight) * EGA_PITCH + (centeredX >> 3)) + EGA_PITCH,
                           carFrames[0]->pitch);
            else
                (*g_videoOps.copy)(canvas, r_vid.front, &blt, 1);
            bm_copy_box(r_vid.front, r_vid.back, centeredX, carBottom - carHeight + 1, centeredX + imageWidth - 1,
                        carBottom);
            goto done;
        }
    }
    carSpritePtr = sp_setup(spriteXPixels, carBottom, carFrames, (char (*)[8])g_carSpriteStrips, frameCnt, workBg,
                            savedBg, leftForeground, rightForeground);
    sp_plan_drive(carSpritePtr, animation, driveSpan);
    if (animation == CAR_BACK_OUT)
        carSpritePtr->page->rect = g_lastCarRect;
    if (animation == CAR_PULL_IN || animation == CAR_DRIVE_IN || animation == CAR_DRIVE_OUT ||
        animation == CAR_BACK_OUT) {
        g_carAnim = 1;
        g_carMoving = 1;
        if (g_carPassPhase != PASS_BY_DEPARTING)
            sy_drive_start();
        while ((*g_videoOps.spriteStep)(carSpritePtr))
            ;
        if (g_carPassPhase != PASS_BY_ARRIVING)
            sy_drive_stop();
        g_autoSprImagePtr = g_spritePic;
        g_carMoving = 0;
        g_carAnim = 0;
        g_lastCarRect = carSpritePtr->page->rect;
    }
    if (g_scene == SCENE_GAS_STATION && animation == CAR_PULL_IN) {
        capSave = ar_bm(0x18, 7, 0, ARENA_HIGH_STACK);
        blt.w = 0x10;
        blt.h = capSave->height;
        blt.sx = g_featurePos.gasCapX - g_picXDraw;
        blt.sy = g_featurePos.gasCapY - g_sprDrawY;
        blt.dx = blt.dy = 0;
        (*g_videoOps.copy)(carFrames[0], capSave, &blt, 1);
        bm_draw_pic(carFrames[0], PIC_FUEL_CAP, (char)blt.sx, (char)blt.sy, 0);
        build_pump_strip(0x40, 0x64, g_xCoordSprite, carBottom, g_pumpWithNozzle, PIC_PUMP_WITH_NOZZLE,
                         PIC_PUMP_MASK_WITH_NOZZLE);
        build_pump_strip(0x40, 0x64, g_xCoordSprite, carBottom, g_pumpNoNozzle, PIC_PUMP_WITHOUT_NOZZLE,
                         PIC_PUMP_MASK_WITHOUT_NOZZLE);
        blt.dx = blt.sx;
        blt.dy = blt.sy;
        blt.sx = blt.sy = 0;
        (*g_videoOps.copy)(capSave, carFrames[0], &blt, 1);
    }
    da_set_view(DASH_VIEW_SHOW);
    if (r_vid.mode != VID_EGA)
        (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    if (g_carPassPhase == PASS_BY_ARRIVING) {
        animation = CAR_DRIVE_OUT;
        g_carPassPhase = PASS_BY_DEPARTING;
        goto again;
    }
done:
    g_spriteBottomCut = g_backdropW = g_backdropX = 0;
    ma_release();
    g_carPassPhase = PASS_BY_NONE;
    cu_ctl(CURSOR_SHOW_NOW);
}

/* Blit a clipped band of a bitmap through the blit hook, then the seven frame cells onto the front
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
    if (g_scene == SCENE_JAIL) {
        r.h = g_backdrop->height;
        r.sy = p7 - (BACKDROP_ROWS - 1);
        r.dy = 0;
        for (i9 = 0; i9 < 7; i9++) {
            r.w = g_jailBars[i9].width;
            r.sx = r.dx = g_jailBars[i9].screenX;
            (*g_videoOps.copy)(r_vid.front, g_backdrop, &r, 1);
        }
    }
}
