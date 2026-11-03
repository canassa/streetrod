/* F_139DE: draw a hot spot and the spots linked behind it (the chain through Hot.f10 masked with
   g_07a8), by kind: 4/12 filled box with optional centred text, 5/6/10 text only, 7/8 header bar,
   13 first-time marker. Grows the touched rectangle g_6c56..g_6c5c. */

#include "GLOBALS.H"
#include "PROTOS.H"

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
