/* EGA.H - declarations of what EGA.C defines. */

#ifndef EGA_H
#define EGA_H
#include "STRUCTS.H"

extern void (far *g_egaHooks[15])();                    /* DS:5BD0 */
void ega_install(void);                                 /* 0x22B28 */
void ega_draw_text(struct Bitmap far *bm, int x, int y, char *str);/* 0x22B3A */

#endif
