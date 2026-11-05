#include <string.h>
#include "CURSOR.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "DASH.H"
#include "DISK.H"
#include "EGALATCH.H"
#include "INPUTDAT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "PC.H"
#include "RACE.H"
#include "STRUCTS.H"

struct Cursor g_cursor2;

static struct Pair g_cursorPairs[GAS_HAND_COUNT] = {{1, 1}, {2, 2}, {1, 1}, {8, 8}};


unsigned cu_shape_off(unsigned n)
{
    return n * g_cursor2.shapeSize;
}

/* Set up the off-screen work bitmap and the scratch strip used by the pop-up code */

void cu_init(void)
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

    bytes = CURSOR_PLANE_BYTES;
    size = (r_vid.planes + 1) * bytes + 0x60;
    if (r_vid.mode != VID_EGA)
        size += (CURSOR_WIDTH >> r_vid.shift) << 4;
    mem = (struct Bitmap far *)me_alloc(size, 1);
    mem->bits.p = (char far *)mem + BITMAP_HEADER_BYTES;
    img = me_new_bitmap(CURSOR_WIDTH << r_vid.shift, CURSOR_SHEET_HEIGHT, 1, BITMAP_WORK, mem);
    g_cursor2.save = (struct Bitmap far *)(img->mask.p + bytes);
    me_set_bitmap(g_cursor2.save, r_vid.mode == VID_EGA ? CURSOR_EGA_SAVE : (char far *)(g_cursor2.save + 1), 0L,
                  (CURSOR_WIDTH >> r_vid.shift) << r_vid.shift, CURSOR_HEIGHT,
                  r_vid.mode == VID_EGA ? VID_EGA : BITMAP_WORK);
    memset(&g_cursor2.clip, 0, 12);
    g_cursor2.state = CURSOR_HIDDEN;
    pict = di_bm(PIC_CURSORS, ARENA_HIGH_STACK);
    if (pict == 0)
        me_out_of_memory();
    blt.w = CURSOR_WIDTH;
    blt.h = CURSOR_SHEET_HEIGHT;
    blt.sx = blt.sy = blt.dx = blt.dy = 0;
    (*g_videoOps.copy)(pict, img, &blt, 3);
    for (step = 1; step <= ~r_vid.mask; step++) {
        tmp = ar_bm(CURSOR_WIDTH, CURSOR_SHEET_HEIGHT, 1, ARENA_LOW_STACK);
        bm_shift_right(pict, step, tmp);
        blt.dx += CURSOR_WIDTH;
        (*g_videoOps.copy)(tmp, img, &blt, 3);
        ma_pop_low(1);
    }
    ma_clear();
    g_cursor2.shape = img->bits.p;
    g_cursor2.mask = img->mask.p;
    if (r_vid.mode == VID_EGA) {
        fp = (unsigned far *)g_cursor2.mask;
        step = img->pixelBytes >> 3;
        while (step--) {
            *fp = ~*fp;
            fp++;
        }
    }
    g_cursor2.shapeSize = img->pitch << 4;
    g_cursor2.shapeIndex = GAS_HAND_EMPTY;
    g_cursor2.shapeOffset = cu_shape_off(GAS_HAND_EMPTY);
    g_cursor2.drawnRect.x2 = -1;
    g_cursorX = 0x9e;
    g_cursorY = 0x80;
    g_savedCursorX = 0;
    g_savedCursorY = 0;
    g_graphicsUp = 2;
}

/* Mouse cursor control. mode >= 0 selects cursor shape number mode and moves it to the
   (clamped) mouse position; -1/-2 show it and -3/-4 hide it (nested by a counter). */

