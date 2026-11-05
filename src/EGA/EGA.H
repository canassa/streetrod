/**
 * @file EGA.H
 * @brief EGA drawing hooks and text rendering.
 */

#ifndef EGA_H
#define EGA_H

#include "DASH.H"
struct Bitmap;

void eg_install(void);
void eg_draw_text(struct Bitmap far *bm, int x, int y, char *str);

#endif
