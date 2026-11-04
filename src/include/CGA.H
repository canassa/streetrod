/* CGA.H - declarations of what CGA.C defines. */

#ifndef CGA_H
#define CGA_H
#include "STRUCTS.H"

extern void (far *g_5b04[15])();                        /* DS:5B04 */
extern unsigned char g_5b40[4];                         /* DS:5B40 */
extern unsigned char g_5b44[4];                         /* DS:5B44 */
extern int g_5b48[5];                                   /* DS:5B48 */
void cga_draw_text(struct Bitmap far *bm, int x, int y, char *s);/* 0x1C2CA */
void cga_recolor_rect(struct Bitmap far *bm, int a, int b, int x1, int y1, int x2, int y2);/* 0x1C803 */
void cga_copy_page(char far *src, char far *dst);       /* 0x1CEE7 */
void cga_reveal(char far *dst, char far *src);          /* 0x1CF40 */
void cga_set_page(int page);                            /* 0x1D05B */
void far cga_screen_off(void);                          /* 0x1D080 */
void far cga_screen_on(void);                           /* 0x1D0A4 */

#endif
