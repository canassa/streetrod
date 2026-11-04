#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "POPUP.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "MESSAGES.H"
#include "PC.H"
#include "SYS.H"
#include "VIDEO.H"

static struct Win *g_popupLayout;
static int g_6c42[2];                      /* PROVISIONAL: 4 unreferenced bytes after g_popupLayout */

struct Win g_popupWins[3] = {{1072, 1, 72, 30, 152, 37, 0, 6, 144}, {1073, 1, 80, 30, 232, 37, 0, 6, 223}, {1074, 2, 80, 20, 232, 46, 0, 6, 223}};
struct Bitmap far *g_popupSave = 0;

/* Copy the rectangle (x, y, w, h) of the back buffer into pict new bitmap and return it.
   The x range is rounded to the pixel grid r_vid.mask. src, when given, is the bitmap to reuse. */

struct Bitmap far *pu_grab(int x, int y, int w, int h, struct Bitmap far *src)
{
    struct Bitmap far *pict;
    int x2;
    struct Blt blit;
    int ye;
    int color;

    x2 = (x + w + ~r_vid.mask & r_vid.mask) > SCREEN_WIDTH ? SCREEN_WIDTH : (x + w + ~r_vid.mask & r_vid.mask);
    ye = y + h > SCREEN_HEIGHT ? SCREEN_HEIGHT : y + h;
    x &= r_vid.mask;
    if (src) {
        color = src->alt.p != 0;
        pict = me_new_bitmap(x2 - x, ye - y, color, -1, src);
    } else
        pict = ar_bm(x2 - x, ye - y, 0, ARENA_HIGH_STACK);
    blit.w = pict->width;
    blit.h = pict->height;
    blit.sx = x;
    blit.sy = y;
    blit.dx = blit.dy = 0;
    cu_ctl(CURSOR_HIDE);
    (*g_videoOps.copy)(r_vid.back, pict, &blit, 1);
    cu_ctl(CURSOR_SHOW);
    return pict;
}

/* Blit bitmap bm to the back buffer at (x, y), x rounded to the pixel grid r_vid.mask */

void pu_put(struct Bitmap far *bm, int x, int y)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x & r_vid.mask;
    pkt.dy = y;
    cu_ctl(CURSOR_HIDE);
    (*g_videoOps.copy)(bm, r_vid.back, &pkt, 1);
    cu_ctl(CURSOR_SHOW);
}

/* Draw bitmap bm at (x, y) on the back buffer and copy the covered rectangle to the front buffer */

void pu_show(struct Bitmap far *bm, int x, int y)
{
    pu_put(bm, x, y);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x, y, bm->width + x - 1, bm->height + y - 1);
}

/* Pop up message window n (negative n: the window is removed again afterwards).
   The text is the run of NUL-terminated lines that follows the window's index byte in g_descHdr. */

void pu_open(int n)
{
    char *txt;
    int close;
    int x0;
    int y1;
    int w1;
    int h1;
    int rec;
    int lines;

    cu_ctl(CURSOR_HIDE);
    if (g_popupSave) {
        pu_put(g_popupSave, g_popupLayout->x, g_popupLayout->y);
        ma_pop_high(1);
        g_popupSave = 0;
    }
    close = n < 0;
    if (close)
        n = -n;
    txt = g_descHdr + n + 1;
    rec = txt[-1] - 1;
    g_popupLayout = &g_popupWins[rec];
    g_popupSave = pu_grab(x0 = g_popupLayout->x, y1 = g_popupLayout->y, w1 = g_popupLayout->w, h1 = g_popupLayout->h, 0);
    bm_draw_pic(r_vid.back, g_popupLayout->bg, (unsigned char)x0, (unsigned char)y1, 0);
    x_colors = bustyle->popup;
    x0 += g_popupLayout->tx;
    y1 += g_popupLayout->ty;
    lines = g_popupLayout->lines;
    while (lines--) {
        (*g_videoOps.drawText)(r_vid.back, ((w1 - da_text_width(txt)) >> 1) + x0, y1, txt);
        txt = txt + strlen(txt) + 1;
        y1 += 9;
    }
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, g_popupLayout->x, g_popupLayout->y, g_popupLayout->x + w1 - 1, g_popupLayout->y + h1 - 1);
    if (close) {
        ma_pop_high(1);
        g_popupSave = 0;
    }
    g_clickEnds = 1;
}

/* Pop up message window n with its position temporarily set to (x, y); idx negative coordinate
   keeps the default */

void pu_open_at(int n, int x, int y)
{
    int idx;
    int ox;
    int oy;

    idx = g_descHdr[abs(n)] - 1;
    ox = g_popupWins[idx].x;
    oy = g_popupWins[idx].y;
    if (x >= 0)
        g_popupWins[idx].x = x;
    if (y >= 0)
        g_popupWins[idx].y = y;
    pu_open(n);
    g_popupWins[idx].x = ox;
    g_popupWins[idx].y = oy;
    g_clickEnds = 0;
}

/* Remove the pop-up window: put back the saved background */

void pu_close(void)
{
    if (g_popupSave) {
        pu_show(g_popupSave, g_popupLayout->x, g_popupLayout->y);
        ma_pop_high(1);
        g_popupSave = 0;
    }
    g_clickEnds = 0;
}
