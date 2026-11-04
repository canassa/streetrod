#include <string.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "BOBS.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARHOP.H"
#include "CARPIC.H"
#include "CRUISE.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRVTICK.H"
#include "GAME.H"
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
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

#pragma intrinsic(abs)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_6c84;                         /* DS:6C84 */
static int g_6c88;                         /* DS:6C88 */
static void near drop_last_word(char *s);
static int near maybe_carhop(int mode, int arg, int pct1, int pct2);
static int g_6c86;                         /* DS:6C86 */
static int g_6c8aaav[12];                  /* DS:6C8A */
static int g_6ca2;                         /* DS:6CA2 */
static void near build_rival_pool(int);
static void near select_rival(int);
static int near king_status(void);
static void near show_rival_car(int, int, int, int, int, int *);
static void near rival_arrives(int, int, int);
static void near list_met_rivals(int *, int *, int *);
static int near spec_class(int);
static int near accepts_bet(int kind);
static void near blit_cell(struct Bitmap far *, int, struct Bitmap far *);
static void near setup_bobs_spots(int, int);
static int near king_chance(void);
static int near look_for_rival(int);
static int near pick_rival(int, int);
static void near play_ending(void);
static void near pop_rival_hood(void);
static int near challenge(int);
static void near play_cell_anim(void);
static void near set_jail_spots(int id, char c);

int g_51ee = 100;   /* DS:51EE */
int g_51f0 = 0;   /* DS:51F0 */

/* maybe_carhop @ 0000:914C (linear 0x0914C, 266 bytes) */

