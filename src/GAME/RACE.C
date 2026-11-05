#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "RACE.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CRUISE.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRVTICK.H"
#include "FAME.H"
#include "FONTDATA.H"
#include "GAME.H"
#include "GARAGE.H"
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
#include "ROAD.H"
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

static struct GasInteraction g_fuelStationActions;
static int near find_gas_spot(int);
static void near clear_gas_spots(void);
static void near place_gas_spot(int, int, int, int, int);
static void near sort_gas_spots(void);
static void near show_tank_cap(struct Dlg *, int);
static void near spots_idle(void);
static void near spots_cap(void);
static void near spots_nozzle(void);
static void near spots_rag(void);
static void near spots_none(void);
static void near leave_station(struct Dlg *);
static void near show_crash(void);
static void near show_race_stats(void);
static void near blow_engine(int n);
static void near drop_transmission(int n);
static int near replace_tires(void);
static void near show_ticket(void);
static int g_finishLineIndex;
static struct RivalPace g_rivalPace;
static int g_shiftPenalty;
static int g_targetLane;
static int g_rpmBand;
static int g_shiftPoint;
static void near init_rival_eng(void);
static void near fit_gear(int rpm, struct Eng *p);
static void near rival_accelerate(void);
static void near move_player(void);
static void near move_rival(void);
static void near cop_appears(void);

static int g_gasSpotIds[6] = {2, 1, 4, 6, 5, 3};


static int near find_gas_spot(int id)
{
    int base, n, i;

    base = ((unsigned)g_hotGasStation - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotGasExit - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    for (i = 0; i < n; i++)
        if (g_hots[base + i].id == id)
            return base + i;
}


static void near sort_gas_spots(void)
{
    int base, n;

    base = ((unsigned)g_hotGasStation - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotGasExit - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    gx_sort_hots(base, n);
}

int g_finishMeanSpeedVal;
char g_failureTire;
struct BodyPoints g_featurePos;
int g_steeringOld;
int g_coveredRacePct;
int g_finishAheadPlayerState;
float g_raceSecsElapsedValue;


static void near clear_gas_spots(void)
{
    int k;

    g_fuelStationActions.ragAvailable = 0;
    g_fuelStationActions.nozzleAvailable = 0;
    g_fuelStationActions.capAvailable = 0;
    for (k = 0; k < 6; k++)
        g_hots[find_gas_spot(g_gasSpotIds[k])].kind = HOT_DISABLED;
}


static void near place_gas_spot(int id, int x, int y, int w, int h)
{
    struct Hot *p;
    p = &g_hots[find_gas_spot(id)];
    p->x2 = (p->x1 = x) + w - 1;
    p->y2 = (p->y1 = y) + h - 1;
    p->kind = HOT_REGION;
}


static void near spots_idle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x3f, 100, 12, 23);
    place_gas_spot(1, 0x54, 0xa8, 0x1f, 15);
    sort_gas_spots();
    g_fuelStationActions.ragAvailable = 1;
    g_fuelStationActions.nozzleAvailable = 1;
}


static void near spots_cap(void)
{
    clear_gas_spots();
    place_gas_spot(4, g_featurePos.gasCapX - 10, g_featurePos.gasCapY - 5, 18, 12);
    sort_gas_spots();
    g_fuelStationActions.capAvailable = 1;
}


static void near spots_nozzle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x34, 100, 16, 23);
    place_gas_spot(4, g_featurePos.gasCapX - 1, g_featurePos.gasCapY - 1, 10, 9);
    sort_gas_spots();
    g_fuelStationActions.nozzleAvailable = 1;
    g_fuelStationActions.capAvailable = 1;
}


static void near spots_rag(void)
{
    clear_gas_spots();
    if (g_featurePos.hasRearGlass)
        place_gas_spot(6, g_featurePos.rearGlassX, g_featurePos.rearGlassY, 20, 19);
    place_gas_spot(5, g_featurePos.frontGlassX, g_featurePos.frontGlassY, 22, 19);
    place_gas_spot(1, 0x3e, 0xa5, 0x35, 18);
    place_gas_spot(3, 0xc4, 0x5a, 16, 14);
    sort_gas_spots();
    g_fuelStationActions.ragAvailable = 1;
}


static void near spots_none(void)
{
    clear_gas_spots();
}


static void near show_tank_cap(struct Dlg *d, int id)
{
    struct Bitmap far *back, far *label, far *work;
    struct Blt box;

    if (d->nozzleOnPump)
        back = d->bm1;
    else
        back = d->bm2;
    cu_ctl(CURSOR_HIDE);
    label = di_bm(id, ARENA_HIGH_STACK);
    cu_ctl(CURSOR_SHOW);
    work = ar_bm(back->width, back->height, 0, ARENA_HIGH_STACK);
    bm_copy(back, work, 1);
    box.w = label->width;
    box.h = box.unused1 = label->height;
    box.sx = box.sy = 0;
    box.dx = (~r_vid.mask & g_featurePos.gasCapX) - box.w + 24;
    box.dy = 12 - label->height;
    bm_draw_masked(label, work, &box, 0);
    pu_show(work, g_featurePos.gasCapX - 16, g_featurePos.gasCapY - 5);
    ma_pop_high(2);
}


