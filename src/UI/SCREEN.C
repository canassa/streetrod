#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include "STRUCTS.H"
#include "SCREEN.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "HOTSPOT.H"
#include "KBDGET.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "PC.H"
#include "SYS.H"
#include "VIDEO.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */

char *g_gearNames[7] = {"0", "1", "2", "3", "4", "N", "D"};   /* DS:0482 */

/* sc_status_init @ 0F38:2022 (linear 0x113A2, 127 bytes) */

void sc_status_init(int y)
{
    g_statusLine.y = y + 1;
    g_statusLine.messageWidth = 0xa0;
    g_statusLine.messageX = 0x58;
    g_statusLine.shown = 0;
    x_colors = bustyle->colors;
    dr_fill_rect(r_vid.back, 0, y, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, 0, y, (SCREEN_WIDTH - 1), y + STATUS_BAR_HEIGHT - 1);
}

struct State7EAA a_7eaap;
struct TextEnd g_penPoint;
struct StatusText g_statusLine; /* DS:824C */
int g_xMoneyRhsVal;

/* sc_status_text @ 0F38:20A1 (linear 0x11421, 287 bytes) */

void sc_status_text(int x, char *s, int w)
{
    struct Colors saved;
    int left;
    int wd;
    int ypos;

    saved = x_colors;
    x_colors = bustyle->colors;
    if (w == 0)
        w = g_statusLine.messageWidth;
    if (x < 0) {
        x = g_statusLine.messageX;
        left = (x + (w - da_text_width(s)) / 2) & 0xfff8;
    } else
        left = x;
    wd = SCREEN_WIDTH - x;
    ypos = g_statusLine.y;
    cu_ctl(CURSOR_HIDE);
    if (g_statusLine.shown)
        dr_fill_rect(r_vid.back, x - 1, ypos, wd, 8);
    (*g_videoOps.drawText)(r_vid.back, left, ypos, s);
    g_statusLine.shown = *s;
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x > 8 ? x - 8 : x, ypos, x + wd, ypos + 7);
    x_colors = saved;
}

/* sc_add_money @ 0F38:21C0 (linear 0x11540, 83 bytes) */

void sc_add_money(int n)
{
    int old;
    char buf[10];

    old = g_statusLine.shown;
    g_statusLine.shown = 1;
    sc_status_text(g_statusLine.valueX, ltoa(a_7eaap.money += n, buf, 10), 0x30);
    g_xMoneyRhsVal = g_penPoint.x;
    g_statusLine.shown = old;
}

/* sc_status_label @ 0F38:2213 (linear 0x11593, 214 bytes) */

void sc_status_label(char *s)
{
    int saved;

    saved = r_vid.copyToFront;
    x_colors = bustyle->colors;
    cu_ctl(CURSOR_HIDE);
    dr_fill_rect(r_vid.back, 0, g_statusLine.y, g_statusLine.messageX & 0xfff8, 8);
    (*g_videoOps.drawText)(r_vid.back, 0, g_statusLine.y, s);
    g_statusLine.valueX = (g_penPoint.x + 7) & 0xfff8;
    r_vid.copyToFront = 0;
    if (s[1] != 'G' && s[1] != ' ')
        sc_add_money(0);
    r_vid.copyToFront = saved;
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, 0, g_statusLine.y, g_statusLine.messageX, g_statusLine.y + 7);
}

/* sc_show_gear @ 0F38:22E9 (linear 0x11669, 49 bytes) */

void sc_show_gear(int i)
{
    int saved;

    saved = g_statusLine.shown;
    g_statusLine.shown = 1;
    sc_status_text(g_statusLine.valueX, g_gearNames[i], 8);
    g_statusLine.shown = saved;
}

char g_displaySplitYVal;

/* sc_fill_rect @ 0F38:231A (linear 0x1169A, 130 bytes) */

void sc_fill_rect(int x, int y, int w, int h, char color)
{
    char old;

    old = x_colors.fill;
    cu_ctl(CURSOR_HIDE);
    x_colors.fill = color;
    dr_fill_rect(r_vid.back, x, y, w, h);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x, y, x + w - 1, y + h - 1);
    x_colors.fill = old;
    cu_ctl(CURSOR_SHOW);
}

