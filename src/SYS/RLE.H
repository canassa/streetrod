/**
 * @file RLE.H
 * @brief Run-length picture expansion through a palette.
 */

#ifndef RLE_H
#define RLE_H

void rl_decode(char far *src, char far *dst, int len, unsigned char *pal, int n);

#endif
