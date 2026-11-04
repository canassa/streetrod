/* EGALATCH.H - declarations of what EGALATCH.ASM defines. */

#ifndef EGALATCH_H
#define EGALATCH_H
#include "STRUCTS.H"

void ea_rect(char far *src, char far *dst, int w, int h, int x, int y, int pitch);/* 0x24E78 */
void ea_to_page(char far *src, int w, int h, char far *dst, int pitch);/* 0x24EFB */

#endif
