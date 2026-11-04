#include <string.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "MENU.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRAW.H"
#include "EDIT.H"
#include "HOTSPOT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MESSAGES.H"
#include "POPUP.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_drawnMinX;                         /* DS:6C56 */
static int g_drawnMinY;                         /* DS:6C58 */
static int g_drawnMaxX;                         /* DS:6C5A */
static int g_drawnMaxY;                         /* DS:6C5C */
static char * near wear_text(int wear);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
void (far *g_drvRecolorRegion)(struct Bitmap far *, int, int, int, int, int, int);
int bq824e;
int ai8b86y[4];
char av8b98;
int a_8b9ap;
int a_8b9ca;
int *az8b9ez;
int x_8ba0;
struct Hot *az8ba2t;
char g_headerEditableState;
struct Hot *av8ba6v;
int ai8bacx;
int g_hotMenuGroupState;

int g_hotChain = 0;   /* DS:07A8 */
char g_wearText[12] = "(xx% worn)";   /* DS:07AA */

/* menu_draw_hot @ 0F38:465E (linear 0x139DE, 1032 bytes)

   Draw a hot spot and the spots linked behind it (the chain through Hot.f10 masked with
   g_hotChain), by kind: 4/12 filled box with optional centred text, 5/6/10 text only, 7/8 header bar,
   13 first-time marker. Grows the touched rectangle g_drawnMinX..g_drawnMaxY. */

void menu_draw_hot(struct Hot *p)
{
    int colr;
    int w, h;
    char *str;
    int nxt;

    colr = 0;
    if (!g_hotChain && r_vid.f16)
        cursor_ctl(-3);
    do {
        switch (p->f0) {
        case 4:
        case 12:
            if (p->f2 > 0) {
                if (p->f2 != colr)
                    x_colors = bustyle->pal[colr = p->f2];
                draw_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
                if (x_colors.frame)
                    draw_bevel(r_vid.back, p->x1, p->y1, w, h);
                if (p->f4) {
                    str = g_239e + p->f4;
                    (*a_78aem)(r_vid.back, ((w - dash_text_width(str)) >> 1) + p->x1, p->y1, str);
                    draw_hline(r_vid.back, p->x1, p->y1 + 9, w);
                }
            }
        box:
            g_drawnMinX = p->x1 > g_drawnMinX ? g_drawnMinX : p->x1;
            g_drawnMaxX = p->x2 < g_drawnMaxX ? g_drawnMaxX : p->x2;
            g_drawnMinY = p->y1 > g_drawnMinY ? g_drawnMinY : p->y1;
            g_drawnMaxY = p->y2 < g_drawnMaxY ? g_drawnMaxY : p->y2;
            continue;
        case 10:
            if (p->f2 != colr)
                x_colors = bustyle->pal[colr = p->f2];
            draw_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
            draw_bevel(r_vid.back, p->x1, p->y1, w, h);
            str = g_239e + p->f4;
            (*a_78aem)(r_vid.back, ((w - dash_text_width(str)) >> 1) + p->x1,
                      p->y1 + ((h - 8) >> 1), str);
            goto box;
        case 5:
            if (p->f2 != colr)
                x_colors = bustyle->pal[colr = p->f2];
            (*a_78aem)(r_vid.back, p->x1, p->y1, g_239e + p->f4);
            goto box;
        case 6:
            if (p->f2 != colr)
                x_colors = bustyle->pal[colr = p->f2];
            str = g_239e + p->f4;
            (*a_78aem)(r_vid.back, ((p->x2 - p->x1 - dash_text_width(str)) >> 1) + p->x1,
                      ((p->y2 - p->y1 - 8) >> 1) + p->y1, str);
            goto box;
        case 7:
            g_headerEditableState = 1;
        case 8:
            if (p->f2 != colr)
                x_colors = bustyle->pal[colr = p->f2];
            draw_fill_rect(r_vid.back, p->x1, p->y1, w = p->x2 - p->x1, 8);
            str = g_239e + p->f4;
            if (p->f0 == 8)
                (*a_78aem)(r_vid.back, ((w - dash_text_width(str)) >> 1) + p->x1, p->y1, str);
            else
                (*a_78aem)(r_vid.back, p->x1, p->y1, str);
            if (!az8ba2t)
                az8ba2t = p;
            goto box;
        case 13:
            if (!a_8b9ca++)
                a_8b9ap = (int)p;
            goto box;
        case 1:
        case 2:
        case 3:
        case 9:
        case 11:
        case 14:
            continue;
        }
        break;
    } while ((nxt = p->f10 & g_hotChain) ? (p = &g_hots[nxt], 1) : 0);
    if (!g_hotChain && r_vid.f16) {
        cursor_ctl(-1);
        bitmap_copy_box(r_vid.back, r_vid.front, p->x1 - 3, p->y1 - 3, p->x2 + 6, p->y2 + 6);
    }
}

