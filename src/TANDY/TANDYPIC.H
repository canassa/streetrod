/* TANDYPIC.H - declarations of what TANDYPIC.ASM defines. */

#ifndef TANDYPIC_H
#define TANDYPIC_H
#include "STRUCTS.H"

void tp_unpack(char far *src, char far *dst, unsigned n, unsigned char *pal);/* 0x2467A */
void tp_unpack_mask(char far *src, char far *dst, unsigned n, unsigned char *pal);/* 0x24740 */
void tp_make_mask(char far *bits, char far *alt, unsigned n);/* 0x24849 */

#endif
