/* EGASHIFT.H - declarations of what EGASHIFT.C defines. */

#ifndef EGASHIFT_H
#define EGASHIFT_H
#include "STRUCTS.H"

extern unsigned char g_5c16[];                          /* DS:5C16 */
extern int g_5c1e;                                      /* DS:5C1E */
void egashift_blit(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r);/* 0x23C3B */

#endif
