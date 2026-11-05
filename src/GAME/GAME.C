#include <stdio.h>
#include <string.h>
#include "GAME.H"
#include "ADS.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CRUISE.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "EDIT.H"
#include "FAME.H"
#include "GARAGE.H"
#include "HOTSPOT.H"
#include "INPUTDAT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "PC.H"
#include "PERF.H"
#include "POOLS.H"
#include "POPUP.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

#define GAME_MENU_BLOCKED 0x309

static void near free_part(struct Node *p7);
static int near counter_offer(unsigned *price);
static int near start_game(void);
static int near read_paper(void);
static void near buy_from_ads(int r);
static int near check_room(void);
static void near sell_parts(void);
static enum GarageMode near your_cars(void);

struct Car *g_old = 0;
struct Car *g_new = 0;
int g_carPulledIn = 0;
int g_picsFreed = 0;
static int g_paperPics[5] = {1022, 1023, 1025, 1024, 1026};
static int g_paperPick = 0;
static int g_paperDay = -1;


static int near start_game(void)
{
    da_set_view(DASH_VIEW_SAVE);
    sy_music_start(2, 10);
    da_intro();
    (*g_videoOps.blank)();
    da_set_view(DASH_VIEW_RESTORE);
    rn_int(-1);
    (*g_videoOps.blank)();
    if (g_loadedIntroBmps == 0)
        di_read_index(2);
    if (g_attractSwitchStartup == 0) {
        if (fa_protect_check() == 0)
            sy_exit();
        cu_ctl(CURSOR_HIDE_NOW);
    }
    rd_generate();
    g_driverName[0] = 0;
    g_demo = g_attractSwitchStartup;
    if (g_loadedIntroBmps == 0) {
        di_load_data();
        if (r_vid.mode == VID_EGA)
            di_cache_list(g_gamePics);
    }
    di_load_list(g_gamePics, 1);
    cu_ctl(CURSOR_SHOW_NOW);
    return -1;
}


void gm_pause(int a)
{
    g_waitUntil = a + g_tick;
    g_input.any = 1;
    sy_wait_input(30000);
    g_waitUntil = 0;
    g_input.any = 0;
}


void gm_message(int n)
{
    long saved;

    saved = g_waitUntil;
    mn_push(KM_MESSAGE);
    pu_open(n);
    gm_pause(g_msgTicks);
    pu_close();
    mn_pop(1);
    g_waitUntil = saved;
}


static int near check_room(void)
{
    if (g_driver.carCount > 15) {
        gm_message(MSG_SELL_SOME_CARS_FIRST);
        return 1;
    }
    if (g_driver.partCount > 130) {
        gm_message(MSG_SELL_SOME_PARTS_FIRST);
        return 1;
    }
    return 0;
}


static int near read_paper(void)
{
    int pick5;
    int tune;
    int old;

    old = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    sc_picture(0x3fd, -1);
    if ((tune = ga_day()) != g_paperDay) {
        if ((pick5 = rn_int(5)) == g_paperPick)
            pick5 = rn_int(5);
        g_paperPick = pick5;
        g_paperDay = tune;
    }
    sc_picture_at(g_paperPics[g_paperPick], 0, 0x88, -1);
    cu_ctl(CURSOR_HIDE);
    ga_date(0x79);
    cu_ctl(CURSOR_SHOW);
    r_vid.copyToFront = old;
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), (SCREEN_BODY_HEIGHT - 1));
    cu_ctl(1);
    return mn_wait(KM_NEWSPAPER);
}


struct Node *near gm_new_part(void)
{
    struct Node *p;

    p = g_driver.freeParts;
    g_driver.freeParts = p->next;
    p->next = 0;
    g_driver.partCount++;
    return p;
}


static void near free_part(struct Node *p)
{
    p->next = g_driver.freeParts;
    p->idx = -1;
    g_driver.freeParts = p;
    g_driver.partCount--;
}


int gm_part_value(struct Node *n)
{
    int r;

    if (g_parts[n->idx].category == PART_ENGINE && g_parts[n->idx].variant == ENGINE_V6)
        r = 100;
    else
        r = n->value;
    return r;
}

