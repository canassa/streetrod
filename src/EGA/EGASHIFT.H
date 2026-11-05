/**
 * @file EGASHIFT.H
 * @brief EGA blits with horizontal pixel alignment.
 */

#ifndef EGASHIFT_H
#define EGASHIFT_H

struct Bitmap;
struct Blt;

extern int g_egaGcFunction;
void ex_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r);

#endif
