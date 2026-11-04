#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "STRUCTS.H"
#include "GARAGE.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "GAME.H"
#include "GARGFX.H"
#include "HOTSPOT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "PERF.H"
#include "POPUP.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "TIRES.H"
#include "VIDEO.H"

static int near day_to_date(int v, int *out);
static void near place_spots(int n, int x, int y);
static void near hide_spots(void);
static void near sort_spots(void);
static int near is_broke(void);
static void near remove_part(struct Node **slot);
static void near tune_timing(struct Car *car);

int g_tirePics[3][4] = {{1065, 1066, 1067, 1068}, {1028, 1029, 1030, 1031}, {1106, 1107, 1108, 1109}};   /* DS:5030 */
int g_colX = 33;   /* DS:5048 */
int g_rowY[4] = {14, 107, 15, 105};   /* DS:504A */
int g_wOff[4] = {0, 5, 9, 14};   /* DS:5052 */

/* ga_day @ 0000:6476 (linear 0x06476, 45 bytes) */

int ga_day(void)
{
    return (int)((g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase) / 6552) > 91 ? 91 : (int)((g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase) / 6552);
}

/* ga_time @ 0000:64A3 (linear 0x064A3, 134 bytes) */

void ga_time(int *a, int *b)
{
    long t;
    int r;

    t = g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase;
    if (t > 0x91908L)
        t = 0x91908L;
    r = (int)(t % 6552);
    *a = (unsigned)r / 546;
    *b = (long)((unsigned long)(unsigned)(r - 546 * *a) * 60) / 546;
}

/* ga_check_end @ 0000:6529 (linear 0x06529, 19 bytes) */

void ga_check_end(int n)
{
    if (ga_day() >= 91)
        g_end |= 1;
}

/* day_to_date @ 0000:653C (linear 0x0653C, 115 bytes) */

static int near day_to_date(int v, int *out)
{
    int r;

    if (v < 15) {
        r = 0;
        *out = v + 16;
    } else if (v < 46) {
        r = 1;
        *out = v - 14;
    } else if (v < 77) {
        r = 2;
        *out = v - 45;
    } else if (v < 91) {
        r = 3;
        *out = v - 76;
    } else {
        r = 3;
        *out = 15;
    }
    return r;
}

/* ga_calendar @ 0000:65AF (linear 0x065AF, 417 bytes) */

void ga_calendar(int a1)
{
    int x;
    struct Bitmap far *pic;
    int py4, px5, row5, m, id, d9, al, y4;
    struct Bitmap far *img;
    int e9, col, day, bl;

    ga_check_end(0);
    if (a1 == 0 && g_end)
        return;
    d9 = ga_day();
    ga_time(&al, &bl);
    m = day_to_date(d9, &day);
    col = (e9 = d9 + 21) % 7;
    row5 = e9 / 7 - g_wOff[m];
    x = 18 * col + g_colX;
    y4 = 7 * row5 + g_rowY[m];
    r_vid.copyToFront = 1;
    px5 = 80;
    py4 = 4;
    id = m < 2 ? 0x4c3 : 0x4c5;
    cu_ctl(CURSOR_HIDE);
    pic = di_bm(id, ARENA_HIGH_STACK);
    bm_draw_pic(pic, 0x4c4, (unsigned char)x, (unsigned char)y4, 0);
    img = ar_bm(pic->width, pic->height, 0, ARENA_HIGH_STACK);
    pu_grab(px5, py4, pic->width, pic->height, img);
    pu_show(pic, px5, py4);
    ma_pop_high(1);
    cu_ctl(CURSOR_SHOW);
    if (a1 == 0) {
        sc_status_text(-1, "", 0);
        gm_pause(500);
        g_gameClock.actionTicks += 0L;
        pu_show(img, px5, py4);
    }
    ma_pop_high(1);
}

char *g_mon[4] = {"June", "July", "August", "September"};   /* DS:5082 */
int g_hIds[5] = {11, 13, 9, 10, 12};   /* DS:508A */

/* ga_date @ 0000:6750 (linear 0x06750, 127 bytes)

   Draws the current date ("Month day, 1963") */

void ga_date(int x)
{
    int date;
    int today;
    int mnum;
    char buf[30];
    struct Colors sv;

    today = ga_day();
    mnum = day_to_date(today, &date);
    sprintf(buf, "%s %d, 1963", g_mon[mnum], date);
    sv = x_colors;
    x_colors = bustyle->hilite;
    (*g_videoOps.drawText)(r_vid.back, 0x61, x, buf);
    x_colors = sv;
}

/* ga_palette @ 0000:67CF (linear 0x067CF, 88 bytes) */

void ga_palette(int a)
{
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) {
        if (gz8249) {
            vi_palette_reg(6, g_paint[a][0]);
            vi_palette_reg(7, g_paint[a][1]);
        }
        g_palette[6] = g_paint[a][0];
        g_palette[7] = g_paint[a][1];
    }
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct State8272 m_8272;

/* ga_hotspot @ 0000:6827 (linear 0x06827, 91 bytes) */

int ga_hotspot(int id)
{
    int n, k, base;

    base = ((unsigned)g_gar - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotCalendar - (unsigned)g_gar) / sizeof(struct Hot) + 1;
    for (k = 0; k < n; k++)
        if (g_hots[base + k].f6 == id)
            return base + k;
}

/* sort_spots @ 0000:6882 (linear 0x06882, 49 bytes) */

static void near sort_spots(void)
{
    int base, n;

    base = ((unsigned)g_gar - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotCalendar - (unsigned)g_gar) / sizeof(struct Hot) + 1;
    gx_sort_hots(base, n);
}

/* hide_spots @ 0000:68B3 (linear 0x068B3, 53 bytes) */

