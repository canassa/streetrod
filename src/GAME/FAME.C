#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "FAME.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "EDIT.H"
#include "GAME.H"
#include "GARAGE.H"
#include "MAIN.H"
#include "MEM.H"
#include "MENU.H"
#include "PC.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_score;                        /* DS:6C5E */
static int near elapsed_hours(void);
static int near score_beats_entry(int i);
static void near insert_entry(int pos);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Entry g_hallFameTbl[10];

struct ManualEntry g_manual[51] = {{2, 2}, {3, 11}, {4, 6}, {6, 5}, {7, 9}, {8, 1}, {9, 4}, {10, 6}, {11, 10}, {12, 1}, {14, 7}, {15, 12}, {16, 2}, {18, 3}, {19, 11}, {20, 5}, {22, 10}, {23, 6}, {1, 3}, {2, 5}, {3, 9}, {4, 4}, {5, 8}, {6, 10}, {7, 7}, {8, 2}, {9, 12}, {10, 5}, {11, 1}, {12, 3}, {13, 11}, {14, 9}, {15, 8}, {16, 4}, {17, 6}, {18, 1}, {19, 10}, {20, 8}, {21, 9}, {22, 12}, {23, 7}, {24, 2}, {25, 3}, {26, 1}, {27, 8}, {28, 5}, {29, 11}, {30, 10}, {31, 4}, {32, 12}, {33, 9}};   /* DS:4B62 */
int g_protectCycle = 0;   /* DS:4BC8 */
int g_fameRowY[10] = {59, 70, 81, 92, 103, 114, 125, 136, 147, 158};   /* DS:4BCA */

/* fa_protect_check @ 0000:2FC8 (linear 0x02FC8, 403 bytes)

   The manual lookup copy protection: asks for the colour of the car key or car
   icon on a random manual page and returns 1 if the answer matches. */

int fa_protect_check(void)
{
    int f;
    int e;
    int d;
    int c;
    char b[26];
    char a[50];

    d = rn_int(0x33);
    f = d < 0x12;
    sc_status_text(-1, "", 0);
    r_vid.copyToFront = 0;
    sc_picture_at(8, 8, 0, 0);
    di_free_pic(8);
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
    sc_text(0x79, 0x3f, b);
    sc_text(c, 0x48, a);
    cu_ctl(CURSOR_SHOW_NOW);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY)
        vi_set_palette(g_protectPal);
    else
        (*g_videoOps.show)();
    gz8249 = 1;
    r_vid.copyToFront = 1;
    g_protectCycle = 1;
    e = mn_wait(0x27);
    g_protectCycle = 0;
    sc_status_text(-1, "", 0);
    if (g_manual[d].color != e)
        return 0;
    return 1;
}

/* fa_end_menu @ 0000:315B (linear 0x0315B, 373 bytes) */

int fa_end_menu(void)
{
    char c01;
    char c1;
    char c21;
    char c3;
    int key;

    sc_status_init(SCREEN_BODY_HEIGHT);
    r_vid.copyToFront = 1;
    c01 = x_colors.text;
    c1 = x_colors.textBg;
    c3 = x_colors.line;
    c21 = x_colors.fill;
    if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) {
        x_colors.text = 3;
        x_colors.textBg = 0;
        x_colors.line = 2;
        x_colors.fill = 1;
    } else {
        x_colors.text = 15;
        x_colors.textBg = 1;
        x_colors.line = 13;
        x_colors.fill = 3;
    }
    sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_BODY_HEIGHT, x_colors.textBg);
    dr_frame(r_vid.back, 100, 50, 120, 90);
    dr_frame(r_vid.front, 100, 50, 120, 90);
    dr_frame(r_vid.back, 102, 52, 116, 86);
    dr_frame(r_vid.front, 102, 52, 116, 86);
    x_colors.text = c01;
    x_colors.textBg = c1;
    x_colors.line = c3;
    x_colors.fill = c21;
    cu_ctl(CURSOR_SHOW_NOW);
    (*g_videoOps.show)();
    key = mn_wait(41);
    cu_ctl(CURSOR_HIDE_NOW);
    switch (key) {
    case -2:
    case -1:
        if (g_picsFreed) {
            di_load_list(g_gamePics, 1);
            g_picsFreed = 0;
        }
        break;
    }
    return key;
}

