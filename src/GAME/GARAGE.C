#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
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
#include "INPUTDAT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "PC.H"
#include "PERF.H"
#include "POOLS.H"
#include "POPUP.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "STRUCTS.H"
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

int g_tirePics[3][4] = {{PIC_TIRE_STANDARD_1, PIC_TIRE_STANDARD_2, PIC_TIRE_STANDARD_3, PIC_TIRE_STANDARD_4},
                        {PIC_TIRE_BRAND_NAME_1, PIC_TIRE_BRAND_NAME_2, PIC_TIRE_BRAND_NAME_3, PIC_TIRE_BRAND_NAME_4},
                        {PIC_TIRE_SLICK_1, PIC_TIRE_SLICK_2, PIC_TIRE_SLICK_3, PIC_TIRE_SLICK_4}};
static int g_colX = 33;
static int g_rowY[4] = {14, 107, 15, 105};
static int g_wOff[4] = {0, 5, 9, 14};


int ga_day(void)
{
    return (int)((g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase) / GAME_DAY_TICKS) >
                   GAME_LAST_DAY
               ? GAME_LAST_DAY
               : (int)((g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase) / GAME_DAY_TICKS);
}


void ga_time(int *a, int *b)
{
    long t;
    int r;

    t = g_gameClock.actionTicks + g_gameClock.biosTicks - g_gameClock.timeBase;
    if (t > GAME_LAST_TICK)
        t = GAME_LAST_TICK;
    r = (int)(t % GAME_DAY_TICKS);
    *a = (unsigned)r / GAME_HOUR_TICKS;
    *b = (long)((unsigned long)(unsigned)(r - GAME_HOUR_TICKS * *a) * 60) / GAME_HOUR_TICKS;
}


void ga_check_end(int n)
{
    if (ga_day() >= GAME_LAST_DAY)
        g_end |= END_TIME_UP;
}


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
    } else if (v < GAME_LAST_DAY) {
        r = 3;
        *out = v - 76;
    } else {
        r = 3;
        *out = 15;
    }
    return r;
}


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
    id = m < 2 ? PIC_CALENDAR_JUNE_JULY : PIC_CALENDAR_AUGUST_SEPTEMBER;
    cu_ctl(CURSOR_HIDE);
    pic = di_bm(id, ARENA_HIGH_STACK);
    bm_draw_pic(pic, PIC_CALENDAR_DAY_OUTLINE, (unsigned char)x, (unsigned char)y4, 0);
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

static char *g_mon[4] = {"June", "July", "August", "September"};
static int g_hIds[5] = {11, 13, 9, 10, 12};

/* Draws the current date ("Month day, 1963") */

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


void ga_palette(enum PaintScheme scheme)
{
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) {
        if (g_displayEnabled) {
            vi_palette_reg(PALETTE_CAR_PRIMARY, g_paint[scheme][PAINT_BASE]);
            vi_palette_reg(PALETTE_CAR_SECONDARY, g_paint[scheme][PAINT_HIGHLIGHT]);
        }
        g_palette[PALETTE_CAR_PRIMARY] = g_paint[scheme][PAINT_BASE];
        g_palette[PALETTE_CAR_SECONDARY] = g_paint[scheme][PAINT_HIGHLIGHT];
    }
}

struct CarDisplay g_carDraw;


int ga_hotspot(int id)
{
    int n, k, base;

    base = ((unsigned)g_gar - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotCalendar - (unsigned)g_gar) / sizeof(struct Hot) + 1;
    for (k = 0; k < n; k++)
        if (g_hots[base + k].id == id)
            return base + k;
}


static void near sort_spots(void)
{
    int base, n;

    base = ((unsigned)g_gar - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotCalendar - (unsigned)g_gar) / sizeof(struct Hot) + 1;
    gx_sort_hots(base, n);
}


static void near hide_spots(void)
{
    int i;

    for (i = 0; i < 5; i++)
        g_hots[ga_hotspot(g_hIds[i])].kind = HOT_DISABLED;
}

/* Rect + hotspot setup for the 5 button rects of panel n */

