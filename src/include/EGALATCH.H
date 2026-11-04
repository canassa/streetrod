/* EGALATCH.H - declarations of what EGALATCH.ASM defines. */

#ifndef EGALATCH_H
#define EGALATCH_H
#include "STRUCTS.H"

void F_24E78(char far *src, char far *dst, int w, int h, int x, int y, int pitch);/* 0x24E78 */
void F_24EFB(char far *src, int w, int h, char far *dst, int pitch);/* 0x24EFB */

#endif
