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
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_carhop;                         /* DS:6C84 */
static int g_ordered;                         /* DS:6C88 */
static void near drop_last_word(char *s);
static int near maybe_carhop(int mode, int arg, int pct1, int pct2);
static int g_poolCount;                         /* DS:6C86 */
static int g_poolRivals[12];                  /* DS:6C8A */
static int g_justArrived;                         /* DS:6CA2 */
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

int g_bannerX = 100;   /* DS:51EE */
int g_passCount = 0;   /* DS:51F0 */

/* maybe_carhop @ 0000:914C (linear 0x0914C, 266 bytes) */

static int near maybe_carhop(int mode, int arg, int pct1, int pct2)
{
    int r;

    r = 0;
    switch (mode) {
    case CARHOP_ENTER:
        if (g_carhop != 0)
            break;
        if (g_passCount < 3 || random_int(100) >= pct1)
            break;
        r = 1;
        cursor_ctl(CURSOR_HIDE_NOW);
        carhop_skate(CARHOP_ENTER, arg);
        g_carhop = 1;
        if (random_int(100) < pct2) {
            F_10AFD(100);
            carhop_skate(CARHOP_LEAVE, arg);
            g_carhop = 0;
            g_ordered = 0;
        }
        break;
    case CARHOP_LEAVE:
        if (g_carhop != 1 || random_int(100) >= pct1)
            break;
        r = 1;
        cursor_ctl(CURSOR_HIDE_NOW);
        carhop_skate(CARHOP_LEAVE, arg);
        g_carhop = 0;
        g_ordered = 0;
        g_passCount = 0;
        break;
    }
    if (r)
        cursor_ctl(CURSOR_SHOW_NOW);
    return r;
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Racer g_racerRecordsArray[NUM_RACERS];
struct Spec *g_oppSpecPointer;
struct Racer *g_selectedRacerPointer;
int g_rivalHereState;

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

int g_lastRival = -1;   /* DS:51F8 */

/* show_rival_car @ 0000:92A4 (linear 0x092A4, 640 bytes) */

static void near show_rival_car(int mode, int b, int c, int d, int e, int *f)
{
    int x, n, old;

    old = g_carPassBy;
    strcpy(g_strOpp, g_desc + g_selectedRacerPointer->nameOffset);
    strcat(g_strOpp, "  ");
    strcat(g_strOpp, g_desc + g_oppSpecPointer->nameOffset);
    x = g_scene == 6;
    if (mode == 4)
        g_carhop = -1;
    g_rivalHereState = mode == 2;
    if (mode == 1 && !maybe_carhop(CARHOP_ENTER, g_rivalHereState, 10 * x + 75 * g_ordered + (g_demo << 3) + 5, 10))
        maybe_carhop(CARHOP_LEAVE, g_rivalHereState, 20, 0);
    if (g_strOpp[n = strlen(g_strOpp) - 1] == '.')
        g_strOpp[n] = 0;
    for (;;) {
        g_wRivBanner = dash_text_width(g_strOpp);
        n = SCREEN_WIDTH - (g_xMoneyRhsVal + g_wRivBanner);
        n = n > 0 ? n >> 1 : 0;
        n = n < 15 ? 15 : n;
        g_bannerX = (n + g_xMoneyRhsVal) & ~7;
        if (g_bannerX + g_wRivBanner > SCREEN_WIDTH) {
            drop_last_word(g_strOpp);
            continue;
        }
        break;
    }
    F_11421(-1, "", 0);
    F_11421(g_bannerX, g_strOpp, g_wRivBanner);
    F_181C8(mode, b, g_selectedRacerPointer->bodyMods, g_selectedRacerPointer->sticker, c, d, e, f);
    if (old)
        mode = 2;
    if (mode == 2)
        g_passCount++;
    if (mode == 2) {
        g_strOpp[0] = 0;
        F_11421(g_bannerX, " ", g_wRivBanner);
    }
    if (mode == 4)
        g_carhop = 0;
    g_rivalHereState = (mode == 2) ? 0 : 1;
    if (!(x && g_rivalHereState))
        if (!maybe_carhop(CARHOP_ENTER, g_rivalHereState, 10 * x + 75 * g_ordered + (g_demo << 3) + 5, g_rivalHereState == 0 ? 8 : 0))
            maybe_carhop(CARHOP_LEAVE, g_rivalHereState, 40, 0);
}

int bn7648;
int g_isKingState;
int g_driverIdxSelVal;

/* spec_class @ 0000:9524 (linear 0x09524, 50 bytes) */

static int near spec_class(int n)
{
    int v;

    v = g7d86n[n].rank;
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
    for (k = 0; k < RACER_KING; k++)
        if (g_racerRecordsArray[k].status != RACER_EMPTY)
            any = 1;
    if (any == 0)
        return 0;
    total = g_dragWins + g_roadWins;
    if (total < 3)
        return 1;
    if (total >= 7 && g_dragWins < 6 && g_roadWins < 6)
        return 3;
    if (total < 7 && g_dragWins < 6 && g_roadWins < 6)
        return 2;
    if (g_dragWins < 6)
        return 4;
    if (g_roadWins < 6)
        return 5;
    if (total >= (g_betRaces >> 1))
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
        if (g_racerRecordsArray[RACER_KING].status == RACER_MET)
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
    struct Cand map[RACER_KING];
    int m, found, k, low;

    for (k = 0; k < RACER_KING; k++) {
        item = &g7d86n[g_racerRecordsArray[k].specIndex];
        map[k].dr = abs(perf_vmax(g_racerRecordsArray[k].specIndex) - perf_vmax(-1));
        map[k].dk = abs(g7d86n[g_racerRecordsArray[k].specIndex].rank - g7d86n[a_7eaap.currentCar->specIndex].rank);
        map[k].taken = (g_racerRecordsArray[k].status != RACER_EMPTY) ? 0 : 1;
    }
    g_poolCount = 0;
    found = 1;
    do {
        if ((flag ? RACER_KING : 9) <= g_poolCount)
            break;
        found = 0;
        low = 0x7d00;
        for (m = 0; m < RACER_KING; m++)
            if (map[m].taken == 0 && map[m].dr < low) {
                result = m;
                low = map[m].dr;
                found = 1;
            }
        if (found) {
            g_poolRivals[g_poolCount++] = result;
            map[result].taken = 1;
        }
    } while (found);
    m = 0;
    k = g_poolCount;
    for (; flag == 0 && k > 6; k--)
        if (map[k - 1].dk > 1)
            g_poolRivals[k - 1] = g_poolRivals[m++];
    if (g_demo) {
        g_poolRivals[g_poolCount++] = g_demoRacer;
        g_poolRivals[g_poolCount++] = g_demoRacer;
        if (g_justArrived) {
            g_poolRivals[(g_poolCount = 1) - 1] = g_demoRacer;
        }
        g_justArrived = 0;
    }
}

/* pick_rival @ 0000:984E (linear 0x0984E, 108 bytes) */

static int near pick_rival(int chance, int arg)
{
    int pick;

    if (random_int(100) < chance)
        return RACER_KING;
    build_rival_pool(arg);
    if (g_poolCount == 0)
        return RACER_KING;
    pick = g_poolRivals[random_int(g_poolCount)];
    if (pick == g_lastRival)
        pick = g_poolRivals[random_int(g_poolCount)];
    g_lastRival = pick;
    return pick;
}

/* select_rival @ 0000:98BA (linear 0x098BA, 365 bytes) */

static void near select_rival(int n)
{
    int skill;

    g_driverIdxSelVal = n;
    g_selectedRacerPointer = &g_racerRecordsArray[n];
    g_oppSpecPointer = &g7d86n[bn7648 = g_selectedRacerPointer->specIndex];
    if (n != RACER_KING) {
        g_isKingState = 0;
        g_selectedRacerPointer->status = RACER_MET;
        skill = (g_oppSpecPointer->rank >> 1) + 1;
        if (g_selectedRacerPointer->sticker == 0 && g_selectedRacerPointer->betRaces > 5)
            g_selectedRacerPointer->sticker = random_int(20) < skill ? random_int(9) : 0;
        else if (g_selectedRacerPointer->sticker > 0 && g_selectedRacerPointer->betRaces > 10)
            g_selectedRacerPointer->sticker = random_int(40) < skill ? random_int(9) : g_selectedRacerPointer->sticker;
        if (w_8df0[bn7648].frontBumperPic > 0 && w_8df0[bn7648].rearBumperPic > 0 && g_selectedRacerPointer->betRaces > 8)
            g_selectedRacerPointer->bodyMods |= random_int(25) < skill ? 3 : g_selectedRacerPointer->bodyMods;
        if (w_8df0[bn7648].roofPic > 0 && g_selectedRacerPointer->betRaces > 8)
            g_selectedRacerPointer->bodyMods |= random_int(30) < skill ? 4 : g_selectedRacerPointer->bodyMods;
    } else
        g_isKingState = 1;
    m_8272.bodyPic = g_oppSpecPointer->bodyPic;
    m_8272.tireVariant = (g_oppSpecPointer->parts >> 4) & 3;
    m_8272.paintScheme = g_selectedRacerPointer->paintScheme;
    F_067CF(m_8272.paintScheme);
    m_8272.overlayPic = g_selectedRacerPointer->overlayPic;
}

/* rival_arrives @ 0000:9A27 (linear 0x09A27, 92 bytes) */

static void near rival_arrives(int n, int a, int mode)
{
    select_rival(n);
    if (mode == 1)
        g_carPassBy = 1;
    if (mode == 2 && g_driverIdxSelVal == RACER_KING)
        g_carPassBy = king_status() == 1;
    show_rival_car(a, bn7648, 1, 0xa3, 0, g_whlPic[m_8272.tireVariant]);
}

/* list_met_rivals @ 0000:9A83 (linear 0x09A83, 113 bytes) */

static void near list_met_rivals(int *ids, int *slots, int *kinds)
{
    int i, n;

    if (king_status() == 0)
        g_racerRecordsArray[RACER_KING].status = RACER_MET;
    else
        g_racerRecordsArray[RACER_KING].status = RACER_NOT_MET;
    for (i = 0, n = 0; i <= NUM_RACERS - 1; i++) {
        if (g_racerRecordsArray[i].status == RACER_MET) {
            slots[n] = i;
            ids[++n] = g_racerRecordsArray[i].nameOffset;
            kinds[n] = 7;
        }
    }
    ids[0] = n;
}

int g_classBetValue;
int d_79a4;

int g_refuseMsgs[2][5] = {{4779, 4803, 4822, 4840, 4663}, {4663, 4675, 4694, 4715, 4727}};   /* DS:51FC */
int g_acceptMsgs[2][3] = {{4859, 4867, 4875}, {4743, 4756, 4767}};   /* DS:5210 */

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
        if (g_rivalHereState) {
            F_11421(-1, "", 0);
            show_rival_car(2, bn7648, 1, 0xa3, 0, g_whlPic[m_8272.tireVariant]);
        }
        choice = menu_ix();
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
    chm = g_selectedRacerPointer->f0;
    p18 = spec_class(bn7648);
    p27 = spec_class(a_7eaap.currentCar->specIndex);
    clsl = (g_oppSpecPointer->parts >> 8) & 7;
    item1 = g_part[a_7eaap.currentCar->rack.parts[0]->idx].variant;
    v11 = g_rivalStakeToleranceTable[chm][row];
    v2 = g_carRatingsTable[p18][clsl];
    v3 = g_carRatingsTable[p27][item1];
    if (v11 + v2 < v3)
        c6 = 0;
    else if (v2 > v3)
        c6 = 2;
    else
        c6 = 1;
    scorel = g_acceptProbability[col][c6];
    if ((a_7eaap.currentCar->appearance & 0x1f00) == 0)
        bonus2 = 0;
    else if (a_7eaap.currentCar->appearance & 0x100)
        bonus2 = 4;
    else
        bonus2 = 6;
    adj9 = (2 * g_selectedRacerPointer->betWins - g_selectedRacerPointer->betRaces) * scorel / 4;
    scorel += adj9 + bonus2;
    if (g_demo)
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

    marks_release();
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
            if (g_kingDragWon == 0) {
                if (a_7eaap.currentCar->fuel < 22) {
                    game_message(0x1761);
                    return 0;
                }
                a_7eaap.raceType = 0;
                game_message(0x11d1);
                g_classBetValue = 0;
            } else {
                a_7eaap.raceType = 1;
                game_message(0x11f7);
                g_classBetValue = 5;
            }
            d_79a4 = 0;
            race_run();
            g_addT += a_7eaap.raceType == 0 ? 0x444u : 0x777u;
            return 2;
        }
        marks_release();
        return 0;
    }
    if (a_7eaap.currentCar->fuel < 7) {
        game_message(0x1761);
        return 0;
    }
    pict = menu_grab(0x12, &x, &sy, ARENA_LOW_STACK);
    menu_push(0x16);
    g_waitUntil = g_tick + 800;
    while ((ch = sys_wait_input(3000)) == 0)
        ;
    switch (ch) {
    case 1:
        num = 0x12;
        a_7eaap.raceType = 0;
        break;
    case 2:
        num = 0x13;
        a_7eaap.raceType = 1;
        break;
    case -23:
    case 1000:
        game_message(0x122c);
        goto quit;
    }
    if (a_7eaap.raceType == 1 && a_7eaap.currentCar->fuel < 17) {
        game_message(0x1761);
        goto quit;
    }
    menu_pop(1);
    prev = -1;
    menu_push(num);
    for (step = 0; step < 2; step++) {
        g_waitUntil = g_tick + 800;
        while ((ch = sys_wait_input(3000)) == 0)
            ;
        m_8272.betType = ch;
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
        if ((long)price <= a_7eaap.money) {
            if (accepts_bet(kind)) {
                g_classBetValue = kind;
                d_79a4 = price;
                menu_pop(1);
                popup_show(pict, x, sy);
                pict = 0;
                game_message(g_acceptMsgs[a_7eaap.raceType][random_int(3)]);
                race_run();
                g_addT += a_7eaap.raceType == 0 ? 0x444u : 0x777u;
                return 2;
            }
            while ((ch = random_int(5)) == prev)
                ;
            game_message(g_refuseMsgs[a_7eaap.raceType][prev = ch]);
        } else
            game_message(0x131b);
    }