static void near hide_spots(void)
{
    int i;

    for (i = 0; i < 5; i++)
        g_hots[ga_hotspot(g_hIds[i])].kind = HOT_DISABLED;
}

/* place_spots @ 0000:68E8 (linear 0x068E8, 446 bytes)

   Rect + hotspot setup for the 5 button rects of panel n */

static void near place_spots(int n, int x, int y)
{
    struct Panel *panel;
    struct Hot *hotspot;

    panel = &ai70e4t[n];
    hotspot = &g_hots[ga_hotspot(11)];
    hotspot->x1 = panel->r[0].x + x;
    hotspot->y1 = panel->r[0].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[0].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[0].h;
    hotspot->kind = HOT_REGION;
    hotspot = &g_hots[ga_hotspot(13)];
    hotspot->x1 = panel->r[1].x + x;
    hotspot->y1 = panel->r[1].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[1].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[1].h;
    hotspot->kind = HOT_REGION;
    hotspot = &g_hots[ga_hotspot(9)];
    hotspot->x1 = panel->r[2].x + x;
    hotspot->y1 = panel->r[2].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[2].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[2].h;
    hotspot->kind = HOT_REGION;
    hotspot = &g_hots[ga_hotspot(10)];
    hotspot->x1 = panel->r[3].x + x;
    hotspot->y1 = panel->r[3].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[3].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[3].h;
    hotspot->kind = HOT_REGION;
    hotspot = &g_hots[ga_hotspot(12)];
    hotspot->x1 = panel->r[4].x + x;
    hotspot->y1 = panel->r[4].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[4].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[4].h;
    hotspot->kind = HOT_REGION;
}

/* ga_clear_name @ 0000:6AA6 (linear 0x06AA6, 50 bytes) */

void ga_clear_name(void)
{
    char *s;

    s = "                                ";
    strcpy(g_carName, s);
}

/* is_broke @ 0000:6AD8 (linear 0x06AD8, 46 bytes) */

static int near is_broke(void)
{
    if (a_7eaap.currentCar == 0 && a_7eaap.otherCars == 0 && a_7eaap.spareParts == 0 && a_7eaap.money < 400L)
        return 1;
    return 0;
}

char g_paint[6][2] = {{4, 12}, {3, 11}, {2, 10}, {5, 13}, {1, 9}, {7, 15}};   /* DS:50C2 */
int g_pad[7] = {0, 5, 5, 5, 5, 0, 0};   /* DS:50CE */

/* ga_menu @ 0000:6B06 (linear 0x06B06, 1125 bytes) */

int near ga_menu(enum GarageMode mode)
{
    char *p;
    int r;
    int n;

    if (g_cVis > 0)
        cu_ctl(CURSOR_HIDE_NOW);
    ga_check_end(0);
    if (g_end) {
        if (g_end & 1)
            cu_ctl(CURSOR_SHOW_NOW);
        return GARAGE_GAME_ENDED;
    }
    cu_ctl(0);
    sc_status_text(-1, "", 0);
    g_scene = SCENE_GARAGE;
    switch (mode) {
    case GARAGE_SWAP:
        if (g_new == 0) {
            cu_ctl(CURSOR_SHOW_NOW);
            gm_message(0x1867);
            cu_ctl(CURSOR_HIDE_NOW);
            break;
        }
        if (g_old) {
            n = gm_car_look(g_old);
            cp_show_car(CAR_BACK_OUT, g_old->specIndex, n, (g_old->appearance >> 8) & 0x1f,
                    0, 0x98, 0, g_tirePics[g_parts[g_old->tires->idx].variant]);
            ga_clear_name();
            mn_push(KM_GARAGE);
            mn_pop(1);
        }
    case GARAGE_REDRAW:
        r_vid.copyToFront = 0;
    case GARAGE_DRIVE_IN:
        if (mode == GARAGE_SWAP)
            mode = GARAGE_DRIVE_IN;
        else {
            sc_picture(0x408, -1);
            vi_garage_pal();
            if (mode == GARAGE_REDRAW && a_7eaap.currentCar == 0 && a_7eaap.otherCars) {
                a_7eaap.currentCar = a_7eaap.otherCars;
                a_7eaap.otherCars = a_7eaap.currentCar->next;
                mode = GARAGE_DRIVE_IN;
            }
        }
        r_vid.copyToFront = 1;
        (*g_videoOps.show)();
        if (a_7eaap.currentCar) {
            m_8272.bodyPic = g7d86n[a_7eaap.currentCar->specIndex].bodyPic;
            m_8272.paintScheme = a_7eaap.currentCar->paintScheme;
            m_8272.tireVariant = g_parts[a_7eaap.currentCar->tires->idx].variant;
            if (mode == GARAGE_DRIVE_IN)
                ga_palette(m_8272.paintScheme);
            m_8272.overlayPic = 0;
            m_8272.specIndex = a_7eaap.currentCar->specIndex;
            m_8272.appearance.sticker = (a_7eaap.currentCar->appearance >> 8) & 0x1f;
            m_8272.appearance.roof = (a_7eaap.currentCar->appearance >> 13) & 1;
            m_8272.appearance.front = (a_7eaap.currentCar->appearance >> 14) & 1;
            m_8272.appearance.rear = (a_7eaap.currentCar->appearance >> 15) & 1;
            for (r = 0; r < 5; r++)
                o_8290[r] = a_7eaap.currentCar->rack.parts[r]->idx;
            n = gm_car_look(a_7eaap.currentCar);
            strcpy(g_carName, g_desc + g7d86n[a_7eaap.currentCar->specIndex].nameOffset);
            if (mode == GARAGE_REDRAW)
                cp_show_car(CAR_SHOW_BACK_PAGE, m_8272.specIndex, n, m_8272.appearance.sticker, 0, 0x98, 0, g_tirePics[m_8272.tireVariant]);
            else
                cp_show_car(CAR_PULL_IN, m_8272.specIndex, n, m_8272.appearance.sticker, 0, 0x98, 0, g_tirePics[m_8272.tireVariant]);
        } else {
            bm_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), 0xc7);
            g_carName[0] = 0;
        }
        break;
    case GARAGE_KEEP:
        break;
    }
    if (a_7eaap.currentCar)
        place_spots(m_8272.specIndex, ac825et, aw8260y);
    else
        hide_spots();
    sort_spots();
    sc_status_label(g_bankrollLabel);
    cu_ctl(CURSOR_SHOW_NOW);
    if (a_7eaap.carCount > 15)
        gm_message(0x0d30);
    else if (a_7eaap.partCount > 130) {
        if (a_7eaap.partCount == 131)
            gm_message(0x0d76);
        else {
            p = g_junk;
            p += strlen(p) + 1;
            p += strcspn(p, "1234567890");
            itoa(a_7eaap.partCount - 130, p, 10);
            p[strlen(p)] = ' ';
            gm_message(0x0dc7);
        }
    }
    if (is_broke()) {
        gm_message(0x17ff);
        g_end |= 0x10;
    } else {
        mu_suspend(0);
        sy_reset_queues();
        r = mn_wait(KM_GARAGE);
    }
    ga_check_end(0);
    if (g_end & 0x10)
        g_end = 0x10;
    if (g_end) {
        r = GARAGE_GAME_ENDED;
        if ((g_end & 1) == 0)
            cu_ctl(CURSOR_HIDE_NOW);
    }
    return r;
}

