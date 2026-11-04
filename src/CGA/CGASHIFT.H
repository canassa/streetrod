/* CGASHIFT.H - declarations of what CGASHIFT.C defines. */

#ifndef CGASHIFT_H
#define CGASHIFT_H
#include "STRUCTS.H"

extern unsigned char g_cgaLeadMask[4];
void cx_blit(struct Bitmap far *src, struct Bitmap far *dst, struct Blt *blt);

#endif
