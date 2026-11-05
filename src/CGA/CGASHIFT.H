/**
 * @file CGASHIFT.H
 * @brief CGA blits with horizontal pixel alignment.
 */

#ifndef CGASHIFT_H
#define CGASHIFT_H

struct Bitmap;
struct Blt;

void cx_blit(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt);

#endif
