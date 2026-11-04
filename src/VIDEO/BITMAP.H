/* BITMAP.H - declarations of what BITMAP.C defines. */

#ifndef BITMAP_H
#define BITMAP_H
#include "STRUCTS.H"

void bm_copy(struct Bitmap far *src, struct Bitmap far *dst, int e);/* 0x19ED0 */
void bm_opaque_span(struct Bitmap far *bm, int *left, int *right);/* 0x19F42 */
struct Bitmap far * bm_shift_right(struct Bitmap far *src, int n, struct Bitmap far *dst);/* 0x1A03F */
void bm_draw_masked(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flag);/* 0x1A2D0 */
void bm_underlay(struct Bitmap far *src, struct Bitmap far *dst, int x, int y);/* 0x1A472 */
void bm_put(struct Bitmap far *dst, struct Bitmap far *bm, int x, int y, int mode);/* 0x1A69F */
void bm_put_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y);/* 0x1A6E0 */
void bm_draw_pic(struct Bitmap far *dst, int n, unsigned char x, unsigned char y, int mode);/* 0x1A734 */
void bm_and_mask(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *in);/* 0x1A79B */
void bm_far_copy(char far *src, char far *dst, unsigned n);/* 0x1A882 */
struct Bitmap far *bm_grab(struct Bitmap far *bm, struct Blt *r, int mode);/* 0x1A89E */
void bm_copy_box(struct Bitmap far *dst, struct Bitmap far *src, int x1, int y1, int x2, int y2);/* 0x1A92A */

#endif
