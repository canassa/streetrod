/* M_1C2CA.H - declarations of what M_1C2CA.C defines. */

#ifndef M_1C2CA_H
#define M_1C2CA_H
#include "STRUCTS.H"

extern void (far *g_5b04[15])();                        /* DS:5B04 */
extern unsigned char g_5b40[4];                         /* DS:5B40 */
extern unsigned char g_5b44[4];                         /* DS:5B44 */
extern int g_5b48[5];                                   /* DS:5B48 */
void F_1C2B8(void);                                     /* 0x1C2B8 */
void F_1C2CA(struct Bitmap far *bm, int x, int y, char *s);/* 0x1C2CA */
void F_1C803(struct Bitmap far *bm, int a, int b, int x1, int y1, int x2, int y2);/* 0x1C803 */
void F_1CEE7(char far *src, char far *dst);             /* 0x1CEE7 */
void F_1CF40(char far *dst, char far *src);             /* 0x1CF40 */
void F_1D05B(int page);                                 /* 0x1D05B */
void far F_1D080(void);                                 /* 0x1D080 */
void far F_1D0A4(void);                                 /* 0x1D0A4 */

#endif
