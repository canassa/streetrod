#include <string.h>
#include <stdlib.h>
#include "MENU.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRAW.H"
#include "EDIT.H"
#include "HOTSPOT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MESSAGES.H"
#include "POPUP.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

static int g_drawnMinX;
static int g_drawnMinY;
static int g_drawnMaxX;
static int g_drawnMaxY;
static char *near wear_text(int wear);

int g_menuGrpHistoryList[4];
char g_hideSelectFrameVal;
struct ListDialog g_menuList;
char g_headerEditableState;
struct Hot *g_hotsCurMenu;
int g_textOfsMenu;
int g_hotMenuGroupState;

static int g_hotChain = 0;
static char g_wearText[12] = "(xx% worn)";

/* Draw a hot spot and the spots linked behind it (the chain through Hot.next masked with
   g_hotChain). Grows the touched rectangle g_drawnMinX..g_drawnMaxY. */

void mn_draw_hot(struct Hot *p)
{
    int colr;
    int w, h;
    char *str;
    int nxt;

    colr = 0;
    if (!g_hotChain && r_vid.copyToFront)
        cu_ctl(CURSOR_HIDE);
    do {
        switch (p->kind) {
        case HOT_BOX:
        case HOT_LINK_BOX:
            if (p->palIndex > 0) {
                if (p->palIndex != colr)
                    x_colors = bustyle->pal[colr = p->palIndex];
                dr_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
                if (x_colors.frame)
                    dr_bevel(r_vid.back, p->x1, p->y1, w, h);
                if (p->textOffset) {
                    str = g_txt + p->textOffset;
                    (*g_videoOps.drawText)(r_vid.back, ((w - da_text_width(str)) >> 1) + p->x1, p->y1, str);
                    dr_hline(r_vid.back, p->x1, p->y1 + 9, w);
                }
            }
        box:
            g_drawnMinX = p->x1 > g_drawnMinX ? g_drawnMinX : p->x1;
            g_drawnMaxX = p->x2 < g_drawnMaxX ? g_drawnMaxX : p->x2;
            g_drawnMinY = p->y1 > g_drawnMinY ? g_drawnMinY : p->y1;
            g_drawnMaxY = p->y2 < g_drawnMaxY ? g_drawnMaxY : p->y2;
            continue;
        case HOT_BUTTON:
            if (p->palIndex != colr)
                x_colors = bustyle->pal[colr = p->palIndex];
            dr_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
            dr_bevel(r_vid.back, p->x1, p->y1, w, h);
            str = g_txt + p->textOffset;
            (*g_videoOps.drawText)(r_vid.back, ((w - da_text_width(str)) >> 1) + p->x1, p->y1 + ((h - 8) >> 1), str);
            goto box;
        case HOT_TEXT:
            if (p->palIndex != colr)
                x_colors = bustyle->pal[colr = p->palIndex];
            (*g_videoOps.drawText)(r_vid.back, p->x1, p->y1, g_txt + p->textOffset);
            goto box;
        case HOT_CENTERED_TEXT:
            if (p->palIndex != colr)
                x_colors = bustyle->pal[colr = p->palIndex];
            str = g_txt + p->textOffset;
            (*g_videoOps.drawText)(r_vid.back, ((p->x2 - p->x1 - da_text_width(str)) >> 1) + p->x1,
                                   ((p->y2 - p->y1 - 8) >> 1) + p->y1, str);
            goto box;
        case HOT_EDIT_HEADER:
            g_headerEditableState = 1;
        case HOT_CENTERED_HEADER:
            if (p->palIndex != colr)
                x_colors = bustyle->pal[colr = p->palIndex];
            dr_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, 8);
            str = g_txt + p->textOffset;
            if (p->kind == HOT_CENTERED_HEADER)
                (*g_videoOps.drawText)(r_vid.back, ((w - da_text_width(str)) >> 1) + p->x1, p->y1, str);
            else
                (*g_videoOps.drawText)(r_vid.back, p->x1, p->y1, str);
            if (!g_menuList.header)
                g_menuList.header = p;
            goto box;
        case HOT_LIST_ROW:
            if (!g_menuList.rows++)
                g_menuList.lineBase = (int)p;
            goto box;
        case HOT_MENU:
        case HOT_LIST_MENU:
        case HOT_LIST_NO_FRAME:
        case HOT_END:
        case HOT_REGION:
        case HOT_DISABLED:
            continue;
        }
        break;
    } while ((nxt = p->next & g_hotChain) ? (p = &g_hots[nxt], 1) : 0);
    if (!g_hotChain && r_vid.copyToFront) {
        cu_ctl(CURSOR_SHOW);
        bm_copy_box(r_vid.back, r_vid.front, p->x1 - 3, p->y1 - 3, p->x2 + 6, p->y2 + 6);
    }
}

