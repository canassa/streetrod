#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FAME.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "EDIT.H"
#include "GAME.H"
#include "GARAGE.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MEM.H"
#include "MENU.H"
#include "PC.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "STRUCTS.H"
#include "SYS.H"
#include "VIDEO.H"

static int g_score;
static int near elapsed_hours(void);
static int near score_beats_entry(int i);
static void near insert_entry(int pos);

struct HallEntry g_hallFameTbl[10];

static struct ManualQuestion g_manual[51] = {
    {2, MANUAL_BROWN},        {3, MANUAL_DARK_PURPLE},   {4, MANUAL_YELLOW},       {6, MANUAL_ORANGE},
    {7, MANUAL_DARK_BLUE},    {8, MANUAL_GREY},          {9, MANUAL_PINK},         {10, MANUAL_YELLOW},
    {11, MANUAL_LIGHT_BLUE},  {12, MANUAL_GREY},         {14, MANUAL_DARK_GREEN},  {15, MANUAL_LIGHT_PURPLE},
    {16, MANUAL_BROWN},       {18, MANUAL_RED},          {19, MANUAL_DARK_PURPLE}, {20, MANUAL_ORANGE},
    {22, MANUAL_LIGHT_BLUE},  {23, MANUAL_YELLOW},       {1, MANUAL_RED},          {2, MANUAL_ORANGE},
    {3, MANUAL_DARK_BLUE},    {4, MANUAL_PINK},          {5, MANUAL_LIGHT_GREEN},  {6, MANUAL_LIGHT_BLUE},
    {7, MANUAL_DARK_GREEN},   {8, MANUAL_BROWN},         {9, MANUAL_LIGHT_PURPLE}, {10, MANUAL_ORANGE},
    {11, MANUAL_GREY},        {12, MANUAL_RED},          {13, MANUAL_DARK_PURPLE}, {14, MANUAL_DARK_BLUE},
    {15, MANUAL_LIGHT_GREEN}, {16, MANUAL_PINK},         {17, MANUAL_YELLOW},      {18, MANUAL_GREY},
    {19, MANUAL_LIGHT_BLUE},  {20, MANUAL_LIGHT_GREEN},  {21, MANUAL_DARK_BLUE},   {22, MANUAL_LIGHT_PURPLE},
    {23, MANUAL_DARK_GREEN},  {24, MANUAL_BROWN},        {25, MANUAL_RED},         {26, MANUAL_GREY},
    {27, MANUAL_LIGHT_GREEN}, {28, MANUAL_ORANGE},       {29, MANUAL_DARK_PURPLE}, {30, MANUAL_LIGHT_BLUE},
    {31, MANUAL_PINK},        {32, MANUAL_LIGHT_PURPLE}, {33, MANUAL_DARK_BLUE}};
int g_protectCycle = 0;
static int g_fameRowY[10] = {59, 70, 81, 92, 103, 114, 125, 136, 147, 158};

/* The manual lookup copy protection: asks for the colour of the car key or car
   icon on a random manual page and returns 1 if the answer matches. */

