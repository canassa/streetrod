/* TANDYBLT.H - declarations of what TANDYBLT.C defines. */

#ifndef TANDYBLT_H
#define TANDYBLT_H
#include "STRUCTS.H"

void tb_compose(struct Bitmap far *dst, struct Bitmap far *src, struct Bitmap far *mask, struct Blt *in);
void tb_masked(struct Bitmap far *a, struct Bitmap far *b, struct Blt *in, int flag);
void tb_copy(struct Bitmap far *a, struct Bitmap far *b, struct Blt *r, int flags);
void far tb_shifted(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt);

#endif
