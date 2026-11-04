/* CGABLT.H - declarations of what CGABLT.C defines. */

#ifndef CGABLT_H
#define CGABLT_H
#include "STRUCTS.H"

void cb_compose(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct Blt *r);/* 0x1D195 */
void cb_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag);/* 0x1D262 */
void cb_copy(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flags);/* 0x1D3C8 */

#endif
