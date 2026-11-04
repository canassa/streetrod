/* CGA.H - declarations of what CGA.C defines. */

#ifndef CGA_H
#define CGA_H
#include "STRUCTS.H"

extern struct Hooks g_cgaHooks;
extern unsigned char g_cgaPixMask[4];
extern unsigned char g_cgaPixKeep[4];
extern int g_cgaRevealOrder[5];
void cg_install(void);
void cg_draw_text(struct Bitmap far *bm, int x, int y, char *s);
void cg_recolor_rect(struct Bitmap far *bm, int a, int b, int x1, int y1, int x2, int y2);
void cg_copy_page(char far *src, char far *dst);
void cg_reveal(char far *dst, char far *src);
void cg_set_page(int page);
void far cg_screen_off(void);
void far cg_screen_on(void);

#endif
