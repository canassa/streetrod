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
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int g7f84q;

static struct Bitmap far * near ad_picture(int n);

struct Pages g_pages = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};   /* DS:04FE */
struct Bitmap far *g_adPicture = 0;   /* DS:051C */
char g_adRedraw = 0;   /* DS:0520 */
int g_adBlt = 0;   /* DS:0522 */
int g_adHeight = 0;   /* DS:0524 */
int g_adSrcX = 0;   /* DS:0526 */
int g_adSrcY = 0;   /* DS:0528 */
int g_adDstX = 0;   /* DS:052A */
int g_adDstY = 0;   /* DS:052C */
int g_052e = 0;   /* DS:052E */
int g_0530[3] = {0, 0, 0};   /* DS:0530 */
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
    pos = 0xbe;
    num = first = *list;
    base = num - base;
    g_pages.cur = 1;
    while (num--) {
        while ((pos += 9 * g_descHdr[*ptr] + 3) > 0xbd) {
            g_pages.start[g_pages.n++] = first - num;
            pos = 0;
        }
        if (num == base)
            g_pages.cur = g_pages.n - 1;
        ptr++;
    }
    x_colors = bustyle->hilite;
    cursor_ctl(-3);
    g_adBlt = 0x40;
    g_adHeight = 0xbe;
    g_adSrcX = g_adSrcY = g_adDstX = g_adDstY = g_052e = 0;
    g_adRedraw = 1;
    menu_push(8);
    cursor_ctl(-1);
}

/* ad_picture @ 0000:09B7 (linear 0x009B7, 20 bytes) */

static struct Bitmap far * near ad_picture(int n)
{
    return F_15BBE(0x403, n);
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
            cursor_ctl(-4);
            x_colors = bustyle->hilite;
            g_adPicture = ad_picture(2);
            draw_fill_rect(r_vid.back, 0x40, 0, 0xc0, 0xbe);
            g_adSrcX = g_adDstX = 0;
            b_78a2(g_adPicture, r_vid.back, (struct Blt *)&g_adBlt, 1);
            g_adSrcX = 0x40;
            g_adDstX = 0x100;
            b_78a2(g_adPicture, r_vid.back, (struct Blt *)&g_adBlt, 1);
            F_19438(1);
            hsave = r_vid.back->height;
            r_vid.back->height = 0xbe;
            y = 0;
            i = g_adResume ? a_7eaap.m7ebe : g_pages.start[g_pages.cur];
            next = g_pages.list + i;
            row7 = g_hotPageRows;
            cntz = 0;
            a_7eaap.m7ebe = i;
            do {
                draw_hline(r_vid.back, 0x44, y + 1, 0xb8);
                y += 3;
                if (y > 0xbd)
                    break;
                cntz++;
                row7++;
                row7->y1 = y - 2;
                row7->f0 = 0xb;
                item = *next++;
                row7->f6 = item >= 0xcd8 ? 0 : i;
                a_78aem(r_vid.back, 0x44, y, str = g_desc + item);
                for (;;) {
                    while ((c6 = *(str += strlen(str) + 1)) == '+') {
                        str++;
                        a_78aem(r_vid.back, g824cd, y, str);
                    }
                    if (c6 < 0x20)
                        break;
                    y += 9;
                    if (y > 0xbd)
                        break;
                    a_78aem(r_vid.back, 0x44, y, str);
                }
                y += 9;
                row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
                if (i >= limit) {
                    i = 0;
                    next = g_pages.list + g_pages.start[0];
                }
            } while (y < 0xbd && ++i <= limit);
            g7f84q = i;
            g_adResume = 1;
            row7->y2 = y + 1 > 0xbd ? 0xbd : y + 1;
            if (row7->y2 < 0xbd)
                draw_hline(r_vid.back, 0x44, y + 1, 0xb8);
            row7++;
            while (row7->f6 >= 0) {
                row7->f0 = 0xe;
                row7++;
            }
            g_adRedraw = 0;
            r_vid.back->height = hsave;
            cursor_ctl(-1);
            if (r_vid.f16)
                bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xbd);
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
            g7f84q = a_7eaap.m7ebe - 1;
            if (g7f84q < 1)
                g7f84q = limit;
            diff = 0xbd;
            while ((diff -= 9 * g_descHdr[g_pages.list[g7f84q]] + 3) > 0)
                if (--g7f84q < 1)
                    g7f84q = limit;
            a_7eaap.m7ebe = g7f84q;
            g_adRedraw = 1;
            continue;
        case -2:
            if (++g_pages.cur >= g_pages.n)
                g_pages.cur = 0;
            a_7eaap.m7ebe = g7f84q + (y > 0xc1 ? 0 : 1);
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
