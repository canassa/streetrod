#include <stdio.h>
#include <string.h>
#include "STRUCTS.H"
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
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "PERF.H"
#include "POPUP.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

static void near free_part(struct Node *p7);
static int near counter_offer(unsigned *price);
static int near start_game(void);
static int near read_paper(void);
static void near buy_from_ads(int r);
static int near check_room(void);
static void near sell_parts(void);
static int near your_cars(void);

struct Car *g_old = 0;   /* DS:4F46 */
struct Car *g_new = 0;   /* DS:4F48 */
int g_4f4a = 0;   /* DS:4F4A */
int g_picsFreed = 0;   /* DS:4F4C */
int g_paperPics[5] = {1022, 1023, 1025, 1024, 1026};   /* DS:4F4E */
int g_paperPick = 0;   /* DS:4F58 */
int g_paperDay = -1;   /* DS:4F5A */

/* start_game @ 0000:39C0 (linear 0x039C0, 195 bytes) */

static int near start_game(void)
{
    da_set_view(0);
    sy_music_start(2, 10);
    da_intro();
    (*g_videoOps.blank)();
    da_set_view(1);
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

/* gm_pause @ 0000:3A83 (linear 0x03A83, 53 bytes) */

void gm_pause(int a)
{
    g_waitUntil = a + g_tick;
    g_input.any = 1;
    sy_wait_input(30000);
    g_waitUntil = 0;
    g_input.any = 0;
}

/* gm_message @ 0000:3AB8 (linear 0x03AB8, 84 bytes) */

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

/* check_room @ 0000:3B0C (linear 0x03B0C, 39 bytes) */

static int near check_room(void)
{
    if (a_7eaap.carCount > 15) {
        gm_message(0x0e27);
        return 1;
    }
    if (a_7eaap.partCount > 130) {
        gm_message(0x0e3f);
        return 1;
    }
    return 0;
}

/* read_paper @ 0000:3B33 (linear 0x03B33, 232 bytes) */

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

/* gm_new_part @ 0000:3C1B (linear 0x03C1B, 35 bytes) */

struct Node * near gm_new_part(void)
{
    struct Node *p;

    p = a_7eaap.freeParts;
    a_7eaap.freeParts = p->next;
    p->next = 0;
    a_7eaap.partCount++;
    return p;
}

/* free_part @ 0000:3C3E (linear 0x03C3E, 28 bytes) */

static void near free_part(struct Node *p)
{
    p->next = a_7eaap.freeParts;
    p->idx = -1;
    a_7eaap.freeParts = p;
    a_7eaap.partCount--;
}

/* gm_part_value @ 0000:3C5A (linear 0x03C5A, 51 bytes) */

int gm_part_value(struct Node *n)
{
    int r;

    if (g_parts[n->idx].category == 0 && g_parts[n->idx].variant == 0)
        r = 100;
    else
        r = n->value;
    return r;
}

/* The file is compiled /Oalsi, but the loops of sell_parts are plain /Ol-off
   loops in the shipped code (list counters and pointers kept in memory, test
   reached by a jmp). No for/while/do formulation reproduces that under /Ol,
   so loop optimization is switched off for this function only. */
#pragma loop_opt(off)

/* sell_parts @ 0000:3C8D (linear 0x03C8D, 804 bytes) */

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
        for (p7 = a_7eaap.spareParts, nl = 0; p7; p7 = p7->next) {
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
        case 0x3ea:
            gm_message(tries3 ? 0x0ed7 : 0x0f2a);
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
        case 2:
        case 3:
            rl = rn_int(20);
            price1 = (float)price1 * ((rl + 70) / 100.0);
            break;
        case 0:
            ;
        case 1:
            cut = price1 * (pct / 100.0) * 0.9;
            price1 -= cut;
            price1 = (unsigned)(price1 * (pct / 100.0) * 0.9) + cut;
            break;
        case 4:
            price1 = 0;
            break;
        }
        price1 = price1 / 5 * 5;
        strcpy(g_salePartName, &g_desc[g_parts[type8].nameOffset]);
        if (price1) {
            msg5 = 0x100a;
            sprintf(g_msgWillYouTake, "Will you take $%-d ?", price1);
        } else
            msg5 = 0x1021;
        pu_open_at(msg5, -1, -1);
        nl = mn_modal(KM_SELL_PART_OFFER);
        pu_close();
        if (nl == KR_SELL_PART_NO_THANKS)
            continue;
        if (sel == 1)
            a_7eaap.spareParts = p7->next;
        else
            list8[sel - 1]->next = p7->next;
        free_part(p7);
        g_gameClock.actionTicks += 0x444L;
        sc_add_money(price1);
    }
}