/* Push hot spot id onto the saved state stack (at most four deep) and draw it: the
   current entry globals are reset, mn_draw_hot draws the linked spot, and the touched area is copied
   from the back buffer to the front when the screen shadow is on */

void mn_push(enum ScreenId id)
{
    struct Hot *p;
    struct Colors saved;

    if (g_menuLevel < 4) {
        cu_ctl(CURSOR_HIDE);
        g_menuGrpHistoryList[g_menuLevel++] = g_hotMenuGroupState = id;
        g_boxSpotHitData.x1 = g_boxSpotHitData.x2 = g_boxSpotHitData.y1 = g_boxSpotHitData.y2 = -1;
        g_textOfsMenu = (p = &g_hots[g_groupHot[id]])->textOffset;
        g_hotsCurMenu = (p + 1)->kind > HOT_LAST_NONCLICKABLE ? p + 1 : 0;
        g_menuList.rows = g_menuList.lineBase = g_menuList.header = 0;
        g_headerEditableState = 0;
        g_hideSelectFrameVal = p->kind == HOT_LIST_NO_FRAME;
        saved = x_colors;
        g_hotChain = -1;
        g_drawnMinX = g_drawnMinY = 1000;
        g_drawnMaxX = g_drawnMaxY = -1;
        if (p->next)
            mn_draw_hot(&g_hots[p->next]);
        g_hotChain = 0;
        x_colors = saved;
        cu_ctl(CURSOR_SHOW);
        if (r_vid.copyToFront && g_drawnMaxX >= 0)
            bm_copy_box(r_vid.back, r_vid.front, g_drawnMinX - 3, g_drawnMinY - 3, g_drawnMaxX + 6, g_drawnMaxY + 6);
        sy_keymap_set(id);
        sy_hover_reset();
    }
}

/* Pop the saved state stack: close the current entry; when keep is set and an entry remains,
   reopen the one below it, otherwise reset the entry globals to "none" */

void mn_pop(int keep)
{
    if (g_menuLevel) {
        sy_keymap_restore(0);
        g_hotsCurMenu = 0;
        if (--g_menuLevel && keep) {
            --g_menuLevel;
            mn_push(g_menuGrpHistoryList[g_menuLevel]);
        } else {
            g_hotMenuGroupState = 0;
            g_hotsCurMenu = 0;
            g_boxSpotHitData.x1 = g_boxSpotHitData.y1 = g_boxSpotHitData.x2 = g_boxSpotHitData.y2 = g_cursHoverCodeVal =
                -1;
        }
    }
}


void mn_reset(void)
{
    g_menuLevel = g_textOfsMenu = 0;
    g_hotsCurMenu = 0;
    g_boxSpotHitData.x1 = g_boxSpotHitData.y1 = g_boxSpotHitData.x2 = g_boxSpotHitData.y2 = g_cursHoverCodeVal = 0;
}

/* Bounding box of hot spot id and the spots linked after it (through f10), widened by 3
   and clipped to the 320 x 200 screen. *x and *y receive the top left corner, *w and *h the size */

void mn_bbox(enum ScreenId id, int *x, int *y, int *w, int *h)
{
    struct Hot *p;

    p = &g_hots[g_groupHot[id]];
    g_drawnMinX = g_drawnMinY = 1000;
    g_drawnMaxX = g_drawnMaxY = 1;
    while (p->next) {
        p = &g_hots[p->next];
        g_drawnMinX = p->x1 > g_drawnMinX ? g_drawnMinX : p->x1;
        g_drawnMaxX = p->x2 < g_drawnMaxX ? g_drawnMaxX : p->x2;
        g_drawnMinY = p->y1 > g_drawnMinY ? g_drawnMinY : p->y1;
        g_drawnMaxY = p->y2 < g_drawnMaxY ? g_drawnMaxY : p->y2;
    }
    if ((*x = g_drawnMinX - 3) < 0)
        *x = 0;
    if ((*y = g_drawnMinY - 3) < 0)
        *y = 0;
    if ((*w = g_drawnMaxX + 3) > 320)
        *w = 320;
    if ((*h = g_drawnMaxY + 3) > 200)
        *h = 200;
    *w -= *x;
    *h -= *y;
}

/* Copy the screen area around hot spot id (see mn_bbox, which updates *px and *py to the
   box corner) into a bitmap of the given mode and return it. The x range is rounded to r_vid.mask */

