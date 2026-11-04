/* TANDYBLT.H - declarations of what TANDYBLT.C defines. */

#ifndef TANDYBLT_H
#define TANDYBLT_H
#include "STRUCTS.H"

void tandyblt_compose(struct Bitmap far *dst, struct Bitmap far *src, struct Bitmap far *mask, struct Blt *in);/* 0x21C0B */
void tandyblt_masked(struct Bitmap far *a, struct Bitmap far *b, struct Blt *in, int flag);/* 0x21D58 */
void tandyblt_copy(struct Bitmap far *a, struct Bitmap far *b, struct Blt *r, int flags);/* 0x21EAB */
void far tandyblt_shifted(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt);/* 0x221AC */

#endif
