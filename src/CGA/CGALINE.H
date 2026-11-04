/* CGALINE.H - declarations of what CGALINE.ASM defines. */

#ifndef CGALINE_H
#define CGALINE_H
#include "STRUCTS.H"

extern char g_cgaColorByte[4];                          /* DS:5B56 */
extern char g_cgaColorMap[16];                          /* DS:5B5A */
extern char g_cgaDitherOdd[16];                         /* DS:5B6A */
extern char g_cgaDitherEven[16];                        /* DS:5B7A */
void cl_draw(int x1, int y1, int x2, int y2, int color);/* 0x1DD22 */
void cl_span(int x1, int x2, int y, int color);    /* 0x1DFD2 */
void cl_fill_below(int y1, int color);             /* 0x1E0FE */

#endif