quit:
    menu_pop(1);
    popup_show(pict, x, sy);
    pict = 0;
    marks_release();
    return 0;
}

/* setup_bobs_spots @ 0000:A0F8 (linear 0x0A0F8, 365 bytes) */

static void near setup_bobs_spots(int a, int b)
{
    struct Hot save;
    struct Hot *hot;
    int y1, x1, y, x;

    hot = g_hotBobs;
    if (g_hotBobs[4].f6 != 4) {
        save = g_hotBobs[3];
        g_hotBobs[3] = g_hotBobs[4];
        g_hotBobs[4] = save;
    }
    hot[4].x1 = 0xc2;
    hot[4].x2 = 0xd8;
    hot[4].y1 = 0x49;
    hot[4].y2 = 0x77;
    hot[2].kind = (b == 1) ? 11 : 14;
    if (a == 0) {
        hot[1].kind = 14;
        hot[3].kind = 14;
    } else {
        x1 = ai70e4t[bn7648].r[1].x + ac825et;
        y1 = ai70e4t[bn7648].r[1].y + aw8260y;
        x = ai70e4t[bn7648].r[1].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].kind = 11;
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
        hot[3].kind = 11;
        if (hot[4].y2 >= hot[1].y1)
            hot[4].y2 = hot[1].y1 - 1;
        if (hot[4].x1 < hot[3].x1) {
            save = hot[3];
            hot[3] = hot[4];
            hot[4] = save;
        }
    }
}

