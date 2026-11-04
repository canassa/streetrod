#include <string.h>
#include "STRUCTS.H"
#include "CURSOR.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "DASH.H"
#include "DISK.H"
#include "EGALATCH.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "M_02B9A.H"
#include "PC.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Cursor g_cursor2;

struct Pair g_cursorPairs[4] = {{1, 1}, {2, 2}, {1, 1}, {8, 8}};   /* DS:06EC */

/* cursor_shape_off @ 0F38:2B1C (linear 0x11E9C, 12 bytes) */

unsigned cursor_shape_off(unsigned n)
{
    return n * g_cursor2.shapeSize;
}

/* cursor_init @ 0F38:2B28 (linear 0x11EA8, 598 bytes)

   Set up the off-screen work bitmap and the scratch strip used by the pop-up code */

void cursor_init(void)
{
    unsigned bytes;
    int size;
    struct Bitmap far *mem;
    struct Bitmap far *img;
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    unsigned far *fp;
    int step;

    bytes = 0x600;
    size = (r_vid.planes + 1) * bytes + 0x60;
    if (r_vid.mode != VID_EGA)
        size += (0x18 >> r_vid.shift) << 4;
    mem = (struct Bitmap far *)mem_alloc(size, 1);
    mem->bits.p = (char far *)mem + 0x30;
    img = mem_new_bitmap(0x18 << r_vid.shift, 0x40, 1, -1, mem);
    g_cursor2.save = (struct Bitmap far *)(img->alt.p + bytes);
    mem_set_bitmap(g_cursor2.save, r_vid.mode == VID_EGA ? CURSOR_EGA_SAVE : (char far *)(g_cursor2.save + 1), 0L,
            (0x18 >> r_vid.shift) << r_vid.shift, 0x10, r_vid.mode == VID_EGA ? VID_EGA : -1);
    memset(&g_cursor2.clip, 0, 12);
    g_cursor2.state = 0;
    pict = disk_bm(2, ARENA_HIGH_STACK);
    if (pict == 0)
        mem_out_of_memory();
    blt.w = 0x18;
    blt.h = 0x40;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_videoOps.copy)(pict, img, &blt, 3);
    for (step = 1; step <= ~r_vid.mask; step++) {
        tmp = arena_bm(0x18, 0x40, 1, ARENA_LOW_STACK);
        bitmap_shift_right(pict, step, tmp);
        blt.dx += 0x18;
        (*g_videoOps.copy)(tmp, img, &blt, 3);
        marks_pop_low(1);
    }
    marks_clear();
    g_cursor2.shape = img->bits.p;
    g_cursor2.mask = img->alt.p;
    if (r_vid.mode == VID_EGA) {
        fp = (unsigned far *)g_cursor2.mask;
        step = img->planesize >> 3;
        while (step--) {
            *fp = ~*fp;
            fp++;
        }
    }
    g_cursor2.shapeSize = img->pitch << 4;
    g_cursor2.shapeIndex = 1;
    g_cursor2.shapeOffset = cursor_shape_off(1);
    g_cursor2.drawnRect.x2 = -1;
    g_cursorX = 0x9e;
    g_cursorY = 0x80;
    g_savedCursorX = 0;
    g_savedCursorY = 0;
    g_graphicsUp = 2;
}

/* cursor_ctl @ 0F38:2D7E (linear 0x120FE, 269 bytes)

   Mouse cursor control. mode >= 0 selects cursor shape number mode and moves it to the
   (clamped) mouse position; -1/-2 show it and -3/-4 hide it (nested by a counter). */

