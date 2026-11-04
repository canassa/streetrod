/* TANDYCPY.H - declarations of what TANDYCPY.ASM defines. */

#ifndef TANDYCPY_H
#define TANDYCPY_H
#include "STRUCTS.H"

void ty_to_screen(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);
void ty_screen(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);

#endif
