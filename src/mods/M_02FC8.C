#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "M_02FC8.H"
#include "BITMAP.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_036A8.H"
#include "M_039C0.H"
#include "M_06476.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11E9C.H"
#include "M_13682.H"
#include "M_139DE.H"
#include "M_14C90.H"
#include "M_15236.H"
#include "M_15396.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_score;                        /* DS:6C5E */
static int near score_index(void);
static int near entry_beats_score(int i);
static void near insert_entry(int pos);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Entry aytablex[10];

struct ManualEntry g_manual[51] = {{2, 2}, {3, 11}, {4, 6}, {6, 5}, {7, 9}, {8, 1}, {9, 4}, {10, 6}, {11, 10}, {12, 1}, {14, 7}, {15, 12}, {16, 2}, {18, 3}, {19, 11}, {20, 5}, {22, 10}, {23, 6}, {1, 3}, {2, 5}, {3, 9}, {4, 4}, {5, 8}, {6, 10}, {7, 7}, {8, 2}, {9, 12}, {10, 5}, {11, 1}, {12, 3}, {13, 11}, {14, 9}, {15, 8}, {16, 4}, {17, 6}, {18, 1}, {19, 10}, {20, 8}, {21, 9}, {22, 12}, {23, 7}, {24, 2}, {25, 3}, {26, 1}, {27, 8}, {28, 5}, {29, 11}, {30, 10}, {31, 4}, {32, 12}, {33, 9}};   /* DS:4B62 */
int g_4bc8 = 0;   /* DS:4BC8 */
int g_4bca[10] = {59, 70, 81, 92, 103, 114, 125, 136, 147, 158};   /* DS:4BCA */

/* protect_check @ 0000:2FC8 (linear 0x02FC8, 403 bytes)

   The manual lookup copy protection: asks for the colour of the car key or car
   icon on a random manual page and returns 1 if the answer matches. */

int protect_check(void)
{
    int f;
    int e;
    int d;
    int c;
    char b[26];
    char a[50];

    d = random_int(0x33);
    f = d < 0x12;
    F_11421(-1, "", 0);
    r_vid.f16 = 0;
    F_119B8(8, 8, 0, 0);
    F_158DE(8);
    switch (f) {
    case 0:
        sprintf(b, "What`is`the`color`of");
        sprintf(a, "the`car`key`on`page`%d?", g_manual[d].page);
        c = 0x72;
        break;
    case 1:
        sprintf(b, "What`is`the`color`of");
        sprintf(a, "    `the`car`icon`on`page`%d?  ", g_manual[d].page);
        c = 0x55;
        break;
    }
    F_119D6(0x79, 0x3f, b);
    F_119D6(c, 0x48, a);
    F_120FE(-2);
    (*s_blit)(r_vid.front->bits.p, r_vid.back->bits.p);
    if (r_vid.mode == -2 || r_vid.mode == -6)
        F_11324(g_0692);
    else
        (*m_present)();
    gz8249 = 1;
    r_vid.f16 = 1;
    g_4bc8 = 1;
    e = F_14294(0x27);
    g_4bc8 = 0;
    F_11421(-1, "", 0);
    if (g_manual[d].color != e)
        return 0;
    return 1;
}

/* F_0315B @ 0000:315B (linear 0x0315B, 373 bytes) */

int F_0315B(void)
{
    char c01;
    char c1;
    char c21;
    char c3;
    int key;

    F_113A2(190);
    r_vid.f16 = 1;
    c01 = x_colors.c[0];
    c1 = x_colors.c[1];
    c3 = x_colors.c[3];
    c21 = x_colors.c[2];
    if (r_vid.mode != -2 && r_vid.mode != -6) {
        x_colors.c[0] = 3;
        x_colors.c[1] = 0;
        x_colors.c[3] = 2;
        x_colors.c[2] = 1;
    } else {
        x_colors.c[0] = 15;
        x_colors.c[1] = 1;
        x_colors.c[3] = 13;
        x_colors.c[2] = 3;
    }
    F_1169A(0, 0, 320, 190, x_colors.c[1]);
    F_13957(r_vid.back, 100, 50, 120, 90);
    F_13957(r_vid.front, 100, 50, 120, 90);
    F_13957(r_vid.back, 102, 52, 116, 86);
    F_13957(r_vid.front, 102, 52, 116, 86);
    x_colors.c[0] = c01;
    x_colors.c[1] = c1;
    x_colors.c[3] = c3;
    x_colors.c[2] = c21;
    F_120FE(-2);
    (*m_present)();
    key = F_14294(41);
    F_120FE(-4);
    switch (key) {
    case -2:
    case -1:
        if (g_4f4c) {
            F_15C4A(g_4c28, 1);
            g_4f4c = 0;
        }
        break;
    }
    return key;
}

/* entry_beats_score @ 0000:32D0 (linear 0x032D0, 36 bytes) */

static int near entry_beats_score(int i)
{
    return atoi(aytablex[i].score) > g_score;
}

int bucount;
char atnewName[16];

/* insert_entry @ 0000:32F4 (linear 0x032F4, 129 bytes) */

static void near insert_entry(int pos)
{
    int cnt;
    char *src;
    char *dst;

    cnt = bucount - pos - 1;
    if (pos >= 0) {
        src = (char *)&aytablex[pos];
        if (cnt > 0) {
            dst = src + sizeof(struct Entry);
            memmove(dst, src, cnt * sizeof(struct Entry));
        }
        strcpy(src, atnewName);
        src += 16;
        itoa(g_score, src, 10);
    }
}

/* score_index @ 0000:3375 (linear 0x03375, 42 bytes) */

static int near score_index(void)
{
    int a;
    int b;

    F_064A3(&a, &b);
    return F_06476() * 24 + a + 12;
}

struct Colors g_4be2 = {{3, 5, 5, 4, 1}};   /* DS:4BE2 */
struct Colors g_4be8 = {{3, 4, 4, 3, 1}};   /* DS:4BE8 */
char g_4bed = 0;   /* DS:4BED */

/* F_0339F @ 0000:339F (linear 0x0339F, 628 bytes) */

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
    bitmap_copy_box(r_vid.back, r_vid.front, 46, g_4bca[0], 211, g_4bca[9] + 8);
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
