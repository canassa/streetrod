#include "STRUCTS.H"
#include "TIRES.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "GAME.H"
#include "GARAGE.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "MUSIC.H"
#include "PC.H"
#include "POPUP.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SPRITE.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct TireWork g_tires;

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */

static struct Bitmap far * near grab_wheel_column(int n);
static void near swap_wheel(int a, int *p, int *q);

int g_spriteBgY = 0;
int g_liftPicIds[3] = {1034, 1035, 1036};
struct Bitmap far *g_wheelBuf = (struct Bitmap far *)&g_tires.wheelHeader;
struct Bitmap far *g_rollBuf = (struct Bitmap far *)&g_tires.rollHeader;
struct Bitmap far *g_jackBuf = (struct Bitmap far *)&g_tires.jackHeader;

/* Build the two scratch bitmaps for a shifted background strip and its 0x28-wide column */

static struct Bitmap far *near grab_wheel_column(int n)
{
    struct Bitmap far *b;
    struct Blt blt;
    struct Bitmap far *a2;
    struct Bitmap far *c;

    a2 = ar_bm(g_tires.column->width + 8, g_tires.column->height, g_tires.column->alt.p != 0L, ARENA_LOW_STACK);
    bm_shift_right(g_tires.column, n & ~r_vid.mask, a2);
    b = ar_bm(a2->width, a2->height, 0, ARENA_LOW_STACK);
    pu_grab((n + g_tires.carX) & r_vid.mask, 0x78, a2->width, a2->height, b);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_videoOps.maskedBlit)(a2, b, &blt, 1);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = 0;
    blt.dx = (n + g_tires.carX) & r_vid.mask;
    blt.dy = 0x78;
    (*g_videoOps.copy)(b, r_vid.back, &blt, 1);
    ma_pop_low(2);
    blt.w = 0x28;
    blt.h = g_tires.liftCar->height;
    blt.sx = n & r_vid.mask;
    blt.sy = blt.dx = 0;
    blt.dy = -(g_tires.liftCar->height + g_spriteBgY - 0x95);
    c = ar_bm(0x28, 0x9c - g_spriteBgY, 1, ARENA_LOW_STACK);
    pu_grab(blt.sx + g_tires.carX, g_spriteBgY, 0x28, 0x9c - g_spriteBgY, c);
    da_fmemset(c->alt.p, 0xff, c->planesize / r_vid.planes);
    (*g_videoOps.maskedBlit)(g_tires.liftCar, c, &blt, 1);
    (*g_videoOps.copy)(g_tires.liftCar, c, &blt, 2);
    return c;
}

/* Build the four shifted copies of a background strip and animate the sprite over them */

