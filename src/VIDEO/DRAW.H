/* DRAW.H - declarations of what DRAW.C defines. */

#ifndef DRAW_H
#define DRAW_H
#include "STRUCTS.H"

void dr_fill_rect(struct Bitmap far *s, int x, int y, int w, int h);/* 0x13682 */
void dr_vline(struct Bitmap far *s, int x, int y, int h);/* 0x1371C */
void dr_hline(struct Bitmap far *s, int x, int y, int w);/* 0x1377F */
void dr_bevel(struct Bitmap far *s, int x, int y, int w, int h);/* 0x137B2 */
void dr_frame(struct Bitmap far *bm, int x, int y, int w, int h);/* 0x13957 */

#endif