static void near place_spots(int n, int x, int y)
{
    struct Panel *panel;
    struct Hot *hotspot;

    panel = &g_bodyRects[n];
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


void ga_clear_name(void)
{
    char *s;

    s = "                                ";
    strcpy(g_carName, s);
}


static int near is_broke(void)
{
    if (g_driver.currentCar == 0 && g_driver.otherCars == 0 && g_driver.spareParts == 0 &&
        g_driver.money < BROKE_CASH_LIMIT)
        return 1;
    return 0;
}

char g_paint[PAINT_SCHEME_COUNT][PAINT_SHADE_COUNT] = {{4, 12}, {3, 11}, {2, 10}, {5, 13}, {1, 9}, {7, 15}};
static int g_pad[7] = {0, 5, 5, 5, 5, 0, 0};


int near ga_menu(enum GarageMode mode)
{
    char *p;
    int r;
    int n;

    if (g_cVis > 0)
        cu_ctl(CURSOR_HIDE_NOW);
    ga_check_end(0);
    if (g_end) {
        if (g_end & END_TIME_UP)
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
            gm_message(MSG_HERE_IT_IS);
            cu_ctl(CURSOR_HIDE_NOW);
            break;
        }
        if (g_old) {
            n = gm_car_look(g_old);
            cp_show_car(CAR_BACK_OUT, g_old->specIndex, n, (g_old->appearance >> 8) & STICKER_INDEX_MASK, 0, 0x98, 0,
                        g_tirePics[g_parts[g_old->tires->idx].variant]);
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
            sc_picture(PIC_GARAGE, -1);
            vi_garage_pal();
            if (mode == GARAGE_REDRAW && g_driver.currentCar == 0 && g_driver.otherCars) {
                g_driver.currentCar = g_driver.otherCars;
                g_driver.otherCars = g_driver.currentCar->next;
                mode = GARAGE_DRIVE_IN;
            }
        }
        r_vid.copyToFront = 1;
        (*g_videoOps.show)();
        if (g_driver.currentCar) {
            g_carDraw.bodyPic = g_carData[g_driver.currentCar->specIndex].bodyPic;
            g_carDraw.paintScheme = g_driver.currentCar->paintScheme;
            g_carDraw.tireVariant = g_parts[g_driver.currentCar->tires->idx].variant;
            if (mode == GARAGE_DRIVE_IN)
                ga_palette(g_carDraw.paintScheme);
            g_carDraw.overlayPic = 0;
            g_carDraw.specIndex = g_driver.currentCar->specIndex;
            g_carDraw.appearance.sticker = (g_driver.currentCar->appearance >> 8) & STICKER_INDEX_MASK;
            g_carDraw.appearance.roof = (g_driver.currentCar->appearance >> 13) & 1;
            g_carDraw.appearance.front = (g_driver.currentCar->appearance >> 14) & 1;
            g_carDraw.appearance.rear = (g_driver.currentCar->appearance >> 15) & 1;
            for (r = 0; r < ENGINE_RACK_SLOTS; r++)
                g_partsPics[r] = g_driver.currentCar->rack.parts[r]->idx;
            n = gm_car_look(g_driver.currentCar);
            strcpy(g_carName, g_desc + g_carData[g_driver.currentCar->specIndex].nameOffset);
            if (mode == GARAGE_REDRAW)
                cp_show_car(CAR_SHOW_BACK_PAGE, g_carDraw.specIndex, n, g_carDraw.appearance.sticker, 0, 0x98, 0,
                            g_tirePics[g_carDraw.tireVariant]);
            else
                cp_show_car(CAR_PULL_IN, g_carDraw.specIndex, n, g_carDraw.appearance.sticker, 0, 0x98, 0,
                            g_tirePics[g_carDraw.tireVariant]);
        } else {
            bm_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), 0xc7);
            g_carName[0] = 0;
        }
        break;
    case GARAGE_KEEP:
        break;
    }
    if (g_driver.currentCar)
        place_spots(g_carDraw.specIndex, g_picXDraw, g_sprDrawY);
    else
        hide_spots();
    sort_spots();
    sc_label(g_cashLabel);
    cu_ctl(CURSOR_SHOW_NOW);
    if (g_driver.carCount > MAX_OWNED_CARS)
        gm_message(MSG_NO_ROOM_SELL_A_CAR);
    else if (g_driver.partCount > MAX_OWNED_PARTS) {
        if (g_driver.partCount == MAX_OWNED_PARTS + 1)
            gm_message(MSG_TOO_MUCH_JUNK_SELL_A_PART);
        else {
            p = g_junk;
            p += strlen(p) + 1;
            p += strcspn(p, "1234567890");
            itoa(g_driver.partCount - MAX_OWNED_PARTS, p, 10);
            p[strlen(p)] = ' ';
            gm_message(MSG_TOO_MUCH_JUNK_SELL_12_PARTS);
        }
    }
    if (is_broke()) {
        gm_message(MSG_YOURE_OUTTA_DOUGH);
        g_end |= END_BROKE;
    } else {
        mu_suspend(0);
        sy_reset_queues();
        r = mn_wait(KM_GARAGE);
    }
    ga_check_end(0);
    if (g_end & END_BROKE)
        g_end = END_BROKE;
    if (g_end) {
        r = GARAGE_GAME_ENDED;
        if ((g_end & END_TIME_UP) == 0)
            cu_ctl(CURSOR_HIDE_NOW);
    }
    return r;
}