static void near swap_wheel(int a, int *p, int *q)
{
    int i;

    g_tires.underlay = di_bm(0x4d2, ARENA_LOW_STACK);
    g_tires.wheelBitmap = di_bm(*p, ARENA_LOW_STACK);
    g_tires.phase = a & ~r_vid.mask;
    g_tires.frames[0] = ar_bm(g_tires.wheelBitmap->width + 8, g_tires.wheelBitmap->height, 1, ARENA_LOW_STACK);
    bm_shift_right(g_tires.wheelBitmap, g_tires.phase, g_tires.frames[0]);
    g_tires.copyBitmap = ar_bm(g_tires.frames[0]->width, g_tires.frames[0]->height, 1, ARENA_LOW_STACK);
    bm_copy(g_tires.frames[0], g_tires.copyBitmap, 3);
    g_tires.underlayY = g_tires.frames[0]->height - g_tires.underlay->height;
    bm_underlay(g_tires.underlay, g_tires.frames[0], g_tires.phase, g_tires.underlayY);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.overlayBounds.x1 = 0;
    g_tires.overlayBounds.y1 = 0;
    g_tires.overlayBounds.x2 = g_tires.backdrop->width - 1;
    g_tires.overlayBounds.y2 = g_tires.backdrop->height + g_spriteBgY - 1;
    g_tires.sprite = sp_setup(a & r_vid.mask, 0x94, &g_tires.copyBitmap, 0, 1, g_wheelBuf, g_tires.backdrop, &g_tires.overlayBounds, 0);
    sp_set_step(g_tires.sprite, 0, 8, 0, 1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite)) {
        if (g_tires.sprite->y == 0x7d)
            *g_tires.copyBitmap = *g_tires.frames[0];
        da_retrace(3);
    }
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    for (i = 1; i < 4; i++) {
        g_tires.rollShift = (i * 2 + g_tires.phase) & ~r_vid.mask;
        if (g_tires.rollShift) {
            g_tires.wheelBitmap = di_bm(p[i], ARENA_HIGH_STACK);
            g_tires.frames[4 - i] = ar_bm(g_tires.wheelBitmap->width + 8, g_tires.wheelBitmap->height, 1, ARENA_LOW_STACK);
            bm_shift_right(g_tires.wheelBitmap, g_tires.rollShift, g_tires.frames[4 - i]);
            ma_pop_high(1);
        } else
            g_tires.frames[4 - i] = di_bm(p[i], ARENA_LOW_STACK);
        bm_underlay(g_tires.underlay, g_tires.frames[4 - i], g_tires.rollShift, g_tires.underlayY);
    }
    i = 4;
    g_tires.sprite = sp_setup(a, 0x9b, g_tires.frames, 0, 4, g_rollBuf, g_tires.rollCanvas, 0, 0);
    sp_set_step(g_tires.sprite, 0, (a + g_tires.frames[0]->width) >> 1, -2, 0);
    so_wheel_roll(1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite))
        da_retrace(2);
    so_wheel_roll(0);
    ma_pop_low(6);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.frames[0] = di_bm(*q, ARENA_LOW_STACK);
    g_tires.phase = ((unsigned char)a & 7) >> 1;
    for (i = 1; i < 4; i++) {
        g_tires.rollShift = (i * 2) & ~r_vid.mask;
        if (g_tires.rollShift) {
            g_tires.wheelBitmap = di_bm(q[i], ARENA_HIGH_STACK);
            g_tires.frames[i] = ar_bm(g_tires.wheelBitmap->width + 8, g_tires.wheelBitmap->height, 1, ARENA_LOW_STACK);
            bm_shift_right(g_tires.wheelBitmap, g_tires.rollShift, g_tires.frames[i]);
            ma_pop_high(1);
        } else
            g_tires.frames[i] = di_bm(q[i], ARENA_LOW_STACK);
    }
    for (i = 0; i < 4; i++) {
        if (i == g_tires.phase) {
            g_tires.copyBitmap = ar_bm(g_tires.frames[i]->width, g_tires.frames[i]->height, 1, ARENA_LOW_STACK);
            bm_copy(g_tires.frames[i], g_tires.copyBitmap, 3);
        }
        bm_underlay(g_tires.underlay, g_tires.frames[i], (i * 2) & ~r_vid.mask, g_tires.underlayY);
    }
    i = 4;
    g_tires.sprite = sp_setup(-g_tires.frames[0]->width, 0x9b, g_tires.frames, 0, 4, g_rollBuf, g_tires.rollCanvas, 0, 0);
    sp_set_step(g_tires.sprite, 0, (a + g_tires.frames[0]->width >> 1) + 1, 2, 0);
    so_wheel_roll(1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite))
        da_retrace(2);
    so_wheel_roll(0);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.overlayBounds.x1 = 0;
    g_tires.overlayBounds.y1 = 0;
    g_tires.overlayBounds.x2 = g_tires.backdrop->width - 1;
    g_tires.overlayBounds.y2 = g_tires.backdrop->height + g_spriteBgY - 1;
    g_tires.sprite = sp_setup(a, 0x9b, &g_tires.frames[g_tires.phase], 0, 1, g_wheelBuf, g_tires.backdrop, &g_tires.overlayBounds, 0);
    sp_set_step(g_tires.sprite, 0, 8, 0, -1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite)) {
        if (g_tires.sprite->y == 0x7d)
            *g_tires.frames[g_tires.phase] = *g_tires.copyBitmap;
        da_retrace(3);
    }
    ma_pop_low(6);
}



