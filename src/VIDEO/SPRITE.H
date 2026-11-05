/**
 * @file SPRITE.H
 * @brief Moving-image frames, motion plans and display pages.
 */

#ifndef SPRITE_H
#define SPRITE_H

#include "BITMAP.H"
#include "CARPIC.H"


struct Bitmap;

/** Countdown marking the end of a sprite's motion plan. */
#define MOTION_END (-1)

/** @brief Mutable countdown and per-update displacement for one motion phase. */
struct Seq {
    /** Remaining countdown, decremented before movement; @ref MOTION_END ends. */
    int count;
    signed char dx; /**< Signed horizontal displacement in pixels. */
    signed char dy; /**< Signed vertical displacement in rows. */
};

/** @brief Borrowed sprite image, optional bottom band and frame-ring links. */
struct Frame {
    struct Bitmap far *image; /**< Image drawn for this frame. */
    /**
     * Optional near band view: @ref EgaSpriteBand on EGA, @ref Bitmap otherwise.
     * The display mode determines the interpretation.
     */
    struct Bitmap *bottomBand;
    struct Frame *next[3]; /**< Ring links advancing one, two or four frames. */
};

/**
 * @brief A moving image with its motion cursor and redraw resources.
 *
 * Bitmaps and foreground rectangles are borrowed for the animation.
 */
struct Sprite {
    struct Frame *frame; /**< Current frame in the animation ring. */
    /** EGA erase source or CGA composition canvas; unused on Tandy. */
    struct Bitmap far *workBitmap;
    /** Background pixels and optional foreground mask for composition. */
    struct Bitmap far *background;
    /** Optional foreground areas restored over the frame. */
    struct Rect *overlayRects[2];
    int x;                  /**< Screen X in pixels before byte alignment. */
    int y;                  /**< Top screen row in pixels. */
    struct Seq *motionStep; /**< Current mutable motion step. */
    struct Page *page;      /**< Destination page and previous clipped bounds. */
};

/* one display page of the sprite engine (16 bytes): its bitmap, the clipped rectangle, the link to the next page */
struct Page {
    struct Bitmap far *bm;
    struct Rect rect;
    int pageIndex;
    struct Page *link;
};


extern struct Seq g_seqSpriteMoveData[];
extern struct Frame g_frmTable[];
extern struct Sprite g_mainCarSprite;
extern struct Page g_sprPageRing[];
struct Rect *sp_rect_and(struct Rect *a, struct Rect *b, struct Rect *out);
struct Rect *sp_car_clip(int x, int y, int w, int h, struct Rect *out);
void sp_clip_box(int a, int b, int c, int d, int amax, int bmax, struct Rect *out);
struct Sprite *sp_setup(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img,
                        struct Bitmap far *save, struct Rect *a8, struct Rect *a9);
struct Rect *sp_rect_sub(struct Rect *a, struct Rect *b, struct Rect *out, int horiz);
void sp_reverse(struct Sprite *s);
void sp_set_step(struct Sprite *s, int i, int count, signed char dx, signed char dy);
void sp_plan_move(struct Sprite *s, enum CarAnimKind mode, int a, int b, int c, int d, int e, int f);
void sp_plan_drive(struct Sprite *s, enum CarAnimKind mode, int d);
void sp_slide_v(int a, int b, int x, int y0, int y1, struct Bitmap far *bm);
void sp_advance(struct Sprite *s);
int sp_clip_span(int pos, int len, struct Sprite *s, struct Blt16 *o);

#endif
