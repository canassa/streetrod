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
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "SYS.H"
#include "VIDEO.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int o_8250;
int au8252x;
int as8254y;
int aq8256z;

char *g_gearNames[7] = {"0", "1", "2", "3", "4", "N", "D"};   /* DS:0482 */

/* screen_status_init @ 0F38:2022 (linear 0x113A2, 127 bytes) */

void screen_status_init(int y)
{
    au8252x = y + 1;
    as8254y = 0xa0;
    o_8250 = 0x58;
    aq8256z = 0;
    x_colors = bustyle->colors;
    draw_fill_rect(r_vid.back, 0, y, 320, 10);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, y, 319, y + 9);
}

void (far *a_78aem)(struct Bitmap far *, int, int, char *);
struct State7EAA a_7eaap;
int g824cd;
int g8258z;
int ar82c0;

/* F_11421 @ 0F38:20A1 (linear 0x11421, 287 bytes) */

void F_11421(int x, char *s, int w)
{
    struct Colors saved;
    int left;
    int wd;
    int ypos;

    saved = x_colors;
    x_colors = bustyle->colors;
    if (w == 0)
        w = as8254y;
    if (x < 0) {
        x = o_8250;
        left = (x + (w - dash_text_width(s)) / 2) & 0xfff8;
    } else
        left = x;
    wd = 320 - x;
    ypos = au8252x;
    cursor_ctl(CURSOR_HIDE);
    if (aq8256z)
        draw_fill_rect(r_vid.back, x - 1, ypos, wd, 8);
    (*a_78aem)(r_vid.back, left, ypos, s);
    aq8256z = *s;
    cursor_ctl(CURSOR_SHOW);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, x > 8 ? x - 8 : x, ypos, x + wd, ypos + 7);
    x_colors = saved;
}

/* F_11540 @ 0F38:21C0 (linear 0x11540, 83 bytes) */

void F_11540(int n)
{
    int old;
    char buf[10];

    old = aq8256z;
    aq8256z = 1;
    F_11421(g8258z, ltoa(a_7eaap.m7eaa += n, buf, 10), 0x30);
    ar82c0 = g824cd;
    aq8256z = old;
}

/* F_11593 @ 0F38:2213 (linear 0x11593, 214 bytes) */

void F_11593(char *s)
{
    int saved;

    saved = r_vid.f16;
    x_colors = bustyle->colors;
    cursor_ctl(CURSOR_HIDE);
    draw_fill_rect(r_vid.back, 0, au8252x, o_8250 & 0xfff8, 8);
    (*a_78aem)(r_vid.back, 0, au8252x, s);
    g8258z = (g824cd + 7) & 0xfff8;
    r_vid.f16 = 0;
    if (s[1] != 'G' && s[1] != ' ')
        F_11540(0);
    r_vid.f16 = saved;
    cursor_ctl(CURSOR_SHOW);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, au8252x, o_8250, au8252x + 7);
}

/* screen_show_gear @ 0F38:22E9 (linear 0x11669, 49 bytes) */

void screen_show_gear(int i)
{
    int saved;

    saved = aq8256z;
    aq8256z = 1;
    F_11421(g8258z, g_gearNames[i], 8);
    aq8256z = saved;
}

char af824ax;

/* screen_fill_rect @ 0F38:231A (linear 0x1169A, 130 bytes) */

void screen_fill_rect(int x, int y, int w, int h, char color)
{
    char old;

    old = x_colors.c[2];
    cursor_ctl(CURSOR_HIDE);
    x_colors.c[2] = color;
    draw_fill_rect(r_vid.back, x, y, w, h);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, x, y, x + w - 1, y + h - 1);
    x_colors.c[2] = old;
    cursor_ctl(CURSOR_SHOW);
}

/* screen_msg_box @ 0F38:239C (linear 0x1171C, 50 bytes) */

void screen_msg_box(char *s)
{
    strcpy(g_boxText, s);
    menu_modal(0x1d);
}

