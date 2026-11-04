/* M_1C11C.H - declarations of what M_1C11C.ASM defines. */

#ifndef M_1C11C_H
#define M_1C11C_H
#include "STRUCTS.H"

void blit_rect(char far *src, char far *dst, int width, int rows, int srcSkip, int dstSkip);/* 0x1C11C */
void F_1C14C(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);/* 0x1C14C */
void F_1C1D4(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);/* 0x1C1D4 */

#endif