void near ga_paint(void)
{
    struct Bitmap far *popup;
    int x;
    enum PaintScheme paint;
    int y;
    int key;

    if (!g_driver.currentCar)
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
    else if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY)
        gm_message(MSG_BETTER_GRAPHICS_TO_PAINT);
    else {
        popup = mn_grab(KM_PAINT, &x, &y, ARENA_HIGH_STACK);
        mn_push(KM_PAINT);
        paint = g_driver.currentCar->paintScheme;
        for (;;) {
            vi_palette_reg(PALETTE_RIVAL_PRIMARY, g_paint[paint][PAINT_BASE]);
            while ((key = sy_wait_input(3000)) == 0)
                ;
            switch (key) {
            case KR_PAINT_PREVIOUS:
                if (--paint < PAINT_RED)
                    paint = PAINT_WHITE;
                continue;
            case KR_PAINT_NEXT:
                if (++paint >= PAINT_SCHEME_COUNT)
                    paint = PAINT_RED;
                continue;
            case KR_PAINT_GO_AHEAD:
                if (g_driver.money < (long)PAINT_PRICE) {
                    gm_message(MSG_PAINT_AINT_FREE);
                    break;
                }
                r_vid.copyToFront = 1;
                mn_pop(1);
                pu_show(popup, x, y);
                ma_pop_high(1);
                pu_show(g_stickerBg, g_carDraw.stickerX, g_carDraw.stickerY);
                g_driver.currentCar->appearance &= APPEARANCE_NO_STICKER;
                ga_palette(g_driver.currentCar->paintScheme = paint);
                vi_palette_reg(PALETTE_RIVAL_PRIMARY, 5);
                g_gameClock.actionTicks += (long)TIME_PAINT;
                sc_add_money(-PAINT_PRICE);
                return;
            case KR_FORGET:
            default:
                break;
            }
            break;
        }
        mn_pop(1);
        vi_palette_reg(PALETTE_RIVAL_PRIMARY, 5);
        pu_show(popup, x, y);
        ma_pop_high(1);
    }
}


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

    p9 = &g_models[g_driver.currentCar->specIndex];
    if (!g_driver.currentCar) {
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
        return;
    }
    if (mask & BODY_ROOF) {
        if (p9->choppedRoofPic == 0) {
            gm_message(MSG_CANT_CHOP_THIS_ROOF);
            return;
        }
        msg = (g_driver.currentCar->appearance & APPEARANCE_ROOF) ? (g_txt + TXT_REBUILD_ROOF_DOLLARS_70)
                                                                  : (g_txt + TXT_CHOP_ROOF_DOLLARS_70);
        cost = ROOF_CHANGE_PRICE;
        val5 = TIME_ROOF_CHANGE;
    }
    if (mask & BODY_FRONT_BUMPER) {
        if (p9->frontBumperRemovalPic == 0) {
            gm_message(MSG_ALREADY_LOST_YOUR_BUMPER);
            return;
        }
        msg = (g_driver.currentCar->appearance & APPEARANCE_FRONT_BUMPER) ? (g_txt + TXT_RESTORE_BUMPER_DOLLARS_15)
                                                                          : (g_txt + TXT_STRIP_BUMPER_DOLLARS_15);
        cost = BUMPER_CHANGE_PRICE;
        val5 = TIME_BUMPER_CHANGE;
    }
    if (mask & BODY_REAR_BUMPER) {
        if (p9->rearBumperRemovalPic == 0) {
            gm_message(MSG_ALREADY_LOST_YOUR_BUMPER);
            return;
        }
        msg = (g_driver.currentCar->appearance & APPEARANCE_REAR_BUMPER) ? (g_txt + TXT_RESTORE_BUMPER_DOLLARS_15)
                                                                         : (g_txt + TXT_STRIP_BUMPER_DOLLARS_15);
        cost = BUMPER_CHANGE_PRICE;
        val5 = TIME_BUMPER_CHANGE;
    }
    if ((long)cost > g_driver.money) {
        gm_message(MSG_CUSTOMIZATION_AINT_FREE);
        return;
    }
    bm1 = mn_grab(KM_BODY_CHANGE, &x, &y, ARENA_HIGH_STACK);
    g_bodyAsk[0].textOffset = msg - g_txt;
    mn_push(KM_BODY_CHANGE);
    while ((key = sy_wait_input(3000)) == 0)
        ;
    switch (key) {
    case KR_OK:
        if (mask & BODY_ROOF)
            g_driver.currentCar->appearance ^= APPEARANCE_ROOF;
        if (mask & BODY_FRONT_BUMPER)
            g_driver.currentCar->appearance ^= APPEARANCE_FRONT_BUMPER;
        if (mask & BODY_REAR_BUMPER)
            g_driver.currentCar->appearance ^= APPEARANCE_REAR_BUMPER;
        cu_ctl(CURSOR_HIDE_NOW);
        mn_pop(1);
        pu_show(bm1, x, y);
        ma_pop_high(1);
        n9 = g_driver.currentCar->specIndex;
        r7 = gm_car_look(g_driver.currentCar);
        cp_show_car(CAR_SHOW_BACKDROP, n9, r7, (g_driver.currentCar->appearance >> 8) & STICKER_INDEX_MASK, 0, 0x98, 0,
                    g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
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

    if (!g_driver.currentCar)
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
    else if (g_driver.money < (long)STICKER_PRICE)
        gm_message(MSG_STICKERS_AINT_FREE);
    else {
        r_vid.copyToFront = 1;
        x = 24;
        y = 11;
        bm1 = ar_bm(264, 69, 0, ARENA_HIGH_STACK);
        pu_grab(x, y, 264, 69, bm1);
        sc_picture_at(PIC_STICKER_MENU, x, y, -1);
        mn_push(KM_STICKERS);
        while ((k = sy_wait_input(3000)) == 0)
            ;
        switch (k) {
        case KR_STICKER_8:
        case KR_STICKER_7:
        case KR_STICKER_6:
        case KR_STICKER_5:
        case KR_STICKER_4:
        case KR_STICKER_3:
        case KR_STICKER_2:
        case KR_STICKER_1:
            cu_ctl(CURSOR_HIDE);
            mn_pop(1);
            pu_show(bm1, x, y);
            ma_pop_high(1);
            idx6 = -30 - k;
            g_driver.currentCar->appearance =
                ((idx6 & STICKER_INDEX_MASK) << 8) | (g_driver.currentCar->appearance & APPEARANCE_NO_STICKER);
            bm3 = di_bm(PIC_STICKER_WIPE, ARENA_HIGH_STACK);
            bm1 = ar_bm(g_stickerBg->width, g_stickerBg->height, 0, ARENA_HIGH_STACK);
            bm_copy(g_stickerBg, bm1, 1);
            q2 = &g_imageDecalLayout[idx6 - 1];
            bm_draw_pic(bm1, q2->id, (char)(q2->dx + g_carDraw.stickerSubX), (char)q2->dy, 0);
            bm4 = ar_bm(bm1->width, bm1->height, 0, ARENA_HIGH_STACK);
            bm_copy(bm1, bm4, 1);
            A2.sx = B1.sx = C.sx = 0;
            A2.sy = B1.sy = C.sy = 0;
            A2.dy = B1.dy = 0;
            A2.dx = B1.dx = 0;
            C.dx = g_carDraw.stickerX;
            C.dy = g_carDraw.stickerY;
            B1.h = B1.unused1 = A2.h = C.h = bm1->height;
            A2.w = B1.w = C.w = 8;
            so_sticker(1);
            for (k = 0; k < bm1->width; k += 8) {
                (*g_videoOps.copy)(bm1, bm4, &A2, 1);
                if (bm1->width - 8 > k)
                    (*g_videoOps.maskedBlit)(bm3, bm4, &B1, 1);
                da_retrace(2);
                (*g_videoOps.copy)(bm4, r_vid.front, &C, 1);
                A2.w += 8;
                C.w += 8;
                B1.dx += 8;
            }
            pu_show(bm1, g_carDraw.stickerX, g_carDraw.stickerY);
            ma_pop_high(3);
            so_sticker(0);
            g_gameClock.actionTicks += (long)TIME_STICKER;
            sc_add_money(-STICKER_PRICE);
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


void near ga_spares(enum PartCategory t, struct Node **list, char *buf, int start)
{
    struct Node *p;
    struct Item *ent7;
    int n;

    n = start;
    p = g_driver.spareParts;
    while (p) {
        if ((ent7 = &g_parts[p->idx])->category == t) {
            n++;
            list[n] = p;
            g_list[n] = ent7->nameOffset;
            if (p->wear != PART_WEAR_UNTRACKED)
                buf[n] = p->wear / 100;
            else
                buf[n] = PART_WEAR_LABEL_NONE;
        }
        p = p->next;
    }
    g_list[0] = n;
}


void near ga_install(struct Node *n, struct Node **slot)
{
    if (*slot) {
        (*slot)->next = g_driver.spareParts;
        g_driver.currentCar->value -= gm_part_value(g_driver.spareParts = *slot);
    }
    *slot = n;
    n->next = 0;
    g_driver.currentCar->value += gm_part_value(n);
}


static void near remove_part(struct Node **slot)
{
    (*slot)->next = g_driver.spareParts;
    g_driver.currentCar->value -= gm_part_value(g_driver.spareParts = *slot);
    *slot = 0;
}


struct Node *near ga_unlink(struct Node *n)
{
    struct Node *p1, *q;

    for (p1 = g_driver.spareParts, q = 0; p1;)
        if (p1 != n)
            p1 = (q = p1)->next;
        else
            break;
    if (q == 0)
        g_driver.spareParts = p1->next;
    else
        q->next = p1->next;
    return p1;
}


void near ga_tires(void)
{
    struct Car *car;
    int sel;
    struct Node *tbl[142];
    char line[142];
    int y, cur;

    car = g_driver.currentCar;
    if (car == 0) {
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
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
        gm_message(MSG_NO_SPARE_TIRES);
        return;
    }
    y = g_parts[car->tires->idx].variant;
    cur = g_parts[tbl[sel]->idx].variant;
    ga_install(ga_unlink(tbl[sel]), &car->tires);
    ti_change(g_tirePics[y], g_tirePics[cur]);
    g_gameClock.actionTicks += (long)TIME_TIRE_CHANGE;
}


static void near car_info(int *ids, char *vals)
{
    int n;
    struct Item *it;
    int count;
    struct Node *node;
    struct Node *gear[7];

    for (n = 0; n < 5; n++)
        gear[n] = g_driver.currentCar->rack.parts[n];
    gear[n++] = g_driver.currentCar->transmission;
    gear[n] = g_driver.currentCar->tires;

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

static char *g_iTxt[4] = {"", "Fuel                  ", "", "Max speed      mph"};
int g_nCarb[5] = {1, 1, 3, 2, 2};
int g_nBolt[6] = {1, 2, 2, 2, 2, 2};
static enum PartCategory g_nextCat[6] = {PART_ENGINE,     PART_MANIFOLD,   PART_CARBURETOR,
                                         PART_CARBURETOR, PART_CARBURETOR, PART_CARBURETOR};
static int g_cPos[5][6] = {
    {5, 5, 0, 0, 0, 0}, {6, 6, 0, 0, 0, 0}, {7, 7, 8, 8, 9, 9}, {10, 10, 11, 11, 0, 0}, {12, 12, 13, 13, 0, 0}};
static int g_boltSlots[10] = {0, 0, 1, 1, 5, 5, 5, 5, 5, 5};
static int g_boltSubslots[10] = {0, 0, 0, 1, 0, 1, 0, 1, 0, 1};


void ga_info(void)
{
    int ids[143];
    char vals[144];

    if (g_driver.currentCar == 0) {
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
        return;
    }
    car_info(ids, vals);
    sprintf(g_iTxt[3] + 9, " %3i mph", ga_ready(g_driver.currentCar, 1) ? pf_vmax(-1) : 0);
    sprintf(g_iTxt[1] + 5, " %2i gallons", g_driver.currentCar->fuel / FUEL_UNITS_PER_GALLON);
    switch (mn_modal_list(KM_CAR_INFO, 1, ids, g_list, vals, g_iTxt)) {
    case KR_ACCEPT:
        return;
    case KR_EMPTY_LIST:
        gm_message(MSG_NO_SPARE_TIRES);
        return;
    }
}


void near ga_transmission(void)
{
    int flags[3]; /* indexed by key 1 or 2 */
    char buf[142];
    int prev;
    struct Car *owner;
    int k;
    struct Node *tbl[142];

    owner = g_driver.currentCar;
    if (!owner) {
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
        return;
    }
    for (k = 0; k < TRANSMISSION_BOLT_COUNT; k++) {
        if (prev = owner->transmissionBolts[k])
            flags[k + 1] = prev == BOLT_INSERTED ? 1 : 2;
        else
            flags[k + 1] = 0;
        g_transBolts[k].kind = HOT_REGION;
    }
    g_carDraw.transmissionPic = owner->transmission ? g_transPics[g_parts[owner->transmission->idx].variant] : 0;
    r_vid.copyToFront = !g_carDraw.transmissionPic;
    sc_picture_at(PIC_TRANSMISSION_DIALOG, 0x20, 10, -1);
    r_vid.copyToFront = 1;
    gx_trans_draw(g_carDraw.transmissionPic, owner->transmissionBolts);
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
                        gm_message(MSG_WHERE_WILL_YOU_PUT_IT);
                        k = 0;
                    } else
                        k = mn_sel();
                    break;
                case KR_CANCEL:
                    k = 0;
                    break;
                case KR_EMPTY_LIST:
                    gm_message(MSG_NO_SPARE_TRANSMISSIONS);
                    k = 0;
                    break;
                }
                if (!k)
                    break;
                prev = g_carDraw.transmissionPic;
                g_carDraw.transmissionPic = g_transPics[g_parts[tbl[k]->idx].variant];
                if (g_carData[owner->specIndex].stockParts & g_parts[tbl[k]->idx].fitMask & SPEC_FIT_MASK) {
                    ga_install(ga_unlink(tbl[k]), &owner->transmission);
                    cu_ctl(CURSOR_HIDE_NOW);
                    so_part_move(1);
                    gx_trans_pic(g_carDraw.transmissionPic, 0);
                    so_part_move(0);
                    so_part_set();
                    g_gameClock.actionTicks += (long)TIME_TRANSMISSION_CHANGE;
                    for (k = TRANSMISSION_BOLT_COUNT; k--;) {
                        flags[k + 1] = 1;
                        gx_trans_bolt(owner->transmissionBolts[k] = BOLT_INSERTED, k);
                        so_bolt();
                        g_transBolts[k].kind = HOT_REGION;
                    }
                    sy_hover_reset();
                    cu_ctl(CURSOR_SHOW_NOW);
                    break;
                }
                gx_trans_pic(g_carDraw.transmissionPic, 1);
                gm_message(MSG_PAY_ATTENTION_WONT_FIT);
                gx_trans_clear(g_carDraw.transmissionPic, 1);
                g_gameClock.actionTicks += (long)TIME_TRANSMISSION_MISFIT;
                g_carDraw.transmissionPic = prev;
            }
            break;
        case 1:
        case 2:
            if (g_carDraw.transmissionPic == 0)
                break;
            if (flags[k--] == 2) {
                if (--owner->transmissionBolts[k] <= BOLT_INSERTED) {
                    flags[k + 1] = owner->transmissionBolts[k] = BOLT_ABSENT;
                    gx_trans_drop(k);
                    so_bolt();
                    sy_hover_reset();
                } else {
                    gx_trans_bolt(owner->transmissionBolts[k], k);
                    so_bolt_turn();
                }
                if (flags[1] || flags[2])
                    break;
                gx_trans_clear(g_carDraw.transmissionPic, 0);
                g_gameClock.actionTicks += (long)TIME_TRANSMISSION_CHANGE;
                g_carDraw.transmissionPic = 0;
                owner->transmissionBolts[0] = owner->transmissionBolts[1] = BOLT_ABSENT;
                remove_part(&owner->transmission);
                sy_hover_reset();
            } else if (flags[k + 1] == 1) {
                if (owner->transmissionBolts[k] == BOLT_TIGHT) {
                    flags[k + 1] = 2;
                    break;
                }
                gx_trans_bolt(++owner->transmissionBolts[k], k);
                so_bolt_turn();
            } else {
                cu_ctl(CURSOR_HIDE_NOW);
                gx_trans_bolt(owner->transmissionBolts[k] = BOLT_INSERTED, k);
                so_bolt();
                cu_ctl(CURSOR_SHOW_NOW);
                flags[k + 1] = 1;
            }
            break;
        }
    }
}


