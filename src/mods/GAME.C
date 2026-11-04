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
#include "HERCDET.H"
#include "HOTSPOT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MEM.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "M_02B9A.H"
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

struct Car *g_4f46 = 0;   /* DS:4F46 */
struct Car *g_4f48 = 0;   /* DS:4F48 */
int g_4f4a = 0;   /* DS:4F4A */
int g_4f4c = 0;   /* DS:4F4C */
int g_4f4e[5] = {1022, 1023, 1025, 1024, 1026};   /* DS:4F4E */
int g_4f58 = 0;   /* DS:4F58 */
int g_4f5a = -1;   /* DS:4F5A */

/* start_game @ 0000:39C0 (linear 0x039C0, 195 bytes) */

static int near start_game(void)
{
    dash_set_view(0);
    sys_music_start(2, 10);
    dash_intro();
    (*bl78be)();
    dash_set_view(1);
    random_int(-1);
    (*bl78be)();
    if (by70e2 == 0)
        F_1552B(2);
    if (ag8bcet == 0) {
        if (fame_protect_check() == 0)
            sys_exit();
        F_120FE(-4);
    }
    F_27198();
    atnewName[0] = 0;
    g_0610 = ag8bcet;
    if (by70e2 == 0) {
        F_15EF9();
        if (r_vid.mode == -2)
            F_15C07(g_4c28);
    }
    F_15C4A(g_4c28, 1);
    F_120FE(-2);
    return -1;
}

/* game_pause @ 0000:3A83 (linear 0x03A83, 53 bytes) */

void game_pause(int a)
{
    g_05cc = a + g_tick;
    g_05d0 = 1;
    sys_wait_input(30000);
    g_05cc = 0;
    g_05d0 = 0;
}

/* game_message @ 0000:3AB8 (linear 0x03AB8, 84 bytes) */

void game_message(int n)
{
    long saved;

    saved = g_05cc;
    F_13DE6(24);
    popup_open(n);
    game_pause(g_05dc);
    popup_close();
    F_13F21(1);
    g_05cc = saved;
}

/* check_room @ 0000:3B0C (linear 0x03B0C, 39 bytes) */

static int near check_room(void)
{
    if (a_7eaap.m7eb6 > 15) {
        game_message(0x0e27);
        return 1;
    }
    if (a_7eaap.m7ebc > 130) {
        game_message(0x0e3f);
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

    old = r_vid.f16;
    r_vid.f16 = 0;
    F_118D4(0x3fd, -1);
    if ((tune = F_06476()) != g_4f5a) {
        if ((pick5 = random_int(5)) == g_4f58)
            pick5 = random_int(5);
        g_4f58 = pick5;
        g_4f5a = tune;
    }
    F_119B8(g_4f4e[g_4f58], 0, 0x88, -1);
    F_120FE(-3);
    F_06750(0x79);
    F_120FE(-1);
    r_vid.f16 = old;
    if (r_vid.f16)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xbd);
    F_120FE(1);
    return F_14294(7);
}

/* game_new_part @ 0000:3C1B (linear 0x03C1B, 35 bytes) */

struct Node * near game_new_part(void)
{
    struct Node *p;

    p = a_7eaap.m7eba;
    a_7eaap.m7eba = p->next;
    p->next = 0;
    a_7eaap.m7ebc++;
    return p;
}

/* free_part @ 0000:3C3E (linear 0x03C3E, 28 bytes) */

static void near free_part(struct Node *p)
{
    p->next = a_7eaap.m7eba;
    p->idx = -1;
    a_7eaap.m7eba = p;
    a_7eaap.m7ebc--;
}

/* game_part_value @ 0000:3C5A (linear 0x03C5A, 51 bytes) */

