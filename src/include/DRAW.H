/* DRAW.H - declarations of what DRAW.C defines. */

#ifndef DRAW_H
#define DRAW_H
#include "STRUCTS.H"

void draw_fill_rect(struct Bitmap far *s, int x, int y, int w, int h);/* 0x13682 */
void draw_vline(struct Bitmap far *s, int x, int y, int h);/* 0x1371C */
void draw_hline(struct Bitmap far *s, int x, int y, int w);/* 0x1377F */
void draw_bevel(struct Bitmap far *s, int x, int y, int w, int h);/* 0x137B2 */
void draw_frame(struct Bitmap far *bm, int x, int y, int w, int h);/* 0x13957 */

#endif
