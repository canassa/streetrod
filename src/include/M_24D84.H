/* M_24D84.H - declarations of what M_24D84.ASM defines. */

#ifndef M_24D84_H
#define M_24D84_H
#include "STRUCTS.H"

void F_24D84(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);/* 0x24D84 */
void F_24DC7(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);/* 0x24DC7 */

#endif