int fa_protect_check(void)
{
    enum ManualKind f;
    enum ManualColor e;
    int d;
    int c;
    char b[26];
    char a[50];

    d = rn_int(MANUAL_ENTRY_COUNT);
    f = d < MANUAL_ICON_COUNT;
    sc_status_text(-1, "", 0);
    r_vid.copyToFront = FALSE;
    sc_picture_at(PIC_COPY_PROTECTION_JUKEBOX, 8, 0, 0);
    di_free_pic(PIC_COPY_PROTECTION_JUKEBOX);
    switch (f) {
    case MANUAL_CAR_KEY:
        sprintf(b, "What`is`the`color`of");
        sprintf(a, "the`car`key`on`page`%d?", g_manual[d].page);
        c = 0x72;
        break;
    case MANUAL_CAR_ICON:
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
    g_displayEnabled = 1;
    r_vid.copyToFront = TRUE;
    g_protectCycle = 1;
    e = mn_wait(KM_MANUAL_COLORS);
    g_protectCycle = 0;
    sc_status_text(-1, "", 0);
    if (g_manual[d].answerColor != e)
        return 0;
    return 1;
}


int fa_end_menu(void)
{
    char c01;
    char c1;
    char c21;
    char c3;
    int key;

    sc_status_init(SCREEN_BODY_HEIGHT);
    r_vid.copyToFront = TRUE;
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
    key = mn_wait(KM_END_MENU);
    cu_ctl(CURSOR_HIDE_NOW);
    switch (key) {
    case KR_LOAD_GAME:
    case KR_NEW_GAME:
        if (g_picsFreed) {
            di_load_list(g_gamePics, 1);
            g_picsFreed = 0;
        }
        break;
    }
    return key;
}


static int near score_beats_entry(int i)
{
    return atoi(g_hallFameTbl[i].elapsedHoursText) > g_score;
}

int g_nScore;
char g_driverName[16];


static void near insert_entry(int pos)
{
    int cnt;
    char *src;
    char *dst;

    cnt = g_nScore - pos - 1;
    if (pos >= 0) {
        src = (char *)&g_hallFameTbl[pos];
        if (cnt > 0) {
            dst = src + sizeof(struct HallEntry);
            memmove(dst, src, cnt * sizeof(struct HallEntry));
        }
        strcpy(src, g_driverName);
        src += 16;
        itoa(g_score, src, 10);
    }
}


static int near elapsed_hours(void)
{
    int a;
    int b;

    ga_time(&a, &b);
    return ga_day() * 24 + a + 12;
}

static struct Colors g_fameColors1 = {3, 5, 5, 4, 1};
static struct Colors g_fameColors2 = {3, 4, 4, 3, 1};


void fa_show(int add)
{
    char buf3[16];
    int n2;
    struct HallEntry *p;
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
    r_vid.copyToFront = TRUE;
    sc_picture(PIC_HALL_OF_FAME, -1);
    g_picturePlacement.maxHeight = old;
    vi_swap_pal();
    sv286 = bustyle->schemes[STYLE_MESSAGE_BUTTON];
    sv23 = bustyle->schemes[STYLE_MESSAGE];
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) {
        bustyle->schemes[STYLE_MESSAGE_BUTTON] = g_fameColors1;
        bustyle->schemes[STYLE_MESSAGE] = g_fameColors2;
    }
    (*g_videoOps.show)();
    if (add) {
        g_score = elapsed_hours();
        for (i = 0; i < g_nScore; i++)
            if (score_beats_entry(i)) {
                pos7 = i;
                break;
            }
        if (g_nScore < HALL_FAME_COUNT) {
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
        (*g_videoOps.drawText)(r_vid.back, 46, g_fameRowY[i], p->driverName);
        x = 161;
        n2 = atoi(p->elapsedHoursText);
        while (n2 < 1000) {
            n2 *= 10;
            x += 6;
        }
        (*g_videoOps.drawText)(r_vid.back, x, g_fameRowY[i], p->elapsedHoursText);
        p++;
        (*g_videoOps.drawText)(r_vid.back, 191, g_fameRowY[i], "hr");
    }
    bm_copy_box(r_vid.back, r_vid.front, 46, g_fameRowY[0], 211, g_fameRowY[HALL_FAME_COUNT - 1] + 8);
    if (pos7 >= 0) {
        strcpy(buf3, g_hallFameTbl[pos7].driverName);
        ed_line(46, g_fameRowY[pos7], buf3, 15);
        strcpy(g_hallFameTbl[pos7].driverName, buf3);
        di_save_hall();
    } else {
        da_wait_key(700);
    }
    (*g_videoOps.blank)();
    bustyle->schemes[STYLE_MESSAGE_BUTTON] = sv286;
    bustyle->schemes[STYLE_MESSAGE] = sv23;
    vi_swap_pal();
}
