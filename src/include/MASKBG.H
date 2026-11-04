/* MASKBG.H - declarations of what MASKBG.ASM defines. */

#ifndef MASKBG_H
#define MASKBG_H
#include "STRUCTS.H"

void maskbg_blit(char far *dst, unsigned seg, unsigned off, unsigned off2, int q, int pitch1, int w, int h, int pitch3, int pitch2);/* 0x24F4A */

#endif