/* sc_msg_box @ 0F38:239C (linear 0x1171C, 50 bytes) */

void sc_msg_box(char *s)
{
    strcpy(g_boxText, s);
    mn_modal(KM_MESSAGE_BOX);
}

char g_promptPalette[18] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14, 0, 0};   /* DS:04AC */
struct PicturePlacement g_picturePlacement = {0, 0, SCREEN_BODY_HEIGHT}; /* DS:04BE */

/* sc_wait_msg @ 0F38:23CE (linear 0x1174E, 390 bytes)

   Show a message and wait for a key ("Hit Return when ready ...") */

void sc_wait_msg(char *msg)
{
    int saved;
    char prev;
    int dx;
    int i;
    struct Bitmap far *back;

    saved = r_vid.copyToFront;
    strcpy(g_promptLine1, msg);
    strcpy(g_promptLine2, "Hit Return when ready ...");
    if (g_graphicsUp >= 1) {
        if (g_driveRebuild) {
            sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
            g_driveRebuild++;
        }
        prev = gz8249;
        if (!prev) {
            gz8249 = 1;
            (*g_videoOps.show)();
        }
        dx = g_displaySplitYVal ? 5 : 0x50;
        for (i = 0; i < 5; i++) {
            g_hotReadyBox[i].y1 += dx;
            g_hotReadyBox[i].y2 += dx;
        }
        if (g_driveRebuild)
            vi_set_palette(g_promptPalette);
        r_vid.copyToFront = 1;
        sy_reset_queues();
        back = r_vid.back;
        r_vid.back = r_vid.front;
        mn_modal(KM_READY);
        r_vid.back = back;
        for (i = 0; i < 5; i++) {
            g_hotReadyBox[i].y1 -= dx;
            g_hotReadyBox[i].y2 -= dx;
        }
        r_vid.copyToFront = saved;
        gz8249 = prev;
        if (!prev)
            (*g_videoOps.blank)();
    } else {
        puts(" ");
        puts(g_promptLine1);
        puts(g_promptLine2);
        if (g_kbdHooked) {
            while (!kg_key())
                ;
        } else {
            while (!kbhit() == 0)
                ;
            getch();
        }
    }
}

/* sc_picture @ 0F38:2554 (linear 0x118D4, 228 bytes)

   Blit bitmap number a to the back buffer at (g_picturePlacement.x, g_picturePlacement.y), then copy the rectangle to the front
   buffer. b >= 0 first fills the screen with color b. */

int sc_picture(int a, int b)
{
    struct Bitmap far *p;
    long unused;    /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
    struct Blt pkt;

    cu_ctl(CURSOR_HIDE);
    p = di_bm(a, ARENA_HIGH_STACK);
    if (b >= 0)
        sc_fill_rect(0, 0, SCREEN_WIDTH, g_picturePlacement.maxHeight, b);
    pkt.w = p->width;
    pkt.h = p->height > g_picturePlacement.maxHeight ? g_picturePlacement.maxHeight : p->height;
    pkt.sx = pkt.sy = pkt.fc = 0;
    pkt.dx = g_picturePlacement.x;
    pkt.dy = g_picturePlacement.y;
    (*g_videoOps.copy)(p, r_vid.back, &pkt, 1);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, g_picturePlacement.x, g_picturePlacement.y, pkt.dx + pkt.w - 1, pkt.dy + pkt.h - 1);
    g_picturePlacement.x = g_picturePlacement.y = 0;
    ma_pop_high(1);
}

/* sc_picture_at @ 0F38:2638 (linear 0x119B8, 30 bytes) */

int sc_picture_at(int a, int b, int c, int d)
{
    g_picturePlacement.x = b;
    g_picturePlacement.y = c;
    return sc_picture(a, d);
}

/* sc_text @ 0F38:2656 (linear 0x119D6, 78 bytes)

   Draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */

void sc_text(int x, int y, char *s)
{
    (*g_videoOps.drawText)(r_vid.back, x, y, s);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x, y, g_penPoint.x - 1, y + 7);
}

/* sc_hline @ 0F38:26A4 (linear 0x11A24, 78 bytes)

   Draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */

void sc_hline(int x, int y, int w)
{
    dr_hline(r_vid.back, x, y, w);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x, y, x + w - 1, y);
}
