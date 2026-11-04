/* CGALINE.H - declarations of what CGALINE.ASM defines. */

#ifndef CGALINE_H
#define CGALINE_H
#include "STRUCTS.H"

extern char g_5b56[4];                                  /* DS:5B56 */
extern char g_5b5a[16];                                 /* DS:5B5A */
extern char g_5b6a[16];                                 /* DS:5B6A */
extern char g_5b7a[16];                                 /* DS:5B7A */
void F_1DD22(int x1, int y1, int x2, int y2, int color);/* 0x1DD22 */
void F_1DFD2(int x1, int x2, int y, int color);         /* 0x1DFD2 */
void F_1E0FE(int y1, int color);                        /* 0x1E0FE */

#endif
