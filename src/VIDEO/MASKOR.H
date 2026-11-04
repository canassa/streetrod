/* MASKOR.H - declarations of what MASKOR.ASM defines. */

#ifndef MASKOR_H
#define MASKOR_H
#include "STRUCTS.H"

void mr_blit(char far *dst, unsigned sseg, unsigned soff, unsigned moff, int a, int w, int h, int c);

#endif
