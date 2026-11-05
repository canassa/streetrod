/* CGASHIFT.H - declarations of what CGASHIFT.C defines. */

#ifndef CGASHIFT_H
#define CGASHIFT_H

struct Bitmap;
struct Blt;

void cx_blit(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt);

#endif
