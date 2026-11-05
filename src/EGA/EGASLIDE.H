/**
 * @file EGASLIDE.H
 * @brief Vertical car sprite slides on EGA displays.
 */

#ifndef EGASLIDE_H
#define EGASLIDE_H

struct Bitmap;

void far ev_sprite(int top, int bottom, int x, int y0, int y1, struct Bitmap far *bm);

#endif
