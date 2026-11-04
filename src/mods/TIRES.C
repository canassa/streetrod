#include "STRUCTS.H"
#include "TIRES.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
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
#include "POPUP.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SPRITE.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Bitmap far *g_6cd4oz;        /* DS:6CD4 */
static int g_wheelX2Value;                       /* DS:6CD8 */
static struct Bitmap far *g_liftPics[3];     /* DS:6CDA */
static int g_6ce6sz;                       /* DS:6CE6 */
static struct Rect g_6ce8rz;               /* DS:6CE8 */
static struct Bitmap far *g_6cf0zz;        /* DS:6CF0 */
static struct Bitmap g_6cf4wz;             /* DS:6CF4 */
static struct Bitmap far *g_6d24ajz;       /* DS:6D24 */
static struct Bitmap far *g_6d28agz;       /* DS:6D28 */
static struct Bitmap far *g_tireBackdrop;       /* DS:6D2C */
static struct Bitmap far *g_columnBitmapPtr;          /* DS:6D30 */
static struct Bitmap far *g_groundStripPointer;          /* DS:6D34 */
static struct Bitmap far *g_tireUnderPicPtr;          /* DS:6D38 */
static struct Sprite *g_6d3caat;           /* DS:6D3C */
static int g_6d3eaas;                      /* DS:6D3E */
static struct Bitmap g_wheelRollHdr;              /* DS:6D40 */
static struct Bitmap g_jackBufHeader;              /* DS:6D70 */
static int g_6da0;                         /* DS:6DA0 */
static int g_6da2;                         /* DS:6DA2 */
static struct Bitmap g_wheelBufferDescImage;               /* DS:6DA4 */
static int g_6dd4;                         /* DS:6DD4 */
static struct Bitmap far *g_vehicleBottomBmp;          /* DS:6DD6 */
static struct Bitmap far *g_groundOverlayBmp;          /* DS:6DDA */
static struct Bitmap far *g_basePicPtr;          /* DS:6DDE */
static struct Bitmap far *g_tireFrames[4];      /* DS:6DE2 */
static struct Bitmap far *g_6df2a;         /* DS:6DF2 */
static struct Bitmap far *g_liftCar;         /* DS:6DF6 */
static struct Bitmap far *g_carLiftBackdrop;         /* DS:6DFA */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */

static struct Bitmap far * near grab_wheel_column(int n);
static void near swap_wheel(int a, int *p, int *q);

int g_spriteBgY = 0;   /* DS:5AA8 */
int g_liftPicIds[3] = {1034, 1035, 1036};   /* DS:5AAA */
struct Bitmap far *g_wheelBuf = (struct Bitmap far *)&g_wheelBufferDescImage;   /* DS:5AB0 */
struct Bitmap far *g_rollBuf = (struct Bitmap far *)&g_wheelRollHdr;   /* DS:5AB4 */
struct Bitmap far *g_jackBuf = (struct Bitmap far *)&g_jackBufHeader;   /* DS:5AB8 */

/* grab_wheel_column @ 0F38:B632 (linear 0x1A9B2, 506 bytes)

   Build the two scratch bitmaps for a shifted background strip and its 0x28-wide column */

static struct Bitmap far *near grab_wheel_column(int n)
{
    struct Bitmap far *b;
    struct Blt blt;
    struct Bitmap far *a2;
    struct Bitmap far *c;

    a2 = F_1934E(g_columnBitmapPtr->width + 8, g_columnBitmapPtr->height, g_columnBitmapPtr->alt.p != 0L, 1);
    bitmap_shift_right(g_columnBitmapPtr, n & ~r_vid.mask, a2);
    b = F_1934E(a2->width, a2->height, 0, 1);
    popup_grab((n + g_6da0) & r_vid.mask, 0x78, a2->width, a2->height, b);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*ab78aat)(a2, b, &blt, 1);
    blt.w = a2->width;
    blt.h = a2->height;
    blt.sx = blt.sy = 0;
    blt.dx = (n + g_6da0) & r_vid.mask;
    blt.dy = 0x78;
    (*b_78a2)(b, r_vid.back, &blt, 1);
    marks_pop_low(2);
    blt.w = 0x28;
    blt.h = g_liftCar->height;
    blt.sx = n & r_vid.mask;
    blt.sy = blt.dx = 0;
    blt.dy = -(g_liftCar->height + g_spriteBgY - 0x95);
    c = F_1934E(0x28, 0x9c - g_spriteBgY, 1, 1);
    popup_grab(blt.sx + g_6da0, g_spriteBgY, 0x28, 0x9c - g_spriteBgY, c);
    dash_fmemset(c->alt.p, 0xff, c->planesize / r_vid.planes);
    (*ab78aat)(g_liftCar, c, &blt, 1);
    (*b_78a2)(g_liftCar, c, &blt, 2);
    return c;
}

