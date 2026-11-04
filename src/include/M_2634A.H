/* M_2634A.H - declarations of what M_2634A.ASM defines. */

#ifndef M_2634A_H
#define M_2634A_H
#include "STRUCTS.H"

void F_2634A(char far *dst, unsigned seg, int off, int off2, int pitch1, int w, int h, int pitch2);/* 0x2634A */
void F_26401(char far *bits, char far *alt, unsigned n);/* 0x26401 */

#endif
