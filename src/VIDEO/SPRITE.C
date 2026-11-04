#include <stdlib.h>
#include "STRUCTS.H"
#include "SPRITE.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "DASH.H"
#include "GARGFX.H"
#include "MAIN.H"
#include "MARKS.H"
#include "PC.H"
#include "POPUP.H"
#include "SYS.H"
#include "VIDEO.H"

/* sp_rect_and @ 0F38:A0FA (linear 0x1947A, 158 bytes)

   Intersect rectangles a and b into out, snapped to the r_vid.mask pixel mask; 0 if they do not
   overlap */

struct Rect *sp_rect_and(struct Rect *a, struct Rect *b, struct Rect *out)
{
    if (b->x2 < a->x1 || a->x2 < b->x1)
        return 0;
    out->x1 = (a->x1 < b->x1) ? b->x1 : a->x1;
    out->x2 = (a->x2 > b->x2) ? b->x2 : a->x2;
    if (a->y1 > b->y2 || a->y2 < b->y1)
        return 0;
    out->y1 = (a->y1 < b->y1) ? b->y1 : a->y1;
    out->y2 = (a->y2 > b->y2) ? b->y2 : a->y2;
    out->x1 &= r_vid.mask;
    out->x2 += ~r_vid.mask;
    out->x2 &= r_vid.mask;
    return out;
}

/* sp_car_clip @ 0F38:A198 (linear 0x19518, 86 bytes)

   Clip the rectangle (x, y, w, h) against the screen window; result in out */

struct Rect *sp_car_clip(int x, int y, int w, int h, struct Rect *out)
{
    struct Rect rect, scr;

    scr.x1 = g_picXDraw;
    scr.x2 = g_autoSprImagePtr->width + g_picXDraw - 1;
    scr.y1 = g_sprDrawY;
    scr.y2 = g_sprDrawY + g_carVisibleHtValue - 1;
    rect.x1 = x;
    rect.x2 = x + w - 1;
    rect.y1 = y;
    rect.y2 = y + h - 1;
    return sp_rect_and(&scr, &rect, out);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Frame g_frmTable[12];
struct Seq g_seqSpriteMoveData[10];
struct Sprite g_mainCarSprite;
struct Page g_sprPageRing[2];

/* sp_clip_box @ 0F38:A1EE (linear 0x1956E, 133 bytes)

   Clip the box (c, d, a, b) to 0..amax x 0..bmax, snap to the r_vid.mask pixel mask; result in out */

void sp_clip_box(int a, int b, int c, int d, int amax, int bmax, struct Rect *out)
{
    if (d < 0) {
        b += d;
        d = 0;
    }
    if (d + b > bmax) {
        if ((b = bmax - d) < 0)
            b = 0;
    }
    if ((c &= r_vid.mask) < 0) {
        a += c;
        c = 0;
    }
    if (c + a > amax) {
        if ((a = amax - c) < 0)
            a = 0;
    }
    a &= r_vid.mask;
    out->x1 = c;
    out->x2 = c + a - 1;
    out->y1 = d;
    out->y2 = d + b - 1;
}

/* sp_setup @ 0F38:A273 (linear 0x195F3, 582 bytes)

   Set up the sprite g_mainCarSprite from n frame images: the ring of frames, the display pages and the
   clipped rectangle */

struct Sprite *sp_setup(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img,
    struct Bitmap far *save, struct Rect *a8, struct Rect *a9)
{
    int i;