int ga_ready(struct Car *car, int flag)
{
    int n, i;

    if (car->rack.parts[0] == 0 || car->wireState <= WIRES_ABSENT || car->rack.parts[1] == 0)
        return 0;
    n = g_nCarb[g_parts[car->rack.parts[1]->idx].variant];
    for (i = 2; i < n + 2; i++)
        if (car->rack.parts[i] == 0)
            return 0;
    for (i = n * 2 + 3; --i >= 0;)
        if (car->engineBolts[i] != BOLT_TIGHT)
            return 0;
    if (flag == 0)
        return 1;
    if (car->transmission == 0)
        return 0;
    if (car->tires == 0)
        return 0;
    for (i = TRANSMISSION_BOLT_COUNT; --i >= 0;)
        if (car->transmissionBolts[i] != BOLT_TIGHT)
            return 0;
    return 1;
}


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
    sc_picture_at(PIC_TUNING_DIALOG, 0x60, 9, -1);
    p = g_scriptObjsData + vi_anim_start(8);
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
            if (car->timing > TIMING_MAX)
                car->timing = TIMING_MAX;
            break;
        case KR_TIMING_RETARD:
            car->timing--;
            if (car->timing < TIMING_MIN)
                car->timing = TIMING_MIN;
            break;
        }
    } while (key != KR_TIMING_DONE);
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    g_voiceNum = 1;
    sy_music_resume();
    vi_anim_stop(8);
    n = ((unsigned)g_gameClock.biosTicks - (unsigned)tmp) * 3;
    n = n > TIME_TUNING_MAX ? TIME_TUNING_MAX : n;
    g_gameClock.actionTicks += n;
    mn_pop(1);
}