static void near leave_station(struct Dlg *d)
{
    struct Bitmap far *p;

    if (d->hand != GAS_HAND_EMPTY)
        cu_ctl(1);
    if (d->nozzleInTank) {
        show_tank_cap(d, PIC_FUEL_CAP);
        if (d->nozzlePenaltyDue) {
            sc_add_money(g_driver.money >= 10 ? -10 : -(int)g_driver.money);
            d->nozzlePenaltyDue = 0;
        }
    }
    if (d->nozzleOnPump == 0) {
        pu_show(g_pumpWithNozzle, 0x40, 0x64);
        if (d->nozzlePenaltyDue)
            sc_add_money(g_driver.money >= 10 ? -10 : -(int)g_driver.money);
    }
    if (d->attendantAbsent == 0) {
        cu_ctl(CURSOR_HIDE);
        p = di_bm(PIC_GAS_ATTENDANT, ARENA_HIGH_STACK);
        cu_ctl(CURSOR_SHOW);
        cp_patch_bg(p, 0x60, 0, 0x30, 0x40, 0xc8, 0x51, 0xa9, 2);
    }
    g_driver.currentCar->fuel = g_driver.currentCar->fuel > 0xaa ? 0xaa : g_driver.currentCar->fuel;
    cp_show_car(CAR_DRIVE_OUT, d->specIndex, d->bodyMods, (g_driver.currentCar->appearance >> 8) & 0x1f, 0, 0xa9, 1,
                g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
    mn_pop(1);
    ma_clear();
    g_scene = SCENE_GARAGE;
    cr_go();
}


void ra_gas_station(void)
{
    struct Bitmap far *label;
    struct Dlg dl;
    int pos;
    int wait1;
    int done;
    int key;

    sc_status_text(-1, "\0", 0);
    g_scene = SCENE_GAS_STATION;
    sc_picture(PIC_GAS_STATION, -1);
    sc_label(g_cashLabel);
    r_vid.copyToFront = 1;
    (*g_videoOps.show)();
    dl.hand = GAS_HAND_EMPTY;
    cu_ctl(1);
    dl.specIndex = g_driver.currentCar->specIndex;
    dl.bodyMods = gm_car_look(g_driver.currentCar);
    cp_show_car(CAR_PULL_IN, dl.specIndex, dl.bodyMods, (g_driver.currentCar->appearance >> 8) & 0x1f, 0, 0xa9, 1,
                g_tirePics[g_parts[g_driver.currentCar->tires->idx].variant]);
    mu_suspend(0);
    label = pu_grab(0x50, 0xaa, 0x28, 0xc, 0);
    dl.bm1 = pu_grab(g_featurePos.gasCapX - 0x10, g_featurePos.gasCapY - 5, 0x18, 0xc, 0);
    r_vid.copyToFront = 0;
    pu_show(g_pumpNoNozzle, 0x40, 0x64);
    dl.bm2 = pu_grab(g_featurePos.gasCapX - 0x10, g_featurePos.gasCapY - 5, 0x18, 0xc, 0);
    pu_show(g_pumpWithNozzle, 0x40, 0x64);
    r_vid.copyToFront = 1;
    pos = 1;
    dl.pumping = 0;
    wait1 = 0x28;
    done = 0;
    dl.nozzleInTank = 0;
    dl.nozzleOnPump = 1;
    dl.attendantAbsent = 1;
    dl.unused = 0;
    dl.nozzlePenaltyDue = 1;
    dl.fillsLeft = (0xbe - g_driver.currentCar->fuel) / 0x1e;
    show_tank_cap(&dl, PIC_FUEL_CAP);
    spots_idle();
    mn_push(KM_GAS_STATION);
    cu_ctl(CURSOR_SHOW_NOW);
top:
    g_waitUntil = g_tick + 20;
    for (;;) {
        while ((key = sy_wait_input(3000)) == 0)
            ;
        switch (key) {
        case KR_TIMEOUT:
            if (dl.hand == GAS_HAND_RAG)
                goto top;
            if (dl.pumping) {
                dl.fillsLeft--;
                g_driver.currentCar->fuel += 0x1e;
                done = 1;
                sc_add_money(-(pos <= g_driver.money ? pos : (int)g_driver.money));
                if (dl.fillsLeft == 0 || pos > g_driver.money) {
                    if (vi_anim_active(2)) {
                        so_pump(0);
                        vi_anim_stop(2);
                    }
                    if (dl.attendantAbsent)
                        goto tail;
                fin:
                    leave_station(&dl);
                    return;
                }
            } else {
                if (--wait1 > 0)
                    goto top;
                mn_pop(1);
                spots_none();
                mn_push(KM_GAS_STATION);
                pos <<= 1;
                dl.unused = 1;
                gm_play_anim(3);
                gm_message(0x19e1);
                dl.attendantAbsent = 0;
                if (done)
                    goto fin;
                dl.nozzlePenaltyDue = 0;
                if (dl.hand == GAS_HAND_EMPTY) {
                    pu_show(g_pumpNoNozzle, 0x40, 0x64);
                    cu_ctl(dl.hand = GAS_HAND_NOZZLE);
                    dl.nozzleOnPump = 0;
                }
                show_tank_cap(&dl, PIC_FUEL_NOZZLE);
                dl.nozzleInTank = 1;
                cu_ctl(dl.hand = GAS_HAND_EMPTY);
                vi_anim_start(2);
                so_pump(1);
                dl.pumping = 1;
            }
            g_waitUntil = g_tick + 0x4c;
            break;
        case KR_GAS_RAG:
            if (g_fuelStationActions.ragAvailable == 0)
                break;
            if (dl.hand == GAS_HAND_EMPTY) {
                cu_ctl(dl.hand = GAS_HAND_RAG);
                sc_picture_at(PIC_FUEL_GAUGE, 0x50, 0xaa, -1);
                mn_pop(1);
                spots_rag();
            redraw:
                mn_push(KM_GAS_STATION);
                goto top;
            } else {
                cu_ctl(dl.hand = GAS_HAND_EMPTY);
                pu_show(label, 0x50, 0xaa);
                mn_pop(1);
                spots_idle();
                goto redraw;
            }
        case KR_GAS_PUMP:
            if (g_fuelStationActions.nozzleAvailable == 0)
                break;
            if (dl.hand == GAS_HAND_EMPTY) {
                cu_ctl(dl.hand = GAS_HAND_NOZZLE);
                pu_show(g_pumpNoNozzle, 0x40, 0x64);
                dl.nozzleOnPump = 0;
            both:
                mn_pop(1);
                spots_nozzle();
            } else {
                if (pos > g_driver.money || done) {
                    dl.nozzlePenaltyDue = 0;
                    goto fin;
                }
                cu_ctl(dl.hand = GAS_HAND_EMPTY);
                pu_show(g_pumpWithNozzle, 0x40, 0x64);
                dl.nozzleOnPump = 1;
                mn_pop(1);
                spots_idle();
            }
            mn_push(KM_GAS_STATION);
        tail:
            g_waitUntil = g_tick + 20;
            dl.pumping = 0;
            break;
        case KR_GAS_CAP:
            if (g_fuelStationActions.capAvailable == 0)
                break;
            if (dl.hand == GAS_HAND_EMPTY) {
                if (vi_anim_active(2)) {
                    so_pump(0);
                    vi_anim_stop(2);
                }
                show_tank_cap(&dl, PIC_FUEL_CAP);
                dl.nozzleInTank = 0;
                cu_ctl(dl.hand = GAS_HAND_NOZZLE);
                if (dl.fillsLeft > 0 && pos <= g_driver.money)
                    gm_message(0x18dd);
                goto both;
            }
            if (dl.fillsLeft == 0 || pos > g_driver.money)
                gm_message(0x10b5);
            else {
                show_tank_cap(&dl, PIC_FUEL_NOZZLE);
                dl.nozzleInTank = 1;
                cu_ctl(dl.hand = GAS_HAND_EMPTY);
                vi_anim_start(2);
                so_pump(1);
                mn_pop(1);
                spots_cap();
                mn_push(KM_GAS_STATION);
                g_waitUntil = g_tick + 0x4c;
                dl.pumping = 1;
            }
            break;
        case KR_GAS_LEAVE:
            gm_message(0x10f6);
            continue;
        case KR_GAS_FRONT_WINDSHIELD:
            break;
        case KR_GAS_REAR_WINDSHIELD:
            break;
        }
    }
}


static int near replace_tires(void)
{
    struct Node *tbl[142];
    int ret;

    ret = 0;
    if (g_driver.currentCar == 0)
        return 0;
    g_driver.currentCar->value -= gm_take_part(g_driver.currentCar->tires, 0);
    g_driver.currentCar->tires = 0;
    ga_spares(PART_TIRES, tbl, 0, 0);
    if (g_list[0]) {
        ga_install(ga_unlink(tbl[1]), &g_driver.currentCar->tires);
    } else {
        g_driver.money -= 0x32;
        if (g_driver.money < 0) {
            ret = (int)-g_driver.money;
            g_driver.money = 0;
        }
        gm_add_part(0x28, g_parts[40].value);
        tbl[0] = g_driver.spareParts;
        g_driver.spareParts = tbl[0]->next;
        ga_install(tbl[0], &g_driver.currentCar->tires);
    }
    return ret;
}

static int g_winMsgs[6] = {4973, 5015, 5073, 5043, 5099, 5130};
static struct Spot g_crashPics[3] = {
    {PIC_CRASH_BLOOD_LEFT, 64, 84}, {PIC_CRASH_BLOOD_CENTER, 208, 82}, {PIC_CRASH_BLOOD_RIGHT, 272, 123}};
static int g_statsDone = 0;


static void near show_ticket(void)
{
    struct Bitmap far *a, far *b;
    int endk, desc3;
    char *p2;
    int wk;
    char d, e, cl;
    int maxw2;
    struct Blt pkt;

    ma_clear();
    a = di_bm(PIC_SPEEDING_TICKET, ARENA_LOW_STACK);
    b = di_bm(PIC_TUNING_HAND, ARENA_LOW_STACK);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.dx = pkt.sx = pkt.sy = 0;
    pkt.dy = 0x45;
    (*g_videoOps.maskedBlit)(b, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = b->width;
    pkt.dy = 0x14;
    (*g_videoOps.copy)(a, r_vid.back, &pkt, 1);
    maxw2 = a->width - 8;
    r_vid.copyToFront = 0;
    x_colors = bustyle->hilite;
    for (p2 = g_driverName, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    (*g_videoOps.drawText)(r_vid.back, 0x5e, 0x48, g_driverName);
    *p2 = d;
    desc3 = g_carData[g_driver.currentCar->specIndex].nameOffset;
    endk = desc3 + strcspn(g_descHdr + desc3 + 8, " `.") + 7;
    for (p2 = g_descHdr + desc3 + 1, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    e = g_descHdr[endk + 1];
    g_descHdr[endk + 1] = 0;
    (*g_videoOps.drawText)(r_vid.back, 0x5e, 0x63, g_descHdr + desc3 + 1);
    g_descHdr[endk + 1] = e;
    *p2 = d;
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.copyToFront = 1;
    cu_ctl(CURSOR_SHOW_NOW);
    sy_reset_queues();
}


static void near show_crash(void)
{
    struct Bitmap far *a, far *b;
    struct Blt pkt;
    int fast, i;

    fast = r_vid.mode == VID_EGA && g_vga == 0;
    so_crash();
    ma_clear();
    a = di_bm(PIC_CRASHED_WINDSHIELD, ARENA_LOW_STACK);
    b = di_bm(PIC_CRASH_WINDOW_CORNER, ARENA_LOW_STACK);
    pkt.w = a->width;
    pkt.h = 0x5a;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = 0xb;
    (*g_videoOps.maskedBlit)(a, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height - 0x59;
    pkt.sy = 0x59;
    pkt.sx = pkt.dx = 0;
    pkt.dy = (fast ? 0x5a : 0x59) + 0xb;
    (*g_videoOps.maskedBlit)(a, r_vid.back, &pkt, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = fast ? 0x78 : 0x77;
    (*g_videoOps.copy)(b, r_vid.back, &pkt, 1);
    da_steer_marker(g_steeringOld);
    (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
    if (g_driveEng.speed > 0x5a) {
        for (i = 0; i < 3; i++) {
            a = di_bm(g_crashPics[i].id, ARENA_LOW_STACK);
            pkt.w = a->width;
            pkt.h = a->height;
            pkt.sx = pkt.sy = 0;
            pkt.dx = g_crashPics[i].x;
            pkt.dy = g_crashPics[i].y;
            da_retrace(0x14 - 8 * i);
            (*g_videoOps.maskedBlit)(a, r_vid.back, &pkt, 1);
            (*g_videoOps.copyPage)(r_vid.back->bits.p, r_vid.front->bits.p);
        }
    }
    cu_ctl(CURSOR_SHOW_NOW);
    (*g_videoOps.show)();
    sy_reset_queues();
    gm_pause(0x64);
    sy_reset_queues();
    ma_clear();
}


void ra_finish_stats(void)
{
    if (g_statsDone == 0) {
        if ((g_roadOppPos.roadIndex - g_trackViewMotion.roadIndex) * 8 + g_roadOppPos.roadFraction <=
            g_trackViewMotion.roadFraction)
            g_finishAheadPlayerState = 1;
        else
            g_finishAheadPlayerState = 0;
        g_finishMeanSpeedVal = g_mphAccumulator / g_driveEng.elapsedTicks;
        g_raceSecsElapsedValue = g_driveEng.elapsedTicks / 6.068827333333334;
        g_coveredRacePct = (int)((g_trackViewMotion.roadIndex - 200) * 100.0 / (g_roadOppPos.roadIndex - 200)) > 99
                               ? 99
                               : (int)((g_trackViewMotion.roadIndex - 200) * 100.0 / (g_roadOppPos.roadIndex - 200));
        g_statsDone = 1;
    }
}


static void near show_race_stats(void)
{
    if (g_resultWin) {
        sprintf(g_raceResult1, "Your``winning``time %2.1f sec.,`avg. speed %2d mph", g_raceSecsElapsedValue,
                g_finishMeanSpeedVal);
        strcpy(g_raceResult2, "Congratulations !");
    } else {
        sprintf(g_raceResult1, "In %2.1f sec you covered only %2d%%", g_raceSecsElapsedValue, g_coveredRacePct);
        sprintf(g_raceResult2, "of the distance. Your avg. speed %2d mph.", g_finishMeanSpeedVal);
    }
    mn_push(KM_RACE_STATS);
    mn_pop(1);
}


static void near blow_engine(int n)
{
    struct Node **p;
    char *q;
    int i;

    if (g_driver.currentCar) {
        for (i = 0, p = g_driver.currentCar->rack.parts; i < 5; i++, p++) {
            g_driver.currentCar->value -= gm_take_part(*p, 0);
            *p = 0;
        }
        for (i = 0, q = g_driver.currentCar->engineBolts; i < 9; i++)
            *q++ = 0;
        g_driver.currentCar->wireState = WIRES_ABSENT;
    }
    sy_reset_queues();
    so_noise();
    gm_popup(-n, -1, 0x49);
}


static void near drop_transmission(int n)
{
    char *q;
    int i;

    if (g_driver.currentCar) {
        g_driver.currentCar->value -= gm_take_part(g_driver.currentCar->transmission, 0);
        g_driver.currentCar->transmission = 0;
        for (i = 0, q = g_driver.currentCar->transmissionBolts; i < 2; i++)
            *q++ = 0;
    }
    sy_reset_queues();
    so_noise();
    gm_popup(-n, -1, 0x49);
}

static int g_tireSlip[3][5] = {{2, 5, 8, 11, 14}, {0, 2, 4, 6, 8}, {0, 0, 1, 2, 4}};
static int g_lastTicks = 0;
static int g_bumpAlternate = 0;
int g_rivalLeft = 1;
static int g_rivalCurveBrake = 0;
static int g_contactFrames = 0;
int g_brakeTicks = 0;


int near ra_outcome(void)
{
    int units;
    char opt;
    int sel;
    enum BetKind kind;
    unsigned v;
    char stop;
    char *ptr;
    char temp[40];
    char engine;
    int all;
    int i;
    long cost;
    struct Car *found;
    int list[25];

    kind = g_raceWagerTerms.kind;
    v = g_raceWagerTerms.amount;
    engine = g_driveEng.stopFlags & STOP_ENGINE;
    opt = g_driveEng.stopFlags & STOP_TRANSMISSION;
    stop = g_driveEng.stopFlags == (STOP_LOST | STOP_ENGINE) ||
           g_driveEng.stopFlags == (STOP_LOST | STOP_TRANSMISSION) ||
           g_driveEng.stopFlags == (STOP_LOST | STOP_ENGINE | STOP_TRANSMISSION);
    if (g_driver.currentCar->transmission->wear > 9900)
        g_driver.currentCar->transmission->wear = 9900;
    if (g_driver.currentCar->rack.parts[0]->wear > 9900)
        g_driver.currentCar->rack.parts[0]->wear = 9900;
    if (g_driver.currentCar->tires->wear > 9900)
        g_driver.currentCar->tires->wear = 9900;
    g_resultWin = 0;
    g_statsDone = 0;
    g_mphAccumulator = 0;
    g_fireLatch = 0;
    g_buttons = 0;
    if (g_driveEng.automatic)
        g_driveEng.autoSelector = SELECTOR_NEUTRAL;
    if (g_failureTire)
        g_driveEng.stopFlags |= STOP_TIRES;
    mn_push(KM_RACE_OUTCOME);
    i = g_driver.raceType == RACE_DRAG ? 5 : 15;
    if (g_driver.currentCar->fuel > i)
        g_driver.currentCar->fuel -= i;
    else
        g_driver.currentCar->fuel = 0;
    (*g_videoOps.show)();
    if (g_oppEngState.stopFlags & STOP_LOST) {
        g_resultWin = 1;
        show_race_stats();
        cu_ctl(CURSOR_SHOW_NOW);
        gm_message(g_isKingState == 0 ? (i = g_winMsgs[kind]) : g_driver.raceType == RACE_DRAG ? 0x14bd : 0x14d6);
        sc_add_money(v);
        if (kind == BET_CAR) {
            found = gm_make_car(g_opponentSpecIndex, g_oppSpecPointer->price, 0);
            found->tires->wear = rn_int(500);
            found->appearance = ((g_selectedRacerPointer->sticker & 0x1f) << 8) | (found->appearance & 0xe0ff);
            found->appearance =
                (((g_selectedRacerPointer->bodyMods & 4) != 0 & 1) << 13) | (found->appearance & 0xdfff);
            found->appearance =
                (((g_selectedRacerPointer->bodyMods & 2) != 0 & 1) << 14) | (found->appearance & 0xbfff);
            found->appearance =
                (((g_selectedRacerPointer->bodyMods & 1) != 0 & 1) << 15) | (found->appearance & 0x7fff);
            found->paintScheme = g_selectedRacerPointer->paintScheme;
            if (found->rank > g_driver.currentCar->rank) {
                g_driver.currentCar->next = g_driver.otherCars->next;
                g_driver.otherCars = g_driver.currentCar;
                g_driver.currentCar = found;
                pf_drivetrain(g_driver.currentCar, &g_driveEng);
            }
            if (g_demo == 0 || g_driverIdxSelVal != g_demoRacer) {
                if (g_selectedRacerPointer->lostCarOnce != 0 || g_isKingState != 0)
                    g_selectedRacerPointer->status = RACER_EMPTY;
                else {
                    for (i = 0, units = 0; i < 25; i++)
                        if (g_carData[i].rank < g_oppSpecPointer->rank)
                            list[++units] = i;
                    if (units > 0) {
                        list[0] = units;
                        sg_init_racer(g_driverIdxSelVal, list);
                        g_selectedRacerPointer->lostCarOnce = 1;
                    } else
                        g_selectedRacerPointer->status = RACER_EMPTY;
                    goto done;
                }
            }
        }
    done:
        mn_pop(1);
        if (g_driveEng.stopFlags & STOP_TIRES)
            if ((i = replace_tires()) != 0)
                goto ret2;
        if (kind == BET_CAR)
            return 1;
        return 3;
    } else if (g_driveEng.stopFlags & STOP_POLICE) {
        so_siren(0);
        show_ticket();
        i = (g_driveEng.stopFlags & STOP_TIRES) ? replace_tires() : 0;
        if (g_driveEng.speed > 0x50)
            i += 0x4b;
        else if (g_driveEng.speed > 0x32)
            i += 0x32;
        else
            i += 0x14;
        mn_push(KM_MESSAGE);
        sy_wait_input(0);
        sprintf(temp, "You have been fined $%2d for speeding", i);
        sc_status_text(-1, temp, SCREEN_WIDTH - g_statusLine.messageX);
        gm_pause(0xfa);
        if (i <= g_driver.money)
            goto pay;
        gm_message(0x1793);
        mn_pop(1);
        mn_pop(1);
        sc_add_money(-g_driver.money);
        sc_status_text(-1, " ", SCREEN_WIDTH - g_statusLine.messageX);
        return 4;
    pay:
        sc_add_money(-i);
        cost = g_waitUntil;
        gm_pause(g_msgTicks);
        g_waitUntil = cost;
        mn_pop(1);
        sc_status_text(-1, "", SCREEN_WIDTH - g_statusLine.messageX);
    } else if (g_driveEng.stopFlags == (STOP_LOST | STOP_JUMP_START)) {
        cu_ctl(CURSOR_SHOW_NOW);
        gm_message(0xecb2);
    } else {
        if (kind == BET_CAR) {
            gm_scrap_car(g_driver.currentCar, 0);
            g_driver.currentCar = 0;
        }
        sc_add_money(-v);
        if (g_driveEng.stopFlags == STOP_LOST) {
            show_race_stats();
            cu_ctl(CURSOR_SHOW_NOW);
            i = kind == BET_CAR ? 0x143a : 0x1427;
            gm_message(g_isKingState == 0 ? i : g_driver.raceType == RACE_DRAG ? 0x1450 : 0x146c);
            mn_pop(1);
            if (kind != BET_CAR)
                return 3;
        ret2:
            return 2;
        } else if (stop) {
            cu_ctl(CURSOR_SHOW_NOW);
            if (engine) {
                blow_engine(kind == BET_CAR ? 0x16bd : 0x1634);
                goto finishstop;
            } else if (opt) {
                drop_transmission(kind == BET_CAR ? 0x1678 : 0x1608);
                goto finishstop;
            }
        finishstop:
            goto fin;
        } else {
            show_crash();
            if (kind == BET_CAR) {
                if (g_driveEng.stopFlags & STOP_TIRES) {
                    gm_message(0x16f6);
                    replace_tires();
                } else
                    gm_message(0x1658);
            } else if (g_driveEng.speed > 0x5a) {
                v = gm_scrap_car(g_driver.currentCar, 0) / 10 + 0x13;
                g_driver.currentCar = 0;
                sc_add_money(v);
                ptr = g_msgTotaled;
                while (*ptr++ != '$')
                    ;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                sy_reset_queues();
                gm_message(0x1513);
            } else {
                if (g_driveEng.speed > 0x3c) {
                    i = 0;
                    v = rn_int(0x28) + 100;
                } else if (g_driveEng.speed > 0x1e) {
                    i = 1;
                    v = rn_int(0x14) + 100;
                } else {
                    if (g_driveEng.stopFlags & STOP_TIRES) {
                        gm_message(0x1590);
                        all = replace_tires();
                        goto fin;
                    }
                    gm_message(0x1561);
                    mn_pop(1);
                    return 1;
                }
                ptr = strchr(i ? g_msgRunCost : g_msgRepairCost, '$') + 1;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                all = g_driveEng.stopFlags & STOP_TIRES;
                if (all)
                    strcpy(g_msgPlusTires, "Plus tires !");
                else
                    g_msgPlusTires[0] = 0;
                sel = i ? KM_GET_RUNNING : KM_REPAIR;
                if ((long)v > g_driver.money) {
                    g_hotRepair[0].kind = g_hotRepair[7].kind = HOT_DISABLED;
                    g_keyMaps[sel][0].val = KV_REPAIR_JUNK;
                    g_keyMaps[sel][1].key = SC_ESC;
                    g_keyMaps[sel][1].val = KV_REPAIR_JUNK;
                } else {
                    g_hotRepair[0].kind = g_hotRepair[7].kind = HOT_BUTTON;
                    g_keyMaps[sel][0].val = KV_REPAIR_FIX;
                    g_keyMaps[sel][1].key = SC_F;
                    g_keyMaps[sel][1].val = KV_REPAIR_FIX;
                }
                mn_push(sel);
                sy_reset_queues();
                while ((i = sy_wait_input(3000)) == 0)
                    ;
                mn_pop(1);
                switch (i) {
                case KR_REPAIR_FIX:
                    sc_add_money(-v);
                    if (all)
                        replace_tires();
                    mn_pop(1);
                    return 1;
                case KR_REPAIR_JUNK:
                    sc_add_money((int)(g_driver.currentCar->value * 0.4));
                    g_driver.currentCar = 0;
                    break;
                default:
                    return;
                }
            }
        }
    fin:
        mn_pop(1);
        goto ret2;
    }
    mn_pop(1);
    return 3;
}

struct RoadEdges g_edgesRoad;
int g_idxAtCop;
enum CopPhase g_pursuitPhaseIndex;
int g_shownShiftPosition;


void near ra_reset_eng(struct Eng *p)
{
    struct Node *car;
    int t;

    _disable();
    if (g_savedCursorX == 0 && g_savedCursorY == 0) {
        g_savedCursorX = g_cursorX;
        g_savedCursorY = g_cursorY;
    }
    g_cursorX = g_cursorY = g_lastDx = g_lastDy = 0;
    _enable();
    g_fireLatch = 0;
    p->speed = p->gearIndex = p->rpmBand = p->steeringInput = p->steering = 0;
    p->stopFlags = p->unused5 = p->elapsedTicks = 0;
    if (p->automatic) {
        p->gear[2].bands[1].maxRpm = p->gear[2].bands[2].minRpm = p->shiftRpm;
        p->autoSelector = SELECTOR_NEUTRAL;
    }
    p->unused3 = p->unused2 = -1;
    p->rpm = 0x5dc;
    p->rpmPerSpeed = p->initialRpmPerSpeed;
    p->accelStep = p->gear[0].bands[0].accelStep;
    p->decelStep = p->gear[0].bands[0].decelStep;
    p->maxRpm = p->gear[0].bands[0].maxRpm;
    p->minRpm = p->gear[0].bands[0].minRpm;
    g_unused6 = 0;
    g_unused7 = g_unused8 = 0;
    g_shownShiftPosition = -1;
    g_finishLineIndex = g_driver.raceType == RACE_DRAG ? 0x17c : 0x3fc;
    car = g_driver.currentCar->tires;
    p->tireSlip = g_tireSlip[g_parts[car->idx].variant][car->wear / 2000];
    if (car->wear < 8000)
        p->tireFailureIndex = 0;
    else {
        if (car->wear + rn_int(2000) <= 9500)
            p->tireFailureIndex = 0;
        else {
            t = (g_finishLineIndex - 200) >> 1;
            p->tireFailureIndex = rn_int(t >> 1) + t + 200;
        }
        goto nx;
    }
nx:
    g_failureTire = 0;
    g_lastTicks = 0;
    g_rivalPace.topSpeed = pf_vmax(g_opponentSpecIndex);
    g_rivalPace.slowSpeed = (int)(g_rivalPace.topSpeed * 0.7);
    g_rivalPace.fastSpeed = (int)(g_rivalPace.topSpeed * 0.8);
    g_rivalPace.speedMargin = g_processorSpeedRating < 4 ? 10 : 16;
    g_bumpAlternate = 0;
    g_statsDone = 0;
}


static void near init_rival_eng(void)
{
    struct Range *r;

    pf_drivetrain_spec(g_opponentSpecIndex, &g_oppEngState);
    g_shiftPenalty = g_selectedRacerPointer->shiftPenalty;
    ra_reset_eng(&g_oppEngState);
    g_oppEngState.gearIndex = 0;
    g_oppEngState.rpmPerSpeed = g_oppEngState.gearRpmPerSpeed[0];
    g_oppEngState.rpm = 0x5dc;
    g_oppEngState.speed = 0x5dc / g_oppEngState.rpmPerSpeed;
    if ((r = g_oppEngState.gear[0].bands + (g_rpmBand = 1))->maxRpm <= 0x5dc) {
        g_rpmBand = 2;
        r++;
    } else if (r->minRpm > g_oppEngState.rpm) {
        g_rpmBand = 0;
        r--;
    }
    g_oppEngState.rpmBand = g_rpmBand;
    g_oppEngState.maxRpm = r->maxRpm;
    g_oppEngState.accelStep = r->accelStep;
    g_shiftPoint = g_oppEngState.autoSelector ? 0x14b4 : g_oppEngState.shiftRpm - 100 * g_shiftPenalty;
    g_roadOppPos.roadIndex = 200;
    g_roadOppPos.roadFraction = 0;
    g_targetLane = g_roadOppPos.lateralX = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200;
}


void near ra_run(void)
{
    int one;
    struct Node *slot;

    one = 1;
    cu_ctl(CURSOR_HIDE_NOW);
    cu_ctl(1);
    sy_reset_queues();
    ra_reset_eng(&g_driveEng);
    init_rival_eng();
    if (g_driver.raceType == RACE_ROAD && g_isKingState == 0) {
        if (g_pursuitPhaseIndex = !rn_int(4))
            g_idxAtCop = rn_int(255) + 0x1fe;
    } else
        g_pursuitPhaseIndex = COP_ABSENT;
    g_phaseDriveRunState = DRIVE_START;
    g_runRacingFlag = 1;
    cr_drive();
    sy_drive_stop();
    g_waitUntil = 0;
    sy_reset_queues();
    sc_label(g_cashLabel);
    if (g_trackViewMotion.roadIndex > 200) {
        if (rn_int(3) > 0) {
            if (g_driver.currentCar->timing > 0)
                g_driver.currentCar->timing =
                    g_driver.currentCar->timing < rn_int(4) ? rn_int(4) : g_driver.currentCar->timing;
            else if (g_driver.currentCar->timing < 0)
                g_driver.currentCar->timing =
                    -rn_int(8) < g_driver.currentCar->timing ? -rn_int(8) : g_driver.currentCar->timing;
            else
                g_driver.currentCar->timing = rn_int(8) - 4;
        }
        slot = g_driver.currentCar->tires;
        slot->wear += (g_parts[slot->idx].variant == TRANS_AUTOMATIC ? (g_driver.raceType == RACE_ROAD ? 700 : 300)
                                                                     : (g_driver.raceType == RACE_ROAD ? 400 : 200)) +
                      rn_int(300);
        if (slot->wear > 10000)
            slot->wear = 10000;
    }
}

/* Steps p->f18 up from 1 until gear[f18].r[1].f2 exceeds f34[f18] * rpm or
   f18 reaches f1c. The four pointers the loop advances in memory are /Ol induction temps for the p->f18 indexing. */

static void near fit_gear(int rpm, struct Eng *p)
{
    struct Range *r;

    for (p->gearIndex = 1;; p->gearIndex++) {
        p->rpmPerSpeed = p->gearRpmPerSpeed[p->gearIndex];
        p->rpm = p->rpmPerSpeed * rpm;
        if ((r = &p->gear[p->gearIndex].bands[1])->maxRpm > p->rpm) {
            p->rpmBand = g_rpmBand = p->rpm >= r->minRpm;
            if (g_rpmBand == 0)
                r = &p->gear[p->gearIndex].bands[0];
            break;
        }
        if (p->gearIndex == p->gearCount) {
            r = &p->gear[p->gearIndex].bands[2];
            p->rpmBand = g_rpmBand = 2;
            break;
        }
    }
    p->accelStep = r->accelStep;
    p->speed = p->rpm / p->rpmPerSpeed;
}


static void near rival_accelerate(void)
{
    struct Range *r;
    int old3;
    int vl;
    int d;
    int liml;
    int i5;
    int ticks;

    old3 = g_oppEngState.speed;
    _disable();
    ticks = g_driveEng.elapsedTicks - g_lastTicks;
    g_lastTicks = g_driveEng.elapsedTicks;
    _enable();
    r = &g_oppEngState.gear[g_oppEngState.gearIndex].bands[g_rpmBand];
    for (i5 = 0; i5 < ticks; i5++) {
        if (g_terrain2[g_roadOppPos.roadIndex] != g_terrain2[g_roadOppPos.roadIndex + 1]) {
            if (g_rivalCurveBrake == 0) {
                g_rivalCurveBrake = 1;
                vl = g_rivalPace.topSpeed - (g_roadOppPos.roadIndex > g_trackViewMotion.roadIndex
                                                 ? g_selectedRacerPointer->drivingPenalty
                                                 : g_selectedRacerPointer->drivingPenalty >> 1);
                if (vl < g_oppEngState.speed)
                    fit_gear(vl, &g_oppEngState);
            }
            continue;
        }
        g_rivalCurveBrake = 0;
        d = g_roadOppPos.roadIndex - g_trackViewMotion.roadIndex;
        if (d > 12 && (g_driver.raceType != RACE_ROAD || g_isKingState == 0)) {
            if (g_driveEng.speed >= 0x61)
                vl = g_rivalPace.fastSpeed;
            else if (g_isKingState)
                vl = g_rivalPace.fastSpeed;
            else
                vl = g_rivalPace.slowSpeed;
        chk:
            if (vl < g_oppEngState.speed) {
                fit_gear(vl, &g_oppEngState);
                continue;
            }
        } else if (d > -5 && d < 5) {
            vl = g_driveEng.speed + g_rivalPace.speedMargin;
            if (g_trackViewMotion.roadIndex >= 30 && vl < g_rivalPace.slowSpeed)
                vl = g_rivalPace.slowSpeed;
            goto chk;
        }
        if (g_pursuitPhaseIndex >= COP_APPROACHING && g_driveEng.speed > 50 &&
            g_oppEngState.speed > g_driveEng.speed + 32)
            fit_gear(g_driveEng.speed + 32, &g_oppEngState);
        else {
            g_oppEngState.rpm += g_oppEngState.accelStep;
            if (g_oppEngState.rpm > g_shiftPoint)
                g_oppEngState.rpm = g_shiftPoint;
            if (g_oppEngState.rpm > g_oppEngState.maxRpm) {
                g_oppEngState.rpmBand++;
                g_oppEngState.maxRpm =
                    (r = &g_oppEngState.gear[g_oppEngState.gearIndex].bands[g_oppEngState.rpmBand])->maxRpm;
                g_oppEngState.accelStep = r->accelStep;
            }
            if (g_oppEngState.gearIndex < g_oppEngState.gearCount &&
                ((g_oppEngState.autoSelector && g_oppEngState.rpm >= g_oppEngState.autoShiftRpm) ||
                 g_oppEngState.rpm >= g_shiftPoint)) {
                g_oppEngState.gearIndex++;
                if (g_oppEngState.autoSelector == SELECTOR_MANUAL)
                    g_shiftPoint = g_oppEngState.gearIndex < g_oppEngState.gearCount
                                       ? g_oppEngState.shiftRpm - 100 * g_shiftPenalty
                                   : g_processorSpeedRating < 4 && r_vid.mode != VID_TANDY ? 0x14b4
                                                                                           : 0x1518;
                g_oppEngState.rpmPerSpeed = g_oppEngState.gearRpmPerSpeed[g_oppEngState.gearIndex];
                g_oppEngState.rpm = g_oppEngState.speed * g_oppEngState.rpmPerSpeed;
                if ((r = &g_oppEngState.gear[g_oppEngState.gearIndex].bands[g_rpmBand = 1])->maxRpm <=
                    g_oppEngState.rpm) {
                    g_rpmBand = 2;
                    r++;
                } else if (r->minRpm > g_oppEngState.rpm) {
                    g_rpmBand = 0;
                    r--;
                }
                g_oppEngState.rpmBand = g_rpmBand;
                g_oppEngState.maxRpm = r->maxRpm;
                g_oppEngState.accelStep = r->accelStep;
            }
            g_oppEngState.speed = g_oppEngState.rpm / g_oppEngState.rpmPerSpeed;
        }
    }
    if (g_roadOppPos.roadIndex >= g_finishLineIndex) {
        liml = g_trackViewMotion.roadIndex + 5 < g_roadOppPos.roadIndex ? 5 : 10;
        g_oppEngState.speed = old3;
        if (liml < old3)
            g_oppEngState.speed -= liml;
        else
            g_oppEngState.speed = 0;
    }
}


int ra_speed_step(int rpm)
{
    if (rpm <= 0)
        return 0;
    if (g_processorSpeedRating < 4) {
        if (rpm <= 0x47)
            return (rpm >> 3) + 1;
        return (rpm >> 2) - 8;
    }
    if (rpm <= 0x61) {
        if (rpm <= 0x37) {
            if (rpm <= 0x1b) {
                if (rpm <= 0xd)
                    return 1;
                return 2;
            }
            if (rpm <= 0x29)
                return 3;
            return 4;
        }
        if (rpm <= 0x53) {
            if (rpm <= 0x45)
                return 5;
            return 6;
        }
        return 7;
    }
    if (rpm <= 0x71) {
        if (rpm <= 0x69) {
            if (rpm <= 0x65)
                return 8;
            return 9;
        }
        if (rpm <= 0x6d)
            return 10;
        return 11;
    }
    if (rpm <= 0x79) {
        if (rpm <= 0x75)
            return 12;
        return 13;
    }
    if (rpm <= 0x81) {
        if (rpm <= 0x7d)
            return 14;
        return 15;
    }
    return 16;
}

struct Bitmap far *g_policeFramesTable[13];
int g_picOpponent;
int g_ramAllowed;
int g_leftEdgeX;
int g_swapArmedRivalValue;
struct Bitmap far *g_spritesOpp[13];
int g_edgeXR;


static void near move_player(void)
{
    g_trackViewMotion.roadFraction += g_trackViewMotion.distanceStep =
        g_driveEng.speed ? ra_speed_step(g_driveEng.speed) : 0;
    if (g_trackViewMotion.roadFraction >= 8)
        rd_advance();
    if (g_trackViewMotion.roadIndex >= g_finishLineIndex && g_statsDone && g_driveEng.speed <= 0) {
        if (g_finishAheadPlayerState)
            g_oppEngState.stopFlags |= STOP_LOST;
        else
            g_driveEng.stopFlags |= STOP_LOST;
        sy_stop_drive();
    }
}


static void near move_rival(void)
{
    int flag;
    int slope;
    int t;
    int dist;

    g_roadOppPos.distanceStep = g_oppEngState.speed ? ra_speed_step(g_oppEngState.speed) : 0;
    if (g_oppEngState.speed == 0)
        g_roadOppPos.distanceStep = 0;
    g_roadOppPos.roadFraction += g_roadOppPos.distanceStep;
    if (g_roadOppPos.roadFraction >= 8 && g_roadOppPos.distanceStep > 0) {
        g_roadOppPos.roadIndex += g_roadOppPos.roadFraction >> 3;
        g_roadOppPos.roadFraction &= 7;
    }
    if (g_roadOppPos.roadIndex >= g_finishLineIndex && g_statsDone && g_driveEng.speed <= 0) {
        if (g_finishAheadPlayerState == 0)
            g_driveEng.stopFlags |= STOP_LOST;
        else
            g_oppEngState.stopFlags |= STOP_LOST;
        sy_stop_drive();
        return;
    }
    flag = g_terrain3[g_roadOppPos.roadIndex + 4] & 0xe0;
    dist = g_roadOppPos.roadIndex - g_trackViewMotion.roadIndex;
    slope = g_terrain2[g_trackViewMotion.roadIndex + 4] - g_terrain2[g_trackViewMotion.roadIndex + 5];
    if (dist > 12) {
        g_targetLane = (g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1;
        if (slope)
            g_rivalLeft = slope < 0;
        g_swapArmedRivalValue = 1;
    } else if (dist > 6) {
        g_ramAllowed = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag          ? (g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1
                           : g_rivalLeft ? ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200
                                         : ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
    } else if ((g_processorSpeedRating < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else if (flag)
            g_targetLane = (g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1;
        else if (g_swapArmedRivalValue) {
            if (g_rivalLeft)
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
            else
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200;
            g_swapArmedRivalValue = 0;
            g_rivalLeft = !g_rivalLeft;
        }
    } else if (dist > -4) {
        if (g_roadOppPos.distanceStep > g_trackViewMotion.distanceStep) {
            if (g_picOpponent >= 9) {
                if ((t = g_trackViewMotion.lateralX - 330) > g_leftEdgeX + 480) {
                    g_rivalLeft = 1;
                    g_targetLane = t;
                    if (g_trackViewMotion.lateralX - g_roadOppPos.lateralX < 280) {
                        g_roadOppPos.roadIndex = g_trackViewMotion.roadIndex - 1;
                        fit_gear(g_driveEng.speed, &g_oppEngState);
                    }
                } else {
                    if ((t = g_trackViewMotion.lateralX + 430) < g_edgeXR - 30) {
                        g_rivalLeft = 0;
                        g_targetLane = t + 200;
                        if (g_roadOppPos.lateralX - g_trackViewMotion.lateralX < 580) {
                            g_roadOppPos.roadIndex = g_trackViewMotion.roadIndex - 2;
                            fit_gear(g_driveEng.speed, &g_oppEngState);
                        }
                    } else {
                        g_roadOppPos.roadIndex = g_trackViewMotion.roadIndex - 2;
                        fit_gear(g_driveEng.speed, &g_oppEngState);
                    }
                }
            } else if (g_picOpponent == 0 || g_picOpponent == 2) {
                g_targetLane = g_trackViewMotion.lateralX > g_roadOppPos.lateralX ? g_trackViewMotion.lateralX - 330
                                                                                  : g_trackViewMotion.lateralX + 530;
            }
        } else if (g_picOpponent == 0)
            g_targetLane = g_trackViewMotion.lateralX - 330;
    } else if (dist > -6) {
        g_ramAllowed = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag          ? (g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1
                           : g_rivalLeft ? ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) - 200
                                         : ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) + 300;
    } else {
        g_targetLane = (g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1;
        g_swapArmedRivalValue = 1;
    }
    if (g_roadOppPos.lateralX > g_targetLane) {
        dist = (g_roadOppPos.lateralX - g_targetLane) >> 3;
        if (g_roadOppPos.lateralX - dist > g_targetLane)
            g_roadOppPos.lateralX -= dist;
    } else {
        dist = (g_targetLane - g_roadOppPos.lateralX) >> 3;
        if (dist + g_roadOppPos.lateralX < g_targetLane)
            g_roadOppPos.lateralX += dist;
    }
}


static void near cop_appears(void)
{
    int n1;
    int old;

    old = g_oppEngState.speed;
    g_selectedRacerPointer = &g_racerRecordsArray[RACER_COP_ENTRY];
    pf_drivetrain_spec(0x19, &g_oppEngState);
    g_shiftPenalty = g_selectedRacerPointer->shiftPenalty;
    fit_gear(old, &g_oppEngState);
    cr_rival_paint(5);
    for (n1 = 0; n1 < 12; n1++)
        g_spritesOpp[n1] = g_policeFramesTable[n1];
    g_rivalPace.topSpeed = pf_vmax(0x19);
    g_rivalPace.slowSpeed = (int)(g_rivalPace.topSpeed * 0.6);
    g_rivalPace.fastSpeed = (int)(g_rivalPace.topSpeed * 0.8);
}

int g_carHit;
int g_carCollisionRange;


void ra_tick(void)
{
    int a1, b, c1, d;

    if (g_driveEng.autoSelector) {
        if (g_driveEng.autoSelector != g_shownShiftPosition)
            sc_show_gear((g_shownShiftPosition = g_driveEng.autoSelector) + 4);
    } else if (g_driveEng.gearIndex != g_shownShiftPosition)
        sc_show_gear(g_shownShiftPosition = g_driveEng.gearIndex);
    if (g_driveEng.tireFailureIndex && g_trackViewMotion.roadIndex > g_driveEng.tireFailureIndex)
        g_failureTire = 1;
    if (g_squealOn == 0) {
        if (abs(g_driveEng.steering) >= 12)
            g_squealOn = 1;
    } else if (abs(g_driveEng.steering) < 10)
        g_squealOn = 0;
    rival_accelerate();
    switch (g_pursuitPhaseIndex) {
    case COP_WAITING:
        if (g_trackViewMotion.roadIndex >= g_idxAtCop) {
            if (g_roadOppPos.roadIndex + 5 <= g_trackViewMotion.roadIndex) {
                g_pursuitPhaseIndex++;
                cop_appears();
            } else
                g_pursuitPhaseIndex = COP_ABSENT;
        }
        break;
    case COP_APPROACHING:
        if (g_roadOppPos.roadIndex + 15 >= g_trackViewMotion.roadIndex) {
            g_pursuitPhaseIndex++;
            so_siren(1);
        }
        break;
    case COP_PURSUING:
        if (g_roadOppPos.roadIndex - 2 > g_trackViewMotion.roadIndex) {
            g_driveEng.stopFlags |= (STOP_LOST | STOP_POLICE);
            sy_stop_drive();
            return;
        }
        break;
    }
    a1 = g_trackViewMotion.roadIndex * 8 + g_trackViewMotion.roadFraction;
    b = g_roadOppPos.roadIndex * 8 + g_roadOppPos.roadFraction;
    c1 = g_trackViewMotion.lateralX;
    d = g_roadOppPos.lateralX;
    move_player();
    move_rival();
    if (g_trackViewMotion.roadIndex >= g_finishLineIndex || g_roadOppPos.roadIndex >= g_finishLineIndex)
        ra_finish_stats();
    if (g_driveEng.stopFlags || g_oppEngState.stopFlags) {
        if (g_pursuitPhaseIndex >= COP_APPROACHING) {
            if (g_driveEng.stopFlags)
                g_driveEng.stopFlags |= (STOP_LOST | STOP_POLICE);
            so_siren(0);
        }
        sy_stop_drive();
        return;
    }
    if (g_carCollisionRange == 0)
        return;
    if (g_demo)
        return;
    if ((g_driver.raceType == RACE_DRAG ? 0x17c : 0x3fc) <= g_trackViewMotion.roadIndex)
        return;
    if (g_carHit && g_pursuitPhaseIndex <= COP_WAITING) {
        g_driveEng.stopFlags |= (STOP_LOST | STOP_COLLISION);
        sy_stop_drive();
        return;
    }
    if (g_picOpponent == 0 && g_trackViewMotion.distanceStep > 1) {
        g_contactFrames++;
        if (g_driveEng.steeringInput < 0) {
            if (g_terrain2[g_trackViewMotion.roadIndex] - g_terrain2[g_trackViewMotion.roadIndex + 1] >= 0 ||
                ((g_edgesRoad.leftEdge + g_trackViewMotion.rightEdge) >> 1) >= g_trackViewMotion.lateralX) {
                so_bump();
                if (g_bumpAlternate == 0) {
                    g_roadOppPos.roadIndex--;
                    g_bumpAlternate = 1;
                } else
                    g_bumpAlternate = 0;
            }
            return;
        }
        if (g_ramAllowed && g_contactFrames > 3 && g_pursuitPhaseIndex < COP_APPROACHING) {
            if (rn_int((g_selectedRacerPointer->drivingPenalty >> 2) + 2) == 0) {
                so_bump();
                so_bump();
                g_trackViewMotion.lateralX += 300;
                if (g_driveEng.speed > 20)
                    g_brakeTicks = 2;
                g_ramAllowed = 0;
                g_trackViewMotion.curveX += rn_int(2) ? 15 : -15;
                so_bump();
            }
            g_contactFrames = 0;
        }
    } else if (g_picOpponent)
        g_contactFrames = 0;
}
