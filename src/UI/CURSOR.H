/* CURSOR.H - declarations of what CURSOR.C defines. */

#ifndef CURSOR_H
#define CURSOR_H
#include "STRUCTS.H"

#define CURSOR_SHOW (-1)
#define CURSOR_SHOW_NOW (-2)
#define CURSOR_HIDE (-3)
#define CURSOR_HIDE_NOW (-4)
#define CURSOR_EGA_SAVE (EGA_VRAM + EGA_PITCH * SCREEN_HEIGHT)

extern struct Pair g_cursorPairs[];
extern struct Cursor g_cursor2;
unsigned cu_shape_off(unsigned n);
void cu_init(void);
void cu_ctl(int a);
void cu_draw(int x, int y);

#endif
