/* BITMAP.H - declarations of what BITMAP.C defines. */

#ifndef BITMAP_H
#define BITMAP_H
#include "STRUCTS.H"

void bm_copy(struct Bitmap far *src, struct Bitmap far *dst, int e);
void bm_opaque_span(struct Bitmap far *bm, int *left, int *right);
struct Bitmap far * bm_shift_right(struct Bitmap far *src, int n, struct Bitmap far *dst);
void bm_draw_masked(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag);
void bm_underlay(struct Bitmap far *src, struct Bitmap far *dst, int x, int y);
void bm_put(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode);
void bm_put_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y);
void bm_draw_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode);
void bm_and_mask(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in);
void bm_far_copy(char far *src, char far *dst, unsigned n);
struct Bitmap far *bm_grab(struct Bitmap far *bm, struct Blt *r, int mode);
void bm_copy_box(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2);

#endif
