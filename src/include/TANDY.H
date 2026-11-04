/* TANDY.H - declarations of what TANDY.C defines. */

#ifndef TANDY_H
#define TANDY_H
#include "STRUCTS.H"

extern void (far *g_tandyHooks[15])();                  /* DS:5B8A */
extern int g_tandyRevealOrder[];                        /* DS:5BC6 */
void tandydrv_install(void);                            /* 0x22352 */
void tandy_draw_text(struct Bitmap far *bm, int x, int y, int glyph);/* 0x22364 */
void tandy_recolor_rect(struct Bitmap far *bm, int from, int to, int x1, int y1, int x2, int y2);/* 0x2244F */
void tandy_copy_page(char far *src, char far *dst);     /* 0x22609 */
void tandy_reveal(char far *a, char far *b);            /* 0x22626 */

#endif
