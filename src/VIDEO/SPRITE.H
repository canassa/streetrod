/* SPRITE.H - declarations of what SPRITE.C defines. */

#ifndef SPRITE_H
#define SPRITE_H
#include "STRUCTS.H"
#include "CARPIC.H"

extern struct Seq g_seqSpriteMoveData[];
extern struct Frame g_frmTable[];
extern struct Sprite g_mainCarSprite;
extern struct Page g_sprPageRing[];
struct Rect *sp_rect_and(struct Rect *a, struct Rect *b, struct Rect *out);
struct Rect *sp_car_clip(int x, int y, int w, int h, struct Rect *out);
void sp_clip_box(int a, int b, int c, int d, int amax, int bmax, struct Rect *out);
struct Sprite *sp_setup(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img, struct Bitmap far *save, struct Rect *a8, struct Rect *a9);
struct Rect *sp_rect_sub(struct Rect *a, struct Rect *b, struct Rect *out, int horiz);
void sp_reverse(struct Sprite *s);
void sp_set_step(struct Sprite *s, int i, int count, signed char dx, signed char dy);
void sp_plan_move(struct Sprite *s, enum CarAnimKind mode, int a, int b, int c, int d, int e, int f);
void sp_plan_drive(struct Sprite *s, enum CarAnimKind mode, int d);
void sp_slide_v(int a, int b, int x, int y0, int y1, struct Bitmap far *bm);
void sp_advance(struct Sprite *s);
int sp_clip_span(int pos, int len, struct Sprite *s, struct Blt16 *o);

#endif
