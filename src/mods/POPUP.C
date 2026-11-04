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
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Win *g_6c40;                 /* DS:6C40 */
static int g_6c42[2];                      /* DS:6C42 PROVISIONAL: 4 unreferenced bytes after g_6c40 */

struct Win g_04c4[3] = {{1072, 1, 72, 30, 152, 37, 0, 6, 144}, {1073, 1, 80, 30, 232, 37, 0, 6, 223}, {1074, 2, 80, 20, 232, 46, 0, 6, 223}};   /* DS:04C4 */
struct Bitmap far *g_04fa = 0;   /* DS:04FA */

/* F_11A72 @ 0F38:26F2 (linear 0x11A72, 253 bytes)

   Copy the rectangle (x, y, w, h) of the back buffer into pict new bitmap and return it.
   The x range is rounded to the pixel grid r_vid.mask. src, when given, is the bitmap to reuse. */

struct Bitmap far *F_11A72(int x, int y, int w, int h, struct Bitmap far *src)
{
    struct Bitmap far *pict;
    int x2;
    struct Blt blit;
    int ye;
    int color;

    x2 = (x + w + ~r_vid.mask & r_vid.mask) > 320 ? 320 : (x + w + ~r_vid.mask & r_vid.mask);
    ye = y + h > 200 ? 200 : y + h;
    x &= r_vid.mask;
    if (src) {
        color = src->alt.p != 0;
        pict = F_0376B(x2 - x, ye - y, color, -1, src);
    } else
        pict = F_1934E(x2 - x, ye - y, 0, 2);
    blit.w = pict->width;
    blit.h = pict->height;
    blit.sx = x;
    blit.sy = y;
    blit.dx = blit.dy = 0;
    F_120FE(-3);
    (*b_78a2)(r_vid.back, pict, &blit, 1);
    F_120FE(-1);
    return pict;
}

/* F_11B6F @ 0F38:27EF (linear 0x11B6F, 100 bytes)

   Blit bitmap bm to the back buffer at (x, y), x rounded to the pixel grid r_vid.mask */

void F_11B6F(struct Bitmap far *bm, int x, int y)
{
    struct Blt pkt;

    pkt.w = bm->width;
    pkt.h = bm->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = x & r_vid.mask;
    pkt.dy = y;
    F_120FE(-3);
    (*b_78a2)(bm, r_vid.back, &pkt, 1);
    F_120FE(-1);
}

/* F_11BD3 @ 0F38:2853 (linear 0x11BD3, 81 bytes)

   Draw bitmap bm at (x, y) on the back buffer and copy the covered rectangle to the front buffer */

void F_11BD3(struct Bitmap far *bm, int x, int y)
{
    F_11B6F(bm, x, y);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, x, y, bm->width + x - 1, bm->height + y - 1);
}

/* F_11C24 @ 0F38:28A4 (linear 0x11C24, 446 bytes)

   Pop up message window n (negative n: the window is removed again afterwards).
   The text is the run of NUL-terminated lines that follows the window's index byte in g_2c01. */

void F_11C24(int n)
{
    char *txt;
    int close;
    int x0;
    int y1;
    int w1;
    int h1;
    int rec;
    int lines;

    F_120FE(-3);
    if (g_04fa) {
        F_11B6F(g_04fa, g_6c40->x, g_6c40->y);
        F_19438(1);
        g_04fa = 0;
    }
    close = n < 0;
    if (close)
        n = -n;
    txt = g_2c01 + n + 1;
    rec = txt[-1] - 1;
    g_6c40 = &g_04c4[rec];
    g_04fa = F_11A72(x0 = g_6c40->x, y1 = g_6c40->y, w1 = g_6c40->w, h1 = g_6c40->h, 0);
    bitmap_draw_pic(r_vid.back, g_6c40->bg, (unsigned char)x0, (unsigned char)y1, 0);
    x_colors = bustyle->popup;
    x0 += g_6c40->tx;
    y1 += g_6c40->ty;
    lines = g_6c40->lines;
    while (lines--) {
        (*a_78aem)(r_vid.back, ((w1 - F_10B48(txt)) >> 1) + x0, y1, txt);
        txt = txt + strlen(txt) + 1;
        y1 += 9;
    }
    F_120FE(-1);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, g_6c40->x, g_6c40->y, g_6c40->x + w1 - 1, g_6c40->y + h1 - 1);
    if (close) {
        F_19438(1);
        g_04fa = 0;
    }
    g_05de = 1;
}

/* F_11DE2 @ 0F38:2A62 (linear 0x11DE2, 125 bytes)

   Pop up message window n with its position temporarily set to (x, y); idx negative coordinate
   keeps the default */

void F_11DE2(int n, int x, int y)
{
    int idx;
    int ox;
    int oy;

    idx = g_2c01[abs(n)] - 1;
    ox = g_04c4[idx].x;
    oy = g_04c4[idx].y;
    if (x >= 0)
        g_04c4[idx].x = x;
    if (y >= 0)
        g_04c4[idx].y = y;
    F_11C24(n);
    g_04c4[idx].x = ox;
    g_04c4[idx].y = oy;
    g_05de = 0;
}

/* F_11E5F @ 0F38:2ADF (linear 0x11E5F, 61 bytes)

   Remove the pop-up window: put back the saved background */

void F_11E5F(void)
{
    if (g_04fa) {
        F_11BD3(g_04fa, g_6c40->x, g_6c40->y);
        F_19438(1);
        g_04fa = 0;
    }
    g_05de = 0;
}
