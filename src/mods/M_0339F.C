#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static int near score_index(void);
static int near entry_beats_score(int i);
static void near insert_entry(int pos);

struct Colors g_4be2 = {{3, 5, 5, 4, 1}};   /* DS:4BE2 */
struct Colors g_4be8 = {{3, 4, 4, 3, 1}};   /* DS:4BE8 */
char g_4bed = 0;   /* DS:4BED */

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
    (*bl78be)();
    g_04c2 = 200;
    r_vid.f16 = 1;
    F_118D4(5, -1);
    g_04c2 = old;
    F_10BAC();
    sv286 = bustyle->c28;
    sv23 = bustyle->c23;
    if (r_vid.mode == -2 || r_vid.mode == -6) {
        bustyle->c28 = g_4be2;
        bustyle->c23 = g_4be8;
    }
    (*m_present)();
    if (add) {
        g_score = score_index();
        for (i = 0; i < bucount; i++)
            if (entry_beats_score(i)) {
                pos7 = i;
                break;
            }
        if (bucount < 10) {
            if (pos7 < 0)
                pos7 = bucount;
            bucount++;
        }
        insert_entry(pos7);
    }
    if (r_vid.mode != -2 && r_vid.mode != -6) {
        x_colors.c[0] = 3;
        x_colors.c[1] = 2;
    } else {
        x_colors.c[0] = 4;
        x_colors.c[1] = 0;
    }
    for (i = 0, p = aytablex; i < bucount; i++) {
        (*a_78aem)(r_vid.back, 46, g_4bca[i], p->name);
        x = 161;
        n2 = atoi(p->score);
        while (n2 < 1000) {
            n2 *= 10;
            x += 6;
        }
        (*a_78aem)(r_vid.back, x, g_4bca[i], p->score);
        p++;
        (*a_78aem)(r_vid.back, 191, g_4bca[i], "hr");
    }
    F_1A92A(r_vid.back, r_vid.front, 46, g_4bca[0], 211, g_4bca[9] + 8);
    if (pos7 >= 0) {
        strcpy(buf3, aytablex[pos7].name);
        F_14C90(46, g_4bca[pos7], buf3, 15);
        strcpy(aytablex[pos7].name, buf3);
        F_16152();
    } else {
        F_0FED1(700);
    }
    (*bl78be)();
    bustyle->c28 = sv286;
    bustyle->c23 = sv23;
    F_10BAC();
}
