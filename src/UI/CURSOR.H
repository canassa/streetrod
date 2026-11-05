/* CURSOR.H - declarations of what CURSOR.C defines. */

#ifndef CURSOR_H
#define CURSOR_H

#include "STRUCTS.H"

#include "BITMAP.H"

struct Bitmap;

/* Mouse cursor state: cu_init sets it up, cu_ctl/cu_draw move it. */
enum CursorDrawState {
    CURSOR_ERASE_BUFFERED = -2,
    CURSOR_ERASE_NOW = -1,
    CURSOR_HIDDEN = 0,
    CURSOR_DRAWN = 1,
    CURSOR_DRAW_BUFFERED = 2
};

struct Cursor {
    struct Bitmap far *save; /* background under the cursor */
    struct Clip clip;        /* last drawn strip; memset as 12 bytes */
    char far *shape;
    char far *mask;
    unsigned shapeOffset;  /* shape offset, cu_shape_off(n) */
    unsigned shapeSize;    /* bytes per shape */
    int shapeIndex;        /* shape number */
    char state;            /* 0 hidden, 1 shown, <0 hide pending */
    struct Rect drawnRect; /* last drawn extent, x2 < 0 when none */
};

#define CURSOR_SHOW (-1)
#define CURSOR_SHOW_NOW (-2)
#define CURSOR_HIDE (-3)
#define CURSOR_HIDE_NOW (-4)
#define CURSOR_EGA_SAVE (EGA_VRAM + EGA_PITCH * SCREEN_HEIGHT)

extern struct Cursor g_cursor2;
unsigned cu_shape_off(unsigned n);
void cu_init(void);
void cu_ctl(int a);
void cu_draw(int x, int y);

#endif
