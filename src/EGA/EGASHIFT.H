/* EGASHIFT.H - declarations of what EGASHIFT.C defines. */

#ifndef EGASHIFT_H
#define EGASHIFT_H
#include "STRUCTS.H"

extern unsigned char g_leftBitMasks[];
extern int g_egaGcFunction;
void ex_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r);

#endif