void cu_ctl(int mode)
{
    int a;
    int b;

    a = g_cursorX;
    b = g_cursorY;
    a = a<0 ? 0 : a>(SCREEN_WIDTH - 1) ? (SCREEN_WIDTH - 1) : a;
    b = b<0 ? 0 : b>(SCREEN_HEIGHT - 1) ? (SCREEN_HEIGHT - 1) : b;
    if (mode >= 0) {
        g_unused14 = g_cursorPairs[g_cursor2.shapeIndex = mode].a;
        g_unused15 = g_cursorPairs[mode].b;
        g_cursor2.shapeOffset = cu_shape_off(g_cursor2.shapeIndex);
        cu_draw(a, b);
    } else if (mode >= CURSOR_SHOW_NOW) {
        if (++g_cVis > 0) {
            g_cursor2.state = (mode == CURSOR_SHOW) + CURSOR_DRAWN;
            if (g_cursor2.drawnRect.x2 >= 0)
                cu_draw(g_cursor2.drawnRect.x1, g_cursor2.drawnRect.y1);
            else
                cu_draw(a, b);
            g_cursor2.state = CURSOR_DRAWN;
        }
    } else {
        if (--g_cVis >= 0) {
            g_cursor2.state = -((mode == CURSOR_HIDE) - 0xff);
            if (g_cursor2.drawnRect.x2 >= 0)
                cu_draw(g_cursor2.drawnRect.x1, g_cursor2.drawnRect.y1);
            else
                cu_draw(a, b);
            g_cursor2.state = CURSOR_HIDDEN;
        }
    }
}

/* Move the saved-background cursor strip to (x, y): restore what it covered, save and draw anew */

void cu_draw(int x, int y)
{
    int a;
    int b;

    if (g_cursor2.state) {
        if (g_cursor2.clip.w > 0) {
            if (r_vid.mode == VID_EGA)
                ea_to_page(g_cursor2.save->bits.p, g_cursor2.clip.w >> 3, g_cursor2.clip.h,
                           r_vid.back->bits.p + EGA_PITCH * g_cursor2.clip.dy + (g_cursor2.clip.dx >> 3),
                           CURSOR_WIDTH >> r_vid.shift);
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
            if (g_cursor2.state < CURSOR_ERASE_NOW)
                return;
            g_cursor2.state = CURSOR_DRAWN;
        } else {
            g_cursor2.clip.dx = x & r_vid.mask;
            g_cursor2.clip.w =
                SCREEN_WIDTH - g_cursor2.clip.dx > CURSOR_WIDTH ? CURSOR_WIDTH : SCREEN_WIDTH - g_cursor2.clip.dx;
            g_cursor2.clip.h = SCREEN_HEIGHT - y > 16 ? 16 : SCREEN_HEIGHT - y;
            g_cursor2.clip.dy = y;
            a = (x & ~r_vid.mask) * (CURSOR_WIDTH >> r_vid.shift) + g_cursor2.shapeOffset;
            (*g_videoOps.cursorBlit)(g_cursor2.shape + a, g_cursor2.mask + a, g_cursor2.save->bits.p,
                                     r_vid.back->bits.p, g_cursor2.clip.dx >> r_vid.shift, y,
                                     g_cursor2.clip.w >> r_vid.shift, g_cursor2.clip.h);
        }
        a = x + 15;
        b = y + 15;
        if (r_vid.copyToFront && g_cursor2.state == CURSOR_DRAWN)
            bm_copy_box(r_vid.back, r_vid.front, g_cursor2.drawnRect.x1 > x ? x : g_cursor2.drawnRect.x1,
                        g_cursor2.drawnRect.y1 > y ? y : g_cursor2.drawnRect.y1,
                        g_cursor2.drawnRect.x2 < a ? a : g_cursor2.drawnRect.x2,
                        g_cursor2.drawnRect.y2 < b ? b : g_cursor2.drawnRect.y2);
        g_cursor2.drawnRect.x1 = x;
        g_cursor2.drawnRect.y1 = y;
        g_cursor2.drawnRect.x2 = a;
        g_cursor2.drawnRect.y2 = b;
    }
}