/* swap_wheel @ 0F38:B82C (linear 0x1ABAC, 1639 bytes)

   Build the four shifted copies of a background strip and animate the sprite over them */

static void near swap_wheel(int a, int *p, int *q)
{
    int i;

    g_tireUnderPicPtr = F_15BBE(0x4d2, 1);
    g_6cf0zz = F_15BBE(*p, 1);
    g_6d3eaas = a & ~r_vid.mask;
    g_tireFrames[0] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
    bitmap_shift_right(g_6cf0zz, g_6d3eaas, g_tireFrames[0]);
    g_6df2a = F_1934E(g_tireFrames[0]->width, g_tireFrames[0]->height, 1, 1);
    bitmap_copy(g_tireFrames[0], g_6df2a, 3);
    g_6ce6sz = g_tireFrames[0]->height - g_tireUnderPicPtr->height;
    bitmap_underlay(g_tireUnderPicPtr, g_tireFrames[0], g_6d3eaas, g_6ce6sz);
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_tireBackdrop->width - 1;
    g_6ce8rz.y2 = g_tireBackdrop->height + g_spriteBgY - 1;
    g_6d3caat = sprite_setup(a & r_vid.mask, 0x94, &g_6df2a, 0, 1, g_wheelBuf, g_tireBackdrop, &g_6ce8rz, 0);
    sprite_set_step(g_6d3caat, 0, 8, 0, 1);
    while ((*g78c6c)(g_6d3caat)) {
        if (g_6d3caat->y == 0x7d)
            *g_6df2a = *g_tireFrames[0];
        F_10AFD(3);
    }
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2 + g_6d3eaas) & ~r_vid.mask;
        if (g_6da2) {
            g_6cf0zz = F_15BBE(p[i], 2);
            g_tireFrames[4 - i] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
            bitmap_shift_right(g_6cf0zz, g_6da2, g_tireFrames[4 - i]);
            F_19438(1);
        } else
            g_tireFrames[4 - i] = F_15BBE(p[i], 1);
        bitmap_underlay(g_tireUnderPicPtr, g_tireFrames[4 - i], g_6da2, g_6ce6sz);
    }
    i = 4;
    g_6d3caat = sprite_setup(a, 0x9b, g_tireFrames, 0, 4, g_rollBuf, g_6d24ajz, 0, 0);
    sprite_set_step(g_6d3caat, 0, (a + g_tireFrames[0]->width) >> 1, -2, 0);
    sound_wheel_roll(1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(2);
    sound_wheel_roll(0);
    marks_pop_low(6);
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_tireFrames[0] = F_15BBE(*q, 1);
    g_6d3eaas = ((unsigned char)a & 7) >> 1;
    for (i = 1; i < 4; i++) {
        g_6da2 = (i * 2) & ~r_vid.mask;
        if (g_6da2) {
            g_6cf0zz = F_15BBE(q[i], 2);
            g_tireFrames[i] = F_1934E(g_6cf0zz->width + 8, g_6cf0zz->height, 1, 1);
            bitmap_shift_right(g_6cf0zz, g_6da2, g_tireFrames[i]);
            F_19438(1);
        } else
            g_tireFrames[i] = F_15BBE(q[i], 1);
    }
    for (i = 0; i < 4; i++) {
        if (i == g_6d3eaas) {
            g_6df2a = F_1934E(g_tireFrames[i]->width, g_tireFrames[i]->height, 1, 1);
            bitmap_copy(g_tireFrames[i], g_6df2a, 3);
        }
        bitmap_underlay(g_tireUnderPicPtr, g_tireFrames[i], (i * 2) & ~r_vid.mask, g_6ce6sz);
    }
    i = 4;
    g_6d3caat = sprite_setup(-g_tireFrames[0]->width, 0x9b, g_tireFrames, 0, 4, g_rollBuf, g_6d24ajz, 0, 0);
    sprite_set_step(g_6d3caat, 0, (a + g_tireFrames[0]->width >> 1) + 1, 2, 0);
    sound_wheel_roll(1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(2);
    sound_wheel_roll(0);
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_tireBackdrop->width - 1;
    g_6ce8rz.y2 = g_tireBackdrop->height + g_spriteBgY - 1;
    g_6d3caat = sprite_setup(a, 0x9b, &g_tireFrames[g_6d3eaas], 0, 1, g_wheelBuf, g_tireBackdrop, &g_6ce8rz, 0);
    sprite_set_step(g_6d3caat, 0, 8, 0, -1);
    while ((*g78c6c)(g_6d3caat)) {
        if (g_6d3caat->y == 0x7d)
            *g_tireFrames[g_6d3eaas] = *g_6df2a;
        F_10AFD(3);
    }
    marks_pop_low(6);
}

/* F_1B213 @ 0F38:BE93 (linear 0x1B213, 3849 bytes)

   The car showroom sprite sequence (strip sprites slide across the screen in both directions) */

void F_1B213(int *a, int *b)
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
    s72 = m_8272.m8272;
    s74 = m_8272.m8274;
    marks_release();
    cursor_ctl(-4);
    r_vid.copyToFront = 0;
    g_6da0 = z_8b7c;
    modelm = a_7eaap.m7eb0->specIndex;
    attr3 = game_car_look(a_7eaap.m7eb0);
    tinto = (a_7eaap.m7eb0->appearance >> 8) & 0x1f;
    for (n = 0; n < 3; n++)
        g_liftPics[n] = F_15BBE(g_liftPicIds[n], 1);
    screen_fill_rect(0x88, 0xba - g_liftPics[1]->height, g_liftPics[1]->width, g_liftPics[1]->height,
            (r_vid.mode != -2 && r_vid.mode != -6) ? g_carBg4[1] : g_carBg16[1]);
    g_spriteBgY = (r_vid.mode == -6) ? 0x97 - g_liftPics[0]->height : 0;
    g_6d28agz = F_1934E(g_liftPics[1]->width, 0xba - g_spriteBgY, 0, 1);
    popup_grab(r_vid.mask & 0x88, g_spriteBgY, g_liftPics[1]->width, 0xba - g_spriteBgY, g_6d28agz);
    carpic_build(&g_liftCar, modelm, attr3, tinto, 0, 0, 0, 1);
    g_6dd4 = g7682n[modelm].wheelX1;
    g_wheelX2Value = g7682n[modelm].wheelX2;
    blt.w = g_liftCar->width;
    blt.h = 0x1c;
    blt.sx = 0;
    blt.sy = ybot = g_liftCar->height - 0x1c;
    g_vehicleBottomBmp = bitmap_grab(g_liftCar, &blt, 1);
    F_118D4(0x408, -1);
    g_basePicPtr = F_15BBE(0x49d, 2);
    bitmap_opaque_span(g_liftCar, &left, &right);
    g_groundStripPointer = F_1934E(g_liftCar->width, g_basePicPtr->height, 0, 2);
    popup_grab(g_6da0, 0x99 - g_basePicPtr->height, g_liftCar->width, g_basePicPtr->height, g_groundStripPointer);
    g_groundOverlayBmp = F_1934E(g_basePicPtr->width + 8, g_basePicPtr->height, 1, 2);
    bitmap_shift_right(g_basePicPtr, left & ~r_vid.mask, g_groundOverlayBmp);
    blt.w = g_groundOverlayBmp->width;
    blt.h = g_groundOverlayBmp->height;
    blt.sx = blt.sy = 0;
    blt.dx = left & r_vid.mask;
    blt.dy = 0;
    (*ab78aat)(g_groundOverlayBmp, g_groundStripPointer, &blt, 1);
    F_19438(1);
    g_groundOverlayBmp = F_1934E(g_basePicPtr->width + 8, g_basePicPtr->height, 1, 2);
    bitmap_shift_right(g_basePicPtr, (right + 1) & ~r_vid.mask, g_groundOverlayBmp);
    blt.w = g_groundOverlayBmp->width;
    blt.dx = right - blt.w + ~r_vid.mask + 1;
    (*ab78aat)(g_groundOverlayBmp, g_groundStripPointer, &blt, 1);
    F_19438(1);
    blt.w = g_groundStripPointer->width;
    blt.dx = g_6da0;
    blt.dy = 0x99 - blt.h;
    (*b_78a2)(g_groundStripPointer, r_vid.back, &blt, 1);
    F_19438(2);
    g_spriteBgY = (r_vid.mode == -6) ? 0x92 - g_liftCar->height : 0;
    g_carLiftBackdrop = F_1934E(g_liftCar->width, 0x99 - g_spriteBgY, 1, 1);
    popup_grab(g_6da0, g_spriteBgY, g_liftCar->width, 0x99 - g_spriteBgY, g_carLiftBackdrop);
    dash_fmemset(g_carLiftBackdrop->alt.p, 0xff, g_carLiftBackdrop->planesize / r_vid.planes);
    clip6.w = g_liftPics[1]->width;
    clip6.h = g_liftPics[1]->height;
    clip6.sx = clip6.sy = 0;
    clip6.dx = 0x88 - g_6da0;
    clip6.dy = -(g_liftPics[1]->height + g_spriteBgY - 0x9a);
    (*ab78aat)(g_liftPics[0], g_carLiftBackdrop, &clip6, 1);
    (*b_78a2)(g_liftPics[0], g_carLiftBackdrop, &clip6, 2);
    g_columnBitmapPtr = F_15BBE(0x4c7, 2);
    g_spriteBgY = (r_vid.mode == -6) ? 0x76 : 0;
    pic = grab_wheel_column(g_wheelX2Value);
    dst = grab_wheel_column(g_6dd4);
    F_19438(1);
    g_6d24ajz = F_1934E(g_liftCar->width + g_6da0, 0x1c, 0, 1);
    popup_grab(0, 0x80, g_liftCar->width + g_6da0, 0x1c, g_6d24ajz);
    blt.w = g_liftCar->width;
    blt.h = g_6d24ajz->height - 8;
    blt.sx = 0;
    blt.sy = g_liftCar->height - 0x15;
    blt.dx = g_6da0;
    blt.dy = 0;
    (*ab78aat)(g_liftCar, g_6d24ajz, &blt, 1);
    blt.w = g_liftPics[1]->width;
    blt.h = g_6d24ajz->height;
    blt.sx = 0;
    blt.sy = g_liftPics[1]->height - blt.h + 2;
    blt.dx = 0x88;
    blt.dy = 0;
    (*ab78aat)(g_liftPics[2], g_6d24ajz, &blt, 1);
    if (r_vid.mode == -2) {
        g_6cd4oz = &g_6cf4wz;
        mem_set_bitmap(g_6cd4oz, g_pageBmps[2]->bits.p, 0, g_liftCar->width, 200, -2);
        mem_set_bitmap(g_wheelBuf, g_pageBmps[2]->bits.p + g_liftCar->pitch, 0, pic->width, 200, -2);
        mem_set_bitmap(g_rollBuf, g_pageBmps[3]->bits.p, 0, 0x140, g_6d24ajz->height, -2);
        mem_set_bitmap(g_jackBuf, g_pageBmps[3]->bits.p, 0, g_6d28agz->width, 200, -2);
        blt.w = g_carLiftBackdrop->width;
        blt.h = g_liftCar->height + 4;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x95 - g_liftCar->height;
        (*b_78a2)(g_carLiftBackdrop, g_6cd4oz, &blt, 1);
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sy = blt.dy = 0x79;
        (*b_78a2)(dst, g_wheelBuf, &blt, 1);
        blt.w = g_6d24ajz->width;
        blt.h = g_6d24ajz->height;
        blt.sy = blt.dy = 0;
        (*b_78a2)(g_6d24ajz, g_rollBuf, &blt, 1);
        blt.w = g_6d28agz->width;
        blt.h = g_liftPics[0]->height + 0x21;
        blt.sy = blt.dy = 0x99 - g_liftPics[0]->height;
        (*b_78a2)(g_6d28agz, g_jackBuf, &blt, 1);
    } else if (r_vid.mode != -6) {
        g_6cd4oz = F_1934E(g_carLiftBackdrop->width, g_carLiftBackdrop->height, 0, 1);
        *g_wheelBuf = *pic;
        g_wheelBuf->bits.p = g_6cd4oz->bits.p;
        g_wheelBuf->alt.p = 0;
        *g_rollBuf = *g_6d24ajz;
        g_rollBuf->bits.p = g_6cd4oz->bits.p;
        g_rollBuf->alt.p = 0;
        *g_jackBuf = *g_6d28agz;
        g_jackBuf->bits.p = g_6cd4oz->bits.p;
        g_jackBuf->alt.p = 0;
    }
    g_spriteBgY = (r_vid.mode == -6) ? 0x97 - g_liftPics[0]->height : 0;
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_6d3caat = sprite_setup(0x88, 0xb9, &g_liftPics[1], 0, 1, g_jackBuf, g_6d28agz, 0, 0);
    sprite_set_step(g_6d3caat, 0, 0x21, 0, -1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(3);
    F_16381(1);
    g_spriteBgY = (r_vid.mode == -6) ? 0x92 - g_liftCar->height : 0;
    g_6cf0zz = F_15BBE(*a, 2);
    bitmap_underlay(g_6cf0zz, g_liftCar, g_6dd4, ybot);
    bitmap_underlay(g_6cf0zz, g_liftCar, g_wheelX2Value, ybot);
    F_19438(1);
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_carLiftBackdrop->width - 1;
    g_6ce8rz.y2 = g_carLiftBackdrop->height + g_spriteBgY - 1;
    g_6d3caat = sprite_setup(g_6da0, 0x98, &g_liftCar, 0, 1, g_6cd4oz, g_carLiftBackdrop, &g_6ce8rz, 0);
    sprite_set_step(g_6d3caat, 0, 7, 0, -1);
    frame = 0;
    rev = 0;
    sound_jack(1);
    while ((*g78c6c)(g_6d3caat)) {
        F_10AFD(3);
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
            g_6d3caat->y++;
        (*ab78aat)(g_liftPics[frame], g_carLiftBackdrop, &clip6, 1);
        (*b_78a2)(g_liftPics[frame], g_carLiftBackdrop, &clip6, 2);
    }
    sound_jack(0);
    g_spriteBgY = (r_vid.mode == -6) ? 0x76 : 0;
    g_tireBackdrop = dst;
    swap_wheel(g_6da0 + g_6dd4, a, b);
    g_tireBackdrop = pic;
    if (r_vid.mode == -2) {
        blt.w = pic->width;
        blt.h = 0x23;
        blt.sx = blt.dx = 0;
        blt.sy = blt.dy = 0x79;
        (*b_78a2)(pic, g_wheelBuf, &blt, 1);
    }
    blt.w = 0x28;
    blt.h = g_6d24ajz->height;
    blt.sx = blt.dx = g_6da0 + g_6dd4;
    blt.sy = 0x9c - blt.h;
    blt.dy = 0;
    (*b_78a2)(r_vid.front, g_6d24ajz, &blt, 1);
    if (r_vid.mode == -2)
        (*b_78a2)(r_vid.front, g_rollBuf, &blt, 1);
    swap_wheel(g_6da0 + g_wheelX2Value, a, b);
    g_spriteBgY = (r_vid.mode == -6) ? 0x92 - g_liftCar->height : 0;
    g_tireFrames[0] = F_15BBE(*b, 1);
    blt.w = g_liftCar->width;
    blt.h = g_tireFrames[0]->height;
    blt.dy = g_liftCar->height - g_tireFrames[0]->height;
    blt.sx = blt.sy = blt.dx = 0;
    (*b_78a2)(g_vehicleBottomBmp, g_liftCar, &blt, 3);
    bitmap_underlay(g_tireFrames[0], g_liftCar, g_6dd4, ybot);
    bitmap_underlay(g_tireFrames[0], g_liftCar, g_wheelX2Value, ybot);
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    frame = 2;
    rev = 1;
    (*ab78aat)(g_liftPics[2], g_carLiftBackdrop, &clip6, 1);
    (*b_78a2)(g_liftPics[2], g_carLiftBackdrop, &clip6, 2);
    g_6ce8rz.x1 = 0;
    g_6ce8rz.y1 = 0;
    g_6ce8rz.x2 = g_carLiftBackdrop->width - 1;
    g_6ce8rz.y2 = g_carLiftBackdrop->height + g_spriteBgY - 1;
    g_6d3caat = sprite_setup(g_6da0, 0x94, &g_liftCar, 0, 1, g_6cd4oz, g_carLiftBackdrop, &g_6ce8rz, 0);
    sprite_set_step(g_6d3caat, 0, 9, 0, 1);
    sound_jack(1);
    while ((*g78c6c)(g_6d3caat)) {
        F_10AFD(3);
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
            g_6d3caat->y--;
        (*ab78aat)(g_liftPics[frame], g_carLiftBackdrop, &clip6, 1);
        (*b_78a2)(g_liftPics[frame], g_carLiftBackdrop, &clip6, 2);
    }
    sound_jack(0);
    F_16381(0);
    g_spriteBgY = (r_vid.mode == -6) ? 0x97 - g_liftPics[0]->height : 0;
    if (r_vid.mode == -2)
        (*dz78b6)(g_pageBmps[0]->bits.p, g_pageBmps[1]->bits.p);
    g_6d3caat = sprite_setup(0x88, 0x99, &g_liftPics[1], 0, 1, g_jackBuf, g_6d28agz, 0, 0);
    sprite_set_step(g_6d3caat, 0, 0x21, 0, 1);
    while ((*g78c6c)(g_6d3caat))
        F_10AFD(3);
    marks_release();
    (*dz78b6)(r_vid.front->bits.p, r_vid.back->bits.p);
    z_8b7c = g_6da0;
    r_vid.copyToFront = sv401;
    m_8272.m8272 = s72;
    m_8272.m8274 = s74;
    g_spriteBgY = 0;
    cursor_ctl(-2);
}