int game_part_value(struct Node *n)
{
    int r;

    if (g_4806[n->idx].f4 == 0 && g_4806[n->idx].f2 == 0)
        r = 100;
    else
        r = n->a;
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
        for (p7 = a_7eaap.m7eb8, nl = 0; p7; p7 = p7->next) {
            nl++;
            g_49e0[nl] = (ip = &g_4806[(list8[nl] = p7)->idx])->f6;
            if (p7->b != -0x80) {
                lbl[nl] = p7->b / 100;
                continue;
            }
            lbl[nl] = 0x80;
        }
        g_49e0[0] = nl;
        strcpy(g_2a3e, "Sell it");
        g_579f[4]->key = 0x1f;
        sel = F_142CF(4, sel <= g_49e0[0] ? sel : g_49e0[0], 0, g_49e0, lbl, 0);
        g_579f[4]->key = 0x2e;
        strcpy(g_2a3e, "Change");
        switch ((int)sel) {
        case -4:
            return;
        case 0x3ea:
            game_message(tries3 ? 0x0ed7 : 0x0f2a);
            return;
        case -3:
            sel = F_14C8B();
            break;
        }
        p7 = list8[sel];
        price1 = game_part_value(p7);
        type8 = p7->idx;
        pct = (10000 - p7->b) / 100;
        switch (g_4806[type8].f4) {
        case 2:
        case 3:
            rl = random_int(20);
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
        strcpy(g_2a5f, &g_2c02[g_4806[type8].f6]);
        if (price1) {
            msg5 = 0x100a;
            sprintf(g_3c0c, "Will you take $%-d ?", price1);
        } else
            msg5 = 0x1021;
        popup_open_at(msg5, -1, -1);
        nl = menu_modal(16);
        popup_close();
        if (nl == -0x15)
            continue;
        if (sel == 1)
            a_7eaap.m7eb8 = p7->next;
        else
            list8[sel - 1]->next = p7->next;
        free_part(p7);
        g_05fc += 0x444L;
        F_11540(price1);
    }
}

#pragma loop_opt(on)

/* game_new_car @ 0000:3FB1 (linear 0x03FB1, 35 bytes) */

struct Node2 * near game_new_car(void)
{
    struct Node2 *p;

    p = a_7eaap.m7eb4;
    a_7eaap.m7eb4 = p->next;
    p->next = 0;
    a_7eaap.m7eb6++;
    return p;
}

/* free_car @ 0000:3FD4 (linear 0x03FD4, 28 bytes) */

static void near free_car(struct Node2 *p)
{
    p->next = a_7eaap.m7eb4;
    p->b = -1;
    a_7eaap.m7eb4 = p;
    a_7eaap.m7eb6--;
}

/* game_add_part @ 0000:3FF0 (linear 0x03FF0, 70 bytes) */

struct Node * near game_add_part(int idx, int a)
{
    struct Node *n;
    struct Item *it;

    n = game_new_part();
    n->idx = idx;
    it = &g_4806[idx];
    n->b = g_4966[it->f4];
    n->a = a;
    n->next = a_7eaap.m7eb8;
    a_7eaap.m7eb8 = n;
    return n;
}

/* game_make_car @ 0000:4036 (linear 0x04036, 848 bytes) */

struct Car * near game_make_car(int idx, int weight, struct Car *old)
{
    struct Node *node;
    struct Car *car;
    int kind, w, i, j;

