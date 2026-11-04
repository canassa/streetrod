/* EGASHIFT.H - declarations of what EGASHIFT.C defines. */

#ifndef EGASHIFT_H
#define EGASHIFT_H
#include "STRUCTS.H"

extern unsigned char g_leftBitMasks[];                  /* DS:5C16 */
extern int g_egaGcFunction;                             /* DS:5C1E */
void ex_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r);/* 0x23C3B */

#endif