struct Bitmap far *mn_grab(enum ScreenId id, int *px, int *py, int mode)
{
    int x;
    int y1;
    int x2;
    struct Bitmap far *bits9;
    int h3;
    int w4;
    int y2;

    mn_bbox(id, px, py, &w4, &h3);
    x = *px;
    y1 = *py;
    x2 = (x + w4 + ~r_vid.mask & r_vid.mask) > 320 ? 320 : (x + w4 + ~r_vid.mask & r_vid.mask);
    y2 = y1 + h3 > 200 ? 200 : y1 + h3;
    x &= r_vid.mask;
    bits9 = ar_bm(x2 - x, y2 - y1, 0, mode);
    return pu_grab(*px, y1, w4, h3, bits9);
}

/* Find the hot spot of the current menu (list at g_hotsCurMenu) under the point (x, y). On a hit
   the spot's box goes to g_boxSpotHitData and its id is returned; on a miss g_boxSpotHitData gets the free
   rectangle around the point and 0 is returned */

int mn_hit(int x, int y)
{
    struct Hot *p;
    enum HotKind kind;

    p = g_hotsCurMenu;
    if (p == 0) {
        g_boxSpotHitData.x1 = g_boxSpotHitData.y1 = -1;
        g_boxSpotHitData.x2 = g_boxSpotHitData.y2 = 1000;
        g_cursHoverCodeVal = 0;
        return 0;
    }
    g_boxSpotHitData.x1 = g_boxSpotHitData.y1 = 0;
    g_boxSpotHitData.x2 = 319;
    g_boxSpotHitData.y2 = 199;
    while ((kind = p->kind) > HOT_LAST_NONCLICKABLE) {
        if (kind == HOT_DISABLED) {
            p++;
            continue;
        }
        if (p->x1 > x) {
            g_boxSpotHitData.x2 = p->x1;
            break;
        }
        if (p->x2 <= x) {
            if (p->x2 > g_boxSpotHitData.x1)
                g_boxSpotHitData.x1 = p->x2;
        next:
            if (kind == HOT_LINK_BOX)
                p = &g_hots[p->id];
            else
                p++;
        } else {
            if (p->y1 - 3 > y) {
                if (p->y1 - 3 < g_boxSpotHitData.y2)
                    g_boxSpotHitData.y2 = p->y1 - 3;
            } else if (p->y2 <= y) {
                if (p->y2 > g_boxSpotHitData.y1)
                    g_boxSpotHitData.y1 = p->y2;
            } else {
                if (kind == HOT_LINK_BOX) {
                    p++;
                    continue;
                }
                g_boxSpotHitData.x1 = p->x1;
                g_boxSpotHitData.x2 = p->x2;
                g_boxSpotHitData.y1 = p->y1;
                g_boxSpotHitData.y2 = p->y2;
                g_curHoverPointer = p;
                return g_cursHoverCodeVal = p->id;
            }
            goto next;
        }
    }
    g_curHoverPointer = 0;
    return g_cursHoverCodeVal = 0;
}

/* Grab the screen area around hot spot id, show the spot (waiting for a key through
   mn_wait), put the saved area back and return the key */

int mn_modal(enum ScreenId id)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    bm1 = mn_grab(id, &x, &y1, ARENA_HIGH_STACK);
    res = mn_wait(id);
    pu_show(bm1, x, y1);
    ma_pop_high(1);
    return res;
}


int mn_wait(enum ScreenId a)
{
    int key;

    mn_push(a);
    while ((key = sy_wait_input(0x7d00)) == 0)
        ;
    mn_pop(1);
    return key;
}

/* Run the mn_list dialog over a grabbed screen area (see mn_modal) and restore the area
   afterwards; 0x3ea when *list is zero */

int mn_modal_list(enum ScreenId a, int b, int *c, int *list, char *buf, char **d)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    if (*list == 0)
        return KR_EMPTY_LIST;
    bm1 = mn_grab(a, &x, &y1, ARENA_HIGH_STACK);
    res = mn_list(a, b, c, list, buf, d);
    pu_show(bm1, x, y1);
    ma_pop_high(1);
    return res;
}

static char g_wearPad[4] = "   ";

/* Wear text for a part: "(new)" when wear is 0, else "(xx% worn)" built in place in g_wearText
   (initialised data "(xx% worn)") with the number written over the xx */

static char *near wear_text(int wear)
{
    char *p;

    if (wear == 0)
        return "(`new`)";
    g_wearText[3] = ' ';
    itoa(wear, p = g_wearText + 1, 10);
    p[strlen(p)] = '%';
    return g_wearText;
}