    car = (old == 0) ? (struct Car *)game_new_car() : old;
    car->f2 = idx;
    car->weight = weight;
    car->f22 = 0x28;
    car->f5 = g7d86n[idx].f2;
    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    car->f4 = i;
    car->f1f = 1;
    car->f20 = random_int(13) - 8;
    car->slot6 = node = (old == 0) ? game_new_part() : car->slot6;
    w = (g7d86n[idx].f4 >> 6) & 3;
    w = (kind = (g7d86n[idx].f4 & 0xf) >> 1) + (w == 3) + 3 * w + 9;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[1];
    node->next = 0;
    car->rack.s[0] = node = (old == 0) ? game_new_part() : car->rack.s[0];
    w = (g7d86n[idx].f4 >> 8 & 7) * 3 + kind;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[0];
    node->next = 0;
    car->slot12 = node = (old == 0) ? game_new_part() : car->slot12;
    w = (g7d86n[idx].f4 >> 4 & 3) + 0x28;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[4];
    node->next = 0;
    car->rack.s[1] = node = (old == 0) ? game_new_part() : car->rack.s[1];
    for (i = 3; i < 9; i++)
        car->b16[i] = 0;
    w = (i = g7d86n[idx].f4 >> 11 & 7) * 3 + kind + 0x19;
    node->idx = w;
    node->a = g_4806[w].f0;
    node->b = g_4966[3];
    node->next = 0;
    w = (g7d86n[idx].f4 >> 14 & 3) + 0x16;
    i = g_5126[i] - 1;
    for (j = 2; j > i; j--)
        car->rack.s[j + 2] = 0;
    for (; i >= 0; i--) {
        car->rack.s[i + 2] = node = (old == 0) ? game_new_part() : car->rack.s[i + 2];
        node->idx = w;
        node->a = g_4806[w].f0;
        node->b = g_4966[2];
        node->next = 0;
        car->b16[2 * i + 3] = 3;
        car->b16[2 * i + 4] = 3;
    }
    for (i = 0; i < 3; i++)
        car->b16[i] = 3;
    for (i = 0; i < 2; i++)
        car->b14[i] = 3;
    car->f24 &= 0xff00;
    car->f24 &= 0xe0ff;
    car->f24 &= 0xdfff;
    car->f24 &= 0xbfff;
    car->f24 &= 0x7fff;
    if (old == 0) {
        if (a_7eaap.m7eb0) {
            car->f26 = a_7eaap.m7eb2;
            a_7eaap.m7eb2 = car;
        } else
            a_7eaap.m7eb0 = car;
    }
    return car;
}

/* game_take_part @ 0000:4386 (linear 0x04386, 57 bytes) */

int near game_take_part(struct Node *n, int keep)
{
    int r;

    if (n == 0)
        return 0;
    r = n->a;
    if (keep) {
        n->next = a_7eaap.m7eb8;
        a_7eaap.m7eb8 = n;
    } else
        free_part(n);
    return r;
}

/* game_scrap_car @ 0000:43BF (linear 0x043BF, 108 bytes) */