#pragma loop_opt(on)

/* gm_new_car @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */

struct Node2 * near gm_new_car(void)
{
    struct Node2 *p;

    p = a_7eaap.freeCars;
    a_7eaap.freeCars = p->next;
    p->next = 0;
    a_7eaap.carCount++;
    return p;
}

/* free_car @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */

static void near free_car(struct Node2 *p)
{
    p->next = a_7eaap.freeCars;
    p->b = -1;
    a_7eaap.freeCars = p;
    a_7eaap.carCount--;
}

/* gm_add_part @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */

struct Node * near gm_add_part(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = gm_new_part();
    n->idx = idx;
    it = &g_parts[idx];
    n->wear = g_partWear[it->category];
    n->value = a;
    n->next = a_7eaap.spareParts;
    a_7eaap.spareParts = n;
    return n;
}

/* gm_make_car @ 0000:4036 (linear 0x04036, 848 bytes) */

struct Car * near gm_make_car(int idx, int value, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;

    car = (old == 0) ? (struct Car *)gm_new_car() : old;
    car->specIndex = idx;
    car->value = value;
    car->fuel = 0x28;
    car->rank = g7d86n[idx].rank;
    while ((i = rn_int(6)) == 5)
        if (rn_int(2) == 1)
            break;
    car->paintScheme = i;
    car->wireState = 1;
    car->timing = rn_int(13) - 8;
    car->transmission = node = (old == 0) ? gm_new_part() : car->transmission;
    w = (g7d86n[idx].parts >> 6) & 3;
    w = (kind = (g7d86n[idx].parts & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[1];
    node->next = 0;
    car->rack.parts[0] = node = (old == 0) ? gm_new_part() : car->rack.parts[0];
    w = (g7d86n[idx].parts >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[0];
    node->next = 0;
    car->tires = node = (old == 0) ? gm_new_part() : car->tires;
    w = (g7d86n[idx].parts >> 4 & 3) + 0x28;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[4];
    node->next = 0;
    car->rack.parts[1] = node = (old == 0) ? gm_new_part() : car->rack.parts[1];
    for (i = 3; i < 9; i++)
        car->engineBolts[i] = 0;
    w = (i = g7d86n[idx].parts >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->value = g_parts[w].value;
    node->wear = g_partWear[3];
    node->next = 0;
    w = (g7d86n[idx].parts >> 14 & 3) + 0x16;
    i = g_nCarb[i] - 1;
    for (j = 2; j > i; j--)
        car->rack.parts[j + 2] = 0;
    for (; i >= 0; i--) {
        car->rack.parts[i + 2] = node = (old == 0) ? gm_new_part() : car->rack.parts[i + 2];
        node->idx = w;
        node->value = g_parts[w].value;
        node->wear = g_partWear[2];
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
        if (a_7eaap.currentCar) {
            car->next = a_7eaap.otherCars;
            a_7eaap.otherCars = car;
        } else
            a_7eaap.currentCar = car;
    }
    return car;
}

/* gm_take_part @ 0000:4386 (linear 0x04386, 57 bytes) */

int near gm_take_part(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->value;
    if (keep) {
        n->next = a_7eaap.spareParts;
        a_7eaap.spareParts = n;
    } else
        free_part(n);
    return r;
}

/* gm_scrap_car @ 0000:43BF (linear 0x043BF, 108 bytes) */

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

/* buy_from_ads @ 0000:442B (linear 0x0442B, 1301 bytes) */

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
    g_scene = 5;
    g_list[1] = 0xcd8;
    n = 2;
    i5 = price6 = 0;
    for (k3 = g_usedCarAdFirst; k3 >= 0; k3 = g7d86n[k3].next) {
        g_list[n++] = g7d86n[k3].nameOffset;
        list4[i5++] = k3;
    }
    g_list[n++] = 0xcfd;
    n0 = n;
    off6 = i5 - n0;
    for (k3 = 0; k3 < 0x2c; k3++) {
        if (g_parts[k3].value >= 0x14) {
            g_list[n++] = g_parts[k3].nameOffset;
            list4[i5++] = k3;
        }
    }
    g_list[0] = n - 1;
    av_open(g_list, (tab == 1) ? 2 : 0x1c);
    goto test;
    do {
        used = n >= n0;
        if (used) {
            k3 = g_parts[i5 = list4[n + off6]].nameOffset;
            price6 = g_parts[i5].value;
            g_hotBuy[0].kind = 0xe;
        } else {
            k3 = (specn = &g7d86n[i5 = list4[n - 2]])->nameOffset;
            price6 = specn->price;
            g_hotBuy[0].kind = 10;
        }
        strcpy(g_offerName, g_desc + k3);
        sprintf(g_buyPrice, "$%-3d ?", price6);
        color = -1;
        for (;;) {
            n = mn_wait(KM_BUY);
            if (n == KR_BUY_YEAH) {
                if (g_demo && (long)price6 > a_7eaap.money)
                    a_7eaap.money += 1999;
                if ((long)price6 > a_7eaap.money) {
                    p1 = strchr(g_msgShort, '$');
                    sprintf(++p1, "%d short.", price6 - (int)a_7eaap.money);
                    gm_message(0xf1a8);
                    break;
                }
                sc_add_money(-price6);
                if (used) {
                    gm_add_part(i5, price6);
                    g_gameClock.actionTicks += 0x222;
                    if (a_7eaap.partCount > 0x82)
                        goto done;
                    break;
                }
                car = gm_make_car(i5, price6, 0);
                car->paintScheme = (color != -1) ? color : rn_int(5);
                gm_message(0xf2de);
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
            x1 = ((unsigned)specn - (unsigned)g7d86n) / 10;
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
    if (a_7eaap.currentCar)
        ga_palette(a_7eaap.currentCar->paintScheme);
    g_scene = 0;
}

char *g_offerText = g_msgHowAbout;   /* DS:4FA4 */

/* counter_offer @ 0000:4940 (linear 0x04940, 98 bytes) */

static int near counter_offer(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_offerText, "How about $%-d ?", *price);
    pu_open_at(0xff7, -1, -1);
    key = mn_wait(KM_COUNTER_OFFER);
    pu_close();
    return key;
}

/* gm_car_look @ 0000:49A2 (linear 0x049A2, 75 bytes) */

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
    if (g_parts[c->rack.parts[0]->idx].variant == 2)
        r |= 8;
    return r;
}

/* your_cars @ 0000:49ED (linear 0x049ED, 1501 bytes) */

static int near your_cars(void)
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
        if (a_7eaap.currentCar) {
            n = 1;
            g_list[1] = g7d86n[(tbl[1] = a_7eaap.currentCar)->specIndex].nameOffset;
        }
        for (cur = a_7eaap.otherCars; cur; cur = cur->next) {
            n++;
            g_list[n] = g7d86n[(tbl[n] = cur)->specIndex].nameOffset;
        }
        g_list[0] = n;
        switch (item = mn_modal_list(KM_YOUR_CARS, (n > item) ? item : n, 0, g_list, 0, 0)) {
        case 0x3ea:
            gm_message(ticks ? 0xee9 : 0xefa);
        case KR_CLOSE_CARS:
            return 0;
        case KR_SWITCH_CAR:
            g_new = tbl[item = mn_sel()];
            g_old = a_7eaap.currentCar;
            if (g_new != g_old) {
                if (item == ((a_7eaap.currentCar != 0) + 1))
                    a_7eaap.otherCars = g_new->next;
                else
                    tbl[item - 1]->next = g_new->next;
                if (a_7eaap.currentCar) {
                    a_7eaap.currentCar->next = a_7eaap.otherCars;
                    a_7eaap.otherCars = a_7eaap.currentCar;
                }
                a_7eaap.currentCar = g_new;
            } else
                g_new = 0;
            return 3;
        case KR_SELL_CAR:
            break;
        default:
            if (item > 0)       /* PROVISIONAL: the original loads g_list[0] here and drops it */
                g_list[0] = g_list[0];
        }
        cur = tbl[item = mn_sel()];
        offer = price = (int)((rn_int(20) + 75.0) * cur->value / 100.0);
        strcpy(g_offerName, g_desc + g7d86n[cur->specIndex].nameOffset);
        g_offerAmount[0] = 0;
        pict = mn_grab(KM_SELL_CAR_OFFER, &rx, &ye, ARENA_LOW_STACK);
        mn_push(KM_SELL_CAR_OFFER);
        do {
            if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                goto tail;
        } while (cash == 0);
        if ((unsigned)(cur->value * 1.15) < cash) {
            gm_message(0xec0);
            goto tail;
        }
        if (cur->value < cash) {
            gm_message(0x106e);
            do {
                if ((n = ed_spot_number(&cash, g_offerAmount)) == KR_NEVER_MIND)
                    goto tail;
            } while (cash == 0);
            if (cur->value < cash) {
                gm_message(0x1078);
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
            gm_message(0x1084);
            goto tail;
        }
        gm_message((cur->value > cash * 2) ? 0x104b : 0x1091);
        g_gameClock.actionTicks += 0x888L;
        sc_add_money(cash);
        if (item == 1 && a_7eaap.currentCar) {
            pu_show(pict, rx, ye);
            ma_pop_low(1);
            pict = 0;
            cu_ctl(CURSOR_HIDE_NOW);
            mid = a_7eaap.currentCar->specIndex;
            result = gm_car_look(a_7eaap.currentCar);
            cp_show_car(CAR_BACK_OUT, mid, result, (a_7eaap.currentCar->appearance >> 8) & 0x1f, 0, 0x98, 0,
                    g_tirePics[g_parts[a_7eaap.currentCar->tires->idx].variant]);
            mn_pop(1);
            ga_clear_name();
            mn_push(KM_GARAGE);
            cu_ctl(CURSOR_SHOW_NOW);
            gm_scrap_car(a_7eaap.currentCar, 0);
            a_7eaap.currentCar = 0;
        } else {
            if (item == ((a_7eaap.currentCar != 0) + 1))
                a_7eaap.otherCars = cur->next;
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

/* gm_play_anim @ 0000:4FCA (linear 0x04FCA, 48 bytes) */

void gm_play_anim(int h)
{
    vi_anim_start(h);
    while (vi_anim_active(h))
        sy_wait_input(10);
}

/* gm_popup @ 0000:4FFA (linear 0x04FFA, 69 bytes) */

void gm_popup(int a, int b, int c)
{
    long saved;

    saved = g_waitUntil;
    pu_open_at(a, b, c);
    gm_pause(g_msgTicks);
    pu_close();
    g_waitUntil = saved;
}

/* gm_run @ 0000:503F (linear 0x0503F, 1007 bytes) */

void near gm_run(void)
{
    int again;
    int kind;
    int v;
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
        if (kind == -666) {
            kind = -1;
            again = 1;
        }
        if (kind && again)
            continue;
        sc_status_label(g_49dc);
        v = 1;
        if (kind == 0 && w == KR_OLD_GAME && a_7eaap.currentCar)
            v = 2;
        if (kind == 0 && w == KR_OLD_GAME)
            v = 1;
        if (w == KR_NEW_GAME && a_7eaap.carCount <= 15 && a_7eaap.partCount <= 130) {
            kind = read_paper();
            if (kind == 2 || kind == 1)
                buy_from_ads(kind);
        }
        again = 0;
        while ((kind = ga_menu(v)) != 0) {
            mu_suspend(0);
            if (kind == 0x29a) {
                if (g_end == 1)
                    da_time_up();
                fa_show(g_end & 2);
                cu_ctl(1);
                g_end = 0;
                again = 1;
                kind = fa_end_menu();
                if (kind != KR_LOAD_GAME && kind != KR_NEW_GAME && kind == 4)
                    return;
                cu_ctl(CURSOR_SHOW_NOW);
                g_4f4a = 0;
                break;
            }
            v = 2;
            switch (kind) {
            case 1:
                if (check_room())
                    v = 0;
                else {
                    if (a_7eaap.currentCar == 0)
                        v = 1;
                    kind = read_paper();
                    if (kind == 2 || kind == 1)
                        buy_from_ads(kind);
                    if (a_7eaap.currentCar == 0)
                        v = 2;
                    if (a_7eaap.currentCar == 0 && a_7eaap.otherCars)
                        v = 1;
                    g_gameClock.actionTicks += 0x222;
                }
                continue;
            case 2:
            case 15:
                if (check_room()) {
                    v = 0;
                    continue;
                }
                if (a_7eaap.currentCar) {
                    if (ga_ready(a_7eaap.currentCar, 1)) {
                        pf_drivetrain(a_7eaap.currentCar, &g_driveEng);
                        if (kind == 2) {
                            if (a_7eaap.currentCar->fuel < 9) {
                                gm_message(0x172e);
                                v = 0;
                                continue;
                            }
                            g_scene = 2;
                            cr_go();
                            g_gameClock.actionTicks += 0x222;
                            bo_visit();
                        } else {
                            if (a_7eaap.currentCar->fuel >= 0x8c) {
                                gm_message(0x19b1);
                                v = 0;
                                continue;
                            }
                            if (a_7eaap.money == 0) {
                                gm_message(0x199f);
                                v = 0;
                                continue;
                            }
                            g_scene = 3;
                            cr_go();
                            g_gameClock.actionTicks += 0x222;
                            ra_gas_station();
                        }
                        g_gameClock.actionTicks += 0x222;
                        v = 1;
                    } else {
                        gm_message(0x0e7f);
                        v = 0;
                    }
                } else {
                    if (a_7eaap.otherCars)
                        gm_message(0x0f16);
                    else
                        gm_message(0x0efa);
                    v = 0;
                }
                continue;
            case 4:
                v = 0;
                switch (kind = sg_menu()) {
                case KR_QUIT:
                    return;
                case KR_MENU_FORGET:
                case KR_CANCEL:
                    continue;
                case KR_SAVE_GAME:
                    if (check_room()) {
                        v = 0;
                        kind = 0x309;
                    } else {
                        sg_save();
                        v = 2;
                        if (a_7eaap.currentCar == 0 && a_7eaap.otherCars)
                            v = 1;
                        continue;
                    }
                case KR_OLD_GAME:
                case KR_NEW_GAME:
                default:
                    if (kind == 0x309)
                        continue;
                }
                break;
            case 5:
                sell_parts();
                v = 0;
                continue;
            case 6:
                v = your_cars();
                continue;
            case 7:
                ga_paint();
                v = 0;
                continue;
            case 8:
                ga_tires();
                v = 0;
                continue;
            case 9:
                ga_transmission();
                continue;
            case 10:
                ma_release();
                ga_hood(a_7eaap.currentCar);
                continue;
            case 13:
                ga_body(4);
                v = 0;
                continue;
            case 11:
                ga_body(2);
                v = 0;
                continue;
            case 12:
                ga_body(1);
                v = 0;
                continue;
            case 14:
                ga_stickers();
                v = 0;
                continue;
            case 3:
                ga_calendar(0);
                v = 0;
                continue;
            case 16:
                ga_info();
                v = 0;
                continue;
            case 17:
                gm_message(0x18f6);
                v = 0;
                continue;
            case KR_MUSIC:
                if (g_musicOn) {
                    sy_music_pause();
                    g_musicOn = 0;
                } else {
                    g_musicOn = 1;
                    sy_music_resume();
                }
                v = 0;
                continue;
            }
            break;
        }
    }
}
