/* F_1438E: list dialog. Shows the entries list[1..list[0]] (offsets into the string table g_2c02,
   or, when negative, d[-entry - 1]) in the windows of the current menu, optionally with a column c
   of x offsets and wear values buf[1..]; arrow/page keys scroll, a positive key2 picks an entry,
   a hotn spot (g_8ba2) shows the picked text and is edited in place on Enter.
   Returns the key2 that left the dialog, 0x3ea when the list is empty. */

#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static char * near F_14341(int wear);

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
                strcat(bufpl, "   ");
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
                            strcat(bufpl, "   ");
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