void ti_change(int *a, int *b)
{
    struct Blt blt;
    int left;
    struct Bitmap far *dst;
    int frame;
    int s72;
    struct Bitmap far *pic;
    int rev;
    int ybot;
    int right;
    int modelm;
    int attr3;
    int tinto;
    int n;
    struct Blt clip6;
    int sv401;
    int s74;

    sv401 = r_vid.copyToFront;
    s72 = g_carDraw.stickerX;
    s74 = g_carDraw.stickerY;
    ma_release();
    cu_ctl(CURSOR_HIDE_NOW);
    r_vid.copyToFront = 0;
    g_tires.carX = g_xCoordSprite;
    modelm = g_driver.currentCar->specIndex;
    attr3 = gm_car_look(g_driver.currentCar);
    tinto = (g_driver.currentCar->appearance >> 8) & 0x1f;
    for (n = 0; n < 3; n++)
        g_tires.liftPics[n] = di_bm(g_liftPicIds[n], ARENA_LOW_STACK);
    sc_fill_rect(0x88, 0xba - g_tires.liftPics[1]->height, g_tires.liftPics[1]->width, g_tires.liftPics[1]->height,
            (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) ? g_carBg4[1] : g_carBg16[1]);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x97 - g_tires.liftPics[0]->height : 0;
    g_tires.jackColumn = ar_bm(g_tires.liftPics[1]->width, 0xba - g_spriteBgY, 0, ARENA_LOW_STACK);
    pu_grab(r_vid.mask & 0x88, g_spriteBgY, g_tires.liftPics[1]->width, 0xba - g_spriteBgY, g_tires.jackColumn);
    cp_build(&g_tires.liftCar, modelm, attr3, tinto, 0, 0, 0, 1);
    g_tires.wheelX1 = g_layoutAutoSprArray[modelm].wheelX1;
    g_tires.wheelX2 = g_layoutAutoSprArray[modelm].wheelX2;
    blt.w = g_tires.liftCar->width;
    blt.h = 0x1c;
    blt.sx = 0;
    blt.sy = ybot = g_tires.liftCar->height - 0x1c;
    g_tires.vehicleBottom = bm_grab(g_tires.liftCar, &blt, ARENA_LOW_STACK);
    sc_picture(0x408, -1);
    g_tires.basePic = di_bm(0x49d, ARENA_HIGH_STACK);
    bm_opaque_span(g_tires.liftCar, &left, &right);
    g_tires.groundStrip = ar_bm(g_tires.liftCar->width, g_tires.basePic->height, 0, ARENA_HIGH_STACK);
    pu_grab(g_tires.carX, 0x99 - g_tires.basePic->height, g_tires.liftCar->width, g_tires.basePic->height, g_tires.groundStrip);
    g_tires.groundOverlay = ar_bm(g_tires.basePic->width + 8, g_tires.basePic->height, 1, ARENA_HIGH_STACK);
    bm_shift_right(g_tires.basePic, left & ~r_vid.mask, g_tires.groundOverlay);
    blt.w = g_tires.groundOverlay->width;
    blt.h = g_tires.groundOverlay->height;
    blt.sx = blt.sy = 0;
    blt.dx = left & r_vid.mask;
    blt.dy = 0;
    (*g_videoOps.maskedBlit)(g_tires.groundOverlay, g_tires.groundStrip, &blt, 1);
    ma_pop_high(1);
    g_tires.groundOverlay = ar_bm(g_tires.basePic->width + 8, g_tires.basePic->height, 1, ARENA_HIGH_STACK);
    bm_shift_right(g_tires.basePic, (right + 1) & ~r_vid.mask, g_tires.groundOverlay);
    blt.w = g_tires.groundOverlay->width;
    blt.dx = right - blt.w + ~r_vid.mask + 1;
    (*g_videoOps.maskedBlit)(g_tires.groundOverlay, g_tires.groundStrip, &blt, 1);
    ma_pop_high(1);
    blt.w = g_tires.groundStrip->width;
    blt.dx = g_tires.carX;
    blt.dy = 0x99 - blt.h;
    (*g_videoOps.copy)(g_tires.groundStrip, r_vid.back, &blt, 1);
    ma_pop_high(2);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x92 - g_tires.liftCar->height : 0;
    g_tires.liftBackdrop = ar_bm(g_tires.liftCar->width, 0x99 - g_spriteBgY, 1, ARENA_LOW_STACK);
    pu_grab(g_tires.carX, g_spriteBgY, g_tires.liftCar->width, 0x99 - g_spriteBgY, g_tires.liftBackdrop);
    da_fmemset(g_tires.liftBackdrop->alt.p, 0xff, g_tires.liftBackdrop->planesize / r_vid.planes);
    clip6.w = g_tires.liftPics[1]->width;
    clip6.h = g_tires.liftPics[1]->height;
    clip6.sx = clip6.sy = 0;
    clip6.dx = 0x88 - g_tires.carX;
    clip6.dy = -(g_tires.liftPics[1]->height + g_spriteBgY - 0x9a);
    (*g_videoOps.maskedBlit)(g_tires.liftPics[0], g_tires.liftBackdrop, &clip6, 1);
    (*g_videoOps.copy)(g_tires.liftPics[0], g_tires.liftBackdrop, &clip6, 2);
    g_tires.column = di_bm(0x4c7, ARENA_HIGH_STACK);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x76 : 0;
    pic = grab_wheel_column(g_tires.wheelX2);
    dst = grab_wheel_column(g_tires.wheelX1);
    ma_pop_high(1);
    g_tires.rollCanvas = ar_bm(g_tires.liftCar->width + g_tires.carX, 0x1c, 0, ARENA_LOW_STACK);
    pu_grab(0, 0x80, g_tires.liftCar->width + g_tires.carX, 0x1c, g_tires.rollCanvas);
    blt.w = g_tires.liftCar->width;
    blt.h = g_tires.rollCanvas->height - 8;
    blt.sx = 0;
    blt.sy = g_tires.liftCar->height - 0x15;
    blt.dx = g_tires.carX;
    blt.dy = 0;
    (*g_videoOps.maskedBlit)(g_tires.liftCar, g_tires.rollCanvas, &blt, 1);
    blt.w = g_tires.liftPics[1]->width;
    blt.h = g_tires.rollCanvas->height;
    blt.sx = 0;
    blt.sy = g_tires.liftPics[1]->height - blt.h + 2;
    blt.dx = 0x88;
    blt.dy = 0;
    (*g_videoOps.maskedBlit)(g_tires.liftPics[2], g_tires.rollCanvas, &blt, 1);
    if (r_vid.mode == VID_EGA) {
        g_tires.liftBuf = &g_tires.liftHeader;
        me_set_bitmap(g_tires.liftBuf, g_pageBmps[2]->bits.p, 0, g_tires.liftCar->width, SCREEN_HEIGHT, VID_EGA);
        me_set_bitmap(g_wheelBuf, g_pageBmps[2]->bits.p + g_tires.liftCar->pitch, 0, pic->width, SCREEN_HEIGHT, VID_EGA);
        me_set_bitmap(g_rollBuf, g_pageBmps[3]->bits.p, 0, SCREEN_WIDTH, g_tires.rollCanvas->height, VID_EGA);
        me_set_bitmap(g_jackBuf, g_pageBmps[3]->bits.p, 0, g_tires.jackColumn->width, SCREEN_HEIGHT, VID_EGA);
        blt.w = g_tires.liftBackdrop->width;
        blt.h = g_tires.liftCar->height + 4;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x95 - g_tires.liftCar->height;
        (*g_videoOps.copy)(g_tires.liftBackdrop, g_tires.liftBuf, &blt, 1);
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sy = blt.dy = 0x79;
        (*g_videoOps.copy)(dst, g_wheelBuf, &blt, 1);
        blt.w = g_tires.rollCanvas->width;
        blt.h = g_tires.rollCanvas->height;
        blt.sy = blt.dy = 0;
        (*g_videoOps.copy)(g_tires.rollCanvas, g_rollBuf, &blt, 1);
        blt.w = g_tires.jackColumn->width;
        blt.h = g_tires.liftPics[0]->height + 0x21;
        blt.sy = blt.dy = 0x99 - g_tires.liftPics[0]->height;
        (*g_videoOps.copy)(g_tires.jackColumn, g_jackBuf, &blt, 1);
    } else if (r_vid.mode != VID_TANDY) {
        g_tires.liftBuf = ar_bm(g_tires.liftBackdrop->width, g_tires.liftBackdrop->height, 0, ARENA_LOW_STACK);
        *g_wheelBuf = *pic;
        g_wheelBuf->bits.p = g_tires.liftBuf->bits.p;
        g_wheelBuf->alt.p = 0;
        *g_rollBuf = *g_tires.rollCanvas;
        g_rollBuf->bits.p = g_tires.liftBuf->bits.p;
        g_rollBuf->alt.p = 0;
        *g_jackBuf = *g_tires.jackColumn;
        g_jackBuf->bits.p = g_tires.liftBuf->bits.p;
        g_jackBuf->alt.p = 0;
    }
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x97 - g_tires.liftPics[0]->height : 0;
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.sprite = sp_setup(0x88, 0xb9, &g_tires.liftPics[1], 0, 1, g_jackBuf, g_tires.jackColumn, 0, 0);
    sp_set_step(g_tires.sprite, 0, 0x21, 0, -1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite))
        da_retrace(3);
    mu_suspend(1);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x92 - g_tires.liftCar->height : 0;
    g_tires.wheelBitmap = di_bm(*a, ARENA_HIGH_STACK);
    bm_underlay(g_tires.wheelBitmap, g_tires.liftCar, g_tires.wheelX1, ybot);
    bm_underlay(g_tires.wheelBitmap, g_tires.liftCar, g_tires.wheelX2, ybot);
    ma_pop_high(1);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.overlayBounds.x1 = 0;
    g_tires.overlayBounds.y1 = 0;
    g_tires.overlayBounds.x2 = g_tires.liftBackdrop->width - 1;
    g_tires.overlayBounds.y2 = g_tires.liftBackdrop->height + g_spriteBgY - 1;
    g_tires.sprite = sp_setup(g_tires.carX, 0x98, &g_tires.liftCar, 0, 1, g_tires.liftBuf, g_tires.liftBackdrop, &g_tires.overlayBounds, 0);
    sp_set_step(g_tires.sprite, 0, 7, 0, -1);
    frame = 0;
    rev = 0;
    so_jack(1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite)) {
        da_retrace(3);
        if (rev) {
            if (frame == 2) {
                frame = 1;
                rev = 0;
            } else
                frame++;
        } else {
            if (frame == 0) {
                frame = 1;
                rev = frame;
            } else
                frame--;
        }
        if (rev == 0)
            g_tires.sprite->y++;
        (*g_videoOps.maskedBlit)(g_tires.liftPics[frame], g_tires.liftBackdrop, &clip6, 1);
        (*g_videoOps.copy)(g_tires.liftPics[frame], g_tires.liftBackdrop, &clip6, 2);
    }
    so_jack(0);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x76 : 0;
    g_tires.backdrop = dst;
    swap_wheel(g_tires.carX + g_tires.wheelX1, a, b);
    g_tires.backdrop = pic;
    if (r_vid.mode == VID_EGA) {
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x79;
        (*g_videoOps.copy)(pic, g_wheelBuf, &blt, 1);
    }
    blt.w = 0x28;
    blt.h = g_tires.rollCanvas->height;
    blt.sx = blt.dx = g_tires.carX + g_tires.wheelX1;
    blt.sy = 0x9c - blt.h;
    blt.dy = 0;
    (*g_videoOps.copy)(r_vid.front, g_tires.rollCanvas, &blt, 1);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copy)(r_vid.front, g_rollBuf, &blt, 1);
    swap_wheel(g_tires.carX + g_tires.wheelX2, a, b);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x92 - g_tires.liftCar->height : 0;
    g_tires.frames[0] = di_bm(*b, ARENA_LOW_STACK);
    blt.w = g_tires.liftCar->width;
    blt.h = g_tires.frames[0]->height;
    blt.dy = g_tires.liftCar->height - g_tires.frames[0]->height;
    blt.sx = blt.sy = blt.dx = 0;
    (*g_videoOps.copy)(g_tires.vehicleBottom, g_tires.liftCar, &blt, 3);
    bm_underlay(g_tires.frames[0], g_tires.liftCar, g_tires.wheelX1, ybot);
    bm_underlay(g_tires.frames[0], g_tires.liftCar, g_tires.wheelX2, ybot);
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    frame = 2;
    rev = 1;
    (*g_videoOps.maskedBlit)(g_tires.liftPics[2], g_tires.liftBackdrop, &clip6, 1);
    (*g_videoOps.copy)(g_tires.liftPics[2], g_tires.liftBackdrop, &clip6, 2);
    g_tires.overlayBounds.x1 = 0;
    g_tires.overlayBounds.y1 = 0;
    g_tires.overlayBounds.x2 = g_tires.liftBackdrop->width - 1;
    g_tires.overlayBounds.y2 = g_tires.liftBackdrop->height + g_spriteBgY - 1;
    g_tires.sprite = sp_setup(g_tires.carX, 0x94, &g_tires.liftCar, 0, 1, g_tires.liftBuf, g_tires.liftBackdrop, &g_tires.overlayBounds, 0);
    sp_set_step(g_tires.sprite, 0, 9, 0, 1);
    so_jack(1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite)) {
        da_retrace(3);
        if (rev) {
            if (frame == 2) {
                frame = 1;
                rev = 0;
            } else
                frame++;
        } else {
            if (frame == 0) {
                frame = 1;
                rev = frame;
            } else
                frame--;
        }
        if (rev == 0)
            g_tires.sprite->y--;
        (*g_videoOps.maskedBlit)(g_tires.liftPics[frame], g_tires.liftBackdrop, &clip6, 1);
        (*g_videoOps.copy)(g_tires.liftPics[frame], g_tires.liftBackdrop, &clip6, 2);
    }
    so_jack(0);
    mu_suspend(0);
    g_spriteBgY = (r_vid.mode == VID_TANDY) ? 0x97 - g_tires.liftPics[0]->height : 0;
    if (r_vid.mode == VID_EGA)
        (*g_videoOps.copyPage)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tires.sprite = sp_setup(0x88, 0x99, &g_tires.liftPics[1], 0, 1, g_jackBuf, g_tires.jackColumn, 0, 0);
    sp_set_step(g_tires.sprite, 0, 0x21, 0, 1);
    while ((*g_videoOps.spriteStep)(g_tires.sprite))
        da_retrace(3);
    ma_release();
    (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    g_xCoordSprite = g_tires.carX;
    r_vid.copyToFront = sv401;
    g_carDraw.stickerX = s72;
    g_carDraw.stickerY = s74;
    g_spriteBgY = 0;
    cu_ctl(CURSOR_SHOW_NOW);
}
