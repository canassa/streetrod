/* EGA.H - declarations of what EGA.C defines. */

#ifndef EGA_H
#define EGA_H
#include "STRUCTS.H"

extern void (far *g_5bd0[15])();                        /* DS:5BD0 */
void F_22B3A(struct Bitmap far *bm, int x, int y, char *str);/* 0x22B3A */

#endif
