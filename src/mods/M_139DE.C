#include "GLOBALS.H"
#include "PROTOS.H"
#include <string.h>
#include <stdlib.h>

static char * near F_14341(int wear);

/* F_139DE: draw a hot spot and the spots linked behind it (the chain through Hot.f10 masked with
   g_07a8), by kind: 4/12 filled box with optional centred text, 5/6/10 text only, 7/8 header bar,
   13 first-time marker. Grows the touched rectangle g_6c56..g_6c5c. */

int g_07a8 = 0;   /* DS:07A8 */
char g_07aa[12] = "(xx% worn)";   /* DS:07AA */

void F_139DE(struct Hot *p)
{
    int colr;
    int w, h;
    char *str;
    int nxt;

    colr = 0;
    if (!g_07a8 && g_vid.f16)
        F_120FE(-3);
    do {
        switch (p->f0) {
        case 4:
        case 12:
            if (p->f2 > 0) {
                if (p->f2 != colr)
                    g_colors = g_style->pal[colr = p->f2];
                F_13682(g_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
                if (g_colors.c[4])
                    F_137B2(g_vid.back, p->x1, p->y1, w, h);
                if (p->f4) {
                    str = g_239e + p->f4;
                    (*g_78ae)(g_vid.back, ((w - F_10B48(str)) >> 1) + p->x1, p->y1, str);
                    F_1377F(g_vid.back, p->x1, p->y1 + 9, w);
                }
            }
        box:
            g_6c56 = p->x1 > g_6c56 ? g_6c56 : p->x1;
            g_6c5a = p->x2 < g_6c5a ? g_6c5a : p->x2;
            g_6c58 = p->y1 > g_6c58 ? g_6c58 : p->y1;
            g_6c5c = p->y2 < g_6c5c ? g_6c5c : p->y2;
            continue;
        case 10:
            if (p->f2 != colr)
                g_colors = g_style->pal[colr = p->f2];
            F_13682(g_vid.back, p->x1, p->y1, w = p->x2 - p->x1, h = p->y2 - p->y1);
            F_137B2(g_vid.back, p->x1, p->y1, w, h);
            str = g_239e + p->f4;
            (*g_78ae)(g_vid.back, ((w - F_10B48(str)) >> 1) + p->x1,
                      p->y1 + ((h - 8) >> 1), str);
            goto box;
        case 5:
            if (p->f2 != colr)
                g_colors = g_style->pal[colr = p->f2];
            (*g_78ae)(g_vid.back, p->x1, p->y1, g_239e + p->f4);
            goto box;
        case 6:
            if (p->f2 != colr)
                g_colors = g_style->pal[colr = p->f2];
            str = g_239e + p->f4;
            (*g_78ae)(g_vid.back, ((p->x2 - p->x1 - F_10B48(str)) >> 1) + p->x1,
                      ((p->y2 - p->y1 - 8) >> 1) + p->y1, str);
            goto box;
        case 7:
            g_8ba4 = 1;
        case 8:
            if (p->f2 != colr)
                g_colors = g_style->pal[colr = p->f2];
            F_13682(g_vid.back, p->x1, p->y1, w = p->x2 - p->x1, 8);
            str = g_239e + p->f4;
            if (p->f0 == 8)
                (*g_78ae)(g_vid.back, ((w - F_10B48(str)) >> 1) + p->x1, p->y1, str);
            else
                (*g_78ae)(g_vid.back, p->x1, p->y1, str);
            if (!g_8ba2)
                g_8ba2 = p;
            goto box;
        case 13:
            if (!g_8b9c++)
                g_8b9a = (int)p;
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
    } while ((nxt = p->f10 & g_07a8) ? (p = &g_09f4[nxt], 1) : 0);
    if (!g_07a8 && g_vid.f16) {
        F_120FE(-1);
        F_1A92A(g_vid.back, g_vid.front, p->x1 - 3, p->y1 - 3, p->x2 + 6, p->y2 + 6);
    }
}

/* F_13DE6: push hot spot id onto the saved state stack (at most four deep) and draw it: the
   current entry globals are reset, F_139DE draws the linked spot, and the touched area is copied
   from the back buffer to the front when the screen shadow is on */


void F_13DE6(int id)
{
    struct Hot *p;
    struct Colors saved;

    if (g_8b8e < 4) {
        F_120FE(-3);
        g_8b86[g_8b8e++] = g_8efe = id;
        g_8b90.x1 = g_8b90.x2 = g_8b90.y1 = g_8b90.y2 = -1;
        g_8bac = (p = &g_09f4[g_2bad[id]])->f4;
        g_8ba6 = (p + 1)->f0 > 9 ? p + 1 : 0;
        g_8b9c = g_8b9a = g_8ba2 = 0;
        g_8ba4 = 0;
        g_8b98 = p->f0 == 3;
        saved = g_colors;
        g_07a8 = -1;
        g_6c56 = g_6c58 = 1000;
        g_6c5a = g_6c5c = -1;
        if (p->f10)
            F_139DE(&g_09f4[p->f10]);
        g_07a8 = 0;
        g_colors = saved;
        F_120FE(-1);
        if (g_vid.f16 && g_6c5a >= 0)
            F_1A92A(g_vid.back, g_vid.front, g_6c56 - 3, g_6c58 - 3, g_6c5a + 6, g_6c5c + 6);
        F_012DE(id);
        F_00D78();
    }
}

/* F_13F21: pop the saved state stack: close the current entry; when keep is set and an entry remains,
   reopen the one below it, otherwise reset the entry globals to "none" */


void F_13F21(int keep)
{
    if (g_8b8e) {
        F_01391(0);
        g_8ba6 = 0;
        if (--g_8b8e && keep) {
            --g_8b8e;
            F_13DE6(g_8b86[g_8b8e]);
        } else {
            g_8efe = 0;
            g_8ba6 = 0;
            g_8b90.x1 = g_8b90.y1 = g_8b90.x2 = g_8b90.y2 = g_8baa = -1;
        }
    }
}

/* F_13F7F @ 0F38:4BFF (linear 0x13f7f, 27 bytes) */


void F_13F7F(void)
{
    g_8b8e = g_8bac = 0;
    g_8ba6 = 0;
    g_8b90.x1 = g_8b90.y1 = g_8b90.x2 = g_8b90.y2 = g_8baa = 0;
}

/* F_13F9A: bounding box of hot spot id and the spots linked after it (through f10), widened by 3
   and clipped to the 320 x 200 screen. *x and *y receive the top left corner, *w and *h the size */


void F_13F9A(int id, int *x, int *y, int *w, int *h)
{
    struct Hot *p;

    p = &g_09f4[g_2bad[id]];
    g_6c56 = g_6c58 = 1000;
    g_6c5a = g_6c5c = 1;
    while (p->f10) {
        p = &g_09f4[p->f10];
        g_6c56 = p->x1 > g_6c56 ? g_6c56 : p->x1;
        g_6c5a = p->x2 < g_6c5a ? g_6c5a : p->x2;
        g_6c58 = p->y1 > g_6c58 ? g_6c58 : p->y1;
        g_6c5c = p->y2 < g_6c5c ? g_6c5c : p->y2;
    }
    if ((*x = g_6c56 - 3) < 0)
        *x = 0;
    if ((*y = g_6c58 - 3) < 0)
        *y = 0;
    if ((*w = g_6c5a + 3) > 320)
        *w = 320;
    if ((*h = g_6c5c + 3) > 200)
        *h = 200;
    *w -= *x;
    *h -= *y;
}

/* F_14090: copy the screen area around hot spot id (see F_13F9A, which updates *px and *py to the
   box corner) into a bitmap of the given mode and return it. The x range is rounded to g_vid.mask */


struct Bitmap far *F_14090(int id, int *px, int *py, int mode)
{
    int x;
    int y1;
    int x2;
    struct Bitmap far *bits9;
    int h3;
    int w4;
    int y2;

    F_13F9A(id, px, py, &w4, &h3);
    x = *px;
    y1 = *py;
    x2 = (x + w4 + ~g_vid.mask & g_vid.mask) > 320 ? 320 : (x + w4 + ~g_vid.mask & g_vid.mask);
    y2 = y1 + h3 > 200 ? 200 : y1 + h3;
    x &= g_vid.mask;
    bits9 = F_1934E(x2 - x, y2 - y1, 0, mode);
    return F_11A72(*px, y1, w4, h3, bits9);
}

/* F_1412D: find the hot spot of the current menu (list at g_8ba6) under the point (x, y). On a hit
   the spot's box goes to g_8b90 and its id (f6) is returned; on a miss g_8b90 gets the free
   rectangle around the point and 0 is returned */


int F_1412D(int x, int y)
{
    struct Hot *p;
    int kind;

    p = g_8ba6;
    if (p == 0) {
        g_8b90.x1 = g_8b90.y1 = -1;
        g_8b90.x2 = g_8b90.y2 = 1000;
        g_8baa = 0;
        return 0;
    }
    g_8b90.x1 = g_8b90.y1 = 0;
    g_8b90.x2 = 319;
    g_8b90.y2 = 199;
    while ((kind = p->f0) > 9) {
        if (kind == 14) {
            p++;
            continue;
        }
        if (p->x1 > x) {
            g_8b90.x2 = p->x1;
            break;
        }
        if (p->x2 <= x) {
            if (p->x2 > g_8b90.x1)
                g_8b90.x1 = p->x2;
        next:
            if (kind == 12)
                p = &g_09f4[p->f6];
            else
                p++;
        } else {
            if (p->y1 - 3 > y) {
                if (p->y1 - 3 < g_8b90.y2)
                    g_8b90.y2 = p->y1 - 3;
            } else if (p->y2 <= y) {
                if (p->y2 > g_8b90.y1)
                    g_8b90.y1 = p->y2;
            } else {
                if (kind == 12) {
                    p++;
                    continue;
                }
                g_8b90.x1 = p->x1;
                g_8b90.x2 = p->x2;
                g_8b90.y1 = p->y1;
                g_8b90.y2 = p->y2;
                g_8ba8 = p;
                return g_8baa = p->f6;
            }
            goto next;
        }
    }
    g_8ba8 = 0;
    return g_8baa = 0;
}

/* F_14241: grab the screen area around hot spot id, show the spot (waiting for a key through
   F_14294), put the saved area back and return the key */


int F_14241(int id)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    bm1 = F_14090(id, &x, &y1, 2);
    res = F_14294(id);
    F_11BD3(bm1, x, y1);
    F_19438(1);
    return res;
}

/* F_14294 @ 0F38:4F14 (linear 0x14294, 59 bytes) */


int F_14294(int a)
{
    int key;

    F_13DE6(a);
    while ((key = F_01417(0x7d00)) == 0)
        ;
    F_13F21(1);
    return key;
}

/* F_142CF: run the F_1438E dialog over a grabbed screen area (see F_14241) and restore the area
   afterwards; 0x3ea when *list is zero */


int F_142CF(int a, int b, int *c, int *list, char *buf, char **d)
{
    struct Bitmap far *bm1;
    int x;
    int y1;
    int res;

    if (*list == 0)
        return 0x3ea;
    bm1 = F_14090(a, &x, &y1, 2);
    res = F_1438E(a, b, c, list, buf, d);
    F_11BD3(bm1, x, y1);
    F_19438(1);
    return res;
}

/* F_14341: wear text for a part: "(new)" when wear is 0, else "(xx% worn)" built in place in g_07aa
   (initialised data "(xx% worn)") with the number written over the xx */

char g_07be[4] = "   ";   /* DS:07BE */

static char * near F_14341(int wear)
{
    char *p;

    if (wear == 0)
        return "(`new`)";
    g_07aa[3] = ' ';
    itoa(wear, p = g_07aa + 1, 10);
    p[strlen(p)] = '%';
    return g_07aa;
}

/* F_1438E: list dialog. Shows the entries list[1..list[0]] (offsets into the string table g_2c02,
   or, when negative, d[-entry - 1]) in the windows of the current menu, optionally with a column c
   of x offsets and wear values buf[1..]; arrow/page keys scroll, a positive key2 picks an entry,
   a hotn spot (g_8ba2) shows the picked text and is edited in place on Enter.
   Returns the key2 that left the dialog, 0x3ea when the list is empty. */

int g_07c6[277] = {0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276};   /* DS:07C6 */

int F_1438E(int kind, int sel, int *c, int *list, char *buf, char **d)
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
    F_13DE6(kind);
    page = g_8b9c;
    g_8b9e = list;
    g_8ba0 = sel + 1;
    if (count5 > page)
        topk = (sel + page > count5) ? count5 - page : sel;
    if (hotn = g_8ba2) {
        bufpl = g_239e + hotn->f4;
        n9 = (hotn->x2 - hotn->x1) / 6;
        if (!g_8ba4) {
            strcpy(bufpl, g_2c02 + lp11[sel]);
            if (buf && (wear = buf1[sel]) != -128) {
                strcat(bufpl, g_07be);
                strcat(bufpl, F_14341(wear));
            }
            F_139DE(hotn);
        }
    }
    ww9 = ((struct Win *)g_8b9a)->tx - (wx8 = ((struct Win *)g_8b9a)->w);
    right = wx8 + ww9 - 1;
    wh9 = ((struct Win *)g_8b9a)->ty - (wy7 = ((struct Win *)g_8b9a)->h);
    bottom9 = page * wh9 + wy7 - 1;
    idx2 = ((struct Win *)g_8b9a)->lines;
    g_style->c2d = g_style->pal[idx2];
    sv5 = g_style->pal[idx2].c[3];
    for (;;) {
        g_colors = g_style->pal[idx2];
        w = (struct Win *)g_8b9a;
        F_120FE(-3);
        i = (count5 - topk > page) ? page : count5 - topk;
        lpk = lp11 + topk;
        pc1 = c ? c1 + topk : 0;
        bpl = buf ? buf1 + topk : 0;
        for (; i--; pc1++, w++, lpk++, bpl++) {
            labeln = (*lpk > -1) ? g_2c02 + *lpk : d[-*lpk - 1];
            x = (c ? *pc1 : 0) + w->w;
            (*g_78ae)(g_vid.back, x, w->h, labeln);
            F_13682(g_vid.back, g_824c - 1, g_824e, w->tx - g_824c + 1, 8);
            if (buf && (wear = *bpl) != -128)
                (*g_78ae)(g_vid.back, w->tx - strlen(g_07aa) * 6, w->h, F_14341(wear));
        }
        if (!g_8b98) {
            w = (struct Win *)(0x12 * sel + g_8b9a);
            g_colors = g_style->pal[w->lines];
            F_13957(g_vid.back, w->w, w->h, ww9, 8);
        }
        F_120FE(-1);
        if (g_8240)
            F_1A92A(g_vid.back, g_vid.front, wx8 - 1, wy7 - 1, right + 1, bottom9 + 1);
restore:
        g_colors = g_style->pal[idx2];
        if (g_vid.mode == -4)
            g_colors.c[0] = 0;
        while ((key2 = F_01417(0)) == 0) {
            i = g_8baa > 0 && topk + g_8baa <= count5;
            if (!i && curl < 0)
                continue;
            if (curl == g_8baa - 1)
                continue;
            F_120FE(-3);
            x1 = y1 = 1000;
            x2 = y2 = -1;
            if (i) {
                w = (struct Win *)(0x12 * (nw5 = g_8baa - 1) + g_8b9a);
                x1 = w->w;
                y1 = w->h;
                x2 = w->tx;
                if (kind != 0x26)
                    (*g_78ba)(g_vid.back, g_colors.c[0], sv5, x1, y1, x2, w->h + 8);
                y2 = w->ty;
            } else
                nw5 = -1;
            if (curl >= 0) {
                w = (struct Win *)(0x12 * curl + g_8b9a);
                if (kind != 0x26)
                    (*g_78ba)(g_vid.back, sv5, g_colors.c[0], w->w, w->h, w->tx, w->h + 8);
                x1 = (w->w > x1) ? x1 : w->w;
                y1 = (w->h > y1) ? y1 : w->h;
                x2 = (w->tx < x2) ? x2 : w->tx;
                y2 = (w->ty < y2) ? y2 : w->ty;
            }
            F_120FE(-1);
            if (g_8240)
                F_1A92A(g_vid.back, g_vid.front, x1, y1, x2, y2);
            curl = nw5;
        }
        if (key2 > 0) {
            if (topk + key2 <= count5) {
                g_8ba0 = topk + key2;
                i = topk + key2 - 1;
                if (hotn) {
                    if (!g_8ba4 || i == sel) {
                        strcpy(bufpl, g_2c02 + lp11[i]);
                        if (kind == 5 && !strcmp(g_4607, bufpl))
                            strcpy(bufpl, "\0");
                        if (buf && (wear = buf1[i]) != -128) {
                            strcat(bufpl, g_07be);
                            strcat(bufpl, F_14341(wear));
                        }
                        F_139DE(hotn);
                    }
                }
                if (!g_8b98) {
                    F_120FE(-3);
                    w = (struct Win *)(0x12 * sel + g_8b9a);
                    g_colors = g_style->pal[idx2];
                    nw5 = g_colors.c[3];
                    g_colors.c[3] = g_colors.c[1];
                    F_13957(g_vid.back, x1 = w->w, y1 = w->h, ww9, 8);
                    x2 = x1-- + ww9;
                    y2 = y1-- + 8;
                    g_colors.c[3] = nw5;
                    w = (struct Win *)(0x12 * i + g_8b9a);
                    F_13957(g_vid.back, w->w, w->h, ww9, 8);
                    F_120FE(-1);
                    if (g_8240)
                        F_1A92A(g_vid.back, g_vid.front,
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
            F_13F21(1);
            return key2;
        case -6:
            if (kind == 5 && !strcmp(g_4607, bufpl))
                strcpy(bufpl, "\0");
            g_colors = g_style->pal[hotn->f2];
            F_13DE6(0x18);
            F_14C90(hotn->x1, hotn->y1, bufpl, n9);
            F_13F21(1);
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
                    w = (struct Win *)(0x12 * nw5 + g_8b9a);
                    F_13682(g_vid.back, w->w, w->h, ww9,
                        ((topk - i > page - nw5) ? page - nw5 : topk - i) * wh9);
                }
            }
        }
    }
}
