/**
 * @file TANDYPIC.H
 * @brief Picture expansion and transparency masks for Tandy pixels.
 */

#ifndef TANDYPIC_H
#define TANDYPIC_H

void tp_unpack(char far *src, char far *dst, unsigned n, unsigned char *pal);
void tp_unpack_mask(char far *src, char far *dst, unsigned n, unsigned char *pal);
void tp_make_mask(char far *bits, char far *alt, unsigned n);

#endif