int g_cgaPicMap[277] = {
    0,   1,   2,   3,   5,   6,   7,   4,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
    22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,
    44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,
    66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,
    88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
    132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197,
    198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241,
    242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
    264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276};

/* List dialog. Shows the entries list[1..list[0]] (offsets into the string table g_desc,
   or, when negative, d[-entry - 1]) in the windows of the current menu, optionally with a column c
   of x offsets and wear values buf[1..]; arrow/page keys scroll, a positive key2 picks an entry,
   a hotn spot (g_menuList.header) shows the picked text and is edited in place on Enter.
   Returns the key2 that left the dialog, 0x3ea when the list is empty. */

int mn_list(enum ScreenId kind, int sel, int *c, int *list, char *buf, char **d)
{
    int *pc1, *c1, curl, idx2, n9, ww9, wy7, wx8;
    struct Win *w;
    int *lpk, wh9, x;
    char *bufpl;
    int i, x1;
    struct Hot *hotn;
    int nw5, y1, x2, key2, y2, page, count5, bottom9, sv5;
    char *bpl, *buf1;
    int *lp11, right;
    char *labeln;
    int topk;
    char wear;

    bufpl = 0;
    topk = 0;
    curl = -1;
    if (*list == 0)
        return KR_EMPTY_LIST;
    sel--;
    lp11 = list + 1;
    count5 = *list;
    if (buf)
        buf1 = buf + 1;
    if (c)
        c1 = c + 1;
    mn_push(kind);
    page = g_menuList.rows;
    g_menuList.list = list;
    g_menuList.selection = sel + 1;
    if (count5 > page)
        topk = (sel + page > count5) ? count5 - page : sel;
    if (hotn = g_menuList.header) {
        bufpl = g_txt + hotn->textOffset;
        n9 = (hotn->x2 - hotn->x1) / 6;
        if (!g_headerEditableState) {
            strcpy(bufpl, g_desc + lp11[sel]);
            if (buf && (wear = buf1[sel]) != -128) {
                strcat(bufpl, g_wearPad);
                strcat(bufpl, wear_text(wear));
            }
            mn_draw_hot(hotn);
        }
    }
    ww9 = ((struct Win *)g_menuList.lineBase)->tx - (wx8 = ((struct Win *)g_menuList.lineBase)->w);
    right = wx8 + ww9 - 1;
    wh9 = ((struct Win *)g_menuList.lineBase)->ty - (wy7 = ((struct Win *)g_menuList.lineBase)->h);
    bottom9 = page * wh9 + wy7 - 1;
    idx2 = ((struct Win *)g_menuList.lineBase)->lines;
    bustyle->c2d = bustyle->pal[idx2];
    sv5 = bustyle->pal[idx2].line;
    for (;;) {
        x_colors = bustyle->pal[idx2];
        w = (struct Win *)g_menuList.lineBase;
        cu_ctl(CURSOR_HIDE);
        i = (count5 - topk > page) ? page : count5 - topk;
        lpk = lp11 + topk;
        pc1 = c ? c1 + topk : 0;
        bpl = buf ? buf1 + topk : 0;
        for (; i--; pc1++, w++, lpk++, bpl++) {
            labeln = (*lpk > -1) ? g_desc + *lpk : d[-*lpk - 1];
            x = (c ? *pc1 : 0) + w->w;
            (*g_videoOps.drawText)(r_vid.back, x, w->h, labeln);
            dr_fill_rect(r_vid.back, g_penPoint.x - 1, g_penPoint.y, w->tx - g_penPoint.x + 1, 8);
            if (buf && (wear = *bpl) != -128)
                (*g_videoOps.drawText)(r_vid.back, w->tx - strlen(g_wearText) * 6, w->h, wear_text(wear));
        }
        if (!g_hideSelectFrameVal) {
            w = (struct Win *)(0x12 * sel + g_menuList.lineBase);
            x_colors = bustyle->pal[w->lines];
            dr_frame(r_vid.back, w->w, w->h, ww9, 8);
        }
        cu_ctl(CURSOR_SHOW);
        if (r_vid.copyToFront)
            bm_copy_box(r_vid.back, r_vid.front, wx8 - 1, wy7 - 1, right + 1, bottom9 + 1);
    restore:
        x_colors = bustyle->pal[idx2];
        if (r_vid.mode == VID_HERCULES)
            x_colors.text = 0;
        while ((key2 = sy_wait_input(0)) == 0) {
            i = g_cursHoverCodeVal > 0 && topk + g_cursHoverCodeVal <= count5;
            if (!i && curl < 0)
                continue;
            if (curl == g_cursHoverCodeVal - 1)
                continue;
            cu_ctl(CURSOR_HIDE);
            x1 = y1 = 1000;
            x2 = y2 = -1;
            if (i) {
                w = (struct Win *)(0x12 * (nw5 = g_cursHoverCodeVal - 1) + g_menuList.lineBase);
                x1 = w->w;
                y1 = w->h;
                x2 = w->tx;
                if (kind != KM_CAR_INFO)
                    (*g_videoOps.recolorRegion)(r_vid.back, x_colors.text, sv5, x1, y1, x2, w->h + 8);
                y2 = w->ty;
            } else
                nw5 = -1;
            if (curl >= 0) {
                w = (struct Win *)(0x12 * curl + g_menuList.lineBase);
                if (kind != KM_CAR_INFO)
                    (*g_videoOps.recolorRegion)(r_vid.back, sv5, x_colors.text, w->w, w->h, w->tx, w->h + 8);
                x1 = (w->w > x1) ? x1 : w->w;
                y1 = (w->h > y1) ? y1 : w->h;
                x2 = (w->tx < x2) ? x2 : w->tx;
                y2 = (w->ty < y2) ? y2 : w->ty;
            }
            cu_ctl(CURSOR_SHOW);
            if (r_vid.copyToFront)
                bm_copy_box(r_vid.back, r_vid.front, x1, y1, x2, y2);
            curl = nw5;
        }
        if (key2 > 0) {
            if (topk + key2 <= count5) {
                g_menuList.selection = topk + key2;
                i = topk + key2 - 1;
                if (hotn) {
                    if (!g_headerEditableState || i == sel) {
                        strcpy(bufpl, g_desc + lp11[i]);
                        if (kind == KM_SAVE && !strcmp(g_blankName, bufpl))
                            strcpy(bufpl, "\0");
                        if (buf && (wear = buf1[i]) != -128) {
                            strcat(bufpl, g_wearPad);
                            strcat(bufpl, wear_text(wear));
                        }
                        mn_draw_hot(hotn);
                    }
                }
                if (!g_hideSelectFrameVal) {
                    cu_ctl(CURSOR_HIDE);
                    w = (struct Win *)(0x12 * sel + g_menuList.lineBase);
                    x_colors = bustyle->pal[idx2];
                    nw5 = x_colors.line;
                    x_colors.line = x_colors.textBg;
                    dr_frame(r_vid.back, x1 = w->w, y1 = w->h, ww9, 8);
                    x2 = x1-- + ww9;
                    y2 = y1-- + 8;
                    x_colors.line = nw5;
                    w = (struct Win *)(0x12 * i + g_menuList.lineBase);
                    dr_frame(r_vid.back, w->w, w->h, ww9, 8);
                    cu_ctl(CURSOR_SHOW);
                    if (r_vid.copyToFront)
                        bm_copy_box(r_vid.back, r_vid.front, (w->w - 1 > x1) ? x1 : w->w - 1,
                                    (w->h - 1 > y1) ? y1 : w->h - 1, (w->w + ww9 + 2 < x2) ? x2 : w->w + ww9 + 2,
                                    (w->h + wh9 + 2 < y2) ? y2 : w->h + wh9 + 2);
                }
                sel = i;
            }
            goto restore;
        }
        switch (key2) {
        default:
            mn_pop(1);
            return key2;
        case KR_EDIT_HEADER:
            if (kind == KM_SAVE && !strcmp(g_blankName, bufpl))
                strcpy(bufpl, "\0");
            x_colors = bustyle->pal[hotn->palIndex];
            mn_push(KM_MESSAGE);
            ed_line(hotn->x1, hotn->y1, bufpl, n9);
            mn_pop(1);
            continue;
        case KR_SCROLL_UP:
            if (topk)
                topk--;
            continue;
        case KR_PAGE_UP:
            if ((topk -= page) < 0)
                topk = 0;
            continue;
        case KR_PAGE_DOWN:
            i = topk;
            topk += page;
            if (topk >= count5)
                topk = count5 - 1;
            goto scroll;
        case KR_SCROLL_DOWN:
            i = topk;
            if (i + 1 < count5)
                topk++;
        scroll:
            if (i < topk) {
                if ((nw5 = count5 - topk) < page) {
                    w = (struct Win *)(0x12 * nw5 + g_menuList.lineBase);
                    dr_fill_rect(r_vid.back, w->w, w->h, ww9, ((topk - i > page - nw5) ? page - nw5 : topk - i) * wh9);
                }
            }
        }
    }
}

/* Accessor, returns g_menuList.selection */

int mn_sel(void)
{
    return g_menuList.selection;
}