#pragma loop_opt(off)


static void near sell_parts(void)
{
    unsigned price1;
    struct Node *list8[142];
    unsigned sel;
    struct Node *p7;
    unsigned pct;
    struct Item *ip;
    char lbl[142];
    int type8;
    int tries3;
    int nl;
    int msg5;
    int cut;
    int rl;

    tries3 = 0;
    sel = 1;
    for (;; tries3++) {
        for (p7 = g_driver.spareParts, nl = 0; p7; p7 = p7->next) {
            nl++;
            g_list[nl] = (ip = &g_parts[(list8[nl] = p7)->idx])->nameOffset;
            if (p7->wear != -0x80) {
                lbl[nl] = p7->wear / 100;
                continue;
            }
            lbl[nl] = 0x80;
        }
        g_list[0] = nl;
        strcpy(g_msgSellChange, "Sell it");
        g_keyMaps[KM_SPARE_PARTS]->key = SC_S;
        sel = mn_modal_list(KM_SPARE_PARTS, sel <= g_list[0] ? sel : g_list[0], 0, g_list, lbl, 0);
        g_keyMaps[KM_SPARE_PARTS]->key = SC_C;
        strcpy(g_msgSellChange, "Change");
        switch ((int)sel) {
        case KR_CANCEL:
            return;
        case KR_EMPTY_LIST:
            gm_message(tries3 ? MSG_ALL_PARTS_GONE : MSG_NO_SPARE_PARTS);
            return;
        case KR_ACCEPT:
            sel = mn_sel();
            break;
        }
        p7 = list8[sel];
        price1 = gm_part_value(p7);
        type8 = p7->idx;
        pct = (10000 - p7->wear) / 100;
        switch (g_parts[type8].category) {
        case PART_CARBURETOR:
        case PART_MANIFOLD:
            rl = rn_int(20);
            price1 = (float)price1 * ((rl + 70) / 100.0);
            break;
        case PART_ENGINE:;
        case PART_TRANSMISSION:
            cut = price1 * (pct / 100.0) * 0.9;
            price1 -= cut;
            price1 = (unsigned)(price1 * (pct / 100.0) * 0.9) + cut;
            break;
        case PART_TIRES:
            price1 = 0;
            break;
        }
        price1 = price1 / 5 * 5;
        strcpy(g_salePartName, &g_desc[g_parts[type8].nameOffset]);
        if (price1) {
            msg5 = MSG_WILL_YOU_TAKE_OFFER;
            sprintf(g_msgWillYouTake, "Will you take $%-d ?", price1);
        } else
            msg5 = MSG_TAKE_IT_FOR_NOTHING;
        pu_open_at(msg5, -1, -1);
        nl = mn_modal(KM_SELL_PART_OFFER);
        pu_close();
        if (nl == KR_SELL_PART_NO_THANKS)
            continue;
        if (sel == 1)
            g_driver.spareParts = p7->next;
        else
            list8[sel - 1]->next = p7->next;
        free_part(p7);
        g_gameClock.actionTicks += 0x444L;
        sc_add_money(price1);
    }
}

#pragma loop_opt(on)


struct Node2 *near gm_new_car(void)
{
    struct Node2 *p;

    p = g_driver.freeCars;
    g_driver.freeCars = p->next;
    p->next = 0;
    g_driver.carCount++;
    return p;
}


static void near free_car(struct Node2 *p)
{
    p->next = g_driver.freeCars;
    p->b = -1;
    g_driver.freeCars = p;
    g_driver.carCount--;
}


struct Node *near gm_add_part(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = gm_new_part();
    n->idx = idx;
    it = &g_parts[idx];
    n->wear = g_partWear[it->category];
    n->value = a;
    n->next = g_driver.spareParts;
    g_driver.spareParts = n;
    return n;
}