/* The garage screen: install, remove and tune parts */

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
        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
        return;
    }
    for (key = 1; key <= ENGINE_BOLT_COUNT; key++) {
        if (k = car->engineBolts[key - 1]) {
            havek[key] = k == BOLT_INSERTED ? 1 : 2;
            g_engineBoltPicIds[key - 1] = g_boltPics[k];
        } else {
            havek[key] = 0;
            g_engineBoltPicIds[key - 1] = 0;
        }
    }
    g_slotSelec.wirePic =
        car->wireState != WIRES_ABSENT
            ? (car->wireState > WIRES_ABSENT ? PIC_IGNITION_WIRES_CONNECTED : PIC_IGNITION_WIRES_DISCONNECTED)
            : 0;
    if (p6 = car->rack.parts[0])
        g_partPics[1][0] =
            g_parts[p6->idx].variant == MANIFOLD_TWO_BARREL ? PIC_MANIFOLD_TWO_BARREL_V6 : PIC_MANIFOLD_TWO_BARREL_V8;
    for (key = 0; key < ENGINE_RACK_SLOTS; key++)
        g_partsPics[key] = (p6 = car->rack.parts[key]) ? g_partPics[key][g_parts[p6->idx].variant] : 0;
    g_cPos[0][0] = g_cPos[0][1] = g_partsPics[0] == PIC_ENGINE_V6 ? 4 : 5;
    cnt1 = (g_partsPics[0] != 0) + (g_partsPics[1] != 0) + (g_partsPics[2] != 0) + (g_partsPics[3] != 0) +
           (g_partsPics[4] != 0);
    n = (p6 = car->rack.parts[1]) ? g_nCarb[g_parts[p6->idx].variant] + 2 : 5;
    if (p6) {
        tmp29 = g_parts[p6->idx].variant;
        for (k = 3; k < ENGINE_BOLT_COUNT; k++)
            g_boltSlots[k + 1] = g_cPos[tmp29][k - 3];
    }
    cu_ctl(CURSOR_HIDE_NOW);
    gx_draw_parts(car);
    gx_draw_bolts(g_boltSlots + 1, g_boltSubslots + 1, car == g_driver.currentCar);
    cu_ctl(CURSOR_SHOW_NOW);
    if (car != g_driver.currentCar)
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
            if (car->wireState > WIRES_ABSENT) {
                g_slotSelec.wirePic = PIC_IGNITION_WIRES_CONNECTED;
                gx_draw_wires();
                g_gameClock.actionTicks += TIME_WIRE_CHANGE;
                gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
                continue;
            }
            g_slotSelec.wirePic = PIC_IGNITION_WIRES_DISCONNECTED;
            gx_draw_wires();
            g_gameClock.actionTicks += TIME_WIRE_CHANGE;
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
                gm_message(MSG_CANT_TUNE_THAT_MESS_KLUTZ);
            continue;
        case KR_ENGINE_PARTS:
            ga_spares(g_nextCat[cnt1], list4, buf, 0);
        again:
            key = mn_modal_list(KM_SPARE_PARTS, 1, 0, g_list, buf, 0);
            switch (key) {
            case KR_ACCEPT:
                key = mn_sel();
                break;
            case KR_EMPTY_LIST:
                if (g_nextCat[cnt1] == PART_ENGINE)
                    gm_message(MSG_NO_SPARE_ENGINE);
                if (g_nextCat[cnt1] == PART_MANIFOLD)
                    gm_message(MSG_NO_SPARE_MANIFOLD);
                if (g_nextCat[cnt1] == PART_CARBURETOR)
                    gm_message(MSG_NO_SPARE_CARBURATOR);
                key = 0;
                break;
            case KR_CANCEL:
                key = 0;
                break;
            }
            if (key == 0)
                continue;
            if (cnt1 >= n) {
                gm_message(MSG_WHERE_WILL_YOU_PUT_IT);
                continue;
            }
            sel2 = cnt1;
            switch (cnt1) {
            default:
                goto again;
            case 0:
                ok = g_parts[list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK;
                g_slotSelec.slot = 0;
                step = 0;
                car->wireState = WIRES_DISCONNECTED;
                g_slotSelec.wirePic = PIC_IGNITION_WIRES_DISCONNECTED;
                g_gameClock.actionTicks += ok ? TIME_ENGINE_CHANGE : TIME_ENGINE_MISFIT;
                break;
            case 1:
                ok = (g_parts[k = list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK) !=
                         0 &&
                     g_manifoldFit[g_parts[car->rack.parts[0]->idx].variant][tmp29 = g_parts[k].variant] != 0;
                g_slotSelec.slot = 1;
                step = 1;
                n = g_nCarb[g_parts[list4[key]->idx].variant] + 2;
                for (k = 4; k < 10; k++)
                    g_boltSlots[k] = g_cPos[tmp29][k - 4];
                g_gameClock.actionTicks += ok ? (unsigned)TIME_MANIFOLD_CHANGE : (unsigned)TIME_MANIFOLD_MISFIT;
                break;
            case 2:
            case 3:
            case 4:
                ok = (g_parts[k = list4[key]->idx].fitMask & g_carData[car->specIndex].stockParts & SPEC_FIT_MASK) !=
                         0 &&
                     g_carbFit[tmp29 = g_parts[car->rack.parts[1]->idx].variant][g_parts[k].variant] != 0;
                g_gameClock.actionTicks += ok ? (unsigned)TIME_CARB_CHANGE : (unsigned)TIME_CARB_MISFIT;
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
            g_partsPics[sel2] = k = g_partPics[sel2][g_parts[list4[key]->idx].variant];
            if (ok) {
                ga_install(ga_unlink(list4[key]), &car->rack.parts[sel2]);
                cu_ctl(CURSOR_HIDE_NOW);
                so_part_move(1);
                gx_draw_part(g_partsPics[sel2], 0);
                so_part_move(0);
                so_part_set();
                if (sel2 == 0) {
                    g_partPics[1][0] = k == PIC_ENGINE_V6 ? PIC_MANIFOLD_TWO_BARREL_V6 : PIC_MANIFOLD_TWO_BARREL_V8;
                    g_cPos[0][0] = g_cPos[0][1] = k == PIC_ENGINE_V6 ? 4 : 5;
                    g_slotSelec.wirePic = PIC_IGNITION_WIRES_DISCONNECTED;
                    gx_draw_wires();
                }
                for (k = g_nBolt[cnt1]; --k >= 0;) {
                    g_slotSelec.subslot = k;
                    gx_draw_bolt(g_engineBoltPicIds[step + k] = g_boltPics[BOLT_INSERTED]);
                    so_bolt();
                    havek[step + k + 1] = 1;
                    car->engineBolts[step + k] = BOLT_INSERTED;
                }
                gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
                cu_ctl(CURSOR_SHOW_NOW);
                cnt1++;
                car->timing = rn_int(TIMING_SETTING_COUNT) + TIMING_MIN;
                continue;
            } else {
                so_part_move(1);
                gx_draw_part(g_partsPics[sel2], 1);
                so_part_move(0);
                so_part_set();
                gm_message(MSG_PAY_ATTENTION_WONT_FIT);
                so_part_move(1);
                gx_erase_part(g_partsPics[sel2], 1);
                so_part_move(0);
                g_partsPics[sel2] = 0;
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
            for (; k < ENGINE_RACK_SLOTS; k++)
                if ((tmp29 += g_nBolt[k]) >= key)
                    break;
            if (g_partsPics[k] == 0)
                continue;
            g_gameClock.actionTicks += TIME_BOLT_WORK;
            if (havek[key] == 2) {
                g_slotSelec.slot = g_boltSlots[key];
                g_slotSelec.subslot = g_boltSubslots[key];
                if (--car->engineBolts[key - 1] <= BOLT_INSERTED) {
                    g_engineBoltPicIds[key - 1] = havek[key] = car->engineBolts[key - 1] = BOLT_ABSENT;
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
                    g_gameClock.actionTicks += TIME_CARB_CHANGE;
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
                    g_gameClock.actionTicks += TIME_MANIFOLD_CHANGE;
                    goto strip;
                }
            } else {
                if (havek[key] == 1) {
                    if (car->engineBolts[key - 1] == BOLT_TIGHT) {
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
                gx_draw_bolt(g_engineBoltPicIds[key - 1] = g_boltPics[BOLT_INSERTED]);
                so_bolt();
                havek[key] = 1;
                car->engineBolts[key - 1] = BOLT_INSERTED;
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
                g_gameClock.actionTicks += TIME_ENGINE_CHANGE;
            }
        }
    strip:
        if (sel2 >= 0) {
            cnt1--;
            remove_part(&car->rack.parts[sel2]);
            cu_ctl(CURSOR_HIDE_NOW);
            g_partsPics[sel2] = 0;
            if (ok)
                g_slotSelec.slot = ok - 1;
            gx_draw_parts(car);
            gx_draw_bolts(g_boltSlots + 1, g_boltSubslots + 1, 1);
            gx_build_hots(g_boltSlots + 1, g_boltSubslots + 1);
            car->timing = rn_int(TIMING_SETTING_COUNT) + TIMING_MIN;
            cu_ctl(CURSOR_SHOW_NOW);
        }
        if (ok == 0)
            continue;
        goto done;
    }
}
