/* SPRITE.H - declarations of what SPRITE.C defines. */

#ifndef SPRITE_H
#define SPRITE_H
#include "STRUCTS.H"
#include "CARPIC.H"

extern struct Seq g_seqSpriteMoveData[];                             /* DS:764E */
extern struct Frame g_frmTable[];                           /* DS:77BA */
extern struct Sprite g_mainCarSprite;                            /* DS:7D18 */
extern struct Page g_sprPageRing[];                     /* DS:7D42 */
struct Rect *sp_rect_and(struct Rect *a, struct Rect *b, struct Rect *out);/* 0x1947A */
struct Rect *sp_car_clip(int x, int y, int w, int h, struct Rect *out);/* 0x19518 */
void sp_clip_box(int a, int b, int c, int d, int amax, int bmax, struct Rect *out);/* 0x1956E */
struct Sprite *sp_setup(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img, struct Bitmap far *save, struct Rect *a8, struct Rect *a9);/* 0x195F3 */
struct Rect *sp_rect_sub(struct Rect *a, struct Rect *b, struct Rect *out, int horiz);/* 0x19839 */
void sp_reverse(struct Sprite *s);                  /* 0x19925 */
void sp_set_step(struct Sprite *s, int i, int count, signed char dx, signed char dy);/* 0x199DD */
void sp_plan_move(struct Sprite *s, enum CarAnimKind mode, int a, int b, int c, int d, int e, int f);/* 0x19A2D */
void sp_plan_drive(struct Sprite *s, enum CarAnimKind mode, int d);/* 0x19B6E */
void sp_slide_v(int a, int b, int x, int y0, int y1, struct Bitmap far *bm);/* 0x19BFE */
void sp_advance(struct Sprite *s);                  /* 0x19DB7 */
int sp_clip_span(int pos, int len, struct Sprite *s, struct Blt16 *o);/* 0x19E42 */

#endif