struct Car *near gm_make_car(int idx, int value, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;

    car = (old == 0) ? (struct Car *)gm_new_car() : old;
    car->specIndex = idx;
    car->value = value;
    car->fuel = 0x28;
    car->rank = g_carData[idx].rank;
    while ((i = rn_int(6)) == 5)
        if (rn_int(2) == 1)
            break;
    car->paintScheme = i;
    car->wireState = WIRES_CONNECTED;
    car->timing = rn_int(13) - 8;
    car->transmission = node = (old == 0) ? gm_new_part() : car->transmission;
    w = (g_carData[idx].parts >> 6) & 3;
    w = (kind = (g_carData[idx].parts & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[PART_TRANSMISSION];
    node->next = 0;
    car->rack.parts[0] = node = (old == 0) ? gm_new_part() : car->rack.parts[0];
    w = (g_carData[idx].parts >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[PART_ENGINE];
    node->next = 0;
    car->tires = node = (old == 0) ? gm_new_part() : car->tires;
    w = (g_carData[idx].parts >> 4 & 3) + 0x28;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[PART_TIRES];
    node->next = 0;
    car->rack.parts[1] = node = (old == 0) ? gm_new_part() : car->rack.parts[1];
    for (i = 3; i < 9; i++)
        car->engineBolts[i] = 0;
    w = (i = g_carData[idx].parts >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[PART_MANIFOLD];
    node->next = 0;
    w = (g_carData[idx].parts >> 14 & 3) + 0x16;
    i = g_nCarb[i] - 1;
    for (j = 2; j > i; j--)
        car->rack.parts[j + 2] = 0;
    for (; i >= 0; i--) {
        car->rack.parts[i + 2] = node = (old == 0) ? gm_new_part() : car->rack.parts[i + 2];
        node->idx = w;
        node->value = g_parts[w].value;
        node->wear = g_partWear[PART_CARBURETOR];
        node->next = 0;
        car->engineBolts[2 * i + 3] = 3;
        car->engineBolts[2 * i + 4] = 3;
    }
    for (i = 0; i < 3; i++)
        car->engineBolts[i] = 3;
    for (i = 0; i < 2; i++)
        car->transmissionBolts[i] = 3;
    car->appearance &= 0xff00;
    car->appearance &= 0xe0ff;
    car->appearance &= 0xdfff;
    car->appearance &= 0xbfff;
    car->appearance &= 0x7fff;
    if (old == 0) {
        if (g_driver.currentCar) {
            car->next = g_driver.otherCars;
            g_driver.otherCars = car;
        } else
            g_driver.currentCar = car;
    }
    return car;
}


int near gm_take_part(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->value;
    if (keep) {
        n->next = g_driver.spareParts;
        g_driver.spareParts = n;
    } else
        free_part(n);
    return r;
}


int near gm_scrap_car(struct Car *c, int keep)
{
    register struct Node **p2;
    int r;
    register int i;

    c->value -= gm_take_part(c->transmission, keep);
    r = 5;
    for (p2 = c->rack.parts, i = r; i; p2++, i--)
        c->value -= gm_take_part(*p2, keep);
    gm_take_part(c->tires, 0);
    r = c->value;
    if (r < 0)
        r = 0;
    free_car((struct Node2 *)c);
    return r;
}


static void near buy_from_ads(int tab)
{
    char list4[70];
    struct Bitmap far *bm1;
    char *p1;
    int off6;
    int used;
    struct Car *car;
    int sv;
    int x1;
    int y;
    int price6;
    struct Spec *specn;
    int n0;
    int ck;
    int i5;
    int k3;
    int n;
    char color;

    color = 0;
    g_scene = SCENE_ADVERTS;
    g_list[1] = DESC_USED_CARS_HEADING;
    n = 2;
    i5 = price6 = 0;
    for (k3 = g_usedCarAdFirst; k3 >= 0; k3 = g_carData[k3].next) {
        g_list[n++] = g_carData[k3].nameOffset;
        list4[i5++] = k3;
    }
    g_list[n++] = DESC_PARTS_HEADING;
    n0 = n;
    off6 = i5 - n0;
    for (k3 = 0; k3 < 0x2c; k3++) {
        if (g_parts[k3].value >= 0x14) {
            g_list[n++] = g_parts[k3].nameOffset;
            list4[i5++] = k3;
        }
    }
    g_list[0] = n - 1;
    av_open(g_list, (tab == KR_PAPER_CARS) ? 2 : 0x1c);
    goto test;
    do {
        used = n >= n0;
        if (used) {
            k3 = g_parts[i5 = list4[n + off6]].nameOffset;
            price6 = g_parts[i5].value;
            g_hotBuy[0].kind = HOT_DISABLED;
        } else {
            k3 = (specn = &g_carData[i5 = list4[n - 2]])->nameOffset;
            price6 = specn->price;
            g_hotBuy[0].kind = HOT_BUTTON;
        }
        strcpy(g_offerName, g_desc + k3);
        sprintf(g_buyPrice, "$%-3d ?", price6);
        color = -1;
        for (;;) {
            n = mn_wait(KM_BUY);
            if (n == KR_BUY_YEAH) {
                if (g_demo && (long)price6 > g_driver.money)
                    g_driver.money += 1999;
                if ((long)price6 > g_driver.money) {
                    p1 = strchr(g_msgShort, '$');
                    sprintf(++p1, "%d short.", price6 - (int)g_driver.money);
                    gm_message(-MSG_NO_GO_YOURE_SHORT);
                    break;
                }
                sc_add_money(-price6);
                if (used) {
                    gm_add_part(i5, price6);
                    g_gameClock.actionTicks += 0x222;
                    if (g_driver.partCount > 0x82)
                        goto done;
                    break;
                }
                car = gm_make_car(i5, price6, 0);
                car->paintScheme = (color != -1) ? color : rn_int(5);
                gm_message(-MSG_ITS_YOURS);
                g_gameClock.actionTicks += 0x444;
                goto done;
            }
            if (n != KR_SEE_IT)
                break;
            cu_ctl(CURSOR_HIDE_NOW);
            ma_clear();
            bm1 = ar_bm(SCREEN_WIDTH, 0x52, 0, ARENA_LOW_BASE);
            pu_grab(0, 0x40, SCREEN_WIDTH, 0x52, bm1);
            x_colors = bustyle->colors;
            ck = (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) ? g_carBg16[5] : g_carBg4[5];
            sc_fill_rect(0, 0x41, SCREEN_WIDTH, 0x46, ck);
            sc_fill_rect(0, 0x87, SCREEN_WIDTH, 0xa, x_colors.textBg);
            if (r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY) {
                sv = x_colors.line;
                x_colors.line = 3;
                dr_vline(r_vid.back, 0, 0x40, 0x52);
                dr_vline(r_vid.back, (SCREEN_WIDTH - 1), 0x40, 0x52);
                dr_vline(r_vid.front, 0, 0x40, 0x52);
                dr_vline(r_vid.front, (SCREEN_WIDTH - 1), 0x40, 0x52);
                sc_hline(0, 0x40, SCREEN_WIDTH);
                sc_hline(0, 0x91, SCREEN_WIDTH);
                x_colors.line = sv;
            }
            if (color == -1) {
                color = rn_int(5);
                ga_palette(color);
            }
            x1 = ((unsigned)specn - (unsigned)g_carData) / 10;
            y = (((specn->parts >> 8) & 7) == 2) ? 8 : 0;
            cp_show_car(CAR_SHOW_SCREEN, x1, y, 0, 0, 0x86, 0, g_tirePics[(specn->parts >> 4) & 3]);
            sc_text((SCREEN_WIDTH - da_text_width(g_offerName)) / 2, 0x88, g_offerName);
            cu_ctl(CURSOR_SHOW_NOW);
            g_input.any = 1;
            mn_wait(KM_MESSAGE);
            g_input.any = 0;
            pu_show(bm1, 0, 0x40);
            ma_clear();
        }
    test:
        n = av_run();
    } while (n != KR_AD_CLOSE);
done:
    av_close();
    if (g_driver.currentCar)
        ga_palette(g_driver.currentCar->paintScheme);
    g_scene = SCENE_NONE;
}

static char *g_offerText = g_msgHowAbout;


static int near counter_offer(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_offerText, "How about $%-d ?", *price);
    pu_open_at(MSG_HOW_ABOUT_OFFER, -1, -1);
    key = mn_wait(KM_COUNTER_OFFER);
    pu_close();
    return key;
}


int gm_car_look(struct Car *c)
{
    int r;

    r = 0;
    if (c->appearance & 0x2000)
        r |= 4;
    if (c->appearance & 0x4000)
        r |= 2;
    if (c->appearance & 0x8000)
        r |= 1;
    if (g_parts[c->rack.parts[0]->idx].variant == ENGINE_V8_LARGE)
        r |= 8;
    return r;
}


static enum GarageMode near your_cars(void)
{
    struct Bitmap far *pict;
    struct Car *tbl[142];
    unsigned offer;
    unsigned price;
    int rx;
    int mid;
    struct Car *cur;
    int ticks;
    int ye;
    int n;
    int result;
    unsigned cash;
    int item;

    pict = 0;
    ticks = 0;
    item = 1;
    for (;; ticks++) {
        n = 0;
        if (g_driver.currentCar) {
            n = 1;
            g_list[1] = g_carData[(tbl[1] = g_driver.currentCar)->specIndex].nameOffset;
        }
        for (cur = g_driver.otherCars; cur; cur = cur->next) {
            n++;
            g_list[n] = g_carData[(tbl[n] = cur)->specIndex].nameOffset;
        }
        g_list[0] = n;
        switch (item = mn_modal_list(KM_YOUR_CARS, (n > item) ? item : n, 0, g_list, 0, 0)) {
        case KR_EMPTY_LIST:
            gm_message(ticks ? MSG_ALL_CARS_GONE : MSG_YOUVE_GOT_NO_CAR_DUMMY);
        case KR_CLOSE_CARS:
            return GARAGE_KEEP;
        case KR_SWITCH_CAR:
            g_new = tbl[item = mn_sel()];
            g_old = g_driver.currentCar;
            if (g_new != g_old) {
                if (item == ((g_driver.currentCar != 0) + 1))
                    g_driver.otherCars = g_new->next;
                else
                    tbl[item - 1]->next = g_new->next;
                if (g_driver.currentCar) {
                    g_driver.currentCar->next = g_driver.otherCars;
                    g_driver.otherCars = g_driver.currentCar;
                }
                g_driver.currentCar = g_new;
            } else
                g_new = 0;
            return GARAGE_SWAP;
        case KR_SELL_CAR:
            break;
        default:
            if (item > 0) /* PROVISIONAL: the original loads g_list[0] here and drops it */
                g_list[0] = g_list[0];
        }
        cur = tbl[item = mn_sel()];
        offer = price = (int)((rn_int(20) + 75.0) * cur->value / 100.0);
        strcpy(g_offerName, g_desc + g_carData[cur->specIndex].nameOffset);
        g_offerAmount[0] = 0;
        pict = mn_grab(KM_SELL_CAR_OFFER, &rx, &ye, ARENA_LOW_STACK);
        mn_push(KM_SELL_CAR_OFFER);
        do {
            if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                goto tail;
        } while (cash == 0);
        if ((unsigned)(cur->value * 1.15) < cash) {
            gm_message(MSG_YOU_MUST_BE_KIDDING);
            goto tail;
        }
        if (cur->value < cash) {
            gm_message(MSG_NO_WAY);
            do {
                if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                    goto tail;
            } while (cash == 0);
            if (cur->value < cash) {
                gm_message(MSG_GET_LOST_OFFER);
                goto tail;
            }
        }
        if (cash > offer) {
            offer = (rn_int(5) + 85.0) * price / 100.0;
            if (cash > offer && counter_offer(&offer) == KR_OK)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                    goto tail;
            } while (cash == 0);
        }
        if (cash > offer) {
            offer = (rn_int(5) + 91.0) * price / 100.0;
            if (cash > offer && counter_offer(&offer) == KR_OK)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                    goto tail;
            } while (cash == 0);
        }
        if ((float)offer < (float)cash * 0.9) {
            gm_message(MSG_NO_THANKS);
            goto tail;
        }
        gm_message((cur->value > cash * 2) ? MSG_CRAZY_ILL_TAKE_IT : MSG_ILL_TAKE_IT);
        g_gameClock.actionTicks += 0x888L;
        sc_add_money(cash);
        if (item == 1 && g_driver.currentCar) {
            pu_show(pict, rx, ye);
            ma_pop_low(1);
            pict = 0;
            cu_ctl(CURSOR_HIDE_NOW);
            mid = g_driver.currentCar->specIndex;
            result = gm_car_look(g_driver.currentCar);
            cp_show_car(CAR_BACK_OUT, mid, result, (g_driver.currentCar->appearance >> 8) & 0x1f, 0, 0x98, 0,
                        g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
            mn_pop(1);
            ga_clear_name();
            mn_push(KM_GARAGE);
            cu_ctl(CURSOR_SHOW_NOW);
            gm_scrap_car(g_driver.currentCar, 0);
            g_driver.currentCar = 0;
        } else {
            if (item == ((g_driver.currentCar != 0) + 1))
                g_driver.otherCars = cur->next;
            else
                tbl[item - 1]->next = cur->next;
            gm_scrap_car(cur, 0);
        }
    tail:
        mn_pop(1);
        if (pict) {
            pu_show(pict, rx, ye);
            ma_pop_low(1);
            pict = 0;
        }
    }
}


void gm_play_anim(int h)
{
    vi_anim_start(h);
    while (vi_anim_active(h))
        sy_wait_input(10);
}


void gm_popup(int a, int b, int c)
{
    long saved;

    saved = g_waitUntil;
    pu_open_at(a, b, c);
    gm_pause(g_msgTicks);
    pu_close();
    g_waitUntil = saved;
}


void near gm_run(void)
{
    int again;
    int kind;
    enum GarageMode v;
    int w;

    again = 1;
    kind = start_game();
    for (;;) {
        if (kind == KR_NEW_GAME)
            again = 1;
        w = kind;
        if (w == KR_NEW_GAME)
            kind = sg_new();
        else
            kind = sg_load();
        if (kind == LOAD_ERROR) {
            kind = LOAD_CANCELLED;
            again = 1;
        }
        if (kind && again)
            continue;
        sc_label(g_cashLabel);
        v = GARAGE_DRIVE_IN;
        if (kind == LOAD_OK && w == KR_OLD_GAME && g_driver.currentCar)
            v = GARAGE_REDRAW;
        if (kind == LOAD_OK && w == KR_OLD_GAME)
            v = GARAGE_DRIVE_IN;
        if (w == KR_NEW_GAME && g_driver.carCount <= 15 && g_driver.partCount <= 130) {
            kind = read_paper();
            if (kind == KR_PAPER_PARTS || kind == KR_PAPER_CARS)
                buy_from_ads(kind);
        }
        again = 0;
        while ((kind = ga_menu(v)) != 0) {
            mu_suspend(0);
            if (kind == GARAGE_GAME_ENDED) {
                if (g_end == 1)
                    da_time_up();
                fa_show(g_end & 2);
                cu_ctl(1);
                g_end = 0;
                again = 1;
                kind = fa_end_menu();
                if (kind != KR_LOAD_GAME && kind != KR_NEW_GAME && kind == KR_END_QUIT)
                    return;
                cu_ctl(CURSOR_SHOW_NOW);
                g_carPulledIn = 0;
                break;
            }
            v = GARAGE_REDRAW;
            switch (kind) {
            case KR_GAR_NEWSPAPER:
                if (check_room())
                    v = GARAGE_KEEP;
                else {
                    if (g_driver.currentCar == 0)
                        v = GARAGE_DRIVE_IN;
                    kind = read_paper();
                    if (kind == KR_PAPER_PARTS || kind == KR_PAPER_CARS)
                        buy_from_ads(kind);
                    if (g_driver.currentCar == 0)
                        v = GARAGE_REDRAW;
                    if (g_driver.currentCar == 0 && g_driver.otherCars)
                        v = GARAGE_DRIVE_IN;
                    g_gameClock.actionTicks += 0x222;
                }
                continue;
            case KR_GAR_STREET:
            case KR_GAR_TANK:
                if (check_room()) {
                    v = GARAGE_KEEP;
                    continue;
                }
                if (g_driver.currentCar) {
                    if (ga_ready(g_driver.currentCar, 1)) {
                        pf_drivetrain(g_driver.currentCar, &g_driveEng);
                        if (kind == KR_GAR_STREET) {
                            if (g_driver.currentCar->fuel < 9) {
                                gm_message(MSG_NOT_ENOUGH_GAS_TO_CRUISE);
                                v = GARAGE_KEEP;
                                continue;
                            }
                            g_scene = SCENE_BOBS;
                            cr_go();
                            g_gameClock.actionTicks += 0x222;
                            bo_visit();
                        } else {
                            if (g_driver.currentCar->fuel >= 0x8c) {
                                gm_message(MSG_YOUVE_GOT_A_FULL_TANK);
                                v = GARAGE_KEEP;
                                continue;
                            }
                            if (g_driver.money == 0) {
                                gm_message(MSG_GAS_AINT_FREE);
                                v = GARAGE_KEEP;
                                continue;
                            }
                            g_scene = SCENE_GAS_STATION;
                            cr_go();
                            g_gameClock.actionTicks += 0x222;
                            ra_gas_station();
                        }
                        g_gameClock.actionTicks += 0x222;
                        v = GARAGE_DRIVE_IN;
                    } else {
                        gm_message(MSG_CAR_NOT_RUNNING);
                        v = GARAGE_KEEP;
                    }
                } else {
                    if (g_driver.otherCars)
                        gm_message(MSG_PICK_A_CAR_FIRST);
                    else
                        gm_message(MSG_YOUVE_GOT_NO_CAR_DUMMY);
                    v = GARAGE_KEEP;
                }
                continue;
            case KR_GAR_QUIT:
                v = GARAGE_KEEP;
                switch (kind = sg_menu()) {
                case KR_QUIT:
                    return;
                case KR_MENU_FORGET:
                case KR_CANCEL:
                    continue;
                case KR_SAVE_GAME:
                    if (check_room()) {
                        v = GARAGE_KEEP;
                        kind = GAME_MENU_BLOCKED;
                    } else {
                        sg_save();
                        v = GARAGE_REDRAW;
                        if (g_driver.currentCar == 0 && g_driver.otherCars)
                            v = GARAGE_DRIVE_IN;
                        continue;
                    }
                case KR_OLD_GAME:
                case KR_NEW_GAME:
                default:
                    if (kind == GAME_MENU_BLOCKED)
                        continue;
                }
                break;
            case KR_GAR_SELL_PARTS:
                sell_parts();
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_CARS:
                v = your_cars();
                continue;
            case KR_GAR_PAINT:
                ga_paint();
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_TIRES:
                ga_tires();
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_TRANSMISSION:
                ga_transmission();
                continue;
            case KR_GAR_HOOD:
                ma_release();
                ga_hood(g_driver.currentCar);
                continue;
            case KR_GAR_ROOF:
                ga_body(4);
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_REAR_BUMPER:
                ga_body(2);
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_FRONT_BUMPER:
                ga_body(1);
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_STICKERS:
                ga_stickers();
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_CALENDAR:
                ga_calendar(0);
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_CAR_INFO:
                ga_info();
                v = GARAGE_KEEP;
                continue;
            case KR_GAR_DRINK:
                gm_message(MSG_DONT_DRINK_AND_DRIVE_BUB);
                v = GARAGE_KEEP;
                continue;
            case KR_MUSIC:
                if (g_musicOn) {
                    sy_music_pause();
                    g_musicOn = 0;
                } else {
                    g_musicOn = 1;
                    sy_music_resume();
                }
                v = GARAGE_KEEP;
                continue;
            }
            break;
        }
    }
}
