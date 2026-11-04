/* TANDY.H - declarations of what TANDY.C defines. */

#ifndef TANDY_H
#define TANDY_H
#include "STRUCTS.H"

extern struct Hooks g_tandyHooks;
extern int g_tandyRevealOrder[];
void ta_install(void);
void ta_draw_text(struct Bitmap far *bm, int x, int y, int glyph);
void ta_recolor_rect(struct Bitmap far *bm, int from, int to, int x1, int y1, int x2, int y2);
void ta_copy_page(char far *src, char far *dst);
void ta_reveal(char far *a, char far *b);

#endif
