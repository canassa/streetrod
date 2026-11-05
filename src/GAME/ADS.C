#include <string.h>
#include "ADS.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "HOTSPOT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "PC.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

int g_adIdxEndPageState;

static struct Bitmap far *near ad_picture(int n);

static struct Pages g_pages = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0};
static struct Bitmap far *g_adPicture = 0;
static char g_adRedraw = 0;
static struct Blt g_adPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int g_adResume = 0;


void av_open(int *list, int base)
{
    int num;
    int pos;
    int *ptr;
    int first;

    g_adResume = 0;
    ma_clear();
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
    cu_ctl(CURSOR_HIDE);
    g_adPacket.w = 0x40;
    g_adPacket.h = SCREEN_BODY_HEIGHT;
    g_adPacket.sx = g_adPacket.sy = g_adPacket.dx = g_adPacket.dy = g_adPacket.unused1 = 0;
    g_adRedraw = 1;
    mn_push(KM_ADVERTS);
    cu_ctl(CURSOR_SHOW);
}


static struct Bitmap far *near ad_picture(int n)
{
    return di_bm(PIC_NEWSPAPER_CLASSIFIEDS, n);
}


int av_run(void)
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
    struct Hotspot *row7;
    int *next;
    int item;

    limit = *g_pages.list;
    for (;;) {
        if (g_adRedraw) {
            cu_ctl(CURSOR_HIDE_NOW);
            x_colors = bustyle->hilite;
            g_adPicture = ad_picture(2);
            dr_fill_rect(r_vid.back, 0x40, 0, 0xc0, SCREEN_BODY_HEIGHT);
            g_adPacket.sx = g_adPacket.dx = 0;
            (*g_videoOps.copy)(g_adPicture, r_vid.back, &g_adPacket, 1);
            g_adPacket.sx = 0x40;
            g_adPacket.dx = 0x100;
            (*g_videoOps.copy)(g_adPicture, r_vid.back, &g_adPacket, 1);
            ma_pop_high(1);
            hsave = r_vid.back->height;
            r_vid.back->height = SCREEN_BODY_HEIGHT;
            y = 0;
            i = g_adResume ? g_driver.adIndex : g_pages.start[g_pages.cur];
            next = g_pages.list + i;
            row7 = &g_hots[HOTS_ADVERTS + 2];
            cntz = 0;
            g_driver.adIndex = i;
            do {
                dr_hline(r_vid.back, 0x44, y + 1, 0xb8);
                y += 3;
                if (y > (SCREEN_BODY_HEIGHT - 1))
                    break;
                cntz++;
                row7++;
                row7->y1 = y - 2;
                row7->kind = HOT_REGION;
                item = *next++;
                row7->hitCode = item >= 0xcd8 ? 0 : i;
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
                dr_hline(r_vid.back, 0x44, y + 1, 0xb8);
            row7++;
            while (row7->hitCode >= 0) {
                row7->kind = HOT_DISABLED;
                row7++;
            }
            g_adRedraw = 0;
            r_vid.back->height = hsave;
            cu_ctl(CURSOR_SHOW);
            if (r_vid.copyToFront)
                bm_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), (SCREEN_BODY_HEIGHT - 1));
            sy_hover_reset();
        }
        while ((key = sy_wait_input(3000)) == 0)
            ;
        switch (key) {
        case KR_AD_CLOSE:
            break;
        case KR_AD_PREVIOUS:
            if (--g_pages.cur < 0)
                g_pages.cur = g_pages.n - 1;
            g_adIdxEndPageState = g_driver.adIndex - 1;
            if (g_adIdxEndPageState < 1)
                g_adIdxEndPageState = limit;
            diff = (SCREEN_BODY_HEIGHT - 1);
            while ((diff -= 9 * g_descHdr[g_pages.list[g_adIdxEndPageState]] + 3) > 0)
                if (--g_adIdxEndPageState < 1)
                    g_adIdxEndPageState = limit;
            g_driver.adIndex = g_adIdxEndPageState;
            g_adRedraw = 1;
            continue;
        case KR_AD_NEXT:
            if (++g_pages.cur >= g_pages.n)
                g_pages.cur = 0;
            g_driver.adIndex = g_adIdxEndPageState + (y > 0xc1 ? 0 : 1);
            g_adRedraw = 1;
            continue;
        default:
            g_adRedraw = 1;
        }
        return key;
    }
}


void av_close(void)
{
    ma_clear();
    g_pages.list = 0;
    g_adPicture = 0;
    mn_pop(1);
}