struct Step g_animSteps[30] = {{2, 15}, {3, 15}, {4, 22}, {5, 15}, {6, 37}, {5, 15}, {6, 45}, {5, 15}, {4, 15}, {3, 15}, {2, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {8, 75}, {11, 15}, {12, 30}, {13, 15}, {12, 15}, {11, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {14, 15}};   /* DS:521E */
char g_animCells[16] = {1, 15, 7, 8, 9, 10, 11, 12, 2, 3, 0, 1, 5, 6, 13, 4};   /* DS:525A */
struct Cell g_endCellsA[20] = {{0, 0, 48, 24, 80, 0}, {48, 0, 32, 24, 88, 0}, {80, 0, 32, 24, 96, 0}, {112, 0, 64, 24, 88, 0}, {0, 24, 40, 44, 96, 0}, {40, 24, 40, 44, 96, 0}, {80, 24, 40, 44, 96, 0}, {120, 24, 40, 44, 96, 0}, {0, 68, 40, 44, 96, 0}, {40, 68, 40, 44, 96, 0}, {80, 68, 40, 44, 96, 0}, {120, 68, 40, 44, 96, 0}, {0, 112, 40, 44, 96, 0}, {40, 112, 40, 44, 96, 0}, {80, 112, 40, 44, 96, 0}, {120, 112, 40, 44, 96, 0}, {0, 156, 48, 44, 88, 0}, {48, 156, 56, 44, 80, 0}, {104, 156, 56, 44, 80, 0}, {160, 156, 40, 44, 96, 0}};   /* DS:526A */
struct Cell g_endCellsB[9] = {{208, 54, 8, 2, 128, 84}, {200, 54, 8, 3, 112, 88}, {184, 54, 16, 4, 96, 94}, {160, 54, 24, 10, 112, 113}, {160, 86, 40, 10, 128, 113}, {160, 65, 48, 20, 136, 104}, {160, 1, 112, 52, 128, 70}, {0, 1, 160, 97, 112, 52}, {0, 99, 224, 101, 80, 51}};   /* DS:52E2 */
int g_endPics[8] = {1201, 1217, 1106, 1107, 1108, 1109, 1073, 0};   /* DS:5318 */
int g_endPics2[3] = {6, 7, 0};   /* DS:5328 */

/* pop_rival_hood @ 0000:A265 (linear 0x0A265, 247 bytes) */

static void near pop_rival_hood(void)
{
    struct Bitmap far *pict;
    struct Car chassis;
    int n;
    struct Node nodes[5];
    struct Node n1;
    struct Node na;

    marks_release();
    pict = popup_grab(0x60, 9, 200, 143, 0);
    chassis.transmission = &n1;
    chassis.tires = &na;
    for (n = 0; n < 5; n++)
        chassis.rack.parts[n] = &nodes[n];
    game_make_car(bn7648, 0, &chassis);
    garage_hood(&chassis);
    F_11421(-1, "", 0);
    menu_push(0x18);
    g_waitUntil = g_tick + 800;
    while ((n = sys_wait_input(3000)) == 0)
        ;
    r_vid.hoodBg = 0;
    popup_show(pict, 0x60, 9);
    marks_release();
    menu_pop(1);
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
    if (g_scene == 6) {
        bl.w = 16;
        bl.h = 50;
        bl.sx = 208;
        bl.sy = 73;
        bl.dx = cell.sx + 24;
        bl.dy = cell.sy;
        (*g_videoOps.copy)(r_vid.front, bm, &bl, 1);
    }
    (*g_videoOps.copy)(bm, dst, &cell, 1);
}

struct Bitmap far *g_barJailPixels;
int g_resultWin;
struct Bitmap far *g_edgeBmpJail;

/* play_cell_anim @ 0000:A416 (linear 0x0A416, 302 bytes) */

static void near play_cell_anim(void)
{
    struct Bitmap far *pict;
    int going;
    int i, key;

    marks_release();
    pict = F_15BBE(0x4c9, ARENA_HIGH_STACK);
    i = 0;
    menu_push(0x18);
    g_waitUntil = g_tick + 10;
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
                blit_cell(pict, g_animCells[1 + g_animSteps[i].frame], r_vid.front);
                g_waitUntil = g_tick + (int)g_animSteps[i].delay;
                i++;
            }
            break;
        }
    } while (going);
    F_10AFD(1);
    blit_cell(pict, g_animCells[1 + 1], r_vid.front);
    blit_cell(pict, g_animCells[1 + 1], r_vid.back);
    menu_pop(1);
    marks_release();
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
    cursor_ctl(CURSOR_HIDE_NOW);
    screen_status_init(0xbe);
    F_119B8(9, 8, 0x14, 2);
    r_vid.copyToFront = 0;
    savedl = g_picMaxH;
    g_picMaxH = SCREEN_HEIGHT;
    (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    F_118D4(0x408, -1);
    g_picMaxH = savedl;
    (*g_videoOps.show)();
    game_pause(0x2ee);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
    g_scene = 1;
    pic15 = arena_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
    popup_grab(0x78, 0x45, 0x50, 0x2c, pic15);
    disk_free_list(g_gamePics);
    g_picsFreed = 1;
    r = disk_load_list(g_endPics, 0);
    r = disk_load_list(g_endPics2, 0);
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.copyToFront = 0;
    (*g_videoOps.show)();
    g_racerRecordsArray[RACER_KING].overlayPic = 0x4c1;
    select_rival(RACER_KING);
    F_181C8(4, bn7648, g_selectedRacerPointer->bodyMods, g_selectedRacerPointer->sticker, 1, 0x98, 0, g_whlPic[m_8272.tireVariant]);
    g_racerRecordsArray[RACER_KING].overlayPic = 0x4dc;
    pic24 = arena_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
    popup_grab(0x78, 0x45, 0x50, 0x2c, pic24);
    pic3 = F_15BBE(6, ARENA_LOW_STACK);
    pic4 = arena_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
    p3 = g_endCellsA;
    blt.w = g_endCellsA[0].w;
    blt.h = g_endCellsA[0].h;
    blt.sx = g_endCellsA[0].sx;
    blt.sy = g_endCellsA[0].sy;
    blt.dx = g_endCellsA[0].x - 0x50;
    blt.dy = 0x2c - g_endCellsA[0].h;
    (*g_videoOps.maskedBlit)(pic3, pic15, &blt, 1);
    blt.w = p3->w;
    blt.h = p3->h;
    blt.sx = p3->x - 0x50;
    blt.sy = 0x2c - p3->h;
    blt.dx = blt.sx;
    blt.dy = blt.sy;
    (*g_videoOps.copy)(pic15, pic24, &blt, 1);
    game_pause(10);
    for (i2 = 1; i2 < 0x14; i2++) {
        if (i2 == 13)
            F_10AFD(0x14);
        p3 = &g_endCellsA[i2];
        bitmap_copy(pic24, pic4, 1);
        blt.w = p3->w;
        blt.h = p3->h;
        blt.sx = p3->sx;
        blt.sy = p3->sy;
        blt.dx = p3->x - 0x50;
        blt.dy = 0x2c - p3->h;
        (*g_videoOps.maskedBlit)(pic3, pic4, &blt, 1);
        blt.w = 0x50;
        blt.h = 0x2c;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0x78;
        blt.dy = 0x45;
        if (i2 < 0x13) {
            F_10AFD(10);
            (*g_videoOps.copy)(pic4, r_vid.front, &blt, 1);
        }
        if (i2 == 0x12)
            (*g_videoOps.copy)(pic4, r_vid.back, &blt, 1);
    }
    marks_clear();
    if (flag8 == 0) {
        pic58 = F_15BBE(7, ARENA_LOW_STACK);
        cntz = 9;
        if (r_vid.mode == VID_TANDY) {
            arena_shrink(10000);
            cntz--;
        }
        v32 = v38l = 0x28;
        v2a = v30 = -0x28;
        for (i2 = 0; i2 < cntz; i2++) {
            p2m = &g_endCellsB[i2];
            w8 = max((int)p2m->w + (int)p2m->x, v32 + v2a) - (minx = min((int)p2m->x, v32));
            h8 = max((int)p2m->h + (int)p2m->y, v38l + v2a);
            h8 -= miny = min((int)p2m->y, v38l);
            pic6m = arena_bm(w8, h8, 0, ARENA_HIGH_STACK);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = minx;
            blt2k.sy = miny;
            blt2k.dx = 0;
            blt2k.dy = 0;
            (*g_videoOps.copy)(r_vid.back, pic6m, &blt2k, 1);
            blt2k.w = v2a = p2m->w;
            blt2k.h = v30 = p2m->h;
            blt2k.sx = p2m->sx;
            blt2k.sy = p2m->sy;
            blt2k.dx = (v32 = p2m->x) - minx;
            blt2k.dy = (v38l = p2m->y) - miny;
            (*g_videoOps.maskedBlit)(pic58, pic6m, &blt2k, 1);
            blt2k.w = w8;
            blt2k.h = h8;
            blt2k.sx = 0;
            blt2k.sy = 0;
            blt2k.dx = minx;
            blt2k.dy = miny;
            F_10AFD(8);
            (*g_videoOps.copy)(pic6m, r_vid.front, &blt2k, 1);
            F_19438(1);
        }
    }
    blt2k.sx = minx;
    blt2k.sy = miny;
    game_pause(0x50);
    F_10AFD(1);
    (*g_videoOps.copy)(r_vid.back, r_vid.front, &blt2k, 1);
    F_10AFD(1);
    (*g_videoOps.copy)(pic4, r_vid.front, &blt, 1);
    (*g_videoOps.copy)(pic4, r_vid.back, &blt, 1);
    game_pause(0x32);
    game_message(0x183c);
    r_vid.copyToFront = 0;
    arena_reset();
    disk_free_list(g_endPics2);
    disk_free_list(g_endPics);
}

