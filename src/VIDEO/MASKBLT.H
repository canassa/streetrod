/* MASKBLT.H - declarations of what MASKBLT.ASM defines. */

#ifndef MASKBLT_H
#define MASKBLT_H
#include "STRUCTS.H"

void mk_copy(char far *dst, unsigned seg, int off, int off2, int pitch1, int w, int h, int pitch2);/* 0x2634A */
void mk_make_mask(char far *bits, char far *alt, unsigned n);/* 0x26401 */

#endif