    y -= (*imgs)->height - 1;
    for (i = 0; i < n; i++) {
        g_frmTable[i].pos = *imgs++;
        if (cell)
            g_frmTable[i].data = r_vid.mode == VID_EGA ? cell++ : &g_bandRecordsSprArray[i];
        else
            g_frmTable[i].data = 0;
        g_frmTable[i].nxt[0] = &g_frmTable[i + 1];
    }
    g_mainCarSprite.frame = g_frmTable[i - 1].nxt[0] = g_frmTable;
    for (i = 0; i < n; i++)
        g_frmTable[i].nxt[1] = g_frmTable[i].nxt[0]->nxt[0];
    for (i = 0; i < n; i++)
        g_frmTable[i].nxt[2] = g_frmTable[i].nxt[1]->nxt[1];
    g_mainCarSprite.img = img;
    g_mainCarSprite.x = x;
    g_mainCarSprite.y = y;
    g_mainCarSprite.save = save;
    g_mainCarSprite.rc[0] = a8;
    g_mainCarSprite.rc[1] = a9;
    if (r_vid.mode == VID_EGA) {
        for (i = 0; i < 2; i++) {
            g_sprPageRing[i].bm = g_pageBmps[i];
            g_sprPageRing[i].pageIndex = i;
            g_sprPageRing[i].rect.x1 = g_sprPageRing[i].rect.y1 = 0;
            g_sprPageRing[i].rect.x2 = g_sprPageRing[i].rect.y2 = -1;
            g_sprPageRing[i].link = &g_sprPageRing[i + 1];
        }
        g_sprPageRing[i - 1].link = g_sprPageRing;
    } else {
        g_sprPageRing[0].bm = r_vid.front;
        g_sprPageRing[0].pageIndex = 0;
        g_sprPageRing[0].rect.x1 = g_sprPageRing[0].rect.y1 = 0;
        g_sprPageRing[0].rect.x2 = g_sprPageRing[0].rect.y2 = -1;
        g_sprPageRing[0].link = g_sprPageRing;
    }
    g_mainCarSprite.page = g_sprPageRing;
    sp_clip_box(g_mainCarSprite.frame->pos->width,
        g_mainCarSprite.frame->pos->height, g_mainCarSprite.x, g_mainCarSprite.y,
        g_sprPageRing[0].bm->width, g_sprPageRing[0].bm->height, &g_sprPageRing[0].rect);
    g_mainCarSprite.seq = g_seqSpriteMoveData;
    return &g_mainCarSprite;
}

/* sp_rect_sub @ 0F38:A4B9 (linear 0x19839, 236 bytes)

   Subtract rectangle a from b along one axis into out; NULL when nothing is left */

struct Rect *sp_rect_sub(struct Rect *a, struct Rect *b, struct Rect *out, int horiz)
{
    *out = *b;
    if (horiz) {
        if (b->x1 <= a->x1 && b->x2 <= a->x2) {
            if (b->x2 >= a->x1) {
                if (b->x2 <= a->x2)
                    out->x2 = a->x1 - 1;
                else
                    out->x2 = b->x2;
            }
        } else {
            if (a->x2 >= b->x1)
                out->x1 = a->x2 + 1;
            if (b->x2 < a->x2)
                out->x2 = a->x2;
        }
    } else {
        if (b->y1 < a->y1) {
            if (b->y2 >= a->y1)
                out->y2 = a->y1 - 1;
        } else if (b->y1 > a->y1) {
            if (b->y1 <= a->y2)
                out->y1 = a->y2 + 1;
        }
    }
    if (out->x2 < out->x1 || out->y2 < out->y1) {
        out->x1 = out->y1 = 1000;
        out->x2 = out->y2 = -1;
        return 0;
    }
    return out;
}

/* sp_reverse @ 0F38:A5A5 (linear 0x19925, 184 bytes)

   Rotate the ring of four frames of f1 sprite and pick the one for the current heading */

void sp_reverse(struct Sprite *s)
{
    struct Frame *f1, *f2, *f3, *f4;
    int idx;

    s->x = g_xCoordSprite;
    f4 = (f3 = (f2 = (f1 = s->frame)->nxt[0])->nxt[0])->nxt[0];
    f1->nxt[0] = f4;
    f2->nxt[0] = f1;
    f3->nxt[0] = f2;
    f4->nxt[0] = f3;
    for (idx = 0; idx < 4; idx++)
        g_frmTable[idx].nxt[1] = g_frmTable[idx].nxt[0]->nxt[0];
    for (idx = 0; idx < 4; idx++)
        g_frmTable[idx].nxt[2] = g_frmTable[idx].nxt[1]->nxt[1];
    switch (g_xCoordSprite & 7) {
    case 0:
        break;
    case 6:
        s->frame = f4;
        return;
    case 4:
        s->frame = f3;
        return;
    case 2:
        s->frame = f2;
    }
}

/* sp_set_step @ 0F38:A65D (linear 0x199DD, 80 bytes)

   Store step i of a sprite's motion sequence and terminate the sequence after it */

void sp_set_step(struct Sprite *s, int i, int count, signed char dx, signed char dy)
{
    s->seq[i].count = count;
    s->seq[i].dx = dx;
    s->seq[i].dy = dy;
    s->seq[i + 1].count = -1;
    s->seq[i + 1].dx = 0;
    s->seq[i + 1].dy = 0;
}

/* sp_plan_move @ 0F38:A6AD (linear 0x19A2D, 321 bytes)

   Split a move of size d into the three phases of a sprite's motion sequence */

void sp_plan_move(struct Sprite *s, enum CarAnimKind mode, int a, int b, int c, int d, int e, int f)
{
    int n1, n2, n3;

    switch (mode) {
    case CAR_DRIVE_IN:
    case CAR_PULL_IN:
        n3 = -(e + f - 100) * d / 100 / c & 0xfffc;
        n2 = f * d / 100 / b & 0xfffe;
        n1 = (d - n2 * b - n3 * c) / a;
        break;
    case CAR_DRIVE_OUT:
    case CAR_BACK_OUT:
        n1 = abs(e * d / 100 / a) & 0xfffc;
        n2 = abs(f * d / 100 / b) & 0xfffe;
        n3 = abs((d - abs(a) * n1 - abs(b) * n2) / c);
        break;
    }
    sp_set_step(s, 0, n1 + 1, (char)a, 0);
    sp_set_step(s, 1, n2, (char)b, 0);
    sp_set_step(s, 2, n3, (char)c, 0);
    sp_set_step(s, 3, -1, 0, 0);
}

/* sp_plan_drive @ 0F38:A7EE (linear 0x19B6E, 144 bytes)

   Build the motion sequence of a sprite for a given direction */

void sp_plan_drive(struct Sprite *s, enum CarAnimKind mode, int d)
{
    switch (mode) {
    case CAR_DRIVE_IN:
    case CAR_PULL_IN:
        if (g_scene == SCENE_GARAGE)
            sp_plan_move(s, CAR_DRIVE_IN, 8, 4, 2, d, 0, 0x3c);
        else
            sp_plan_move(s, CAR_DRIVE_IN, 8, 4, 2, d, 0x19, 0x37);
        break;
    case CAR_BACK_OUT:
        sp_reverse(s);
        sp_plan_move(s, CAR_BACK_OUT, -2, -4, -4, d, 0x28, 0x3c);
        break;
    case CAR_DRIVE_OUT:
        sp_plan_move(s, CAR_DRIVE_OUT, 2, 4, 8, d, 0xa, 0x3c);
        break;
    }
}

/* sp_slide_v @ 0F38:A87E (linear 0x19BFE, 441 bytes)

   Slide a one-frame sprite of bitmap bm from y0 to y1 at column x, refreshing the front page under
   it */
/* Local names are placeholders chosen to reproduce the original stack layout. */

void sp_slide_v(int a, int b, int x, int y0, int y1, struct Bitmap far *bm)
{
    struct Sprite *sp;
    int n1;
    int dir;
    struct Bitmap far *save;
    struct Bitmap far *img2;
    int old;

    old = g_xCoordSprite;
    n1 = y1 - y0;
    dir = 1;
    if (n1 < 0) {
        n1 = -n1;
        dir = -1;
    }
    n1++;
    img2 = pu_grab(x, 0, bm->width, SCREEN_HEIGHT, 0);
    save = ar_bm(img2->width, img2->height, 0, ARENA_HIGH_STACK);
    bm_copy(img2, save, 1);
    sp = sp_setup(x, bm->height + y0 - 1, &bm, 0, 1, img2, save, 0, 0);
    sp_set_step(sp, 0, n1, 0, (char)dir);
    sp->page->bm = r_vid.back;
    while ((*g_videoOps.spriteStep)(sp)) {
        if (g_boltOverlay)
            (*g_videoOps.maskedBlit)(g_boltOverlay, r_vid.back, &g_cpyBoltPatchRecord, 1);
        if (r_vid.copyToFront)
            bm_copy_box(r_vid.back, r_vid.front, x, a, bm->width + x, b);
    }
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.front, r_vid.back, x, (y0 > y1 ? y1 : y0) < 0 ? 0 : (y0 > y1 ? y1 : y0), bm->width + x,
            (y0 < y1 ? y1 : y0) + bm->height > (SCREEN_HEIGHT - 1) ? (SCREEN_HEIGHT - 1) : (y0 < y1 ? y1 : y0) + bm->height);
    g_xCoordSprite = old;
    ma_pop_high(2);
}

