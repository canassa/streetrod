/* MASKBLT.H - declarations of what MASKBLT.ASM defines. */

#ifndef MASKBLT_H
#define MASKBLT_H

void mk_copy(char far *dst, unsigned seg, int off, int off2, int pitch1, int w, int h, int pitch2);
void mk_make_mask(char far *bits, char far *alt, unsigned n);

#endif
