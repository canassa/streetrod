/* RLE.H - declarations of what RLE.ASM defines. */

#ifndef RLE_H
#define RLE_H

void rl_decode(char far *src, char far *dst, int len, unsigned char *pal, int n);

#endif
