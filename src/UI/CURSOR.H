/**
 * @file CURSOR.H
 * @brief Mouse cursor shapes, visibility and background restoration.
 */

#ifndef CURSOR_H
#define CURSOR_H
/** Bytes per pre-shifted cursor sheet plane, including alignment copies. */
#define CURSOR_PLANE_BYTES 0x600
/** Pixel rows holding four stacked cursor shapes. */
#define CURSOR_SHEET_HEIGHT 64
/** Pixel rows saved beneath one cursor shape. */
#define CURSOR_HEIGHT 16
/** Pixel span reserved for a cursor, including horizontal alignment padding. */
#define CURSOR_WIDTH 24

#include "STRUCTS.H"

#include "BITMAP.H"

struct Bitmap;

/** @brief Cursor update and visible-page refresh policy. */
enum CursorDrawState {
    CURSOR_ERASE_BUFFERED = -2, /**< Restore only the back buffer. */
    CURSOR_ERASE_NOW = -1,      /**< Restore and refresh the visible page. */
    CURSOR_HIDDEN = 0,          /**< Skip drawing and restoration. */
    CURSOR_DRAWN = 1,           /**< Draw and refresh the visible page. */
    CURSOR_DRAW_BUFFERED = 2    /**< Draw only into the back buffer. */
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

/**
 * @name Visibility control commands
 * @brief Adjust nested visibility; NOW variants also refresh the visible page.
 * @{
 */
#define CURSOR_SHOW (-1)
#define CURSOR_SHOW_NOW (-2)
#define CURSOR_HIDE (-3)
#define CURSOR_HIDE_NOW (-4)
/** @} */
/** Off-screen EGA storage immediately below the visible page. */
#define CURSOR_EGA_SAVE (EGA_VRAM + EGA_PITCH * SCREEN_HEIGHT)

extern struct Cursor g_cursor2;
unsigned cu_shape_off(unsigned n);
void cu_init(void);
void cu_ctl(int a);
void cu_draw(int x, int y);

#endif
