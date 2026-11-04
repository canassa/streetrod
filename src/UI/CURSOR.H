/* CURSOR.H - declarations of what CURSOR.C defines. */

#ifndef CURSOR_H
#define CURSOR_H
#include "STRUCTS.H"

#define CURSOR_SHOW (-1)
#define CURSOR_SHOW_NOW (-2)
#define CURSOR_HIDE (-3)
#define CURSOR_HIDE_NOW (-4)
#define CURSOR_EGA_SAVE (EGA_VRAM + EGA_PITCH * SCREEN_HEIGHT)

extern struct Pair g_cursorPairs[];                            /* DS:06EC */
extern struct Cursor g_cursor2;                         /* DS:7AEC */
unsigned cu_shape_off(unsigned n);                  /* 0x11E9C */
void cu_init(void);                                 /* 0x11EA8 */
void cu_ctl(int a);                                 /* 0x120FE */
void cu_draw(int x, int y);                         /* 0x1220B */

#endif