/* menu_push @ 0F38:4A66 (linear 0x13DE6, 315 bytes)

   Push hot spot id onto the saved state stack (at most four deep) and draw it: the
   current entry globals are reset, menu_draw_hot draws the linked spot, and the touched area is copied
   from the back buffer to the front when the screen shadow is on */

void menu_push(int id)
{
    struct Hot *p;
    struct Colors saved;

    if (g_menuLevel < 4) {
        cursor_ctl(-3);
        ai8b86y[g_menuLevel++] = g_hotMenuGroupState = id;
        ao8b90v.x1 = ao8b90v.x2 = ao8b90v.y1 = ao8b90v.y2 = -1;
        ai8bacx = (p = &g_hots[g_groupHot[id]])->f4;
        av8ba6v = (p + 1)->f0 > 9 ? p + 1 : 0;
        a_8b9ca = a_8b9ap = az8ba2t = 0;
        g_headerEditableState = 0;
        av8b98 = p->f0 == 3;
        saved = x_colors;
        g_hotChain = -1;
        g_drawnMinX = g_drawnMinY = 1000;
        g_drawnMaxX = g_drawnMaxY = -1;
        if (p->f10)
            menu_draw_hot(&g_hots[p->f10]);
        g_hotChain = 0;
        x_colors = saved;
        cursor_ctl(-1);
        if (r_vid.f16 && g_drawnMaxX >= 0)
            bitmap_copy_box(r_vid.back, r_vid.front, g_drawnMinX - 3, g_drawnMinY - 3, g_drawnMaxX + 6, g_drawnMaxY + 6);
        sys_keymap_set(id);
        sys_hover_reset();
    }
}

/* menu_pop @ 0F38:4BA1 (linear 0x13F21, 94 bytes)

   Pop the saved state stack: close the current entry; when keep is set and an entry remains,
   reopen the one below it, otherwise reset the entry globals to "none" */

void menu_pop(int keep)
{
    if (g_menuLevel) {
        sys_keymap_restore(0);
        av8ba6v = 0;
        if (--g_menuLevel && keep) {
            --g_menuLevel;
            menu_push(ai8b86y[g_menuLevel]);
        } else {
            g_hotMenuGroupState = 0;
            av8ba6v = 0;
            ao8b90v.x1 = ao8b90v.y1 = ao8b90v.x2 = ao8b90v.y2 = az8baa = -1;
        }
    }
}

/* menu_reset @ 0F38:4BFF (linear 0x13F7F, 27 bytes) */

void menu_reset(void)
{
    g_menuLevel = ai8bacx = 0;
    av8ba6v = 0;
    ao8b90v.x1 = ao8b90v.y1 = ao8b90v.x2 = ao8b90v.y2 = az8baa = 0;
}

/* menu_bbox @ 0F38:4C1A (linear 0x13F9A, 246 bytes)

   Bounding box of hot spot id and the spots linked after it (through f10), widened by 3
   and clipped to the 320 x 200 screen. *x and *y receive the top left corner, *w and *h the size */

