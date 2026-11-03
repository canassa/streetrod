#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near score_index(void);
static int near entry_beats_score(int i);
static void near insert_entry(int pos);

void F_0339F(int add)
{
    char buf3[16];
    int n2;
    struct Entry *p;
    int x;
    struct Colors sv286;
    int pos7;
    int i;
    struct Colors sv23;
    int old;

    old = g_04c2;
    pos7 = -1;
    (*g_78be)();
    g_04c2 = 200;
    g_vid.f16 = 1;
    F_118D4(5, -1);
    g_04c2 = old;
    F_10BAC();
    sv286 = g_style->c28;
    sv23 = g_style->c23;
    if (g_vid.mode == -2 || g_vid.mode == -6) {
        g_style->c28 = g_4be2;
        g_style->c23 = g_4be8;
    }
    (*g_present)();
    if (add) {
        g_score = score_index();
        for (i = 0; i < g_count; i++)
            if (entry_beats_score(i)) {
                pos7 = i;
                break;
            }
        if (g_count < 10) {
            if (pos7 < 0)
                pos7 = g_count;
            g_count++;
        }
        insert_entry(pos7);
    }
    if (g_vid.mode != -2 && g_vid.mode != -6) {
        g_colors.c[0] = 3;
        g_colors.c[1] = 2;
    } else {
        g_colors.c[0] = 4;
        g_colors.c[1] = 0;
    }
    for (i = 0, p = g_table; i < g_count; i++) {
        (*g_78ae)(g_vid.back, 46, g_4bca[i], p->name);
        x = 161;
        n2 = atoi(p->score);
        while (n2 < 1000) {
            n2 *= 10;
            x += 6;
        }
        (*g_78ae)(g_vid.back, x, g_4bca[i], p->score);
        p++;
        (*g_78ae)(g_vid.back, 191, g_4bca[i], "hr");
    }
    F_1A92A(g_vid.back, g_vid.front, 46, g_4bca[0], 211, g_4bca[9] + 8);
    if (pos7 >= 0) {
        strcpy(buf3, g_table[pos7].name);
        F_14C90(46, g_4bca[pos7], buf3, 15);
        strcpy(g_table[pos7].name, buf3);
        F_16152();
    } else {
        F_0FED1(700);
    }
    (*g_78be)();
    g_style->c28 = sv286;
    g_style->c23 = sv23;
    F_10BAC();
}
