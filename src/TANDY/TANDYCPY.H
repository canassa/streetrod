/**
 * @file TANDYCPY.H
 * @brief Rectangle transfers to Tandy interleaved video memory.
 */

#ifndef TANDYCPY_H
#define TANDYCPY_H

void ty_to_screen(char far *src, char far *dst, int width, int rows, int src_skip, int dst_skip, int row);
void ty_screen(char far *src, char far *dst, int width, int rows, int sx, int sy, int dx, int dy);

#endif