/* ga_paint @ 0000:6F6B (linear 0x06F6B, 431 bytes) */

void near ga_paint(void)
{
    struct Bitmap far *bm1;
    int x;
    int idx4;
    int y;
    int key;

    if (!a_7eaap.currentCar)
        gm_message(0x0efa);
    else if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY)
        gm_message(0x18ad);
    else {
        bm1 = mn_grab(KM_PAINT, &x, &y, ARENA_HIGH_STACK);
        mn_push(KM_PAINT);
        idx4 = a_7eaap.currentCar->paintScheme;
        for (;;) {
            vi_palette_reg(12, g_paint[idx4][0]);
            while ((key = sy_wait_input(3000)) == 0)
                ;
            switch (key) {
            case KR_PAINT_PREVIOUS:
                if (--idx4 < 0)
                    idx4 = 5;
                continue;
            case KR_PAINT_NEXT:
                if (++idx4 >= 6)
                    idx4 = 0;
                continue;
            case KR_PAINT_GO_AHEAD:
                if (a_7eaap.money < 20L) {
                    gm_message(0x10a1);
                    break;
                }
                r_vid.copyToFront = 1;
                mn_pop(1);
                pu_show(bm1, x, y);
                ma_pop_high(1);
                pu_show(g_stickerBg, m_8272.stickerX, m_8272.stickerY);
                a_7eaap.currentCar->appearance &= 0xe0ff;
                ga_palette(a_7eaap.currentCar->paintScheme = idx4);
                vi_palette_reg(12, 5);
                g_gameClock.actionTicks += 0x888L;
                sc_add_money(-20);
                return;
            case KR_FORGET:
            default:
                break;
            }
            break;
        }
        mn_pop(1);
        vi_palette_reg(12, 5);
        pu_show(bm1, x, y);
        ma_pop_high(1);
    }
}

/* ga_body @ 0000:711A (linear 0x0711A, 589 bytes) */

