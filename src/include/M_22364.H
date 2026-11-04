/* M_22364.H - declarations of what M_22364.C defines. */

#ifndef M_22364_H
#define M_22364_H
#include "STRUCTS.H"

extern void (far *g_5b8a[15])();                        /* DS:5B8A */
extern int g_5bc6[];                                    /* DS:5BC6 */
void F_22364(struct Bitmap far *bm, int x, int y, int glyph);/* 0x22364 */
void F_2244F(struct Bitmap far *bm, int from, int to, int x1, int y1, int x2, int y2);/* 0x2244F */
void F_22609(char far *src, char far *dst);             /* 0x22609 */
void F_22626(char far *a, char far *b);                 /* 0x22626 */

#endif
