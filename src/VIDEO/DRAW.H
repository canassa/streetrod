/**
 * @file DRAW.H
 * @brief Interface rectangle fills, lines and borders.
 */

#ifndef DRAW_H
#define DRAW_H

struct Bitmap;

void dr_fill_rect(struct Bitmap far *s, int x, int y, int w, int h);
void dr_vline(struct Bitmap far *s, int x, int y, int h);
void dr_hline(struct Bitmap far *s, int x, int y, int w);
void dr_bevel(struct Bitmap far *s, int x, int y, int w, int h);
void dr_frame(struct Bitmap far *bm, int x, int y, int w, int h);

#endif
