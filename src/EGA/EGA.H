/* EGA.H - declarations of what EGA.C defines. */

#ifndef EGA_H
#define EGA_H
#include "STRUCTS.H"

extern struct Hooks g_egaHooks;
void eg_install(void);
void eg_draw_text(struct Bitmap far *bm, int x, int y, char *str);

#endif
