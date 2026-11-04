#include <string.h>
#include <stdlib.h>
#include "BOBS.H"
#include "ADS.H"
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
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

enum CarhopState {
    CARHOP_DISABLED = -1,
    CARHOP_ABSENT = 0,
    CARHOP_PRESENT = 1
};

enum RivalArrival {
    RIVAL_REQUESTED = 0,
    RIVAL_PASS_BY = 1,
    RIVAL_CHALLENGE = 2
};

enum KingStatus {
    KING_READY = 0,
    KING_NOVICE = 1,
    KING_NEED_WINS = 2,
    KING_NEED_WIN_RATE = 3,
    KING_NEED_DRAG_WINS = 4,
    KING_NEED_ROAD_WINS = 5
};

static enum CarhopState g_carhop;
static int g_ordered;
static void near drop_last_word(char *s);
static int near maybe_carhop(enum CarhopMode mode, int arg, int pct1, int pct2);
static int g_poolCount;
static int g_poolRivals[12];
static int g_justArrived;
static void near build_rival_pool(int);
static void near select_rival(int);
static enum KingStatus near king_status(void);
static void near show_rival_car(enum CarAnimKind, int, int, int, int, int *);
static void near rival_arrives(int, enum CarAnimKind, enum RivalArrival);
static void near list_met_rivals(int *, int *, int *);
static int near spec_class(int);
static int near accepts_bet(enum BetKind kind);
static void near blit_cell(struct Bitmap far *, int, struct Bitmap far *);
static void near setup_bobs_spots(int, int);
static int near king_chance(void);
static int near look_for_rival(enum CarAnimKind);
static int near pick_rival(int, int);
static void near play_ending(void);
static void near pop_rival_hood(void);
static int near challenge(int);
static void near play_cell_anim(void);
static void near set_jail_spots(enum HotKind id, char c);

int g_bannerX = 100;
int g_passCount = 0;


static int near maybe_carhop(enum CarhopMode mode, int arg, int pct1, int pct2)
{
    int r;

    r = 0;
    switch (mode) {
    case CARHOP_ENTER:
        if (g_carhop != CARHOP_ABSENT)
            break;
        if (g_passCount < 3 || rn_int(100) >= pct1)
            break;
        r = 1;
        cu_ctl(CURSOR_HIDE_NOW);
        ch_skate(CARHOP_ENTER, arg);
        g_carhop = CARHOP_PRESENT;
        if (rn_int(100) < pct2) {
            da_retrace(100);
            ch_skate(CARHOP_LEAVE, arg);
            g_carhop = CARHOP_ABSENT;
            g_ordered = 0;
        }
        break;
    case CARHOP_LEAVE:
        if (g_carhop != CARHOP_PRESENT || rn_int(100) >= pct1)
            break;
        r = 1;
        cu_ctl(CURSOR_HIDE_NOW);
        ch_skate(CARHOP_LEAVE, arg);
        g_carhop = CARHOP_ABSENT;
        g_ordered = 0;
        g_passCount = 0;
        break;
    }
    if (r)
        cu_ctl(CURSOR_SHOW_NOW);
    return r;
}

struct Racer g_racerRecordsArray[NUM_RACERS];
struct Spec *g_oppSpecPointer;
struct Racer *g_selectedRacerPointer;
int g_rivalHereState;


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

int g_lastRival = -1;


