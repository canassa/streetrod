/* CGABLT.H - declarations of what CGABLT.C defines. */

#ifndef CGABLT_H
#define CGABLT_H

struct Bitmap;
struct Blt;

void cb_compose(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct Blt *r);
void cb_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag);
void cb_copy(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flags);

#endif
