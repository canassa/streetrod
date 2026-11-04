#include <string.h>
#include "STRUCTS.H"
#include "ADS.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "HOTSPOT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "PC.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int g_adIdxEndPageState;

static struct Bitmap far * near ad_picture(int n);

struct Pages g_pages = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};   /* DS:04FE */
struct Bitmap far *g_adPicture = 0;   /* DS:051C */
char g_adRedraw = 0;   /* DS:0520 */
struct Blt g_adPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* DS:0522 */
int g_adResume = 0;   /* DS:0536 */

/* ads_open @ 0000:08B4 (linear 0x008B4, 259 bytes) */

void ads_open(int *list, int base)
{
    int num;
    int pos;
    int *ptr;
    int first;

    g_adResume = 0;
    marks_clear();
    g_pages.list = list;
    g_pages.n = 0;
    ptr = list + 1;
    pos = SCREEN_BODY_HEIGHT;
    num = first = *list;
    base = num - base;
    g_pages.cur = 1;
    while (num--) {
        while ((pos += 9 * g_descHdr[*ptr] + 3) > (SCREEN_BODY_HEIGHT - 1)) {
            g_pages.start[g_pages.n++] = first - num;
            pos = 0;
        }
        if (num == base)
            g_pages.cur = g_pages.n - 1;
        ptr++;
    }
    x_colors = bustyle->hilite;
    cursor_ctl(CURSOR_HIDE);
    g_adPacket.w = 0x40;
    g_adPacket.h = SCREEN_BODY_HEIGHT;
    g_adPacket.sx = g_adPacket.sy = g_adPacket.dx = g_adPacket.dy = g_adPacket.fc = 0;
    g_adRedraw = 1;
    menu_push(8);
    cursor_ctl(CURSOR_SHOW);
}

/* ad_picture @ 0000:09B7 (linear 0x009B7, 20 bytes) */

static struct Bitmap far * near ad_picture(int n)
{
    return disk_bm(0x403, n);
}

/* ads_run @ 0000:09CB (linear 0x009CB, 909 bytes) */

int ads_run(void)
{
    int hsave;
    int key;
    char c6;
    int y;
    int i;
    int diff;
    char *str;
    int limit;
    int cntz;
    struct Hot *row7;
    int *next;
    int item;

    limit = *g_pages.list;
    for (;;) {
        if (g_adRedraw) {
            cursor_ctl(CURSOR_HIDE_NOW);
            x_colors = bustyle->hilite;
            g_adPicture = ad_picture(2);
            draw_fill_rect(r_vid.back, 0x40, 0, 0xc0, SCREEN_BODY_HEIGHT);
            g_adPacket.sx = g_adPacket.dx = 0;
            (*g_videoOps.copy)(g_adPicture, r_vid.back, &g_adPacket, 1);
            g_adPacket.sx = 0x40;
            g_adPacket.dx = 0x100;
            (*g_videoOps.copy)(g_adPicture, r_vid.back, &g_adPacket, 1);
            F_19438(1);
            hsave = r_vid.back->height;
            r_vid.back->height = SCREEN_BODY_HEIGHT;
            y = 0;
            i = g_adResume ? a_7eaap.adIndex : g_pages.start[g_pages.cur];
            next = g_pages.list + i;
            row7 = g_hotPageRows;
            cntz = 0;
            a_7eaap.adIndex = i;
            do {
                draw_hline(r_vid.back, 0x44, y + 1, 0xb8);
                y += 3;
                if (y > (SCREEN_BODY_HEIGHT - 1))
                    break;
                cntz++;
                row7++;
                row7->y1 = y - 2;
                row7->kind = 0xb;
                item = *next++;
                row7->f6 = item >= 0xcd8 ? 0 : i;
                g_videoOps.drawText(r_vid.back, 0x44, y, str = g_desc + item);
                for (;;) {
                    while ((c6 = *(str += strlen(str) + 1)) == '+') {
                        str++;
                        g_videoOps.drawText(r_vid.back, g_penPoint.x, y, str);
                    }
                    if (c6 < 0x20)
                        break;
                    y += 9;
                    if (y > (SCREEN_BODY_HEIGHT - 1))
                        break;
                    g_videoOps.drawText(r_vid.back, 0x44, y, str);
                }
                y += 9;
                row7->y2 = y + 1 > (SCREEN_BODY_HEIGHT - 1) ? (SCREEN_BODY_HEIGHT - 1) : y + 1;
                if (i >= limit) {
                    i = 0;
                    next = g_pages.list + g_pages.start[0];
                }
            } while (y < (SCREEN_BODY_HEIGHT - 1) && ++i <= limit);
            g_adIdxEndPageState = i;
            g_adResume = 1;
            row7->y2 = y + 1 > (SCREEN_BODY_HEIGHT - 1) ? (SCREEN_BODY_HEIGHT - 1) : y + 1;
            if (row7->y2 < (SCREEN_BODY_HEIGHT - 1))
                draw_hline(r_vid.back, 0x44, y + 1, 0xb8);
            row7++;
            while (row7->f6 >= 0) {
                row7->kind = 0xe;
                row7++;
            }
            g_adRedraw = 0;
            r_vid.back->height = hsave;
            cursor_ctl(CURSOR_SHOW);
            if (r_vid.copyToFront)
                bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), (SCREEN_BODY_HEIGHT - 1));
            sys_hover_reset();
        }
        while ((key = sys_wait_input(3000)) == 0)
            ;
        switch (key) {
        case -3:
            break;
        case -1:
            if (--g_pages.cur < 0)
                g_pages.cur = g_pages.n - 1;
            g_adIdxEndPageState = a_7eaap.adIndex - 1;
            if (g_adIdxEndPageState < 1)
                g_adIdxEndPageState = limit;
            diff = (SCREEN_BODY_HEIGHT - 1);
            while ((diff -= 9 * g_descHdr[g_pages.list[g_adIdxEndPageState]] + 3) > 0)
                if (--g_adIdxEndPageState < 1)
                    g_adIdxEndPageState = limit;
            a_7eaap.adIndex = g_adIdxEndPageState;
            g_adRedraw = 1;
            continue;
        case -2:
            if (++g_pages.cur >= g_pages.n)
                g_pages.cur = 0;
            a_7eaap.adIndex = g_adIdxEndPageState + (y > 0xc1 ? 0 : 1);
            g_adRedraw = 1;
            continue;
        default:
            g_adRedraw = 1;
        }
        return key;
    }
}

/* ads_close @ 0000:0D58 (linear 0x00D58, 32 bytes) */

void ads_close(void)
{
    marks_clear();
    g_pages.list = 0;
    g_adPicture = 0;
    menu_pop(1);
}
