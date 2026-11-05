/* EGA.H - declarations of what EGA.C defines. */

#ifndef EGA_H
#define EGA_H

#include "DASH.H"
struct Bitmap;

void eg_install(void);
void eg_draw_text(struct Bitmap far *bm, int x, int y, char *str);

#endif
