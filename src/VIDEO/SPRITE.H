/* SPRITE.H - declarations of what SPRITE.C defines. */

#ifndef SPRITE_H
#define SPRITE_H

#include "CARPIC.H"

struct Bitmap;

struct Seq {
    int count;
    signed char dx;
    signed char dy;
};

/* one animation frame: 12 bytes, the ring links nxt[0..2] (sp_setup builds g_frmTable[]) */
struct Frame {
    struct Bitmap far *pos;
    struct Bitmap *data;
    struct Frame *nxt[3];
};

struct Sprite {
    struct Frame *frame;
    struct Bitmap far *img;
    struct Bitmap far *save;
    struct Rect *rc[2];
    int x;
    int y;
    struct Seq *seq;
    struct Page *page;
};

/* one display page of the sprite engine (16 bytes): its bitmap, the clipped rectangle, the link to the next page */
struct Page {
    struct Bitmap far *bm;
    struct Rect rect;
    int pageIndex;
    struct Page *link;
};

/* 16-byte blit packet of da_draw_shifter (struct Blt without the last two words) */
struct Blt16 {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int fc;
    int fe;
};
#include "CARPIC.H"

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