void near ga_body(unsigned mask)
{
    struct Bitmap far *bm1;
    int n9;
    char *msg;
    unsigned val5;
    int x;
    int r7;
    struct Model *p9;
    int y;
    int key;
    int cost;

    p9 = &g_models[a_7eaap.currentCar->specIndex];
    if (!a_7eaap.currentCar) {
        gm_message(0x0efa);
        return;
    }
    if (mask & 4) {
        if (p9->roofPic == 0) {
            gm_message(0x1914);
            return;
        }
        msg = (a_7eaap.currentCar->appearance & 0x2000) ? (g_txt + 1299) : (g_txt + 1283);
        cost = 70;
        val5 = 0x2220;
    }
    if (mask & 2) {
        if (p9->frontBumperPic == 0) {
            gm_message(0x1936);
            return;
        }
        msg = (a_7eaap.currentCar->appearance & 0x4000) ? (g_txt + 1337) : (g_txt + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if (mask & 1) {
        if (p9->rearBumperPic == 0) {
            gm_message(0x1936);
            return;
        }
        msg = (a_7eaap.currentCar->appearance & 0x8000) ? (g_txt + 1337) : (g_txt + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if ((long)cost > a_7eaap.money) {
        gm_message(0x1959);
        return;
    }
    bm1 = mn_grab(KM_BODY_CHANGE, &x, &y, ARENA_HIGH_STACK);
    g_hotBodyChange[0].textOffset = msg - g_txt;
    mn_push(KM_BODY_CHANGE);
    while ((key = sy_wait_input(3000)) == 0)
        ;
    switch (key) {
    case KR_OK:
        if (mask & 4)
            a_7eaap.currentCar->appearance ^= 0x2000;
        if (mask & 2)
            a_7eaap.currentCar->appearance ^= 0x4000;
        if (mask & 1)
            a_7eaap.currentCar->appearance ^= 0x8000;
        cu_ctl(CURSOR_HIDE_NOW);
        mn_pop(1);
        pu_show(bm1, x, y);
        ma_pop_high(1);
        n9 = a_7eaap.currentCar->specIndex;
        r7 = gm_car_look(a_7eaap.currentCar);
        cp_show_car(CAR_SHOW_BACKDROP, n9, r7, (a_7eaap.currentCar->appearance >> 8) & 0x1f, 0, 0x98, 0,
                g_tirePics[g_parts[a_7eaap.currentCar->tires->idx].variant]);
        so_noise();
        cu_ctl(CURSOR_SHOW_NOW);
        g_gameClock.actionTicks += val5;
        sc_add_money(-cost);
        break;
    case KR_BODY_FORGET:
        mn_pop(1);
        pu_show(bm1, x, y);
        ma_pop_high(1);
        break;
    }
}

/* ga_stickers @ 0000:7367 (linear 0x07367, 832 bytes) */

void near ga_stickers(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *bm3;
    struct Pos6 *q2;
    struct Bitmap far *bm4;
    struct Blt B1;
    struct Blt A2;
    struct Blt C;
    int x;
    int y;
    int idx6;
    int k;

    if (!a_7eaap.currentCar)
        gm_message(0x0efa);
    else if (a_7eaap.money < 5L)
        gm_message(0x1975);
    else {
        r_vid.copyToFront = 1;
        x = 24;
        y = 11;
        bm1 = ar_bm(264, 69, 0, ARENA_HIGH_STACK);
        pu_grab(x, y, 264, 69, bm1);
        sc_picture_at(1200, x, y, -1);
        mn_push(KM_STICKERS);
        while ((k = sy_wait_input(3000)) == 0)
            ;
        switch (k) {
        case KR_STICKER_8: case KR_STICKER_7: case KR_STICKER_6: case KR_STICKER_5:
        case KR_STICKER_4: case KR_STICKER_3: case KR_STICKER_2: case KR_STICKER_1:
            cu_ctl(CURSOR_HIDE);
            mn_pop(1);
            pu_show(bm1, x, y);
            ma_pop_high(1);
            idx6 = -30 - k;
            a_7eaap.currentCar->appearance = ((idx6 & 0x1f) << 8) | (a_7eaap.currentCar->appearance & 0xe0ff);
            bm3 = di_bm(1218, ARENA_HIGH_STACK);
            bm1 = ar_bm(g_stickerBg->width, g_stickerBg->height, 0, ARENA_HIGH_STACK);
            bm_copy(g_stickerBg, bm1, 1);
            q2 = &a784als[idx6 - 1];
            bm_draw_pic(bm1, q2->id, (char)(q2->dx + m_8272.stickerSubX), (char)q2->dy, 0);
            bm4 = ar_bm(bm1->width, bm1->height, 0, ARENA_HIGH_STACK);
            bm_copy(bm1, bm4, 1);
            A2.sx = B1.sx = C.sx = 0;
            A2.sy = B1.sy = C.sy = 0;
            A2.dy = B1.dy = 0;
            A2.dx = B1.dx = 0;
            C.dx = m_8272.stickerX;
            C.dy = m_8272.stickerY;
            B1.h = B1.fc = A2.h = C.h = bm1->height;
            A2.w = B1.w = C.w = 8;
            so_sticker(1);
            for (k = 0; k < bm1->width; k += 8) {
                (*g_videoOps.copy)(bm1, bm4, &A2, 1);
                if (bm1->width - 8 > k)
                    (*g_videoOps.maskedBlit)(bm3, bm4, &B1, 1);
                da_wait_retrace(2);
                (*g_videoOps.copy)(bm4, r_vid.front, &C, 1);
                A2.w += 8;
                C.w += 8;
                B1.dx += 8;
            }
            pu_show(bm1, m_8272.stickerX, m_8272.stickerY);
            ma_pop_high(3);
            so_sticker(0);
            g_gameClock.actionTicks += 0x222L;
            sc_add_money(-5);
            cu_ctl(CURSOR_SHOW);
            break;
        case KR_STICKER_SKIP:
            mn_pop(1);
            pu_show(bm1, x, y);
            ma_pop_high(1);
            break;
        }
    }
}

/* ga_spares @ 0000:76A7 (linear 0x076A7, 154 bytes) */

void near ga_spares(enum PartCategory t, struct Node **list, char *buf, int start)
{
    struct Node *p;
    struct Item *ent7;
    int n;

    n = start;
    p = a_7eaap.spareParts;
    while (p) {
        if ((ent7 = &g_parts[p->idx])->category == t) {
            n++;
            list[n] = p;
            g_list[n] = ent7->nameOffset;
            if (p->wear != -128)
                buf[n] = p->wear / 100;
            else
                buf[n] = 0x80;
        }
        p = p->next;
    }
    g_list[0] = n;
}

/* ga_install @ 0000:7741 (linear 0x07741, 72 bytes) */

void near ga_install(struct Node *n, struct Node **slot)
{
    if (*slot) {
        (*slot)->next = a_7eaap.spareParts;
        a_7eaap.currentCar->value -= gm_part_value(a_7eaap.spareParts = *slot);
    }
    *slot = n;
    n->next = 0;
    a_7eaap.currentCar->value += gm_part_value(n);
}

/* remove_part @ 0000:7789 (linear 0x07789, 46 bytes) */

static void near remove_part(struct Node **slot)
{
    (*slot)->next = a_7eaap.spareParts;
    a_7eaap.currentCar->value -= gm_part_value(a_7eaap.spareParts = *slot);
    *slot = 0;
}

/* ga_unlink @ 0000:77B7 (linear 0x077B7, 73 bytes) */

struct Node * near ga_unlink(struct Node *n)
{
    struct Node *p1, *q;

    for (p1 = a_7eaap.spareParts, q = 0; p1;)
        if (p1 != n)
            p1 = (q = p1)->next;
        else
            break;
    if (q == 0)
        a_7eaap.spareParts = p1->next;
    else
        q->next = p1->next;
    return p1;
}

/* ga_tires @ 0000:7800 (linear 0x07800, 241 bytes) */

void near ga_tires(void)
{
    struct Car *car;
    int sel;
    struct Node *tbl[142];
    char line[142];
    int y, cur;

    car = a_7eaap.currentCar;
    if (car == 0) {
        gm_message(0x0efa);
        return;
    }
    ga_spares(PART_TIRES, tbl, line, 0);
    switch (mn_modal_list(KM_SPARE_PARTS, 1, 0, g_list, line, 0)) {
    case KR_CANCEL:
        return;
    case KR_ACCEPT:
        sel = mn_sel();
        break;
    case KR_EMPTY_LIST:
        gm_message(0x0f4e);
        return;
    }
    y = g_parts[car->tires->idx].variant;
    cur = g_parts[tbl[sel]->idx].variant;
    ga_install(ga_unlink(tbl[sel]), &car->tires);
    ti_change(g_tirePics[y], g_tirePics[cur]);
    g_gameClock.actionTicks += 0x444L;
}

/* build_car_info @ 0000:78F1 (linear 0x078F1, 250 bytes) */

static void near build_car_info(int *ids, char *vals)
{
    int n;
    struct Item *it;
    int count;
    struct Node *node;
    struct Node *gear[7];

    for (n = 0; n < 5; n++)
        gear[n] = a_7eaap.currentCar->rack.parts[n];
    gear[n++] = a_7eaap.currentCar->transmission;
    gear[n] = a_7eaap.currentCar->tires;

    for (count = 0, n = 0; n < 7; n++) {
        if (node = gear[n]) {
            count++;
            it = &g_parts[node->idx];
            ids[count] = g_pad[n];
            g_list[count] = it->nameOffset;
            if (node->wear != -0x80)
                vals[count] = node->wear / 100;
            else
                vals[count] = 0x80;
        }
    }
    for (n = 1; n <= 4; n++) {
        g_list[++count] = -n;
        vals[count] = 0x80;
        ids[count] = 0;
    }
    g_list[0] = count;
}

char *g_iTxt[4] = {"", "Fuel                  ", "", "Max speed      mph"};   /* DS:511E */
int g_nCarb[5] = {1, 1, 3, 2, 2};   /* DS:5126 */
int g_nBolt[6] = {1, 2, 2, 2, 2, 2};   /* DS:5130 */
enum PartCategory g_nextPartCategory[6] = {PART_ENGINE, PART_MANIFOLD, PART_CARBURETOR, PART_CARBURETOR, PART_CARBURETOR, PART_CARBURETOR};   /* DS:513C */
int g_cPos[5][6] = {{5, 5, 0, 0, 0, 0}, {6, 6, 0, 0, 0, 0}, {7, 7, 8, 8, 9, 9}, {10, 10, 11, 11, 0, 0}, {12, 12, 13, 13, 0, 0}};   /* DS:5148 */
int g_boltSlots[10] = {0, 0, 1, 1, 5, 5, 5, 5, 5, 5};   /* DS:5184 */
int g_boltSubslots[10] = {0, 0, 0, 1, 0, 1, 0, 1, 0, 1};   /* DS:5198 */

/* ga_info @ 0000:79EB (linear 0x079EB, 185 bytes) */

void ga_info(void)
{
    int ids[143];
    char vals[144];

    if (a_7eaap.currentCar == 0) {
        gm_message(0x0efa);
        return;
    }
    build_car_info(ids, vals);
    sprintf(g_iTxt[3] + 9, " %3i mph", ga_ready(a_7eaap.currentCar, 1) ? pf_vmax(-1) : 0);
    sprintf(g_iTxt[1] + 5, " %2i gallons", a_7eaap.currentCar->fuel / 10);
    switch (mn_modal_list(KM_CAR_INFO, 1, ids, g_list, vals, g_iTxt)) {
    case KR_ACCEPT:
        return;
    case KR_EMPTY_LIST:
        gm_message(0x0f4e);
        return;
    }
}

/* ga_transmission @ 0000:7AA4 (linear 0x07AA4, 1090 bytes) */

void near ga_transmission(void)
{
    int flags[3];   /* by key 1, 2; flags[0] is the original's unused 2-byte slot below flags[1] */
    char buf[142];
    int prev;
    struct Car *owner;
    int k;
    struct Node *tbl[142];

    owner = a_7eaap.currentCar;
    if (!owner) {
        gm_message(0x0efa);
        return;
    }
    for (k = 0; k < 2; k++) {
        if (prev = owner->transmissionBolts[k])
            flags[k + 1] = prev == 1 ? 1 : 2;
        else
            flags[k + 1] = 0;
        g_hotTransBolts[k].kind = HOT_REGION;
    }
    m_8272.transmissionPic = owner->transmission ? g_transPics[g_parts[owner->transmission->idx].variant] : 0;
    r_vid.copyToFront = !m_8272.transmissionPic;
    sc_picture_at(0x42e, 0x20, 10, -1);
    r_vid.copyToFront = 1;
    gx_trans_draw(m_8272.transmissionPic, owner->transmissionBolts);
    mn_push(KM_TRANSMISSION);
    for (;;) {
        while ((k = sy_wait_input(3000)) == 0)
            ;
        switch (k) {
        case KR_TRANS_DONE:
            mn_pop(1);
            return;
        case KR_TRANS_PARTS:
            ga_spares(PART_TRANSMISSION, tbl, buf, 0);
            for (;;) {
                switch (k = mn_modal_list(KM_SPARE_PARTS, 1, 0, g_list, buf, 0)) {
                case KR_ACCEPT:
                    if (owner->transmission) {
                        gm_message(0x1875);
                        k = 0;
                    } else
                        k = mn_sel();
                    break;
                case KR_CANCEL:
                    k = 0;
                    break;
                case KR_EMPTY_LIST:
                    gm_message(0xf72);
                    k = 0;
                    break;
                }
                if (!k)
                    break;
                prev = m_8272.transmissionPic;
                m_8272.transmissionPic = g_transPics[g_parts[tbl[k]->idx].variant];
                if (g7d86n[owner->specIndex].parts & g_parts[tbl[k]->idx].fitMask & 0xf) {
                    ga_install(ga_unlink(tbl[k]), &owner->transmission);
                    cu_ctl(CURSOR_HIDE_NOW);
                    so_part_move(1);
                    gx_trans_pic(m_8272.transmissionPic, 0);
                    so_part_move(0);
                    so_part_set();
                    g_gameClock.actionTicks += 0x666L;
                    for (k = 2; k--;) {
                        flags[k + 1] = 1;
                        gx_trans_bolt(owner->transmissionBolts[k] = 1, k);
                        so_bolt();
                        g_hotTransBolts[k].kind = HOT_REGION;
                    }
                    sy_hover_reset();
                    cu_ctl(CURSOR_SHOW_NOW);
                    break;
                }
                    gx_trans_pic(m_8272.transmissionPic, 1);
                    gm_message(0x10cd);
                    gx_trans_clear(m_8272.transmissionPic, 1);
                    g_gameClock.actionTicks += 0x888L;
                    m_8272.transmissionPic = prev;
            }
            break;
        case 1:
        case 2:
            if (m_8272.transmissionPic == 0)
                break;
            if (flags[k--] == 2) {
                if (--owner->transmissionBolts[k] <= 1) {
                    flags[k + 1] = owner->transmissionBolts[k] = 0;
                    gx_trans_drop(k);
                    so_bolt();
                    sy_hover_reset();
                } else {
                    gx_trans_bolt(owner->transmissionBolts[k], k);
                    so_bolt_turn();
                }
                if (flags[1] || flags[2])
                    break;
                gx_trans_clear(m_8272.transmissionPic, 0);
                g_gameClock.actionTicks += 0x666L;
                m_8272.transmissionPic = 0;
                owner->transmissionBolts[0] = owner->transmissionBolts[1] = 0;
                remove_part(&owner->transmission);
                sy_hover_reset();
            } else if (flags[k + 1] == 1) {
                if (owner->transmissionBolts[k] == 3) {
                    flags[k + 1] = 2;
                    break;
                }
                gx_trans_bolt(++owner->transmissionBolts[k], k);
                so_bolt_turn();
            } else {
                cu_ctl(CURSOR_HIDE_NOW);
                gx_trans_bolt(owner->transmissionBolts[k] = 1, k);
                so_bolt();
                cu_ctl(CURSOR_SHOW_NOW);
                flags[k + 1] = 1;
            }
            break;
        }
    }
}

/* ga_ready @ 0000:7EE6 (linear 0x07EE6, 203 bytes) */

int ga_ready(struct Car *car, int flag)
{
    int n, i;

    if (car->rack.parts[0] == 0 || car->wireState <= 0 || car->rack.parts[1] == 0)
        return 0;
    n = g_nCarb[g_parts[car->rack.parts[1]->idx].variant];
    for (i = 2; i < n + 2; i++)
        if (car->rack.parts[i] == 0)
            return 0;
    for (i = n * 2 + 3; --i >= 0; )
        if (car->engineBolts[i] != 3)
            return 0;
    if (flag == 0)
        return 1;
    if (car->transmission == 0)
        return 0;
    if (car->tires == 0)
        return 0;
    for (i = 2; --i >= 0; )
        if (car->transmissionBolts[i] != 3)
            return 0;
    return 1;
}

/* tune_timing @ 0000:7FB1 (linear 0x07FB1, 506 bytes) */

static void near tune_timing(struct Car *car)
{
    struct Obj *p;
    long tmp;
    struct Blt blt;
    union Word w;
    int key;
    int y;
    unsigned n;
    int gear;

    mn_push(KM_TIMING);
    blt.w = 8;
    blt.h = 11;
    blt.sx = 0x30;
    blt.sy = 0;
    blt.dx = 0xb0;
    sc_picture_at(0x42f, 0x60, 9, -1);
    p = bm7568 + vi_anim_start(8);
    tmp = g_gameClock.biosTicks;
    sy_music_pause();
    g_voiceNum = 2;
    if (g_soundOn) {
        outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
        g_speakerOn = 1;
    }
    do {
        if (g_soundOn && g_soundOn != g_speakerOn) {
            outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
            g_speakerOn = 1;
        }
        gear = car->timing;
        w.w = (abs(gear) << 10) + 30000;
        outp(PIT_CH2, w.b[0]);
        outp(PIT_CH2, w.b[1]);
        y = blt.dy = gear * 2 + 0x4b;
        cu_ctl(CURSOR_HIDE_NOW);
        (*g_videoOps.copy)(p->img, r_vid.back, &blt, 1);
        bm_copy_box(r_vid.back, r_vid.front, 0xb0, y, 0xb7, y + 10);
        cu_ctl(CURSOR_SHOW_NOW);
        while ((key = sy_wait_input(0)) == 0) {
            if (g_soundOn && g_soundOn != g_speakerOn) {
                outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
                g_speakerOn = 1;
            }
        }
        switch (key) {
        case KR_TIMING_ADVANCE:
            car->timing++;
            if (car->timing > 4)
                car->timing = 4;
            break;
        case KR_TIMING_RETARD:
            car->timing--;
            if (car->timing < -8)
                car->timing = -8;
            break;
        }
    } while (key != KR_TIMING_DONE);
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    g_voiceNum = 1;
    sy_music_resume();
    vi_anim_stop(8);
    n = ((unsigned)g_gameClock.biosTicks - (unsigned)tmp) * 3;
    n = n > 0x444 ? 0x444 : n;
    g_gameClock.actionTicks += n;
    mn_pop(1);
}

/* ga_hood @ 0000:81AB (linear 0x081AB, 2938 bytes)

   The garage screen: install, remove and tune parts */

void near ga_hood(struct Car *car)
{
    struct Node *list4[142];
    struct Node *p6;
    int cnt1;
    int sel2;
    int key;
    int ok;
    int k;
    int tmp29;
    int step;
    char buf[142];
    int n;
    int havek[10];

    if (car == 0) {
        gm_message(0xefa);
        return;
    }
    for (key = 1; key <= 9; key++) {
        if (k = car->engineBolts[key - 1]) {
            havek[key] = k == 1 ? 1 : 2;
            g_engineBoltPicIds[key - 1] = g_boltPics[k];
        } else {
            havek[key] = 0;
            g_engineBoltPicIds[key - 1] = 0;
        }
    }
    g_slotSelec.wirePic = car->wireState != WIRES_ABSENT ? (car->wireState > 0 ? 0x447 : 0x446) : 0;
    if (p6 = car->rack.parts[0])
        g_partPics[1][0] = g_parts[p6->idx].variant == MANIFOLD_TWO_BARREL ? 0x438 : 0x439;
    for (key = 0; key < 5; key++)
        o_8290[key] = (p6 = car->rack.parts[key]) ? g_partPics[key][g_parts[p6->idx].variant] : 0;
    g_cPos[0][0] = g_cPos[0][1] = o_8290[0] == 0x434 ? 4 : 5;
    cnt1 = (o_8290[0] != 0) + (o_8290[1] != 0) + (o_8290[2] != 0) + (o_8290[3] != 0) + (o_8290[4] != 0);
    n = (p6 = car->rack.parts[1]) ? g_nCarb[g_parts[p6->idx].variant] + 2 : 5;
    if (p6) {
        tmp29 = g_parts[p6->idx].variant;
        for (k = 3; k < 9; k++)
            g_boltSlots[k + 1] = g_cPos[tmp29][k - 3];
    }
    cu_ctl(CURSOR_HIDE_NOW);
    gx_draw_parts(car);
    gx_draw_bolts(g_boltSlots + 1, g_boltSubslots + 1, car == a_7eaap.currentCar);
    cu_ctl(CURSOR_SHOW_NOW);
    if (car != a_7eaap.currentCar)
        return;
    gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
    mn_push(KM_ENGINE);
    for (;;) {
        while ((key = sy_wait_input(3000)) == 0)
            ;
        switch (key) {
        case KR_ENGINE_DONE:
            mn_pop(1);
            r_vid.hoodBg = 0;
            ma_release();
            return;
        case KR_ENGINE_WIRES:
            car->wireState = -car->wireState;
            if (car->wireState == WIRES_ABSENT)
                continue;
            if (car->wireState > 0) {
                g_slotSelec.wirePic = 0x447;
                gx_draw_wires();
                g_gameClock.actionTicks += 0x6d;
                gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
                continue;
            }
            g_slotSelec.wirePic = 0x446;
            gx_draw_wires();
            g_gameClock.actionTicks += 0x6d;
            g_slotSelec.slot = 0;
            gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
            break;
        case KR_ENGINE_TIMING:
            if (ga_ready(car, 0)) {
                tune_timing(car);
                cu_ctl(CURSOR_HIDE_NOW);
                gx_draw_bolts(g_boltSlots + 1, g_boltSubslots + 1, 1);
                cu_ctl(CURSOR_SHOW_NOW);
            } else
                gm_message(0xea1);
            continue;
        case KR_ENGINE_PARTS:
            ga_spares(g_nextPartCategory[cnt1], list4, buf, 0);
        again:
            key = mn_modal_list(KM_SPARE_PARTS, 1, 0, g_list, buf, 0);
            switch (key) {
            case KR_ACCEPT:
                key = mn_sel();
                break;
            case KR_EMPTY_LIST:
                if (g_nextPartCategory[cnt1] == PART_ENGINE)
                    gm_message(0xf97);
                if (g_nextPartCategory[cnt1] == PART_MANIFOLD)
                    gm_message(0xfd7);
                if (g_nextPartCategory[cnt1] == PART_CARBURETOR)
                    gm_message(0xfb5);
                key = 0;
                break;
            case KR_CANCEL:
                key = 0;
                break;
            }
            if (key == 0)
                continue;
            if (cnt1 >= n) {
                gm_message(0x1875);
                continue;
            }
            sel2 = cnt1;
            switch (cnt1) {
            default:
                goto again;
            case 0:
                ok = g_parts[list4[key]->idx].fitMask & g7d86n[car->specIndex].parts & 0xf;
                g_slotSelec.slot = 0;
                step = 0;
                car->wireState = WIRES_DISCONNECTED;
                g_slotSelec.wirePic = 0x446;
                g_gameClock.actionTicks += ok ? 0x666 : 0x888;
                break;
            case 1:
                ok = (g_parts[k = list4[key]->idx].fitMask & g7d86n[car->specIndex].parts & 0xf) != 0
                     && g_manifoldFit[g_parts[car->rack.parts[0]->idx].variant][tmp29 = g_parts[k].variant] != 0;
                g_slotSelec.slot = 1;
                step = 1;
                n = g_nCarb[g_parts[list4[key]->idx].variant] + 2;
                for (k = 4; k < 10; k++)
                    g_boltSlots[k] = g_cPos[tmp29][k - 4];
                g_gameClock.actionTicks += ok ? 0x333u : 0x444u;
                break;
            case 2:
            case 3:
            case 4:
                ok = (g_parts[k = list4[key]->idx].fitMask & g7d86n[car->specIndex].parts & 0xf) != 0
                     && g_carbFit[tmp29 = g_parts[car->rack.parts[1]->idx].variant][g_parts[k].variant] != 0;
                g_gameClock.actionTicks += ok ? 0x199u : 0x222u;
                step = 3;
                sel2 = 2;
                switch (g_slotSelec.slot = g_cPos[tmp29][0]) {
                case 4:
                case 5:
                case 6:
                    break;
                case 7:
                    if (car->rack.parts[2] == 0)
                        break;
                    if (car->rack.parts[3]) {
                        g_slotSelec.slot = 9;
                        step += 4;
                        sel2 = 4;
                    } else {
                        g_slotSelec.slot = 8;
                        step += 2;
                        sel2 = 3;
                    }
                    break;
                case 10:
                case 12:
                    if (car->rack.parts[2] == 0)
                        break;
                    g_slotSelec.slot++;
                    step += 2;
                    sel2++;
                    break;
                default:
                    goto again;
                }
            }
            o_8290[sel2] = k = g_partPics[sel2][g_parts[list4[key]->idx].variant];
            if (ok) {
                ga_install(ga_unlink(list4[key]), &car->rack.parts[sel2]);
                cu_ctl(CURSOR_HIDE_NOW);
                so_part_move(1);
                gx_draw_part(o_8290[sel2], 0);
                so_part_move(0);
                so_part_set();
                if (sel2 == 0) {
                    g_partPics[1][0] = k == 0x434 ? 0x438 : 0x439;
                    g_cPos[0][0] = g_cPos[0][1] = k == 0x434 ? 4 : 5;
                    g_slotSelec.wirePic = 0x446;
                    gx_draw_wires();
                }
                for (k = g_nBolt[cnt1]; --k >= 0;) {
                    g_slotSelec.subslot = k;
                    gx_draw_bolt(g_engineBoltPicIds[step + k] = g_boltPics[1]);
                    so_bolt();
                    havek[step + k + 1] = 1;
                    car->engineBolts[step + k] = 1;
                }
                gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
                cu_ctl(CURSOR_SHOW_NOW);
                cnt1++;
                car->timing = rn_int(13) - 8;
                continue;
            } else {
                so_part_move(1);
                gx_draw_part(o_8290[sel2], 1);
                so_part_move(0);
                so_part_set();
                gm_message(0x10cd);
                so_part_move(1);
                gx_erase_part(o_8290[sel2], 1);
                so_part_move(0);
                o_8290[sel2] = 0;
                if (sel2 == 0) {
                    car->wireState = WIRES_ABSENT;
                    g_slotSelec.wirePic = 0;
                }
                goto again;
            }
        default:
            if (key < 1 || key > 9)
                continue;
            k = tmp29 = 0;
            for (; k < 5; k++)
                if ((tmp29 += g_nBolt[k]) >= key)
                    break;
            if (o_8290[k] == 0)
                continue;
            g_gameClock.actionTicks += 0x36;
            if (havek[key] == 2) {
                g_slotSelec.slot = g_boltSlots[key];
                g_slotSelec.subslot = g_boltSubslots[key];
                if (--car->engineBolts[key - 1] <= 1) {
                    g_engineBoltPicIds[key - 1] = havek[key] = car->engineBolts[key - 1] = 0;
                    gx_drop_bolt();
                    so_bolt();
                } else {
                    gx_draw_bolt(g_engineBoltPicIds[key - 1] = g_boltPics[car->engineBolts[key - 1]]);
                    so_bolt_turn();
                }
            done:
                sel2 = -1;
                ok = 0;
                switch (g_slotSelec.slot) {
                default:
                    if (g_boltSubslots[k = key] == 1)
                        k--;
                    if (havek[k] != 0 || havek[k + 1] != 0)
                        goto strip;
                    sel2 = (k - 4) / 2 + 2;
                    if (cnt1 == 3)
                        ok = 2;
                    g_gameClock.actionTicks += 0x199;
                    goto strip;
                case 0:
                    break;
                case 1:
                    if (cnt1 != 2)
                        goto strip;
                    for (key = 2; key < 4 && havek[key] == 0; key++)
                        ;
                    if (key != 4)
                        goto strip;
                    sel2 = 1;
                    ok = 1;
                    g_gameClock.actionTicks += 0x333;
                    goto strip;
                }
            } else {
                if (havek[key] == 1) {
                    if (car->engineBolts[key - 1] == 3) {
                        havek[key] = 2;
                        continue;
                    }
                    g_engineBoltPicIds[key - 1] = g_boltPics[++car->engineBolts[key - 1]];
                    g_slotSelec.slot = g_boltSlots[key];
                    g_slotSelec.subslot = g_boltSubslots[key];
                    gx_draw_bolt(g_engineBoltPicIds[key - 1]);
                    so_bolt_turn();
                    continue;
                }
                g_slotSelec.slot = g_boltSlots[key];
                g_slotSelec.subslot = g_boltSubslots[key];
                gx_draw_bolt(g_engineBoltPicIds[key - 1] = g_boltPics[1]);
                so_bolt();
                havek[key] = 1;
                car->engineBolts[key - 1] = 1;
                continue;
            }
        }
        sel2 = -1;
        ok = 0;
        if (cnt1 == 1 && car->wireState == WIRES_DISCONNECTED) {
            for (key = 1; key < 2 && !havek[key]; key++)
                continue;
            if (key == 2) {
                sel2 = 0;
                car->wireState = WIRES_ABSENT;
                g_slotSelec.wirePic = 0;
                g_gameClock.actionTicks += 0x666;
            }
        }
    strip:
        if (sel2 >= 0) {
            cnt1--;
            remove_part(&car->rack.parts[sel2]);
            cu_ctl(CURSOR_HIDE_NOW);
            o_8290[sel2] = 0;
            if (ok)
                g_slotSelec.slot = ok - 1;
            gx_draw_parts(car);
            gx_draw_bolts(g_boltSlots + 1, g_boltSubslots + 1, 1);
            gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
            car->timing = rn_int(13) - 8;
            cu_ctl(CURSOR_SHOW_NOW);
        }
        if (ok == 0)
            continue;
        goto done;
    }
}