static int near maybe_carhop(int mode, int arg, int pct1, int pct2)
{
    int r;

    r = 0;
    switch (mode) {
    case 0:
        if (g_6c84 != 0)
            break;
        if (g_51f0 < 3 || random_int(100) >= pct1)
            break;
        r = 1;
        F_120FE(-4);
        F_18D59(0, arg);
        g_6c84 = 1;
        if (random_int(100) < pct2) {
            F_10AFD(100);
            F_18D59(1, arg);
            g_6c84 = 0;
            g_6c88 = 0;
        }
        break;
    case 1:
        if (g_6c84 != 1 || random_int(100) >= pct1)
            break;
        r = 1;
        F_120FE(-4);
        F_18D59(1, arg);
        g_6c84 = 0;
        g_6c88 = 0;
        g_51f0 = 0;
        break;
    }
    if (r)
        F_120FE(-2);
    return r;
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Spec *ar7d7au;
struct Racer *g7d84o;
int bm7f86;

/* drop_last_word @ 0000:9256 (linear 0x09256, 78 bytes) */

static void near drop_last_word(char *s)
{
    int i;

    while ((i = strlen(s) - 1) >= 0) {
        if (s[i] != ' ')
            s[i] = 0;
        else
            break;
    }
    if (i >= 0)
        s[i] = 0;
}

int g_51f8 = -1;   /* DS:51F8 */

/* show_rival_car @ 0000:92A4 (linear 0x092A4, 640 bytes) */

static void near show_rival_car(int mode, int b, int c, int d, int e, int *f)
{
    int x, n, old;

    old = g_5a24;
    strcpy(g7f32s, g_2c02 + g7d84o->f6);
    strcat(g7f32s, "  ");
    strcat(g7f32s, g_2c02 + ar7d7au->f6);
    x = g_0284 == 6;
    if (mode == 4)
        g_6c84 = -1;
    bm7f86 = mode == 2;
    if (mode == 1 && !maybe_carhop(0, bm7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, 10))
        maybe_carhop(1, bm7f86, 20, 0);
    if (g7f32s[n = strlen(g7f32s) - 1] == '.')
        g7f32s[n] = 0;
    for (;;) {
        cz8bc0y = dash_text_width(g7f32s);
        n = 320 - (ar82c0 + cz8bc0y);
        n = n > 0 ? n >> 1 : 0;
        n = n < 15 ? 15 : n;
        g_51ee = (n + ar82c0) & ~7;
        if (g_51ee + cz8bc0y > 320) {
            drop_last_word(g7f32s);
            continue;
        }
        break;
    }
    F_11421(-1, "", 0);
    F_11421(g_51ee, g7f32s, cz8bc0y);
    F_181C8(mode, b, g7d84o->fe, g7d84o->ff, c, d, e, f);
    if (old)
        mode = 2;
    if (mode == 2)
        g_51f0++;
    if (mode == 2) {
        g7f32s[0] = 0;
        F_11421(g_51ee, " ", cz8bc0y);
    }
    if (mode == 4)
        g_6c84 = 0;
    bm7f86 = (mode == 2) ? 0 : 1;
    if (!(x && bm7f86))
        if (!maybe_carhop(0, bm7f86, 10 * x + 75 * g_6c88 + (g_0610 << 3) + 5, bm7f86 == 0 ? 8 : 0))
            maybe_carhop(1, bm7f86, 40, 0);
}

int bn7648;
int g7d40q;
int g7f82r;

/* spec_class @ 0000:9524 (linear 0x09524, 50 bytes) */

static int near spec_class(int n)
{
    int v;

    v = g7d86n[n].f2;
    if (v < 3)
        return 0;
    if (v >= 7)
        return 2;
    return 1;
}

/* king_status @ 0000:9556 (linear 0x09556, 155 bytes) */

static int near king_status(void)
{
    int k, any, total;

    any = 0;
    for (k = 0; k < 21; k++)
        if (av7ff8[k].fb != -1)
            any = 1;
    if (any == 0)
        return 0;
    total = g_4972 + g_4974;
    if (total < 3)
        return 1;
    if (total >= 7 && g_4972 < 6 && g_4974 < 6)
        return 3;
    if (total < 7 && g_4972 < 6 && g_4974 < 6)
        return 2;
    if (g_4972 < 6)
        return 4;
    if (g_4974 < 6)
        return 5;
    if (total >= (g_4970 >> 1))
        return 0;
    return 3;
}

/* king_chance @ 0000:95F1 (linear 0x095F1, 85 bytes) */

static int near king_chance(void)
{
    int kind, v;

    if (bn7648 == 25)
        return 0;
    kind = king_status();
    if (kind) {
        if (kind == 1)
            v = 5;
        else if (kind == 2)
            v = 8;
        else
            v = 12;
    } else {
        if (av7ff8[21].fb == 1)
            v = 10;
        else
            v = 50;
    }
    return v;
}

/* build_rival_pool @ 0000:9646 (linear 0x09646, 520 bytes) */

static void near build_rival_pool(int flag)
{
    int result;
    struct Spec *item;
    struct Cand map[21];
    int m, found, k, low;

    for (k = 0; k < 21; k++) {
        item = &g7d86n[av7ff8[k].f8];
        map[k].dr = abs(F_0E218(av7ff8[k].f8) - F_0E218(-1));
        map[k].dk = abs(g7d86n[av7ff8[k].f8].f2 - g7d86n[a_7eaap.m7eb0->f2].f2);
        map[k].taken = (av7ff8[k].fb != -1) ? 0 : 1;
    }
    g_6c86 = 0;
    found = 1;
    do {
        if ((flag ? 21 : 9) <= g_6c86)
            break;
        found = 0;
        low = 0x7d00;
        for (m = 0; m < 21; m++)
            if (map[m].taken == 0 && map[m].dr < low) {
                result = m;
                low = map[m].dr;
                found = 1;
            }
        if (found) {
            g_6c8aaav[g_6c86++] = result;
            map[result].taken = 1;
        }
    } while (found);
    m = 0;
    k = g_6c86;
    for (; flag == 0 && k > 6; k--)
        if (map[k - 1].dk > 1)
            g_6c8aaav[k - 1] = g_6c8aaav[m++];
    if (g_0610) {
        g_6c8aaav[g_6c86++] = g_4ff4;
        g_6c8aaav[g_6c86++] = g_4ff4;
        if (g_6ca2) {
            g_6c8aaav[(g_6c86 = 1) - 1] = g_4ff4;
        }
        g_6ca2 = 0;
    }
}

/* pick_rival @ 0000:984E (linear 0x0984E, 108 bytes) */

static int near pick_rival(int chance, int arg)
{
    int pick;

    if (random_int(100) < chance)
        return 0x15;
    build_rival_pool(arg);
    if (g_6c86 == 0)
        return 0x15;
    pick = g_6c8aaav[random_int(g_6c86)];
    if (pick == g_51f8)
        pick = g_6c8aaav[random_int(g_6c86)];
    g_51f8 = pick;
    return pick;
}

/* select_rival @ 0000:98BA (linear 0x098BA, 365 bytes) */

static void near select_rival(int n)
{
    int skill;

    g7f82r = n;
    g7d84o = &av7ff8[n];
    ar7d7au = &g7d86n[bn7648 = g7d84o->f8];
    if (n != 0x15) {
        g7d40q = 0;
        g7d84o->fb = 1;
        skill = (ar7d7au->f2 >> 1) + 1;
        if (g7d84o->ff == 0 && g7d84o->fc > 5)
            g7d84o->ff = random_int(20) < skill ? random_int(9) : 0;
        else if (g7d84o->ff > 0 && g7d84o->fc > 10)
            g7d84o->ff = random_int(40) < skill ? random_int(9) : g7d84o->ff;
        if (w_8df0[bn7648].f6 > 0 && w_8df0[bn7648].f8 > 0 && g7d84o->fc > 8)
            g7d84o->fe |= random_int(25) < skill ? 3 : g7d84o->fe;
        if (w_8df0[bn7648].f2 > 0 && g7d84o->fc > 8)
            g7d84o->fe |= random_int(30) < skill ? 4 : g7d84o->fe;
    } else
        g7d40q = 1;
    m_8272.m827a = ar7d7au->f8;
    m_8272.m827e = (ar7d7au->f4 >> 4) & 3;
    m_8272.m827c = g7d84o->fa;
    F_067CF(m_8272.m827c);
    m_8272.m8280 = g7d84o->f4;
}

/* rival_arrives @ 0000:9A27 (linear 0x09A27, 92 bytes) */

static void near rival_arrives(int n, int a, int mode)
{
    select_rival(n);
    if (mode == 1)
        g_5a24 = 1;
    if (mode == 2 && g7f82r == 0x15)
        g_5a24 = king_status() == 1;
    show_rival_car(a, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
}

/* list_met_rivals @ 0000:9A83 (linear 0x09A83, 113 bytes) */

static void near list_met_rivals(int *ids, int *slots, int *kinds)
{
    int i, n;

    if (king_status() == 0)
        av7ff8[21].fb = 1;
    else
        av7ff8[21].fb = 0;
    for (i = 0, n = 0; i <= 21; i++) {
        if (av7ff8[i].fb == 1) {
            slots[n] = i;
            ids[++n] = av7ff8[i].f6;
            kinds[n] = 7;
        }
    }
    ids[0] = n;
}

int as79a2z;
int d_79a4;

int g_51fc[2][5] = {{4779, 4803, 4822, 4840, 4663}, {4663, 4675, 4694, 4715, 4727}};   /* DS:51FC */
int g_5210[2][3] = {{4859, 4867, 4875}, {4743, 4756, 4767}};   /* DS:5210 */

/* look_for_rival @ 0000:9AF4 (linear 0x09AF4, 209 bytes) */

static int near look_for_rival(int n)
{
    int list[24], sel[24], kinds[24], choice;

    F_11421(-1, "", 0);
    list_met_rivals(list, sel, kinds);
    choice = F_142CF(0x24, 1, kinds, list, 0, 0);
    switch (choice) {
    case -4:
        return 0;
    case -3:
        if (bm7f86) {
            F_11421(-1, "", 0);
            show_rival_car(2, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
        }
        choice = F_14C8B();
        rival_arrives(sel[choice - 1], n, 0);
        return 1;
    case 0x3ea:
        game_message(0x198c);
        return 0;
    }
}

/* accepts_bet @ 0000:9BC5 (linear 0x09BC5, 424 bytes) */

static int near accepts_bet(int kind)
{
    int item1;
    unsigned v11, v2;
    int adj9, chm, scorel, row, p27, p18, c6, bonus2;
    unsigned v3;
    int clsl, col;

    if (kind == 0) {
        row = 0;
        col = 0;
    } else {
        if (kind == 5) {
            row = 2;
            col = 3;
        } else if (kind == 1 || kind == 2) {
            row = 1;
            col = 1;
        } else if (kind == 3 || kind == 4) {
            row = 1;
            col = 2;
        }
        goto calc;
    }
calc:
    chm = g7d84o->f0;
    p18 = spec_class(bn7648);
    p27 = spec_class(a_7eaap.m7eb0->f2);
    clsl = (ar7d7au->f4 >> 8) & 7;
    item1 = g_4806[a_7eaap.m7eb0->rack.s[0]->idx].f2;
    v11 = ag8baey[chm][row];
    v2 = g7d68m[p18][clsl];
    v3 = g7d68m[p27][item1];
    if (v11 + v2 < v3)
        c6 = 0;
    else if (v2 > v3)
        c6 = 2;
    else
        c6 = 1;
    scorel = h_8dd8[col][c6];
    if ((a_7eaap.m7eb0->f24 & 0x1f00) == 0)
        bonus2 = 0;
    else if (a_7eaap.m7eb0->f24 & 0x100)
        bonus2 = 4;
    else
        bonus2 = 6;
    adj9 = (2 * g7d84o->fd - g7d84o->fc) * scorel / 4;
    scorel += adj9 + bonus2;
    if (g_0610)
        scorel += (scorel >> 2) + 10;
    return random_int(100) <= scorel;
}

/* challenge @ 0000:9D6D (linear 0x09D6D, 907 bytes) */

static int near challenge(int arg)
{
    int num;
    struct Bitmap far *pict;
    unsigned price;
    int kind;
    int x;
    int ch;
    int step;
    int sy;
    int prev;

    F_193FE();
    if (arg) {
        ch = king_status();
        switch (ch) {
        case 2:
            game_message(0x1113);
            break;
        case 3:
            game_message(0x113d);
            break;
        case 4:
            game_message(0x117d);
            break;
        case 5:
            game_message(0x11a7);
            break;
        case 0:
            if (g_4976 == 0) {
                if (a_7eaap.m7eb0->f22 < 22) {
                    game_message(0x1761);
                    return 0;
                }
                a_7eaap.m7ec0 = 0;
                game_message(0x11d1);
                as79a2z = 0;
            } else {
                a_7eaap.m7ec0 = 1;
                game_message(0x11f7);
                as79a2z = 5;
            }
            d_79a4 = 0;
            race_run();
            g_05fc += a_7eaap.m7ec0 == 0 ? 0x444u : 0x777u;
            return 2;
        }
        F_193FE();
        return 0;
    }
    if (a_7eaap.m7eb0->f22 < 7) {
        game_message(0x1761);
        return 0;
    }
    pict = F_14090(0x12, &x, &sy, 1);
    F_13DE6(0x16);
    g_05cc = g_tick + 800;
    while ((ch = sys_wait_input(3000)) == 0)
        ;
    switch (ch) {
    case 1:
        num = 0x12;
        a_7eaap.m7ec0 = 0;
        break;
    case 2:
        num = 0x13;
        a_7eaap.m7ec0 = 1;
        break;
    case -23:
    case 1000:
        game_message(0x122c);
        goto quit;
    }
    if (a_7eaap.m7ec0 == 1 && a_7eaap.m7eb0->f22 < 17) {
        game_message(0x1761);
        goto quit;
    }
    F_13F21(1);
    prev = -1;
    F_13DE6(num);
    for (step = 0; step < 2; step++) {
        g_05cc = g_tick + 800;
        while ((ch = sys_wait_input(3000)) == 0)
            ;
        m_8272.m8278 = ch;
        switch (ch) {
        case -23:
        case 1000:
            game_message(0x122c);
            goto quit;
        case 1:
            price = 0;
            kind = 0;
            break;
        case 2:
            price = 10;
            kind = 1;
            break;
        case 3:
            price = 50;
            kind = 2;
            break;
        case 7:
            price = 25;
            kind = 3;
            break;
        case 8:
            price = 100;
            kind = 4;
            break;
        case 9:
            price = 0;
            kind = 5;
            break;
        }
        if ((long)price <= a_7eaap.m7eaa) {
            if (accepts_bet(kind)) {
                as79a2z = kind;
                d_79a4 = price;
                F_13F21(1);
                F_11BD3(pict, x, sy);
                pict = 0;
                game_message(g_5210[a_7eaap.m7ec0][random_int(3)]);
                race_run();
                g_05fc += a_7eaap.m7ec0 == 0 ? 0x444u : 0x777u;
                return 2;
            }
            while ((ch = random_int(5)) == prev)
                ;
            game_message(g_51fc[a_7eaap.m7ec0][prev = ch]);
        } else
            game_message(0x131b);
    }
quit:
    F_13F21(1);
    F_11BD3(pict, x, sy);
    pict = 0;
    F_193FE();
    return 0;
}

/* setup_bobs_spots @ 0000:A0F8 (linear 0x0A0F8, 365 bytes) */

static void near setup_bobs_spots(int a, int b)
{
    struct Hot save;
    struct Hot *hot;
    int y1, x1, y, x;

    hot = g_20aa;
    if (g_20aa[4].f6 != 4) {
        save = g_20aa[3];
        g_20aa[3] = g_20aa[4];
        g_20aa[4] = save;
    }
    hot[4].x1 = 0xc2;
    hot[4].x2 = 0xd8;
    hot[4].y1 = 0x49;
    hot[4].y2 = 0x77;
    hot[2].f0 = (b == 1) ? 11 : 14;
    if (a == 0) {
        hot[1].f0 = 14;
        hot[3].f0 = 14;
    } else {
        x1 = ai70e4t[bn7648].r[1].x + ac825et;
        y1 = ai70e4t[bn7648].r[1].y + aw8260y;
        x = ai70e4t[bn7648].r[1].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].f0 = 11;
        x1 = ai70e4t[bn7648].r[3].x + ac825et;
        y1 = ai70e4t[bn7648].r[3].y + aw8260y;
        x = ai70e4t[bn7648].r[3].w + x1 - 1;
        y = ai70e4t[bn7648].r[3].h + y1 - 1;
        if (hot[1].x2 >= x1)
            x1 = hot[1].x2 + 1;
        hot[3].x1 = x1;
        hot[3].x2 = x;
        hot[3].y1 = y1;
        hot[3].y2 = y;
        hot[3].f0 = 11;
        if (hot[4].y2 >= hot[1].y1)
            hot[4].y2 = hot[1].y1 - 1;
        if (hot[4].x1 < hot[3].x1) {
            save = hot[3];
            hot[3] = hot[4];
            hot[4] = save;
        }
    }
}

struct Step g_521e[30] = {{2, 15}, {3, 15}, {4, 22}, {5, 15}, {6, 37}, {5, 15}, {6, 45}, {5, 15}, {4, 15}, {3, 15}, {2, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {8, 75}, {11, 15}, {12, 30}, {13, 15}, {12, 15}, {11, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {14, 15}};   /* DS:521E */
char g_525a[16] = {1, 15, 7, 8, 9, 10, 11, 12, 2, 3, 0, 1, 5, 6, 13, 4};   /* DS:525A */
struct Cell g_526a[20] = {{0, 0, 48, 24, 80, 0}, {48, 0, 32, 24, 88, 0}, {80, 0, 32, 24, 96, 0}, {112, 0, 64, 24, 88, 0}, {0, 24, 40, 44, 96, 0}, {40, 24, 40, 44, 96, 0}, {80, 24, 40, 44, 96, 0}, {120, 24, 40, 44, 96, 0}, {0, 68, 40, 44, 96, 0}, {40, 68, 40, 44, 96, 0}, {80, 68, 40, 44, 96, 0}, {120, 68, 40, 44, 96, 0}, {0, 112, 40, 44, 96, 0}, {40, 112, 40, 44, 96, 0}, {80, 112, 40, 44, 96, 0}, {120, 112, 40, 44, 96, 0}, {0, 156, 48, 44, 88, 0}, {48, 156, 56, 44, 80, 0}, {104, 156, 56, 44, 80, 0}, {160, 156, 40, 44, 96, 0}};   /* DS:526A */
struct Cell g_52e2[9] = {{208, 54, 8, 2, 128, 84}, {200, 54, 8, 3, 112, 88}, {184, 54, 16, 4, 96, 94}, {160, 54, 24, 10, 112, 113}, {160, 86, 40, 10, 128, 113}, {160, 65, 48, 20, 136, 104}, {160, 1, 112, 52, 128, 70}, {0, 1, 160, 97, 112, 52}, {0, 99, 224, 101, 80, 51}};   /* DS:52E2 */
int g_5318[8] = {1201, 1217, 1106, 1107, 1108, 1109, 1073, 0};   /* DS:5318 */
int g_5328[3] = {6, 7, 0};   /* DS:5328 */

/* pop_rival_hood @ 0000:A265 (linear 0x0A265, 247 bytes) */

static void near pop_rival_hood(void)
{
    struct Bitmap far *pict;
    struct Car chassis;
    int n;
    struct Node nodes[5];
    struct Node n1;
    struct Node na;

    F_193FE();
    pict = F_11A72(0x60, 9, 200, 143, 0);
    chassis.slot6 = &n1;
    chassis.slot12 = &na;
    for (n = 0; n < 5; n++)
        chassis.rack.s[n] = &nodes[n];
    game_make_car(bn7648, 0, &chassis);
    garage_hood(&chassis);
    F_11421(-1, "", 0);
    F_13DE6(0x18);
    g_05cc = g_tick + 800;
    while ((n = sys_wait_input(3000)) == 0)
        ;
    r_vid.f8 = 0;
    F_11BD3(pict, 0x60, 9);
    F_193FE();
    F_13F21(1);
}

/* blit_cell @ 0000:A35C (linear 0x0A35C, 186 bytes)

   Blit sprite-sheet cell n (7 per row) of bm into dst */

static void near blit_cell(struct Bitmap far *bm, int n, struct Bitmap far *dst)
{
    struct Blt cell;
    int row;
    struct Blt bl;
    int col;

    if (n < 14) {
        row = n / 7;
        col = n - 7 * row;
    } else {
        row = 0;
        col = 0;
    }
    cell.w = 40;
    cell.h = 50;
    cell.sx = 40 * col;
    cell.sy = 50 * row;
    cell.dx = 184;
    cell.dy = 73;
    if (g_0284 == 6) {
        bl.w = 16;
        bl.h = 50;
        bl.sx = 208;
        bl.sy = 73;
        bl.dx = cell.sx + 24;
        bl.dy = cell.sy;
        (*b_78a2)(r_vid.front, bm, &bl, 1);
    }
    (*b_78a2)(bm, dst, &cell, 1);
}

struct Bitmap far *c_6e64;
int bo7676;
void (far *dz78b6)(char far *, char far *);
struct Bitmap far *x_82b8;

/* play_cell_anim @ 0000:A416 (linear 0x0A416, 302 bytes) */

static void near play_cell_anim(void)
{
    struct Bitmap far *pict;
    int going;
    int i, key;

    F_193FE();
    pict = F_15BBE(0x4c9, 2);
    i = 0;
    F_13DE6(0x18);
    g_05cc = g_tick + 10;
    going = 1;
    do {
        while ((key = sys_wait_input(3000)) == 0)
            ;
        switch (key) {
        case -26:
            going = 0;
            break;
        case 1000:
            if (i >= 31)
                going = 0;
            else {
                F_10AFD(1);
                blit_cell(pict, g_525a[1 + g_521e[i].frame], r_vid.front);
                g_05cc = g_tick + (int)g_521e[i].delay;
                i++;
            }
            break;
        }
    } while (going);
    F_10AFD(1);
    blit_cell(pict, g_525a[1 + 1], r_vid.front);
    blit_cell(pict, g_525a[1 + 1], r_vid.back);
    F_13F21(1);
    F_193FE();
}

/* play_ending @ 0000:A544 (linear 0x0A544, 1607 bytes) */

static void near play_ending(void)
{
    struct Bitmap far *pic4;
    int h8;
    int r;
    int flag8;
    struct Bitmap far *pic15;
    struct Bitmap far *pic24;
    struct Blt blt;
    struct Cell *p3;
    int v30;
    struct Bitmap far *pic58;
    int v2a;
    int i2;
    int v32;
    int minx;
    int miny;
    int v38l;
    struct Bitmap far *pic3;
    struct Cell *p2m;
    int w8;
    int cntz;
    struct Blt blt2k;
    struct Bitmap far *pic6m;
    int savedl;

    flag8 = 0;
    F_120FE(-4);
    screen_status_init(0xbe);
    F_119B8(9, 8, 0x14, 2);
    r_vid.f16 = 0;
    savedl = g_04c2;
    g_04c2 = 0xc8;
    (*dz78b6)(r_vid.front->bits.p, r_vid.back->bits.p);
    F_118D4(0x408, -1);
    g_04c2 = savedl;
    (*m_present)();
    game_pause(0x2ee);
    (*s_blit)(r_vid.front->bits.p, r_vid.back->bits.p);
    g_0284 = 1;
    pic15 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic15);
    F_15C95(g_4c28);
    g_4f4c = 1;
    r = F_15C4A(g_5318, 0);
    r = F_15C4A(g_5328, 0);
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.f16 = 0;
    (*m_present)();
    av7ff8[21].f4 = 0x4c1;
    select_rival(0x15);
    F_181C8(4, bn7648, g7d84o->fe, g7d84o->ff, 1, 0x98, 0, g_5030[m_8272.m827e]);
    av7ff8[21].f4 = 0x4dc;
    pic24 = F_1934E(0x50, 0x2c, 0, 3);
    F_11A72(0x78, 0x45, 0x50, 0x2c, pic24);
    pic3 = F_15BBE(6, 1);
    pic4 = F_1934E(0x50, 0x2c, 0, 3);
    p3 = g_526a;
    blt.w = g_526a[0].w;
    blt.h = g_526a[0].h;
    blt.sx = g_526a[0].sx;
    blt.sy = g_526a[0].sy;
    blt.dx = g_526a[0].x - 0x50;
    blt.dy = 0x2c - g_526a[0].h;
    (*ab78aat)(pic3, pic15, &blt, 1);
    blt.w = p3->w;
    blt.h = p3->h;
    blt.sx = p3->x - 0x50;
    blt.sy = 0x2c - p3->h;
    blt.dx = blt.sx;
    blt.dy = blt.sy;
    (*b_78a2)(pic15, pic24, &blt, 1);
    game_pause(10);
    for (i2 = 1; i2 < 0x14; i2++) {
        if (i2 == 13)
            F_10AFD(0x14);
        p3 = &g_526a[i2];
        bitmap_copy(pic24, pic4, 1);
        blt.w = p3->w;
        blt.h = p3->h;
        blt.sx = p3->sx;
        blt.sy = p3->sy;
        blt.dx = p3->x - 0x50;
        blt.dy = 0x2c - p3->h;
        (*ab78aat)(pic3, pic4, &blt, 1);
        blt.w = 0x50;
        blt.h = 0x2c;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0x78;
        blt.dy = 0x45;
        if (i2 < 0x13) {
            F_10AFD(10);
            (*b_78a2)(pic4, r_vid.front, &blt, 1);
        }
        if (i2 == 0x12)
            (*b_78a2)(pic4, r_vid.back, &blt, 1);
    }
    F_193D4();
    if (flag8 == 0) {
        pic58 = F_15BBE(7, 1);
        cntz = 9;
        if (r_vid.mode == -6) {
            F_1929A(10000);
            cntz--;
        }
        v32 = v38l = 0x28;
        v2a = v30 = -0x28;
        for (i2 = 0; i2 < cntz; i2++) {
            p2m = &g_52e2[i2];
            w8 = max((int)p2m->w + (int)p2m->x, v32 + v2a) - (minx = min((int)p2m->x, v32));
            h8 = max((int)p2m->h + (int)p2m->y, v38l + v2a);
            h8 -= miny = min((int)p2m->y, v38l);
            pic6m = F_1934E(w8, h8, 0, 2);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = minx;
            blt2k.sy = miny;
            blt2k.dx = 0;
            blt2k.dy = 0;
            (*b_78a2)(r_vid.back, pic6m, &blt2k, 1);
            blt2k.w = v2a = p2m->w;
            blt2k.h = v30 = p2m->h;
            blt2k.sx = p2m->sx;
            blt2k.sy = p2m->sy;
            blt2k.dx = (v32 = p2m->x) - minx;
            blt2k.dy = (v38l = p2m->y) - miny;
            (*ab78aat)(pic58, pic6m, &blt2k, 1);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = 0;
            blt2k.sy = 0;
            blt2k.dx = minx;
            blt2k.dy = miny;
            F_10AFD(8);
            (*b_78a2)(pic6m, r_vid.front, &blt2k, 1);
            F_19438(1);
        }
    }
    blt2k.sx = minx;
    blt2k.sy = miny;
    game_pause(0x50);
    F_10AFD(1);
    (*b_78a2)(r_vid.back, r_vid.front, &blt2k, 1);
    F_10AFD(1);
    (*b_78a2)(pic4, r_vid.front, &blt, 1);
    (*b_78a2)(pic4, r_vid.back, &blt, 1);
    game_pause(0x32);
    game_message(0x183c);
    r_vid.f16 = 0;
    F_1923C();
    F_15C95(g_5328);
    F_15C95(g_5318);
}

struct Btn g_5338[7] = {{&c_6e64, 0, 0, 16, 149, 40, 5}, {&c_6e64, 0, 0, 16, 149, 96, 5}, {&c_6e64, 0, 0, 16, 149, 152, 5}, {&c_6e64, 0, 0, 16, 149, 208, 5}, {&c_6e64, 0, 0, 16, 149, 264, 5}, {&x_82b8, 0, 0, 32, 149, 0, 5}, {&x_82b8, 32, 0, 32, 149, 288, 5}};   /* DS:5338 */
struct Pair g_539a[6] = {{32, 8}, {56, 40}, {112, 40}, {168, 40}, {224, 40}, {280, 8}};   /* DS:539A */

/* bobs_visit @ 0000:AB8B (linear 0x0AB8B, 1210 bytes) */

void bobs_visit(void)
{
    int n;
    int rival;
    int choice;
    int flag;
    char class;
    int place;
    int status;

    n = 0;
    class = 4;
    flag = 0;
    F_11421(-1, "", 0);
    g_472a = 0x9e;
    g_472c = 0x80;
    F_118D4(0x445, -1);
    F_10B8C();
    F_11593(g_49dc);
    g_cursor2.r.x2 = -1;
    F_120FE(1);
    (*m_present)();
    F_16381(0);
    if (random_int(100) < 25)
        play_cell_anim();
    F_120FE(-2);
    bm7f86 = 0;
    g_6c84 = -1;
    g_6c88 = 0;
    g_6ca2 = 1;
    g_0284 = 2;
    g_05ee = g_471c = g_4732 = 0;
    for (;;) {
        setup_bobs_spots(bm7f86, !g_6c84 & !g_6c88);
        F_13DE6(0x25);
        g_05cc = flag == 0 ? (unsigned)(class == 4 ? 140 : 280) + g_tick : g_tick + 5;
        flag = 0;
        rival = class == 4 ? 0 : king_chance();
        F_06529(0);
        if (g_0282)
            return;
        while ((choice = sys_wait_input(3000)) == 0)
            ;
        switch (choice) {
        case 1:
            g7f32s[0] = 0;
            F_11421(g_51ee, " ", cz8bc0y);
            F_13F21(1);
            g_0284 = 1;
            cruise_go();
            return;
        case 1000:
            F_06529(0);
            if (g_0282)
                return;
            if (bm7f86 == 0)
                goto race;
            F_11421(-1, "", 0);
            show_rival_car(2, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
            n = 0;
            F_13F21(1);
            break;
        default:
out:
            F_13F21(1);
            break;
        case 4:
            if ((status = look_for_rival(class)) == 0)
                flag = 1;
            else
                n = 0;
            F_13F21(1);
            break;
        case 2:
            if (bm7f86 == 0) {
                g_05cc = g_tick;
race:
                rival_arrives(pick_rival(rival, 0), class, 2);
                class = 1;
                F_13F21(1);
            } else {
                aq8256z = 1;
                F_11421(-1, "", 0);
                F_11421(g_51ee, "", cz8bc0y);
test:
                while ((F_11421(-1, "", 0), challenge(g7d40q))) {
                    g7f32s[0] = 0;
                    F_11421(g_51ee, " ", cz8bc0y);
                    place = race_outcome();
                    (*bl78be)();
                    cruise_rival_paint(-1);
                    if (m_8272.m8278 != 1)
                        g_4970++;
                    if (g7d40q) {
                        if (bo7676 == 0) {
                            g_4976 = 0;
                            av7ff8[21].fb = 0;
                            g_4972 -= 3;
                            g_4974 -= 2;
                        } else if (a_7eaap.m7ec0 == 0)
                            g_4976 = 1;
                        else {
                            F_16381(0);
                            F_13F21(1);
                            play_ending();
                            g_0282 |= 2;
                            return;
                        }
                    }
                    if (place == 4) {
                        F_16381(0);
                        F_13F21(1);
                        bobs_jail();
                        F_120FE(-4);
                        g_0282 |= 4;
                        return;
                    }
                    if (bo7676 && m_8272.m8278 != 1) {
                        if (a_7eaap.m7ec0 == 0)
                            g_4972++;
                        else
                            g_4974++;
                    }
                    if (m_8272.m8278 != 1) {
                        g7d84o->fc++;
                        if (bo7676 == 0)
                            g7d84o->fd++;
                    }
                    if (place == 2 || place == 1) {
                        F_13F21(1);
                        g_0284 = place == 2 ? 0 : 1;
                        if (g_0284 == 1)
                            g_51ec = 1;
                        cruise_go();
                        g_51ec = 0;
                        return;
                    }
                    F_16381(0);
                    F_118D4(0x445, -1);
                    F_11593(g_49dc);
                    (*m_present)();
                    class = 4;
                    bm7f86 = 0;
                    g_6c84 = -1;
                    g_6c88 = 0;
                    n = 0;
                    if (g_4976 == 0)
                        goto out;
                    rival_arrives(0x15, 4, 0);
                    goto test;
                }
                show_rival_car(2, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
                n = 0;
                F_13F21(1);
            }
            break;
        case 3:
            if (bm7f86 == 0)
                goto out;
            if (n > 0 || g7d40q) {
                game_message(0x122c);
                show_rival_car(2, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
                n = 0;
            } else {
                pop_rival_hood();
                n++;
                g_05fc += 0x111L;
            }
            F_13F21(1);
            break;
        case 5:
            if (g_6c84 == 0)
                g_6c88 = 1;
            F_13F21(1);
            break;
        }
    }
}

/* set_jail_spots @ 0000:B045 (linear 0x0B045, 71 bytes) */

static void near set_jail_spots(int id, char c)
{
    struct Hot *h;

    h = g_20aa;
    if (id == 14)
        g_20aa[0].f4 = g_28ec - g_239e;
    else
        h->f4 = g_28f9 - g_239e;
    h[1].f0 = id;
    h[2].f0 = id;
    h[3].f0 = id;
    h[4].f0 = id;
    g_579f[37]->key = c;
}

/* bobs_jail @ 0000:B08C (linear 0x0B08C, 829 bytes) */

void bobs_jail(void)
{
    struct Bitmap far *pict;
    struct Blt blt;
    char mode;
    int i;
    int flag;
    int ch;
    int who;
    int old;

    old = 0;
    mode = 4;
    flag = 0;
    r_vid.f16 = 0;
    F_120FE(-3);
    F_193D4();
    x_82b8 = F_15BBE(0x4e7, 1);
    c_6e64 = F_15BBE(0x4e9, 1);
    F_118D4(0x445, -1);
    F_10B8C();
    pict = F_15BBE(0x4e8, 2);
    blt.w = pict->width;
    blt.h = 5;
    blt.dx = blt.dy = blt.sx = blt.sy = 0;
    (*b_78a2)(pict, r_vid.back, &blt, 1);
    blt.h = pict->height - 5;
    blt.sy = 5;
    blt.dy = 190 - blt.h;
    (*b_78a2)(pict, r_vid.back, &blt, 1);
    for (i = 0; i < 7; i++) {
        blt.w = g_5338[i].w;
        blt.h = g_5338[i].h;
        blt.sx = g_5338[i].sx;
        blt.sy = g_5338[i].sy;
        blt.dx = g_5338[i].x;
        blt.dy = g_5338[i].y;
        (*b_78a2)(*g_5338[i].src, r_vid.back, &blt, 1);
    }
    F_11421(-1, "", 0);
    F_11593(g_49dc);
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.f16 = 1;
    (*m_present)();
    bm7f86 = 0;
    g_6c84 = -1;
    g_6c88 = 0;
    F_120FE(1);
    F_120FE(-1);
    g_0284 = 6;
    set_jail_spots(14, 16);
    for (;;) {
        F_13DE6(0x25);
        if (flag == 0)
            g_05cc = (mode == 4 ? 140u : 280u) + g_tick;
        else
            g_05cc = g_tick + 5;
        flag = 0;
        who = mode == 4 ? 0 : king_chance();
        while ((ch = sys_wait_input(3000)) == 0)
            ;
        switch (ch) {
        case 1:
            F_13F21(1);
            set_jail_spots(11, 34);
            g_0284 = 1;
            return;
        case 1000:
            if (random_int(100) < 6) {
                F_120FE(-4);
                F_11421(-1, "", 0);
                play_cell_anim();
                F_120FE(-2);
            }
            rival_arrives(pick_rival(who, 1), mode, 1);
            mode = 1;
            g7f32s[0] = 0;
            F_11421(g_51ee, " ", cz8bc0y);
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            F_13F21(0);
        }
    }
}
