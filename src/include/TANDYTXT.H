/* TANDYTXT.H - declarations of what TANDYTXT.ASM defines. */

#ifndef TANDYTXT_H
#define TANDYTXT_H
#include "STRUCTS.H"

int F_24BE2(int x, int glyph, char *buf, int c1, int c2);/* 0x24BE2 */
void F_24CAB(char *buf, unsigned seg, unsigned off, int y, int h, int m, int ph, int n);/* 0x24CAB */

#endif