/* sp_advance @ 0F38:AA37 (linear 0x19DB7, 139 bytes)

   Advance a sprite one tick along its motion sequence */

void sp_advance(struct Sprite *s)
{
    int n;

    g_xCoordSprite = s->x;
    g_spritePic = s->frame->pos;
    s->seq->count--;
    if (s->seq->count == 0)
        s->seq++;
    if (s->seq->count != -1) {
        s->x += s->seq->dx;
        s->y += s->seq->dy;
        n = s->seq->dx;
        if (n < 0)
            n = -n;
        if (n)
            s->frame = s->frame->nxt[n >> 2];
    }
}

/* sp_clip_span @ 0F38:AAC2 (linear 0x19E42, 142 bytes)

   Clip a span [pos, pos+len) against a sprite's frame, masked to r_vid.mask */

int sp_clip_span(int pos, int len, struct Sprite *s, struct Blt16 *o)
{
    int a, b, c, d;

    b = s->x & r_vid.mask;
    a = s->frame->pos->width;
    if (pos + len <= b || b + a < pos)
        return 0;
    c = (b < pos) ? pos : b;
    d = (b + a > pos + len) ? pos + len : b + a;
    d = (d + ~r_vid.mask) & r_vid.mask;
    o->sx = (c - b < 0) ? 0 : c - b;
    o->dx = c;
    o->w = d - c;
    return 1;
}