/* score_beats_entry @ 0000:32D0 (linear 0x032D0, 36 bytes) */

static int near score_beats_entry(int i)
{
    return atoi(g_hallFameTbl[i].score) > g_score;
}

int g_nScore;
char g_driverName[16];

/* insert_entry @ 0000:32F4 (linear 0x032F4, 129 bytes) */

static void near insert_entry(int pos)
{
    int cnt;
    char *src;
    char *dst;

    cnt = g_nScore - pos - 1;
    if (pos >= 0) {
        src = (char *)&g_hallFameTbl[pos];
        if (cnt > 0) {
            dst = src + sizeof(struct Entry);
            memmove(dst, src, cnt * sizeof(struct Entry));
        }
        strcpy(src, g_driverName);
        src += 16;
        itoa(g_score, src, 10);
    }
}

/* elapsed_hours @ 0000:3375 (linear 0x03375, 42 bytes) */

static int near elapsed_hours(void)
{
    int a;
    int b;

    ga_time(&a, &b);
    return ga_day() * 24 + a + 12;
}

struct Colors g_fameColors1 = {3, 5, 5, 4, 1};   /* DS:4BE2 */
struct Colors g_fameColors2 = {3, 4, 4, 3, 1};   /* DS:4BE8 */
char g_4bed = 0;   /* DS:4BED */

/* fa_show @ 0000:339F (linear 0x0339F, 628 bytes) */

void fa_show(int add)
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

    old = g_picturePlacement.maxHeight;
    pos7 = -1;
    (*g_videoOps.blank)();
    g_picturePlacement.maxHeight = SCREEN_HEIGHT;
    r_vid.copyToFront = 1;
    sc_picture(5, -1);
    g_picturePlacement.maxHeight = old;
    vi_swap_pal();
    sv286 = bustyle->c28;
    sv23 = bustyle->c23;
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) {
        bustyle->c28 = g_fameColors1;
        bustyle->c23 = g_fameColors2;
    }
    (*g_videoOps.show)();
    if (add) {
        g_score = elapsed_hours();
        for (i = 0; i < g_nScore; i++)
            if (score_beats_entry(i)) {
                pos7 = i;
                break;
            }
        if (g_nScore < 10) {
            if (pos7 < 0)
                pos7 = g_nScore;
            g_nScore++;
        }
        insert_entry(pos7);
    }
    if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) {
        x_colors.text = 3;
        x_colors.textBg = 2;
    } else {
        x_colors.text = 4;
        x_colors.textBg = 0;
    }
    for (i = 0, p = g_hallFameTbl; i < g_nScore; i++) {
        (*g_videoOps.drawText)(r_vid.back, 46, g_fameRowY[i], p->name);
        x = 161;
        n2 = atoi(p->score);
        while (n2 < 1000) {
            n2 *= 10;
            x += 6;
        }
        (*g_videoOps.drawText)(r_vid.back, x, g_fameRowY[i], p->score);
        p++;
        (*g_videoOps.drawText)(r_vid.back, 191, g_fameRowY[i], "hr");
    }
    bm_copy_box(r_vid.back, r_vid.front, 46, g_fameRowY[0], 211, g_fameRowY[9] + 8);
    if (pos7 >= 0) {
        strcpy(buf3, g_hallFameTbl[pos7].name);
        ed_line(46, g_fameRowY[pos7], buf3, 15);
        strcpy(g_hallFameTbl[pos7].name, buf3);
        di_save_hall();
    } else {
        da_wait_key(700);
    }
    (*g_videoOps.blank)();
    bustyle->c28 = sv286;
    bustyle->c23 = sv23;
    vi_swap_pal();
}
