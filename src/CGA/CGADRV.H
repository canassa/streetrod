/* CGADRV.H - declarations of what CGADRV.ASM defines. */

#ifndef CGADRV_H
#define CGADRV_H
#include "STRUCTS.H"

void cgadrv_blit_rect(char far *src, char far *dst, int width, int rows, int srcSkip, int dstSkip);/* 0x1C11C */
void cgadrv_blit_to_ilv(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);/* 0x1C14C */
void cgadrv_blit_ilv(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);/* 0x1C1D4 */

#endif
