/**
 * @file EGARECOL.H
 * @brief Replacement of selected colors in EGA rectangles.
 */

#ifndef EGARECOL_H
#define EGARECOL_H

struct Bitmap;

void er_rect(struct Bitmap far *bm, int mode, char color, int x1, int y1, int x2, int y2);

#endif
