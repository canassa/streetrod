/* EGABLIT.H - declarations of what EGABLIT.C defines. */

#ifndef EGABLIT_H
#define EGABLIT_H
#include "STRUCTS.H"

void egablit_copy(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *r, int flags);/* 0x23262 */
void egablit_sprite(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Bitmap far *bm3, struct Mask *mask, struct Blt *r);/* 0x234E9 */
void egablit_masked(struct Bitmap far *bm1, struct Bitmap far *bm2, struct Blt *r, int flag);/* 0x236D5 */
void egablit_latch_pack(char far *src, int w, int h, char far *dst, int pitch);/* 0x2382D */

#endif
