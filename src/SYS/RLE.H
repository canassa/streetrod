/* RLE.H - declarations of what RLE.ASM defines. */

#ifndef RLE_H
#define RLE_H
#include "STRUCTS.H"

void rle_decode(char far *src, char far *dst, int len, unsigned char *pal, int n);/* 0x0E926 */

#endif