void menu_bbox(int id, int *x, int *y, int *w, int *h)
{
    struct Hot *p;

    p = &g_hots[g_groupHot[id]];
    g_drawnMinX = g_drawnMinY = 1000;
    g_drawnMaxX = g_drawnMaxY = 1;
    while (p->f10) {
        p = &g_hots[p->f10];
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

/* menu_grab @ 0F38:4D10 (linear 0x14090, 157 bytes)

   Copy the screen area around hot spot id (see menu_bbox, which updates *px and *py to the
   box corner) into a bitmap of the given mode and return it. The x range is rounded to r_vid.mask */

struct Bitmap far *menu_grab(int id, int *px, int *py, int mode)
{
    int x;
    int y1;
    int x2;
    struct Bitmap far *bits9;
    int h3;
    int w4;
    int y2;

    menu_bbox(id, px, py, &w4, &h3);
    x = *px;
    y1 = *py;
    x2 = (x + w4 + ~r_vid.mask & r_vid.mask) > 320 ? 320 : (x + w4 + ~r_vid.mask & r_vid.mask);
    y2 = y1 + h3 > 200 ? 200 : y1 + h3;
    x &= r_vid.mask;
    bits9 = F_1934E(x2 - x, y2 - y1, 0, mode);
    return popup_grab(*px, y1, w4, h3, bits9);
}

/* menu_hit @ 0F38:4DAD (linear 0x1412D, 276 bytes)

   Find the hot spot of the current menu (list at av8ba6v) under the point (x, y). On a hit
   the spot's box goes to ao8b90v and its id (f6) is returned; on a miss ao8b90v gets the free
   rectangle around the point and 0 is returned */

int menu_hit(int x, int y)
{
    struct Hot *p;
    int kind;

    p = av8ba6v;
    if (p == 0) {
        ao8b90v.x1 = ao8b90v.y1 = -1;
        ao8b90v.x2 = ao8b90v.y2 = 1000;
        az8baa = 0;
        return 0;
    }
    ao8b90v.x1 = ao8b90v.y1 = 0;
    ao8b90v.x2 = 319;
    ao8b90v.y2 = 199;
    while ((kind = p->f0) > 9) {
        if (kind == 14) {
            p++;
            continue;
        }
        if (p->x1 > x) {
            ao8b90v.x2 = p->x1;
            break;
        }
        if (p->x2 <= x) {
            if (p->x2 > ao8b90v.x1)
                ao8b90v.x1 = p->x2;
        next:
            if (kind == 12)
                p = &g_hots[p->f6];
            else
                p++;
        } else {
            if (p->y1 - 3 > y) {
                if (p->y1 - 3 < ao8b90v.y2)
                    ao8b90v.y2 = p->y1 - 3;
            } else if (p->y2 <= y) {
                if (p->y2 > ao8b90v.y1)
                    ao8b90v.y1 = p->y2;
            } else {
                if (kind == 12) {
                    p++;
                    continue;
                }
                ao8b90v.x1 = p->x1;
                ao8b90v.x2 = p->x2;
                ao8b90v.y1 = p->y1;
                ao8b90v.y2 = p->y2;
                at8ba8v = p;
                return az8baa = p->f6;
            }
            goto next;
        }
    }
    at8ba8v = 0;
    return az8baa = 0;
}

/* menu_modal @ 0F38:4EC1 (linear 0x14241, 83 bytes)

   Grab the screen area around hot spot id, show the spot (waiting for a key through
   menu_wait), put the saved area back and return the key */

int menu_modal(int id)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    bm1 = menu_grab(id, &x, &y1, 2);
    res = menu_wait(id);
    popup_show(bm1, x, y1);
    F_19438(1);
    return res;
}

/* menu_wait @ 0F38:4F14 (linear 0x14294, 59 bytes) */

int menu_wait(int a)
{
    int key;

    menu_push(a);
    while ((key = sys_wait_input(0x7d00)) == 0)
        ;
    menu_pop(1);
    return key;
}

/* F_142CF @ 0F38:4F4F (linear 0x142CF, 114 bytes)

   Run the menu_list dialog over a grabbed screen area (see menu_modal) and restore the area
   afterwards; 0x3ea when *list is zero */

int F_142CF(int a, int b, int *c, int *list, char *buf, char **d)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    if (*list == 0)
        return 0x3ea;
    bm1 = menu_grab(a, &x, &y1, 2);
    res = menu_list(a, b, c, list, buf, d);
    popup_show(bm1, x, y1);
    F_19438(1);
    return res;
}

char g_wearPad[4] = "   ";   /* DS:07BE */

/* wear_text @ 0F38:4FC1 (linear 0x14341, 77 bytes)

   Wear text for a part: "(new)" when wear is 0, else "(xx% worn)" built in place in g_wearText
   (initialised data "(xx% worn)") with the number written over the xx */