int near game_scrap_car(struct Car *c, int keep)
{
    register struct Node **p2;
    int r;
    register int i;

    c->weight -= game_take_part(c->slot6, keep);
    r = 5;
    for (p2 = c->rack.s, i = r; i; p2++, i--)
        c->weight -= game_take_part(*p2, keep);
    game_take_part(c->slot12, 0);
    r = c->weight;
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
    g_0284 = 5;
    g_49e0[1] = 0xcd8;
    n = 2;
    i5 = price6 = 0;
    for (k3 = a_7d16q; k3 >= 0; k3 = g7d86n[k3].next) {
        g_49e0[n++] = g7d86n[k3].f6;
        list4[i5++] = k3;
    }
    g_49e0[n++] = 0xcfd;
    n0 = n;
    off6 = i5 - n0;
    for (k3 = 0; k3 < 0x2c; k3++) {
        if (g_4806[k3].f0 >= 0x14) {
            g_49e0[n++] = g_4806[k3].f6;
            list4[i5++] = k3;
        }
    }
    g_49e0[0] = n - 1;
    ads_open(g_49e0, (tab == 1) ? 2 : 0x1c);
    goto test;
    do {
        used = n >= n0;
        if (used) {
            k3 = g_4806[i5 = list4[n + off6]].f6;
            price6 = g_4806[i5].f0;
            g_11b0[0].f0 = 0xe;
        } else {
            k3 = (specn = &g7d86n[i5 = list4[n - 2]])->f6;
            price6 = specn->f0;
            g_11b0[0].f0 = 10;
        }
        strcpy(g_2990, g_2c02 + k3);
        sprintf(g_29d6, "$%-3d ?", price6);
        color = -1;
        for (;;) {
            n = F_14294(9);
            if (n == -20) {
                if (g_0610 && (long)price6 > a_7eaap.m7eaa)
                    a_7eaap.m7eaa += 1999;
                if ((long)price6 > a_7eaap.m7eaa) {
                    p1 = strchr(g_3a5a, '$');
                    sprintf(++p1, "%d short.", price6 - (int)a_7eaap.m7eaa);
                    game_message(0xf1a8);
                    break;
                }
                F_11540(-price6);
                if (used) {
                    game_add_part(i5, price6);
                    g_05fc += 0x222;
                    if (a_7eaap.m7ebc > 0x82)
                        goto done;
                    break;
                }
                car = game_make_car(i5, price6, 0);
                car->f4 = (color != -1) ? color : random_int(5);
                game_message(0xf2de);
                g_05fc += 0x444;
                goto done;
            }
            if (n != -25)
                break;
            F_120FE(-4);
            F_193D4();
            bm1 = F_1934E(0x140, 0x52, 0, 0);
            F_11A72(0, 0x40, 0x140, 0x52, bm1);
            x_colors = bustyle->colors;
            ck = (r_vid.mode == -2 || r_vid.mode == -6) ? g_02c2[5] : g_02b4[5];
            screen_fill_rect(0, 0x41, 0x140, 0x46, ck);
            screen_fill_rect(0, 0x87, 0x140, 0xa, x_colors.c[1]);
            if (r_vid.mode != -2 && r_vid.mode != -6) {
                sv = x_colors.c[3];
                x_colors.c[3] = 3;
                draw_vline(r_vid.back, 0, 0x40, 0x52);
                draw_vline(r_vid.back, 0x13f, 0x40, 0x52);
                draw_vline(r_vid.front, 0, 0x40, 0x52);
                draw_vline(r_vid.front, 0x13f, 0x40, 0x52);
                screen_hline(0, 0x40, 0x140);
                screen_hline(0, 0x91, 0x140);
                x_colors.c[3] = sv;
            }
            if (color == -1) {
                color = random_int(5);
                F_067CF(color);
            }
            x1 = ((unsigned)specn - (unsigned)g7d86n) / 10;
            y = (((specn->f4 >> 8) & 7) == 2) ? 8 : 0;
            F_181C8(7, x1, y, 0, 0, 0x86, 0, g_5030[(specn->f4 >> 4) & 3]);
            screen_text((0x140 - dash_text_width(g_2990)) / 2, 0x88, g_2990);
            F_120FE(-2);
            g_05d0 = 1;
            F_14294(0x18);
            g_05d0 = 0;
            F_11BD3(bm1, 0, 0x40);
            F_193D4();
        }
test:
        n = ads_run();
    } while (n != -3);
done:
    ads_close();
    if (a_7eaap.m7eb0)
        F_067CF(a_7eaap.m7eb0->f4);
    g_0284 = 0;
}

char *g_4fa4 = g_3bf9;   /* DS:4FA4 */

/* counter_offer @ 0000:4940 (linear 0x04940, 98 bytes) */

static int near counter_offer(unsigned *price)
{
    unsigned r;
    int key;

    if (r = *price % 5)
        if ((*price -= r) == 0)
            *price = 5;
    sprintf(g_4fa4, "How about $%-d ?", *price);
    popup_open_at(0xff7, -1, -1);
    key = F_14294(0xd);
    popup_close();
    return key;
}

/* game_car_look @ 0000:49A2 (linear 0x049A2, 75 bytes) */