struct Btn g_jailBars[7] = {{&g_barJailPixels, 0, 0, 16, 149, 40, 5}, {&g_barJailPixels, 0, 0, 16, 149, 96, 5}, {&g_barJailPixels, 0, 0, 16, 149, 152, 5}, {&g_barJailPixels, 0, 0, 16, 149, 208, 5}, {&g_barJailPixels, 0, 0, 16, 149, 264, 5}, {&g_edgeBmpJail, 0, 0, 32, 149, 0, 5}, {&g_edgeBmpJail, 32, 0, 32, 149, 288, 5}};   /* DS:5338 */
struct Pair g_jailGaps[6] = {{32, 8}, {56, 40}, {112, 40}, {168, 40}, {224, 40}, {280, 8}};   /* DS:539A */

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
    g_cursorX = 0x9e;
    g_cursorY = 0x80;
    F_118D4(0x445, -1);
    video_bobs_pal();
    F_11593(g_49dc);
    g_cursor2.drawnRect.x2 = -1;
    cursor_ctl(1);
    (*g_videoOps.show)();
    F_16381(0);
    if (random_int(100) < 25)
        play_cell_anim();
    cursor_ctl(CURSOR_SHOW_NOW);
    g_rivalHereState = 0;
    g_carhop = -1;
    g_ordered = 0;
    g_justArrived = 1;
    g_scene = 2;
    g_mouse.left = g_joyButtons = g_buttons = 0;
    for (;;) {
        setup_bobs_spots(g_rivalHereState, !g_carhop & !g_ordered);
        menu_push(0x25);
        g_waitUntil = flag == 0 ? (unsigned)(class == 4 ? 140 : 280) + g_tick : g_tick + 5;
        flag = 0;
        rival = class == 4 ? 0 : king_chance();
        F_06529(0);
        if (g_end)
            return;
        while ((choice = sys_wait_input(3000)) == 0)
            ;
        switch (choice) {
        case 1:
            g_strOpp[0] = 0;
            F_11421(g_bannerX, " ", g_wRivBanner);
            menu_pop(1);
            g_scene = 1;
            cruise_go();
            return;
        case 1000:
            F_06529(0);
            if (g_end)
                return;
            if (g_rivalHereState == 0)
                goto race;
            F_11421(-1, "", 0);
            show_rival_car(2, bn7648, 1, 0xa3, 0, g_whlPic[m_8272.tireVariant]);
            n = 0;
            menu_pop(1);
            break;
        default:
out:
            menu_pop(1);
            break;
        case 4:
            if ((status = look_for_rival(class)) == 0)
                flag = 1;
            else
                n = 0;
            menu_pop(1);
            break;
        case 2:
            if (g_rivalHereState == 0) {
                g_waitUntil = g_tick;
race:
                rival_arrives(pick_rival(rival, 0), class, 2);
                class = 1;
                menu_pop(1);
            } else {
                g_statusStrShown = 1;
                F_11421(-1, "", 0);
                F_11421(g_bannerX, "", g_wRivBanner);
test:
                while ((F_11421(-1, "", 0), challenge(g_isKingState))) {
                    g_strOpp[0] = 0;
                    F_11421(g_bannerX, " ", g_wRivBanner);
                    place = race_outcome();
                    (*g_videoOps.blank)();
                    cruise_rival_paint(-1);
                    if (m_8272.betType != 1)
                        g_betRaces++;
                    if (g_isKingState) {
                        if (g_resultWin == 0) {
                            g_kingDragWon = 0;
                            g_racerRecordsArray[RACER_KING].status = RACER_NOT_MET;
                            g_dragWins -= 3;
                            g_roadWins -= 2;
                        } else if (a_7eaap.raceType == 0)
                            g_kingDragWon = 1;
                        else {
                            F_16381(0);
                            menu_pop(1);
                            play_ending();
                            g_end |= 2;
                            return;
                        }
                    }
                    if (place == 4) {
                        F_16381(0);
                        menu_pop(1);
                        bobs_jail();
                        cursor_ctl(CURSOR_HIDE_NOW);
                        g_end |= 4;
                        return;
                    }
                    if (g_resultWin && m_8272.betType != 1) {
                        if (a_7eaap.raceType == 0)
                            g_dragWins++;
                        else
                            g_roadWins++;
                    }
                    if (m_8272.betType != 1) {
                        g_selectedRacerPointer->betRaces++;
                        if (g_resultWin == 0)
                            g_selectedRacerPointer->betWins++;
                    }
                    if (place == 2 || place == 1) {
                        menu_pop(1);
                        g_scene = place == 2 ? 0 : 1;
                        if (g_scene == 1)
                            g_keepCruiseMusic = 1;
                        cruise_go();
                        g_keepCruiseMusic = 0;
                        return;
                    }
                    F_16381(0);
                    F_118D4(0x445, -1);
                    F_11593(g_49dc);
                    (*g_videoOps.show)();
                    class = 4;
                    g_rivalHereState = 0;
                    g_carhop = -1;
                    g_ordered = 0;
                    n = 0;
                    if (g_kingDragWon == 0)
                        goto out;
                    rival_arrives(RACER_KING, 4, 0);
                    goto test;
                }
                show_rival_car(2, bn7648, 1, 0xa3, 0, g_whlPic[m_8272.tireVariant]);
                n = 0;
                menu_pop(1);
            }
            break;
        case 3:
            if (g_rivalHereState == 0)
                goto out;
            if (n > 0 || g_isKingState) {
                game_message(0x122c);
                show_rival_car(2, bn7648, 1, 0xa3, 0, g_whlPic[m_8272.tireVariant]);
                n = 0;
            } else {
                pop_rival_hood();
                n++;
                g_addT += 0x111L;
            }
            menu_pop(1);
            break;
        case 5:
            if (g_carhop == 0)
                g_ordered = 1;
            menu_pop(1);
            break;
        }
    }
}