char g_promptPalette[18] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14, 0, 0};   /* DS:04AC */
int g_picX = 0;   /* DS:04BE */
int g_picY = 0;   /* DS:04C0 */
int g_picMaxH = 190;   /* DS:04C2 */

/* screen_wait_msg @ 0F38:23CE (linear 0x1174E, 390 bytes)

   Show a message and wait for a key ("Hit Return when ready ...") */

void screen_wait_msg(char *msg)
{
    int saved;
    char prev;
    int dx;
    int i;
    struct Bitmap far *back;

    saved = r_vid.f16;
    strcpy(g_promptLine1, msg);
    strcpy(g_promptLine2, "Hit Return when ready ...");
    if (g_graphicsUp >= 1) {
        if (g_driveRebuild) {
            screen_fill_rect(0, 0, 320, 200, 0);
            g_driveRebuild++;
        }
        prev = gz8249;
        if (!prev) {
            gz8249 = 1;
            (*m_present)();
        }
        dx = af824ax ? 5 : 0x50;
        for (i = 0; i < 5; i++) {
            g_hotReadyBox[i].y1 += dx;
            g_hotReadyBox[i].y2 += dx;
        }
        if (g_driveRebuild)
            video_set_palette(g_promptPalette);
        r_vid.f16 = 1;
        sys_reset_queues();
        back = r_vid.back;
        r_vid.back = r_vid.front;
        menu_modal(0x20);
        r_vid.back = back;
        for (i = 0; i < 5; i++) {
            g_hotReadyBox[i].y1 -= dx;
            g_hotReadyBox[i].y2 -= dx;
        }
        r_vid.f16 = saved;
        gz8249 = prev;
        if (!prev)
            (*bl78be)();
    } else {
        puts(" ");
        puts(g_promptLine1);
        puts(g_promptLine2);
        if (g_kbdHooked) {
            while (!kbdget_key())
                ;
        } else {
            while (!kbhit() == 0)
                ;
            getch();
        }
    }
}

/* F_118D4 @ 0F38:2554 (linear 0x118D4, 228 bytes)

   Blit bitmap number a to the back buffer at (g_picX, g_picY), then copy the rectangle to the front
   buffer. b >= 0 first fills the screen with color b. */

int F_118D4(int a, int b)
{
    struct Bitmap far *p;
    long unused;    /* PROVISIONAL: never used; fills the 4-byte frame slot above the Blt (original local unknown) */
    struct Blt pkt;

    cursor_ctl(CURSOR_HIDE);
    p = F_15BBE(a, ARENA_HIGH_STACK);
    if (b >= 0)
        screen_fill_rect(0, 0, 320, g_picMaxH, b);
    pkt.w = p->width;
    pkt.h = p->height > g_picMaxH ? g_picMaxH : p->height;
    pkt.sx = pkt.sy = pkt.fc = 0;
    pkt.dx = g_picX;
    pkt.dy = g_picY;
    (*b_78a2)(p, r_vid.back, &pkt, 1);
    cursor_ctl(CURSOR_SHOW);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, g_picX, g_picY, pkt.dx + pkt.w - 1, pkt.dy + pkt.h - 1);
    g_picX = g_picY = 0;
    F_19438(1);
}

/* F_119B8 @ 0F38:2638 (linear 0x119B8, 30 bytes) */

int F_119B8(int a, int b, int c, int d)
{
    g_picX = b;
    g_picY = c;
    return F_118D4(a, d);
}

/* screen_text @ 0F38:2656 (linear 0x119D6, 78 bytes)

   Draw string s at (x, y) on the back buffer, then copy the 8 pixel high strip to the front buffer */

void screen_text(int x, int y, char *s)
{
    (*a_78aem)(r_vid.back, x, y, s);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, x, y, g824cd - 1, y + 7);
}

/* screen_hline @ 0F38:26A4 (linear 0x11A24, 78 bytes)

   Draw a horizontal run of w pixels at (x, y) on the back buffer and copy it to the front buffer */

void screen_hline(int x, int y, int w)
{
    draw_hline(r_vid.back, x, y, w);
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, x, y, x + w - 1, y);
}