void cursor_ctl(int mode)
{
    int a;
    int b;

    a = g_cursorX;
    b = g_cursorY;
    a = a < 0 ? 0 : a > (SCREEN_WIDTH - 1) ? (SCREEN_WIDTH - 1) : a;
    b = b < 0 ? 0 : b > (SCREEN_HEIGHT - 1) ? (SCREEN_HEIGHT - 1) : b;
    if (mode >= 0) {
        g_4742 = g_cursorPairs[g_cursor2.shapeIndex = mode].a;
        g_4744 = g_cursorPairs[mode].b;
        g_cursor2.shapeOffset = cursor_shape_off(g_cursor2.shapeIndex);
        cursor_draw(a, b);
    } else if (mode >= CURSOR_SHOW_NOW) {
        if (++g_cVis > 0) {
            g_cursor2.state = (mode == CURSOR_SHOW) + 1;
            if (g_cursor2.drawnRect.x2 >= 0)
                cursor_draw(g_cursor2.drawnRect.x1, g_cursor2.drawnRect.y1);
            else
                cursor_draw(a, b);
            g_cursor2.state = 1;
        }
    } else {
        if (--g_cVis >= 0) {
            g_cursor2.state = -((mode == CURSOR_HIDE) - 0xff);
            if (g_cursor2.drawnRect.x2 >= 0)
                cursor_draw(g_cursor2.drawnRect.x1, g_cursor2.drawnRect.y1);
            else
                cursor_draw(a, b);
            g_cursor2.state = 0;
        }
    }
}

/* cursor_draw @ 0F38:2E8B (linear 0x1220B, 488 bytes)

   Move the saved-background cursor strip to (x, y): restore what it covered, save and draw anew */

void cursor_draw(int x, int y)
{
    int a;
    int b;

    if (g_cursor2.state) {
        if (g_cursor2.clip.w > 0) {
            if (r_vid.mode == VID_EGA)
                egalatch_to_page(g_cursor2.save->bits.p, g_cursor2.clip.w >> 3, g_cursor2.clip.h, r_vid.back->bits.p + EGA_PITCH * g_cursor2.clip.dy + (g_cursor2.clip.dx >> 3), 0x18 >> r_vid.shift);
            else
                (*g_videoOps.copy)(g_cursor2.save, r_vid.back, (struct Blt *)&g_cursor2.clip, 1);
            g_cursor2.clip.w = 0;
        } else {
            g_cursor2.drawnRect.x1 = g_cursor2.drawnRect.y1 = 1000;
            g_cursor2.drawnRect.x2 = g_cursor2.drawnRect.y2 = -1;
        }
        if (g_cursor2.state < 0) {
            if (g_cursor2.drawnRect.x2 < 0)
                return;
            if (g_cursor2.state < -1)
                return;
            g_cursor2.state = 1;
        } else {
            g_cursor2.clip.dx = x & r_vid.mask;
            g_cursor2.clip.w = SCREEN_WIDTH - g_cursor2.clip.dx > 0x18 ? 0x18 : SCREEN_WIDTH - g_cursor2.clip.dx;
            g_cursor2.clip.h = SCREEN_HEIGHT - y > 16 ? 16 : SCREEN_HEIGHT - y;
            g_cursor2.clip.dy = y;
            a = (x & ~r_vid.mask) * (0x18 >> r_vid.shift) + g_cursor2.shapeOffset;
            (*g_videoOps.cursorBlit)(g_cursor2.shape + a, g_cursor2.mask + a, g_cursor2.save->bits.p, r_vid.back->bits.p, g_cursor2.clip.dx >> r_vid.shift, y, g_cursor2.clip.w >> r_vid.shift, g_cursor2.clip.h);
        }
        a = x + 15;
        b = y + 15;
        if (r_vid.copyToFront && g_cursor2.state == 1)
            bitmap_copy_box(r_vid.back, r_vid.front, g_cursor2.drawnRect.x1 > x ? x : g_cursor2.drawnRect.x1, g_cursor2.drawnRect.y1 > y ? y : g_cursor2.drawnRect.y1, g_cursor2.drawnRect.x2 < a ? a : g_cursor2.drawnRect.x2, g_cursor2.drawnRect.y2 < b ? b : g_cursor2.drawnRect.y2);
        g_cursor2.drawnRect.x1 = x;
        g_cursor2.drawnRect.y1 = y;
        g_cursor2.drawnRect.x2 = a;
        g_cursor2.drawnRect.y2 = b;
    }
}