static char * near wear_text(int wear)
{
    char *p;

    if (wear == 0)
        return "(`new`)";
    g_wearText[3] = ' ';
    itoa(wear, p = g_wearText + 1, 10);
    p[strlen(p)] = '%';
    return g_wearText;
}

int g_cgaPicMap[277] = {0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276};   /* DS:07C6 */

/* menu_list @ 0F38:500E (linear 0x1438E, 2301 bytes)

   List dialog. Shows the entries list[1..list[0]] (offsets into the string table g_desc,
   or, when negative, d[-entry - 1]) in the windows of the current menu, optionally with a column c
   of x offsets and wear values buf[1..]; arrow/page keys scroll, a positive key2 picks an entry,
   a hotn spot (az8ba2t) shows the picked text and is edited in place on Enter.
   Returns the key2 that left the dialog, 0x3ea when the list is empty. */

int menu_list(int kind, int sel, int *c, int *list, char *buf, char **d)
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
        return 0x3ea;
    sel--;
    lp11 = list + 1;
    count5 = *list;
    if (buf)
        buf1 = buf + 1;
    if (c)
        c1 = c + 1;
    menu_push(kind);
    page = a_8b9ca;
    az8b9ez = list;
    x_8ba0 = sel + 1;
    if (count5 > page)
        topk = (sel + page > count5) ? count5 - page : sel;
    if (hotn = az8ba2t) {
        bufpl = g_239e + hotn->f4;
        n9 = (hotn->x2 - hotn->x1) / 6;
        if (!g_headerEditableState) {
            strcpy(bufpl, g_desc + lp11[sel]);
            if (buf && (wear = buf1[sel]) != -128) {
                strcat(bufpl, g_wearPad);
                strcat(bufpl, wear_text(wear));
            }
            menu_draw_hot(hotn);
        }
    }
    ww9 = ((struct Win *)a_8b9ap)->tx - (wx8 = ((struct Win *)a_8b9ap)->w);
    right = wx8 + ww9 - 1;
    wh9 = ((struct Win *)a_8b9ap)->ty - (wy7 = ((struct Win *)a_8b9ap)->h);
    bottom9 = page * wh9 + wy7 - 1;
    idx2 = ((struct Win *)a_8b9ap)->lines;
    bustyle->c2d = bustyle->pal[idx2];
    sv5 = bustyle->pal[idx2].line;
    for (;;) {
        x_colors = bustyle->pal[idx2];
        w = (struct Win *)a_8b9ap;
        cursor_ctl(-3);
        i = (count5 - topk > page) ? page : count5 - topk;
        lpk = lp11 + topk;
        pc1 = c ? c1 + topk : 0;
        bpl = buf ? buf1 + topk : 0;
        for (; i--; pc1++, w++, lpk++, bpl++) {
            labeln = (*lpk > -1) ? g_desc + *lpk : d[-*lpk - 1];
            x = (c ? *pc1 : 0) + w->w;
            (*a_78aem)(r_vid.back, x, w->h, labeln);
            draw_fill_rect(r_vid.back, g824cd - 1, bq824e, w->tx - g824cd + 1, 8);
            if (buf && (wear = *bpl) != -128)
                (*a_78aem)(r_vid.back, w->tx - strlen(g_wearText) * 6, w->h, wear_text(wear));
        }
        if (!av8b98) {
            w = (struct Win *)(0x12 * sel + a_8b9ap);
            x_colors = bustyle->pal[w->lines];
            draw_frame(r_vid.back, w->w, w->h, ww9, 8);
        }
        cursor_ctl(-1);
        if (r_vid.f16)
            bitmap_copy_box(r_vid.back, r_vid.front, wx8 - 1, wy7 - 1, right + 1, bottom9 + 1);
restore:
        x_colors = bustyle->pal[idx2];
        if (r_vid.mode == -4)
            x_colors.text = 0;
        while ((key2 = sys_wait_input(0)) == 0) {
            i = az8baa > 0 && topk + az8baa <= count5;
            if (!i && curl < 0)
                continue;
            if (curl == az8baa - 1)
                continue;
            cursor_ctl(-3);
            x1 = y1 = 1000;
            x2 = y2 = -1;
            if (i) {
                w = (struct Win *)(0x12 * (nw5 = az8baa - 1) + a_8b9ap);
                x1 = w->w;
                y1 = w->h;
                x2 = w->tx;
                if (kind != 0x26)
                    (*g_drvRecolorRegion)(r_vid.back, x_colors.text, sv5, x1, y1, x2, w->h + 8);
                y2 = w->ty;
            } else
                nw5 = -1;
            if (curl >= 0) {
                w = (struct Win *)(0x12 * curl + a_8b9ap);
                if (kind != 0x26)
                    (*g_drvRecolorRegion)(r_vid.back, sv5, x_colors.text, w->w, w->h, w->tx, w->h + 8);
                x1 = (w->w > x1) ? x1 : w->w;
                y1 = (w->h > y1) ? y1 : w->h;
                x2 = (w->tx < x2) ? x2 : w->tx;
                y2 = (w->ty < y2) ? y2 : w->ty;
            }
            cursor_ctl(-1);
            if (r_vid.f16)
                bitmap_copy_box(r_vid.back, r_vid.front, x1, y1, x2, y2);
            curl = nw5;
        }
        if (key2 > 0) {
            if (topk + key2 <= count5) {
                x_8ba0 = topk + key2;
                i = topk + key2 - 1;
                if (hotn) {
                    if (!g_headerEditableState || i == sel) {
                        strcpy(bufpl, g_desc + lp11[i]);
                        if (kind == 5 && !strcmp(g_blankName, bufpl))
                            strcpy(bufpl, "\0");
                        if (buf && (wear = buf1[i]) != -128) {
                            strcat(bufpl, g_wearPad);
                            strcat(bufpl, wear_text(wear));
                        }
                        menu_draw_hot(hotn);
                    }
                }
                if (!av8b98) {
                    cursor_ctl(-3);
                    w = (struct Win *)(0x12 * sel + a_8b9ap);
                    x_colors = bustyle->pal[idx2];
                    nw5 = x_colors.line;
                    x_colors.line = x_colors.textBg;
                    draw_frame(r_vid.back, x1 = w->w, y1 = w->h, ww9, 8);
                    x2 = x1-- + ww9;
                    y2 = y1-- + 8;
                    x_colors.line = nw5;
                    w = (struct Win *)(0x12 * i + a_8b9ap);
                    draw_frame(r_vid.back, w->w, w->h, ww9, 8);
                    cursor_ctl(-1);
                    if (r_vid.f16)
                        bitmap_copy_box(r_vid.back, r_vid.front,
                            (w->w - 1 > x1) ? x1 : w->w - 1,
                            (w->h - 1 > y1) ? y1 : w->h - 1,
                            (w->w + ww9 + 2 < x2) ? x2 : w->w + ww9 + 2,
                            (w->h + wh9 + 2 < y2) ? y2 : w->h + wh9 + 2);
                }
                sel = i;
            }
            goto restore;
        }
        switch (key2) {
        default:
            menu_pop(1);
            return key2;
        case -6:
            if (kind == 5 && !strcmp(g_blankName, bufpl))
                strcpy(bufpl, "\0");
            x_colors = bustyle->pal[hotn->f2];
            menu_push(0x18);
            edit_line(hotn->x1, hotn->y1, bufpl, n9);
            menu_pop(1);
            continue;
        case -7:
            if (topk)
                topk--;
            continue;
        case -8:
            if ((topk -= page) < 0)
                topk = 0;
            continue;
        case -9:
            i = topk;
            topk += page;
            if (topk >= count5)
                topk = count5 - 1;
            goto scroll;
        case -10:
            i = topk;
            if (i + 1 < count5)
                topk++;
scroll:
            if (i < topk) {
                if ((nw5 = count5 - topk) < page) {
                    w = (struct Win *)(0x12 * nw5 + a_8b9ap);
                    draw_fill_rect(r_vid.back, w->w, w->h, ww9,
                        ((topk - i > page - nw5) ? page - nw5 : topk - i) * wh9);
                }
            }
        }
    }
}

/* F_14C8B @ 0F38:590B (linear 0x14C8B, 4 bytes)

   Accessor, returns x_8ba0 */

int F_14C8B(void)
{
    return x_8ba0;
}