int game_car_look(struct Car *c)
{
    int r;

    r = 0;
    if (c->f24 & 0x2000)
        r |= 4;
    if (c->f24 & 0x4000)
        r |= 2;
    if (c->f24 & 0x8000)
        r |= 1;
    if (g_4806[c->rack.s[0]->idx].f2 == 2)
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
        if (a_7eaap.m7eb0) {
            n = 1;
            g_49e0[1] = g7d86n[(tbl[1] = a_7eaap.m7eb0)->f2].f6;
        }
        for (cur = a_7eaap.m7eb2; cur; cur = cur->f26) {
            n++;
            g_49e0[n] = g7d86n[(tbl[n] = cur)->f2].f6;
        }
        g_49e0[0] = n;
        switch (item = F_142CF(0xb, (n > item) ? item : n, 0, g_49e0, 0, 0)) {
        case 0x3ea:
            game_message(ticks ? 0xee9 : 0xefa);
        case -3:
            return 0;
        case -1:
            g_4f48 = tbl[item = F_14C8B()];
            g_4f46 = a_7eaap.m7eb0;
            if (g_4f48 != g_4f46) {
                if (item == ((a_7eaap.m7eb0 != 0) + 1))
                    a_7eaap.m7eb2 = g_4f48->f26;
                else
                    tbl[item - 1]->f26 = g_4f48->f26;
                if (a_7eaap.m7eb0) {
                    a_7eaap.m7eb0->f26 = a_7eaap.m7eb2;
                    a_7eaap.m7eb2 = a_7eaap.m7eb0;
                }
                a_7eaap.m7eb0 = g_4f48;
            } else
                g_4f48 = 0;
            return 3;
        case -2:
            break;
        default:
            if (item > 0)       /* PROVISIONAL: the original loads g_49e0[0] here and drops it */
                g_49e0[0] = g_49e0[0];
        }
        cur = tbl[item = F_14C8B()];
        offer = price = (int)((random_int(20) + 75.0) * cur->weight / 100.0);
        strcpy(g_2990, g_2c02 + g7d86n[cur->f2].f6);
        g_29f8[0] = 0;
        pict = F_14090(12, &rx, &ye, 1);
        F_13DE6(12);
        do {
            if ((n = F_15127(&cash, g_29f8)) == -22)
                goto tail;
        } while (cash == 0);
        if ((unsigned)(cur->weight * 1.15) < cash) {
            game_message(0xec0);
            goto tail;
        }
        if (cur->weight < cash) {
            game_message(0x106e);
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
            if (cur->weight < cash) {
                game_message(0x1078);
                goto tail;
            }
        }
        if (cash > offer) {
            offer = (random_int(5) + 85.0) * price / 100.0;
            if (cash > offer && counter_offer(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if (cash > offer) {
            offer = (random_int(5) + 91.0) * price / 100.0;
            if (cash > offer && counter_offer(&offer) == -20)
                cash = offer;
        }
        if (cash > offer) {
            do {
                if ((n = F_15127(&cash, g_29f8)) == -22)
                    goto tail;
            } while (cash == 0);
        }
        if ((float)offer < (float)cash * 0.9) {
            game_message(0x1084);
            goto tail;
        }
        game_message((cur->weight > cash * 2) ? 0x104b : 0x1091);
        g_05fc += 0x888L;
        F_11540(cash);
        if (item == 1 && a_7eaap.m7eb0) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
            F_120FE(-4);
            mid = a_7eaap.m7eb0->f2;
            result = game_car_look(a_7eaap.m7eb0);
            F_181C8(6, mid, result, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                    g_5030[g_4806[a_7eaap.m7eb0->slot12->idx].f2]);
            F_13F21(1);
            F_06AA6();
            F_13DE6(10);
            F_120FE(-2);
            game_scrap_car(a_7eaap.m7eb0, 0);
            a_7eaap.m7eb0 = 0;
        } else {
            if (item == ((a_7eaap.m7eb0 != 0) + 1))
                a_7eaap.m7eb2 = cur->f26;
            else
                tbl[item - 1]->f26 = cur->f26;
            game_scrap_car(cur, 0);
        }
tail:
        F_13F21(1);
        if (pict) {
            F_11BD3(pict, rx, ye);
            F_1941E(1);
            pict = 0;
        }
    }
}

/* game_play_anim @ 0000:4FCA (linear 0x04FCA, 48 bytes) */

void game_play_anim(int h)
{
    video_anim_start(h);
    while (video_anim_active(h))
        sys_wait_input(10);
}

/* game_popup @ 0000:4FFA (linear 0x04FFA, 69 bytes) */

void game_popup(int a, int b, int c)
{
    long saved;

    saved = g_05cc;
    popup_open_at(a, b, c);
    game_pause(g_05dc);
    popup_close();
    g_05cc = saved;
}

/* game_run @ 0000:503F (linear 0x0503F, 1007 bytes) */

void near game_run(void)
{
    int again;
    int kind;
    int v;
    int w;

    again = 1;
    kind = start_game();
    for (;;) {
        if (kind == -1)
            again = 1;
        w = kind;
        if (w == -1)
            kind = savegame_new();
        else
            kind = savegame_load();
        if (kind == -666) {
            kind = -1;
            again = 1;
        }
        if (kind && again)
            continue;
        F_11593(g_49dc);
        v = 1;
        if (kind == 0 && w == -2 && a_7eaap.m7eb0)
            v = 2;
        if (kind == 0 && w == -2)
            v = 1;
        if (w == -1 && a_7eaap.m7eb6 <= 15 && a_7eaap.m7ebc <= 130) {
            kind = read_paper();
            if (kind == 2 || kind == 1)
                buy_from_ads(kind);
        }
        again = 0;
        while ((kind = F_06B06(v)) != 0) {
            F_16381(0);
            if (kind == 0x29a) {
                if (g_0282 == 1)
                    dash_time_up();
                fame_show(g_0282 & 2);
                F_120FE(1);
                g_0282 = 0;
                again = 1;
                kind = fame_end_menu();
                if (kind != -2 && kind != -1 && kind == 4)
                    return;
                F_120FE(-2);
                g_4f4a = 0;
                break;
            }
            v = 2;
            switch (kind) {
            case 1:
                if (check_room())
                    v = 0;
                else {
                    if (a_7eaap.m7eb0 == 0)
                        v = 1;
                    kind = read_paper();
                    if (kind == 2 || kind == 1)
                        buy_from_ads(kind);
                    if (a_7eaap.m7eb0 == 0)
                        v = 2;
                    if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2)
                        v = 1;
                    g_05fc += 0x222;
                }
                continue;
            case 2:
            case 15:
                if (check_room()) {
                    v = 0;
                    continue;
                }
                if (a_7eaap.m7eb0) {
                    if (F_07EE6(a_7eaap.m7eb0, 1)) {
                        perf_drivetrain(a_7eaap.m7eb0, &e_78e0);
                        if (kind == 2) {
                            if (a_7eaap.m7eb0->f22 < 9) {
                                game_message(0x172e);
                                v = 0;
                                continue;
                            }
                            g_0284 = 2;
                            cruise_go();
                            g_05fc += 0x222;
                            bobs_visit();
                        } else {
                            if (a_7eaap.m7eb0->f22 >= 0x8c) {
                                game_message(0x19b1);
                                v = 0;
                                continue;
                            }
                            if (a_7eaap.m7eaa == 0) {
                                game_message(0x199f);
                                v = 0;
                                continue;
                            }
                            g_0284 = 3;
                            cruise_go();
                            g_05fc += 0x222;
                            race_gas_station();
                        }
                        g_05fc += 0x222;
                        v = 1;
                    } else {
                        game_message(0x0e7f);
                        v = 0;
                    }
                } else {
                    if (a_7eaap.m7eb2)
                        game_message(0x0f16);
                    else
                        game_message(0x0efa);
                    v = 0;
                }
                continue;
            case 4:
                v = 0;
                switch (kind = savegame_menu()) {
                case -20:
                    return;
                case -22:
                case -4:
                    continue;
                case -3:
                    if (check_room()) {
                        v = 0;
                        kind = 0x309;
                    } else {
                        savegame_save();
                        v = 2;
                        if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2)
                            v = 1;
                        continue;
                    }
                case -2:
                case -1:
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
                F_06F6B();
                v = 0;
                continue;
            case 8:
                F_07800();
                v = 0;
                continue;
            case 9:
                F_07AA4();
                continue;
            case 10:
                F_193FE();
                garage_hood(a_7eaap.m7eb0);
                continue;
            case 13:
                F_0711A(4);
                v = 0;
                continue;
            case 11:
                F_0711A(2);
                v = 0;
                continue;
            case 12:
                F_0711A(1);
                v = 0;
                continue;
            case 14:
                F_07367();
                v = 0;
                continue;
            case 3:
                F_065AF(0);
                v = 0;
                continue;
            case 16:
                F_079EB();
                v = 0;
                continue;
            case 17:
                game_message(0x18f6);
                v = 0;
                continue;
            case -40:
                if (g_58d0) {
                    sys_music_pause();
                    g_58d0 = 0;
                } else {
                    g_58d0 = 1;
                    sys_music_resume();
                }
                v = 0;
                continue;
            }
            break;
        }
    }
}