/* set_jail_spots @ 0000:B045 (linear 0x0B045, 71 bytes) */

static void near set_jail_spots(int id, char c)
{
    struct Hot *h;

    h = g_hotBobs;
    if (id == 14)
        g_hotBobs[0].textOffset = g_msgWannaQuit - g_txt;
    else
        h->textOffset = g_msgBackToGarage - g_txt;
    h[1].kind = id;
    h[2].kind = id;
    h[3].kind = id;
    h[4].kind = id;
    g_keyMaps[37]->key = c;
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
    r_vid.copyToFront = 0;
    cursor_ctl(CURSOR_HIDE);
    marks_clear();
    g_edgeBmpJail = F_15BBE(0x4e7, ARENA_LOW_STACK);
    g_barJailPixels = F_15BBE(0x4e9, ARENA_LOW_STACK);
    F_118D4(0x445, -1);
    video_bobs_pal();
    pict = F_15BBE(0x4e8, ARENA_HIGH_STACK);
    blt.w = pict->width;
    blt.h = 5;
    blt.dx = blt.dy = blt.sx = blt.sy = 0;
    (*g_videoOps.copy)(pict, r_vid.back, &blt, 1);
    blt.h = pict->height - 5;
    blt.sy = 5;
    blt.dy = 190 - blt.h;
    (*g_videoOps.copy)(pict, r_vid.back, &blt, 1);
    for (i = 0; i < 7; i++) {
        blt.w = g_jailBars[i].w;
        blt.h = g_jailBars[i].h;
        blt.sx = g_jailBars[i].sx;
        blt.sy = g_jailBars[i].sy;
        blt.dx = g_jailBars[i].x;
        blt.dy = g_jailBars[i].y;
        (*g_videoOps.copy)(*g_jailBars[i].src, r_vid.back, &blt, 1);
    }
    F_11421(-1, "", 0);
    F_11593(g_49dc);
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.copyToFront = 1;
    (*g_videoOps.show)();
    g_rivalHereState = 0;
    g_carhop = -1;
    g_ordered = 0;
    cursor_ctl(1);
    cursor_ctl(CURSOR_SHOW);
    g_scene = 6;
    set_jail_spots(14, 16);
    for (;;) {
        menu_push(0x25);
        if (flag == 0)
            g_waitUntil = (mode == 4 ? 140u : 280u) + g_tick;
        else
            g_waitUntil = g_tick + 5;
        flag = 0;
        who = mode == 4 ? 0 : king_chance();
        while ((ch = sys_wait_input(3000)) == 0)
            ;
        switch (ch) {
        case 1:
            menu_pop(1);
            set_jail_spots(11, 34);
            g_scene = 1;
            return;
        case 1000:
            if (random_int(100) < 6) {
                cursor_ctl(CURSOR_HIDE_NOW);
                F_11421(-1, "", 0);
                play_cell_anim();
                cursor_ctl(CURSOR_SHOW_NOW);
            }
            rival_arrives(pick_rival(who, 1), mode, 1);
            mode = 1;
            g_strOpp[0] = 0;
            F_11421(g_bannerX, " ", g_wRivBanner);
        case 2:
        case 3:
        case 4:
        case 5:
        default:
            menu_pop(0);
        }
    }
}
