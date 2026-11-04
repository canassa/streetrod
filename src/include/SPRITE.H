/* SPRITE.H - declarations of what SPRITE.C defines. */

#ifndef SPRITE_H
#define SPRITE_H
#include "STRUCTS.H"

extern struct Seq g_764e[];                             /* DS:764E */
extern struct Frame g77bal[];                           /* DS:77BA */
extern struct Sprite g7d18m;                            /* DS:7D18 */
extern struct Page g_sprPageRing[];                     /* DS:7D42 */
struct Rect *sprite_rect_and(struct Rect *a, struct Rect *b, struct Rect *out);/* 0x1947A */
struct Rect *sprite_car_clip(int x, int y, int w, int h, struct Rect *out);/* 0x19518 */
void sprite_clip_box(int a, int b, int c, int d, int amax, int bmax, struct Rect *out);/* 0x1956E */
struct Sprite *sprite_setup(int x, int y, struct Bitmap far **imgs, char (*cell)[8], int n, struct Bitmap far *img, struct Bitmap far *save, struct Rect *a8, struct Rect *a9);/* 0x195F3 */
struct Rect *sprite_rect_sub(struct Rect *a, struct Rect *b, struct Rect *out, int horiz);/* 0x19839 */
void sprite_reverse(struct Sprite *s);                  /* 0x19925 */
void sprite_set_step(struct Sprite *s, int i, int count, signed char dx, signed char dy);/* 0x199DD */
void sprite_plan_move(struct Sprite *s, int mode, int a, int b, int c, int d, int e, int f);/* 0x19A2D */
void sprite_plan_drive(struct Sprite *s, int mode, int d);/* 0x19B6E */
void sprite_slide_v(int a, int b, int x, int y0, int y1, struct Bitmap far *bm);/* 0x19BFE */
void sprite_advance(struct Sprite *s);                  /* 0x19DB7 */
int sprite_clip_span(int pos, int len, struct Sprite *s, struct Blt16 *o);/* 0x19E42 */

#endif
