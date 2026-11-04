/* CGADRV.H - declarations of what CGADRV.ASM defines. */

#ifndef CGADRV_H
#define CGADRV_H
#include "STRUCTS.H"

void cd_blit_rect(char far *src, char far *dst, int width, int rows, int srcSkip, int dstSkip);
void cd_blit_to_ilv(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);
void cd_blit_ilv(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);

#endif