static void near show_rival_car(enum CarAnimKind mode, int b, int c, int d, int e, int *f)
{
    int x, n, old;

    old = g_carPassBy;
    strcpy(g_strOpp, g_desc + g_selectedRacerPointer->nameOffset);
    strcat(g_strOpp, "  ");
    strcat(g_strOpp, g_desc + g_oppSpecPointer->nameOffset);
    x = g_scene == SCENE_JAIL;
    if (mode == CAR_PULL_IN)
        g_carhop = CARHOP_DISABLED;
    g_rivalHereState = mode == CAR_DRIVE_OUT;
    if (mode == CAR_DRIVE_IN &&
        !maybe_carhop(CARHOP_ENTER, g_rivalHereState, 10 * x + 75 * g_ordered + (g_demo << 3) + 5, 10))
        maybe_carhop(CARHOP_LEAVE, g_rivalHereState, 20, 0);
    if (g_strOpp[n = strlen(g_strOpp) - 1] == '.')
        g_strOpp[n] = 0;
    for (;;) {
        g_wRivBanner = da_text_width(g_strOpp);
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
    sc_status_text(-1, "", 0);
    sc_status_text(g_bannerX, g_strOpp, g_wRivBanner);
    cp_show_car(mode, b, g_selectedRacerPointer->bodyMods, g_selectedRacerPointer->sticker, c, d, e, f);
    if (old)
        mode = CAR_DRIVE_OUT;
    if (mode == CAR_DRIVE_OUT)
        g_passCount++;
    if (mode == CAR_DRIVE_OUT) {
        g_strOpp[0] = 0;
        sc_status_text(g_bannerX, " ", g_wRivBanner);
    }
    if (mode == CAR_PULL_IN)
        g_carhop = CARHOP_ABSENT;
    g_rivalHereState = (mode == CAR_DRIVE_OUT) ? 0 : 1;
    if (!(x && g_rivalHereState))
        if (!maybe_carhop(CARHOP_ENTER, g_rivalHereState, 10 * x + 75 * g_ordered + (g_demo << 3) + 5,
                          g_rivalHereState == 0 ? 8 : 0))
            maybe_carhop(CARHOP_LEAVE, g_rivalHereState, 40, 0);
}

int g_opponentSpecIndex;
int g_isKingState;
int g_driverIdxSelVal;


static int near spec_class(int n)
{
    int v;

    v = g_carData[n].rank;
    if (v < 3)
        return 0;
    if (v >= 7)
        return 2;
    return 1;
}


static enum KingStatus near king_status(void)
{
    int k, any, total;

    any = 0;
    for (k = 0; k < RACER_KING; k++)
        if (g_racerRecordsArray[k].status != RACER_EMPTY)
            any = 1;
    if (any == 0)
        return KING_READY;
    total = g_dragWins + g_roadWins;
    if (total < 3)
        return KING_NOVICE;
    if (total >= 7 && g_dragWins < 6 && g_roadWins < 6)
        return KING_NEED_WIN_RATE;
    if (total < 7 && g_dragWins < 6 && g_roadWins < 6)
        return KING_NEED_WINS;
    if (g_dragWins < 6)
        return KING_NEED_DRAG_WINS;
    if (g_roadWins < 6)
        return KING_NEED_ROAD_WINS;
    if (total >= (g_betRaces >> 1))
        return KING_READY;
    return KING_NEED_WIN_RATE;
}


static int near king_chance(void)
{
    enum KingStatus kind;
    int v;

    if (g_opponentSpecIndex == 25)
        return 0;
    kind = king_status();
    if (kind) {
        if (kind == KING_NOVICE)
            v = 5;
        else if (kind == KING_NEED_WINS)
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


static void near build_rival_pool(int flag)
{
    int result;
    struct Spec *item;
    struct Cand map[RACER_KING];
    int m, found, k, low;

    for (k = 0; k < RACER_KING; k++) {
        item = &g_carData[g_racerRecordsArray[k].specIndex];
        map[k].dr = abs(pf_vmax(g_racerRecordsArray[k].specIndex) - pf_vmax(-1));
        map[k].dk =
            abs(g_carData[g_racerRecordsArray[k].specIndex].rank - g_carData[g_driver.currentCar->specIndex].rank);
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


static int near pick_rival(int chance, int arg)
{
    int pick;

    if (rn_int(100) < chance)
        return RACER_KING;
    build_rival_pool(arg);
    if (g_poolCount == 0)
        return RACER_KING;
    pick = g_poolRivals[rn_int(g_poolCount)];
    if (pick == g_lastRival)
        pick = g_poolRivals[rn_int(g_poolCount)];
    g_lastRival = pick;
    return pick;
}


static void near select_rival(int n)
{
    int skill;

    g_driverIdxSelVal = n;
    g_selectedRacerPointer = &g_racerRecordsArray[n];
    g_oppSpecPointer = &g_carData[g_opponentSpecIndex = g_selectedRacerPointer->specIndex];
    if (n != RACER_KING) {
        g_isKingState = 0;
        g_selectedRacerPointer->status = RACER_MET;
        skill = (g_oppSpecPointer->rank >> 1) + 1;
        if (g_selectedRacerPointer->sticker == 0 && g_selectedRacerPointer->betRaces > 5)
            g_selectedRacerPointer->sticker = rn_int(20) < skill ? rn_int(9) : 0;
        else if (g_selectedRacerPointer->sticker > 0 && g_selectedRacerPointer->betRaces > 10)
            g_selectedRacerPointer->sticker = rn_int(40) < skill ? rn_int(9) : g_selectedRacerPointer->sticker;
        if (g_models[g_opponentSpecIndex].frontBumperPic > 0 && g_models[g_opponentSpecIndex].rearBumperPic > 0 &&
            g_selectedRacerPointer->betRaces > 8)
            g_selectedRacerPointer->bodyMods |= rn_int(25) < skill ? 3 : g_selectedRacerPointer->bodyMods;
        if (g_models[g_opponentSpecIndex].roofPic > 0 && g_selectedRacerPointer->betRaces > 8)
            g_selectedRacerPointer->bodyMods |= rn_int(30) < skill ? 4 : g_selectedRacerPointer->bodyMods;
    } else
        g_isKingState = 1;
    g_carDraw.bodyPic = g_oppSpecPointer->bodyPic;
    g_carDraw.tireVariant = (g_oppSpecPointer->parts >> 4) & 3;
    g_carDraw.paintScheme = g_selectedRacerPointer->paintScheme;
    ga_palette(g_carDraw.paintScheme);
    g_carDraw.overlayPic = g_selectedRacerPointer->overlayPic;
}


static void near rival_arrives(int n, enum CarAnimKind a, enum RivalArrival mode)
{
    select_rival(n);
    if (mode == RIVAL_PASS_BY)
        g_carPassBy = 1;
    if (mode == RIVAL_CHALLENGE && g_driverIdxSelVal == RACER_KING)
        g_carPassBy = king_status() == KING_NOVICE;
    show_rival_car(a, g_opponentSpecIndex, 1, 0xa3, 0, g_tirePics[g_carDraw.tireVariant]);
}


static void near list_met_rivals(int *ids, int *slots, int *kinds)
{
    int i, n;

    if (king_status() == KING_READY)
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

struct Wager g_raceWagerTerms;

int g_refuseMsgs[2][5] = {{4779, 4803, 4822, 4840, 4663}, {4663, 4675, 4694, 4715, 4727}};
int g_acceptMsgs[2][3] = {{4859, 4867, 4875}, {4743, 4756, 4767}};


static int near look_for_rival(enum CarAnimKind n)
{
    int list[24], sel[24], kinds[24], choice;

    sc_status_text(-1, "", 0);
    list_met_rivals(list, sel, kinds);
    choice = mn_modal_list(KM_CALL_RIVAL, 1, kinds, list, 0, 0);
    switch (choice) {
    case KR_CANCEL:
        return 0;
    case KR_ACCEPT:
        if (g_rivalHereState) {
            sc_status_text(-1, "", 0);
            show_rival_car(CAR_DRIVE_OUT, g_opponentSpecIndex, 1, 0xa3, 0, g_tirePics[g_carDraw.tireVariant]);
        }
        choice = mn_sel();
        rival_arrives(sel[choice - 1], n, RIVAL_REQUESTED);
        return 1;
    case KR_EMPTY_LIST:
        gm_message(0x198c);
        return 0;
    }
}


static int near accepts_bet(enum BetKind kind)
{
    int item1;
    unsigned v11, v2;
    int adj9, chm, scorel, row, p27, p18, c6, bonus2;
    unsigned v3;
    int clsl, col;

    if (kind == BET_NONE) {
        row = 0;
        col = 0;
    } else {
        if (kind == BET_CAR) {
            row = 2;
            col = 3;
        } else if (kind == BET_10 || kind == BET_50) {
            row = 1;
            col = 1;
        } else if (kind == BET_25 || kind == BET_100) {
            row = 1;
            col = 2;
        }
        goto calc;
    }
calc:
    chm = g_selectedRacerPointer->f0;
    p18 = spec_class(g_opponentSpecIndex);
    p27 = spec_class(g_driver.currentCar->specIndex);
    clsl = (g_oppSpecPointer->parts >> 8) & 7;
    item1 = g_parts[g_driver.currentCar->rack.parts[0]->idx].variant;
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
    if ((g_driver.currentCar->appearance & 0x1f00) == 0)
        bonus2 = 0;
    else if (g_driver.currentCar->appearance & 0x100)
        bonus2 = 4;
    else
        bonus2 = 6;
    adj9 = (2 * g_selectedRacerPointer->betWins - g_selectedRacerPointer->betRaces) * scorel / 4;
    scorel += adj9 + bonus2;
    if (g_demo)
        scorel += (scorel >> 2) + 10;
    return rn_int(100) <= scorel;
}


static int near challenge(int arg)
{
    int num;
    struct Bitmap far *pict;
    unsigned price;
    enum BetKind kind;
    int x;
    int ch;
    int step;
    int sy;
    int prev;

    ma_release();
    if (arg) {
        ch = king_status();
        switch (ch) {
        case KING_NEED_WINS:
            gm_message(0x1113);
            break;
        case KING_NEED_WIN_RATE:
            gm_message(0x113d);
            break;
        case KING_NEED_DRAG_WINS:
            gm_message(0x117d);
            break;
        case KING_NEED_ROAD_WINS:
            gm_message(0x11a7);
            break;
        case KING_READY:
            if (g_kingDragWon == 0) {
                if (g_driver.currentCar->fuel < 22) {
                    gm_message(0x1761);
                    return 0;
                }
                g_driver.raceType = RACE_DRAG;
                gm_message(0x11d1);
                g_raceWagerTerms.kind = BET_NONE;
            } else {
                g_driver.raceType = RACE_ROAD;
                gm_message(0x11f7);
                g_raceWagerTerms.kind = BET_CAR;
            }
            g_raceWagerTerms.amount = 0;
            ra_run();
            g_gameClock.actionTicks += g_driver.raceType == RACE_DRAG ? 0x444u : 0x777u;
            return 2;
        }
        ma_release();
        return 0;
    }
    if (g_driver.currentCar->fuel < 7) {
        gm_message(0x1761);
        return 0;
    }
    pict = mn_grab(KM_DRAG_WAGER, &x, &sy, ARENA_LOW_STACK);
    mn_push(KM_RACE_TYPE);
    g_waitUntil = g_tick + 800;
    while ((ch = sy_wait_input(3000)) == 0)
        ;
    switch (ch) {
    case KR_RACE_DRAG:
        num = KM_DRAG_WAGER;
        g_driver.raceType = RACE_DRAG;
        break;
    case KR_RACE_ROAD:
        num = KM_ROAD_WAGER;
        g_driver.raceType = RACE_ROAD;
        break;
    case KR_FORGET:
    case KR_TIMEOUT:
        gm_message(0x122c);
        goto quit;
    }
    if (g_driver.raceType == RACE_ROAD && g_driver.currentCar->fuel < 17) {
        gm_message(0x1761);
        goto quit;
    }
    mn_pop(1);
    prev = -1;
    mn_push(num);
    for (step = 0; step < 2; step++) {
        g_waitUntil = g_tick + 800;
        while ((ch = sy_wait_input(3000)) == 0)
            ;
        g_carDraw.betType = ch;
        switch (ch) {
        case KR_FORGET:
        case KR_TIMEOUT:
            gm_message(0x122c);
            goto quit;
        case KR_BET_FOR_KICKS:
            price = 0;
            kind = BET_NONE;
            break;
        case KR_BET_10:
            price = 10;
            kind = BET_10;
            break;
        case KR_BET_50:
            price = 50;
            kind = BET_50;
            break;
        case KR_BET_25:
            price = 25;
            kind = BET_25;
            break;
        case KR_BET_100:
            price = 100;
            kind = BET_100;
            break;
        case KR_BET_PINK_SLIPS:
            price = 0;
            kind = BET_CAR;
            break;
        }
        if ((long)price <= g_driver.money) {
            if (accepts_bet(kind)) {
                g_raceWagerTerms.kind = kind;
                g_raceWagerTerms.amount = price;
                mn_pop(1);
                pu_show(pict, x, sy);
                pict = 0;
                gm_message(g_acceptMsgs[g_driver.raceType][rn_int(3)]);
                ra_run();
                g_gameClock.actionTicks += g_driver.raceType == RACE_DRAG ? 0x444u : 0x777u;
                return 2;
            }
            while ((ch = rn_int(5)) == prev)
                ;
            gm_message(g_refuseMsgs[g_driver.raceType][prev = ch]);
        } else
            gm_message(0x131b);
    }
quit:
    mn_pop(1);
    pu_show(pict, x, sy);
    pict = 0;
    ma_release();
    return 0;
}


static void near setup_bobs_spots(int a, int b)
{
    struct Hot save;
    struct Hot *hot;
    int y1, x1, y, x;

    hot = g_hotBobs;
    if (g_hotBobs[4].id != 4) {
        save = g_hotBobs[3];
        g_hotBobs[3] = g_hotBobs[4];
        g_hotBobs[4] = save;
    }
    hot[4].x1 = 0xc2;
    hot[4].x2 = 0xd8;
    hot[4].y1 = 0x49;
    hot[4].y2 = 0x77;
    hot[2].kind = (b == 1) ? HOT_REGION : HOT_DISABLED;
    if (a == 0) {
        hot[1].kind = HOT_DISABLED;
        hot[3].kind = HOT_DISABLED;
    } else {
        x1 = g_bodyRects[g_opponentSpecIndex].r[1].x + g_picXDraw;
        y1 = g_bodyRects[g_opponentSpecIndex].r[1].y + g_sprDrawY;
        x = g_bodyRects[g_opponentSpecIndex].r[1].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].kind = HOT_REGION;
        x1 = g_bodyRects[g_opponentSpecIndex].r[3].x + g_picXDraw;
        y1 = g_bodyRects[g_opponentSpecIndex].r[3].y + g_sprDrawY;
        x = g_bodyRects[g_opponentSpecIndex].r[3].w + x1 - 1;
        y = g_bodyRects[g_opponentSpecIndex].r[3].h + y1 - 1;
        if (hot[1].x2 >= x1)
            x1 = hot[1].x2 + 1;
        hot[3].x1 = x1;
        hot[3].x2 = x;
        hot[3].y1 = y1;
        hot[3].y2 = y;
        hot[3].kind = HOT_REGION;
        if (hot[4].y2 >= hot[1].y1)
            hot[4].y2 = hot[1].y1 - 1;
        if (hot[4].x1 < hot[3].x1) {
            save = hot[3];
            hot[3] = hot[4];
            hot[4] = save;
        }
    }
}

struct Step g_animSteps[30] = {{2, 15}, {3, 15}, {4, 22},  {5, 15},  {6, 37},  {5, 15},  {6, 45},  {5, 15},
                               {4, 15}, {3, 15}, {2, 15},  {8, 90},  {7, 15},  {9, 15},  {10, 30}, {9, 15},
                               {7, 7},  {8, 75}, {11, 15}, {12, 30}, {13, 15}, {12, 15}, {11, 15}, {8, 90},
                               {7, 15}, {9, 15}, {10, 30}, {9, 15},  {7, 7},   {14, 15}};
char g_animCells[16] = {1, 15, 7, 8, 9, 10, 11, 12, 2, 3, 0, 1, 5, 6, 13, 4};
struct Cell g_endCellsA[20] = {
    {0, 0, 48, 24, 80, 0},   {48, 0, 32, 24, 88, 0},   {80, 0, 32, 24, 96, 0},    {112, 0, 64, 24, 88, 0},
    {0, 24, 40, 44, 96, 0},  {40, 24, 40, 44, 96, 0},  {80, 24, 40, 44, 96, 0},   {120, 24, 40, 44, 96, 0},
    {0, 68, 40, 44, 96, 0},  {40, 68, 40, 44, 96, 0},  {80, 68, 40, 44, 96, 0},   {120, 68, 40, 44, 96, 0},
    {0, 112, 40, 44, 96, 0}, {40, 112, 40, 44, 96, 0}, {80, 112, 40, 44, 96, 0},  {120, 112, 40, 44, 96, 0},
    {0, 156, 48, 44, 88, 0}, {48, 156, 56, 44, 80, 0}, {104, 156, 56, 44, 80, 0}, {160, 156, 40, 44, 96, 0}};
struct Cell g_endCellsB[9] = {{208, 54, 8, 2, 128, 84},    {200, 54, 8, 3, 112, 88},    {184, 54, 16, 4, 96, 94},
                              {160, 54, 24, 10, 112, 113}, {160, 86, 40, 10, 128, 113}, {160, 65, 48, 20, 136, 104},
                              {160, 1, 112, 52, 128, 70},  {0, 1, 160, 97, 112, 52},    {0, 99, 224, 101, 80, 51}};
int g_endPics[8] = {1201, 1217, 1106, 1107, 1108, 1109, 1073, 0};
int g_endPics2[3] = {6, 7, 0};


static void near pop_rival_hood(void)
{
    struct Bitmap far *pict;
    struct Car chassis;
    int n;
    struct Node nodes[5];
    struct Node n1;
    struct Node na;

    ma_release();
    pict = pu_grab(0x60, 9, 200, 143, 0);
    chassis.transmission = &n1;
    chassis.tires = &na;
    for (n = 0; n < 5; n++)
        chassis.rack.parts[n] = &nodes[n];
    gm_make_car(g_opponentSpecIndex, 0, &chassis);
    ga_hood(&chassis);
    sc_status_text(-1, "", 0);
    mn_push(KM_MESSAGE);
    g_waitUntil = g_tick + 800;
    while ((n = sy_wait_input(3000)) == 0)
        ;
    r_vid.hoodBg = 0;
    pu_show(pict, 0x60, 9);
    ma_release();
    mn_pop(1);
}

/* Blit sprite-sheet cell n (7 per row) of bm into dst */

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
    if (g_scene == SCENE_JAIL) {
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


static void near play_cell_anim(void)
{
    struct Bitmap far *pict;
    int going;
    int i, key;

    ma_release();
    pict = di_bm(0x4c9, ARENA_HIGH_STACK);
    i = 0;
    mn_push(KM_MESSAGE);
    g_waitUntil = g_tick + 10;
    going = 1;
    do {
        while ((key = sy_wait_input(3000)) == 0)
            ;
        switch (key) {
        case KR_MESSAGE_DISMISS:
            going = 0;
            break;
        case KR_TIMEOUT:
            if (i >= 31)
                going = 0;
            else {
                da_retrace(1);
                blit_cell(pict, g_animCells[1 + g_animSteps[i].frame], r_vid.front);
                g_waitUntil = g_tick + (int)g_animSteps[i].delay;
                i++;
            }
            break;
        }
    } while (going);
    da_retrace(1);
    blit_cell(pict, g_animCells[1 + 1], r_vid.front);
    blit_cell(pict, g_animCells[1 + 1], r_vid.back);
    mn_pop(1);
    ma_release();
}


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
    cu_ctl(CURSOR_HIDE_NOW);
    sc_status_init(0xbe);
    sc_picture_at(9, 8, 0x14, 2);
    r_vid.copyToFront = 0;
    savedl = g_picturePlacement.maxHeight;
    g_picturePlacement.maxHeight = SCREEN_HEIGHT;
    (*g_videoOps.copyPage)(r_vid.front->bits.p, r_vid.back->bits.p);
    sc_picture(0x408, -1);
    g_picturePlacement.maxHeight = savedl;
    (*g_videoOps.show)();
    gm_pause(0x2ee);
    (*g_videoOps.reveal)(r_vid.front->bits.p, r_vid.back->bits.p);
    g_scene = SCENE_GARAGE;
    pic15 = ar_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
    pu_grab(0x78, 0x45, 0x50, 0x2c, pic15);
    di_free_list(g_gamePics);
    g_picsFreed = 1;
    r = di_load_list(g_endPics, 0);
    r = di_load_list(g_endPics2, 0);
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.copyToFront = 0;
    (*g_videoOps.show)();
    g_racerRecordsArray[RACER_KING].overlayPic = 0x4c1;
    select_rival(RACER_KING);
    cp_show_car(CAR_PULL_IN, g_opponentSpecIndex, g_selectedRacerPointer->bodyMods, g_selectedRacerPointer->sticker, 1,
                0x98, 0, g_tirePics[g_carDraw.tireVariant]);
    g_racerRecordsArray[RACER_KING].overlayPic = 0x4dc;
    pic24 = ar_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
    pu_grab(0x78, 0x45, 0x50, 0x2c, pic24);
    pic3 = di_bm(6, ARENA_LOW_STACK);
    pic4 = ar_bm(0x50, 0x2c, 0, ARENA_HIGH_BASE);
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
    gm_pause(10);
    for (i2 = 1; i2 < 0x14; i2++) {
        if (i2 == 13)
            da_retrace(0x14);
        p3 = &g_endCellsA[i2];
        bm_copy(pic24, pic4, 1);
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
            da_retrace(10);
            (*g_videoOps.copy)(pic4, r_vid.front, &blt, 1);
        }
        if (i2 == 0x12)
            (*g_videoOps.copy)(pic4, r_vid.back, &blt, 1);
    }
    ma_clear();
    if (flag8 == 0) {
        pic58 = di_bm(7, ARENA_LOW_STACK);
        cntz = 9;
        if (r_vid.mode == VID_TANDY) {
            ar_shrink(10000);
            cntz--;
        }
        v32 = v38l = 0x28;
        v2a = v30 = -0x28;
        for (i2 = 0; i2 < cntz; i2++) {
            p2m = &g_endCellsB[i2];
            w8 = max((int)p2m->w + (int)p2m->x, v32 + v2a) - (minx = min((int)p2m->x, v32));
            h8 = max((int)p2m->h + (int)p2m->y, v38l + v2a);
            h8 -= miny = min((int)p2m->y, v38l);
            pic6m = ar_bm(w8, h8, 0, ARENA_HIGH_STACK);
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
            da_retrace(8);
            (*g_videoOps.copy)(pic6m, r_vid.front, &blt2k, 1);
            ma_pop_high(1);
        }
    }
    blt2k.sx = minx;
    blt2k.sy = miny;
    gm_pause(0x50);
    da_retrace(1);
    (*g_videoOps.copy)(r_vid.back, r_vid.front, &blt2k, 1);
    da_retrace(1);
    (*g_videoOps.copy)(pic4, r_vid.front, &blt, 1);
    (*g_videoOps.copy)(pic4, r_vid.back, &blt, 1);
    gm_pause(0x32);
    gm_message(0x183c);
    r_vid.copyToFront = 0;
    ar_reset();
    di_free_list(g_endPics2);
    di_free_list(g_endPics);
}

struct Btn g_jailBars[7] = {{&g_barJailPixels, 0, 0, 16, 149, 40, 5},  {&g_barJailPixels, 0, 0, 16, 149, 96, 5},
                            {&g_barJailPixels, 0, 0, 16, 149, 152, 5}, {&g_barJailPixels, 0, 0, 16, 149, 208, 5},
                            {&g_barJailPixels, 0, 0, 16, 149, 264, 5}, {&g_edgeBmpJail, 0, 0, 32, 149, 0, 5},
                            {&g_edgeBmpJail, 32, 0, 32, 149, 288, 5}};
struct Pair g_jailGaps[6] = {{32, 8}, {56, 40}, {112, 40}, {168, 40}, {224, 40}, {280, 8}};


void bo_visit(void)
{
    int n;
    int rival;
    int choice;
    int flag;
    char class;
    int place;
    int status;

    n = 0;
    class = CAR_PULL_IN;
    flag = 0;
    sc_status_text(-1, "", 0);
    g_cursorX = 0x9e;
    g_cursorY = 0x80;
    sc_picture(0x445, -1);
    vi_bobs_pal();
    sc_label(g_cashLabel);
    g_cursor2.drawnRect.x2 = -1;
    cu_ctl(1);
    (*g_videoOps.show)();
    mu_suspend(0);
    if (rn_int(100) < 25)
        play_cell_anim();
    cu_ctl(CURSOR_SHOW_NOW);
    g_rivalHereState = 0;
    g_carhop = CARHOP_DISABLED;
    g_ordered = 0;
    g_justArrived = 1;
    g_scene = SCENE_BOBS;
    g_mouse.left = g_joyButtons = g_buttons = 0;
    for (;;) {
        setup_bobs_spots(g_rivalHereState, !g_carhop & !g_ordered);
        mn_push(KM_BOBS);
        g_waitUntil = flag == 0 ? (unsigned)(class == CAR_PULL_IN ? 140 : 280) + g_tick : g_tick + 5;
        flag = 0;
        rival = class == CAR_PULL_IN ? 0 : king_chance();
        ga_check_end(0);
        if (g_end)
            return;
        while ((choice = sy_wait_input(3000)) == 0)
            ;
        switch (choice) {
        case KR_BOBS_GARAGE:
            g_strOpp[0] = 0;
            sc_status_text(g_bannerX, " ", g_wRivBanner);
            mn_pop(1);
            g_scene = SCENE_GARAGE;
            cr_go();
            return;
        case KR_TIMEOUT:
            ga_check_end(0);
            if (g_end)
                return;
            if (g_rivalHereState == 0)
                goto race;
            sc_status_text(-1, "", 0);
            show_rival_car(CAR_DRIVE_OUT, g_opponentSpecIndex, 1, 0xa3, 0, g_tirePics[g_carDraw.tireVariant]);
            n = 0;
            mn_pop(1);
            break;
        default:
        out:
            mn_pop(1);
            break;
        case KR_BOBS_FIND_RIVAL:
            if ((status = look_for_rival(class)) == 0)
                flag = 1;
            else
                n = 0;
            mn_pop(1);
            break;
        case KR_BOBS_CHALLENGE:
            if (g_rivalHereState == 0) {
                g_waitUntil = g_tick;
            race:
                rival_arrives(pick_rival(rival, 0), class, RIVAL_CHALLENGE);
                class = CAR_DRIVE_IN;
                mn_pop(1);
            } else {
                g_statusLine.shown = 1;
                sc_status_text(-1, "", 0);
                sc_status_text(g_bannerX, "", g_wRivBanner);
            test:
                while ((sc_status_text(-1, "", 0), challenge(g_isKingState))) {
                    g_strOpp[0] = 0;
                    sc_status_text(g_bannerX, " ", g_wRivBanner);
                    place = ra_outcome();
                    (*g_videoOps.blank)();
                    cr_rival_paint(RIVAL_PAINT_RESTORE);
                    if (g_carDraw.betType != KR_BET_FOR_KICKS)
                        g_betRaces++;
                    if (g_isKingState) {
                        if (g_resultWin == 0) {
                            g_kingDragWon = 0;
                            g_racerRecordsArray[RACER_KING].status = RACER_NOT_MET;
                            g_dragWins -= 3;
                            g_roadWins -= 2;
                        } else if (g_driver.raceType == RACE_DRAG)
                            g_kingDragWon = 1;
                        else {
                            mu_suspend(0);
                            mn_pop(1);
                            play_ending();
                            g_end |= 2;
                            return;
                        }
                    }
                    if (place == 4) {
                        mu_suspend(0);
                        mn_pop(1);
                        bo_jail();
                        cu_ctl(CURSOR_HIDE_NOW);
                        g_end |= 4;
                        return;
                    }
                    if (g_resultWin && g_carDraw.betType != KR_BET_FOR_KICKS) {
                        if (g_driver.raceType == RACE_DRAG)
                            g_dragWins++;
                        else
                            g_roadWins++;
                    }
                    if (g_carDraw.betType != KR_BET_FOR_KICKS) {
                        g_selectedRacerPointer->betRaces++;
                        if (g_resultWin == 0)
                            g_selectedRacerPointer->betWins++;
                    }
                    if (place == 2 || place == 1) {
                        mn_pop(1);
                        g_scene = place == 2 ? SCENE_NONE : SCENE_GARAGE;
                        if (g_scene == SCENE_GARAGE)
                            g_keepCruiseMusic = 1;
                        cr_go();
                        g_keepCruiseMusic = 0;
                        return;
                    }
                    mu_suspend(0);
                    sc_picture(0x445, -1);
                    sc_label(g_cashLabel);
                    (*g_videoOps.show)();
                    class = CAR_PULL_IN;
                    g_rivalHereState = 0;
                    g_carhop = CARHOP_DISABLED;
                    g_ordered = 0;
                    n = 0;
                    if (g_kingDragWon == 0)
                        goto out;
                    rival_arrives(RACER_KING, CAR_PULL_IN, RIVAL_REQUESTED);
                    goto test;
                }
                show_rival_car(CAR_DRIVE_OUT, g_opponentSpecIndex, 1, 0xa3, 0, g_tirePics[g_carDraw.tireVariant]);
                n = 0;
                mn_pop(1);
            }
            break;
        case KR_BOBS_HOOD:
            if (g_rivalHereState == 0)
                goto out;
            if (n > 0 || g_isKingState) {
                gm_message(0x122c);
                show_rival_car(CAR_DRIVE_OUT, g_opponentSpecIndex, 1, 0xa3, 0, g_tirePics[g_carDraw.tireVariant]);
                n = 0;
            } else {
                pop_rival_hood();
                n++;
                g_gameClock.actionTicks += 0x111L;
            }
            mn_pop(1);
            break;
        case KR_BOBS_ORDER:
            if (g_carhop == CARHOP_ABSENT)
                g_ordered = 1;
            mn_pop(1);
            break;
        }
    }
}


static void near set_jail_spots(enum HotKind id, char c)
{
    struct Hot *h;

    h = g_hotBobs;
    if (id == HOT_DISABLED)
        g_hotBobs[0].textOffset = g_msgWannaQuit - g_txt;
    else
        h->textOffset = g_msgBackToGarage - g_txt;
    h[1].kind = id;
    h[2].kind = id;
    h[3].kind = id;
    h[4].kind = id;
    g_keyMaps[KM_BOBS]->key = c;
}


void bo_jail(void)
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
    mode = CAR_PULL_IN;
    flag = 0;
    r_vid.copyToFront = 0;
    cu_ctl(CURSOR_HIDE);
    ma_clear();
    g_edgeBmpJail = di_bm(0x4e7, ARENA_LOW_STACK);
    g_barJailPixels = di_bm(0x4e9, ARENA_LOW_STACK);
    sc_picture(0x445, -1);
    vi_bobs_pal();
    pict = di_bm(0x4e8, ARENA_HIGH_STACK);
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
    sc_status_text(-1, "", 0);
    sc_label(g_cashLabel);
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.copyToFront = 1;
    (*g_videoOps.show)();
    g_rivalHereState = 0;
    g_carhop = CARHOP_DISABLED;
    g_ordered = 0;
    cu_ctl(1);
    cu_ctl(CURSOR_SHOW);
    g_scene = SCENE_JAIL;
    set_jail_spots(HOT_DISABLED, SC_Q);
    for (;;) {
        mn_push(KM_BOBS);
        if (flag == 0)
            g_waitUntil = (mode == CAR_PULL_IN ? 140u : 280u) + g_tick;
        else
            g_waitUntil = g_tick + 5;
        flag = 0;
        who = mode == CAR_PULL_IN ? 0 : king_chance();
        while ((ch = sy_wait_input(3000)) == 0)
            ;
        switch (ch) {
        case KR_BOBS_GARAGE:
            mn_pop(1);
            set_jail_spots(HOT_REGION, SC_G);
            g_scene = SCENE_GARAGE;
            return;
        case KR_TIMEOUT:
            if (rn_int(100) < 6) {
                cu_ctl(CURSOR_HIDE_NOW);
                sc_status_text(-1, "", 0);
                play_cell_anim();
                cu_ctl(CURSOR_SHOW_NOW);
            }
            rival_arrives(pick_rival(who, 1), mode, RIVAL_PASS_BY);
            mode = CAR_DRIVE_IN;
            g_strOpp[0] = 0;
            sc_status_text(g_bannerX, " ", g_wRivBanner);
        case KR_BOBS_CHALLENGE:
        case KR_BOBS_HOOD:
        case KR_BOBS_FIND_RIVAL:
        case KR_BOBS_ORDER:
        default:
            mn_pop(0);
        }
    }
}
