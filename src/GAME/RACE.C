#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "STRUCTS.H"
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
#include "ROAD.H"
#include "SAVEGAME.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct GasInteraction g_fuelStationActions; /* DS:6CA4 */
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
static int g_finishLineIndex;                       /* DS:6CAA */
static struct RivalPace g_rivalPace; /* DS:6CAC */
static int g_shiftPenalty; /* DS:6CB4 */
static int g_targetLane;                         /* DS:6CB6 */
static int g_rpmBand;                         /* DS:6CB8 */
static int g_shiftPoint;                         /* DS:6CBA */
static void near init_rival_eng(void);
static void near fit_gear(int rpm, struct Eng *p);
static void near rival_accelerate(void);
static void near move_player(void);
static void near move_rival(void);
static void near cop_appears(void);

int g_gasSpotIds[6] = {2, 1, 4, 6, 5, 3};   /* DS:53B6 */

/* find_gas_spot @ 0000:B3CA (linear 0x0B3CA, 91 bytes) */

static int near find_gas_spot(int id)
{
    int base, n, i;

    base = ((unsigned)g_hotGasStation - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotGasExit - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    for (i = 0; i < n; i++)
        if (g_hots[base + i].id == id)
            return base + i;
}

/* sort_gas_spots @ 0000:B425 (linear 0x0B425, 49 bytes) */

static void near sort_gas_spots(void)
{
    int base, n;

    base = ((unsigned)g_hotGasStation - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_hotGasExit - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    gx_sort_hots(base, n);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int g_finishMeanSpeedVal;
char g_failureTire;
struct BodyPoints g_featurePos; /* DS:8264 */
int g_steeringOld;
int dz82c8x;
int g_finishAheadPlayerState;
float g_raceSecsElapsedValue;

/* clear_gas_spots @ 0000:B456 (linear 0x0B456, 70 bytes) */

static void near clear_gas_spots(void)
{
    int k;

    g_fuelStationActions.ragAvailable = 0;
    g_fuelStationActions.nozzleAvailable = 0;
    g_fuelStationActions.capAvailable = 0;
    for (k = 0; k < 6; k++)
        g_hots[find_gas_spot(g_gasSpotIds[k])].kind = HOT_DISABLED;
}

/* place_gas_spot @ 0000:B49C (linear 0x0B49C, 66 bytes) */

static void near place_gas_spot(int id, int x, int y, int w, int h)
{
    struct Hot *p;
    p = &g_hots[find_gas_spot(id)];
    p->x2 = (p->x1 = x) + w - 1;
    p->y2 = (p->y1 = y) + h - 1;
    p->kind = HOT_REGION;
}

/* spots_idle @ 0000:B4DE (linear 0x0B4DE, 71 bytes) */

static void near spots_idle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x3f, 100, 12, 23);
    place_gas_spot(1, 0x54, 0xa8, 0x1f, 15);
    sort_gas_spots();
    g_fuelStationActions.ragAvailable = 1;
    g_fuelStationActions.nozzleAvailable = 1;
}

/* spots_cap @ 0000:B525 (linear 0x0B525, 45 bytes) */

static void near spots_cap(void)
{
    clear_gas_spots();
    place_gas_spot(4, g_featurePos.gasCapX - 10, g_featurePos.gasCapY - 5, 18, 12);
    sort_gas_spots();
    g_fuelStationActions.capAvailable = 1;
}

/* spots_nozzle @ 0000:B552 (linear 0x0B552, 73 bytes) */

static void near spots_nozzle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x34, 100, 16, 23);
    place_gas_spot(4, g_featurePos.gasCapX - 1, g_featurePos.gasCapY - 1, 10, 9);
    sort_gas_spots();
    g_fuelStationActions.nozzleAvailable = 1;
    g_fuelStationActions.capAvailable = 1;
}

/* spots_rag @ 0000:B59B (linear 0x0B59B, 124 bytes) */

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

/* spots_none @ 0000:B617 (linear 0x0B617, 4 bytes) */

static void near spots_none(void)
{
    clear_gas_spots();
}

/* show_tank_cap @ 0000:B61B (linear 0x0B61B, 252 bytes) */

static void near show_tank_cap(struct Dlg *d, int id)
{
    struct Bitmap far *back, far *label, far *work;
    struct Blt box;

    if (d->f14)
        back = d->bm1;
    else
        back = d->bm2;
    cu_ctl(CURSOR_HIDE);
    label = di_bm(id, ARENA_HIGH_STACK);
    cu_ctl(CURSOR_SHOW);
    work = ar_bm(back->width, back->height, 0, ARENA_HIGH_STACK);
    bm_copy(back, work, 1);
    box.w = label->width;
    box.h = box.fc = label->height;
    box.sx = box.sy = 0;
    box.dx = (~r_vid.mask & g_featurePos.gasCapX) - box.w + 24;
    box.dy = 12 - label->height;
    bm_draw_masked(label, work, &box, 0);
    pu_show(work, g_featurePos.gasCapX - 16, g_featurePos.gasCapY - 5);
    ma_pop_high(2);
}

/* leave_station @ 0000:B717 (linear 0x0B717, 394 bytes) */

static void near leave_station(struct Dlg *d)
{
    struct Bitmap far *p;

    if (d->f0c != 1)
        cu_ctl(1);
    if (d->f12) {
        show_tank_cap(d, 0x472);
        if (d->f18) {
            sc_add_money(a_7eaap.money >= 10 ? -10 : -(int)a_7eaap.money);
            d->f18 = 0;
        }
    }
    if (d->f14 == 0) {
        pu_show(g_pumpWithNozzle, 0x40, 0x64);
        if (d->f18)
            sc_add_money(a_7eaap.money >= 10 ? -10 : -(int)a_7eaap.money);
    }
    if (d->f10 == 0) {
        cu_ctl(CURSOR_HIDE);
        p = di_bm(0x4d1, ARENA_HIGH_STACK);
        cu_ctl(CURSOR_SHOW);
        cp_patch_bg(p, 0x60, 0, 0x30, 0x40, 0xc8, 0x51, 0xa9, 2);
    }
    a_7eaap.currentCar->fuel = a_7eaap.currentCar->fuel > 0xaa ? 0xaa : a_7eaap.currentCar->fuel;
    cp_show_car(CAR_DRIVE_OUT, d->specIndex, d->bodyMods, (a_7eaap.currentCar->appearance >> 8) & 0x1f, 0, 0xa9, 1,
            g_tirePics[g_parts[a_7eaap.currentCar->tires->idx].variant]);
    mn_pop(1);
    ma_clear();
    g_scene = 1;
    cr_go();
}

/* ra_gas_station @ 0000:B8A1 (linear 0x0B8A1, 1587 bytes) */

void ra_gas_station(void)
{
    struct Bitmap far *label;
    struct Dlg dl;
    int pos;
    int wait1;
    int done;
    int key;

    sc_status_text(-1, "\0", 0);
    g_scene = 3;
    sc_picture(0x442, -1);
    sc_status_label(g_bankrollLabel);
    r_vid.copyToFront = 1;
    (*g_videoOps.show)();
    dl.f0c = 1;
    cu_ctl(1);
    dl.specIndex = a_7eaap.currentCar->specIndex;
    dl.bodyMods = gm_car_look(a_7eaap.currentCar);
    cp_show_car(CAR_PULL_IN, dl.specIndex, dl.bodyMods, (a_7eaap.currentCar->appearance >> 8) & 0x1f, 0, 0xa9, 1,
            g_tirePics[g_parts[a_7eaap.currentCar->tires->idx].variant]);
    mu_suspend(0);
    label = pu_grab(0x50, 0xaa, 0x28, 0xc, 0);
    dl.bm1 = pu_grab(g_featurePos.gasCapX - 0x10, g_featurePos.gasCapY - 5, 0x18, 0xc, 0);
    r_vid.copyToFront = 0;
    pu_show(g_pumpNoNozzle, 0x40, 0x64);
    dl.bm2 = pu_grab(g_featurePos.gasCapX - 0x10, g_featurePos.gasCapY - 5, 0x18, 0xc, 0);
    pu_show(g_pumpWithNozzle, 0x40, 0x64);
    r_vid.copyToFront = 1;
    pos = 1;
    dl.f1a = 0;
    wait1 = 0x28;
    done = 0;
    dl.f12 = 0;
    dl.f14 = 1;
    dl.f10 = 1;
    dl.f16 = 0;
    dl.f18 = 1;
    dl.fillsLeft = (0xbe - a_7eaap.currentCar->fuel) / 0x1e;
    show_tank_cap(&dl, 0x472);
    spots_idle();
    mn_push(KM_GAS_STATION);
    cu_ctl(CURSOR_SHOW_NOW);
top:
    g_waitUntil = g_tick + 20;
    for (;;) {
        while ((key = sy_wait_input(3000)) == 0)
            ;
        switch (key) {
        case 1000:
            if (dl.f0c == 3)
                goto top;
            if (dl.f1a) {
                dl.fillsLeft--;
                a_7eaap.currentCar->fuel += 0x1e;
                done = 1;
                sc_add_money(-(pos <= a_7eaap.money ? pos : (int)a_7eaap.money));
                if (dl.fillsLeft == 0 || pos > a_7eaap.money) {
                    if (vi_anim_active(2)) {
                        so_pump(0);
                        vi_anim_stop(2);
                    }
                    if (dl.f10)
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
                dl.f16 = 1;
                gm_play_anim(3);
                gm_message(0x19e1);
                dl.f10 = 0;
                if (done)
                    goto fin;
                dl.f18 = 0;
                if (dl.f0c == 1) {
                    pu_show(g_pumpNoNozzle, 0x40, 0x64);
                    cu_ctl(dl.f0c = 2);
                    dl.f14 = 0;
                }
                show_tank_cap(&dl, 0x471);
                dl.f12 = 1;
                cu_ctl(dl.f0c = 1);
                vi_anim_start(2);
                so_pump(1);
                dl.f1a = 1;
            }
            g_waitUntil = g_tick + 0x4c;
            break;
        case 1:
            if (g_fuelStationActions.ragAvailable == 0)
                break;
            if (dl.f0c == 1) {
                cu_ctl(dl.f0c = 3);
                sc_picture_at(0x473, 0x50, 0xaa, -1);
                mn_pop(1);
                spots_rag();
        redraw:
                mn_push(KM_GAS_STATION);
                goto top;
            } else {
                cu_ctl(dl.f0c = 1);
                pu_show(label, 0x50, 0xaa);
                mn_pop(1);
                spots_idle();
                goto redraw;
            }
        case 2:
            if (g_fuelStationActions.nozzleAvailable == 0)
                break;
            if (dl.f0c == 1) {
                cu_ctl(dl.f0c = 2);
                pu_show(g_pumpNoNozzle, 0x40, 0x64);
                dl.f14 = 0;
            both:
                mn_pop(1);
                spots_nozzle();
            } else {
                if (pos > a_7eaap.money || done) {
                    dl.f18 = 0;
                    goto fin;
                }
                cu_ctl(dl.f0c = 1);
                pu_show(g_pumpWithNozzle, 0x40, 0x64);
                dl.f14 = 1;
                mn_pop(1);
                spots_idle();
            }
            mn_push(KM_GAS_STATION);
        tail:
            g_waitUntil = g_tick + 20;
            dl.f1a = 0;
            break;
        case 4:
            if (g_fuelStationActions.capAvailable == 0)
                break;
            if (dl.f0c == 1) {
                if (vi_anim_active(2)) {
                    so_pump(0);
                    vi_anim_stop(2);
                }
                show_tank_cap(&dl, 0x472);
                dl.f12 = 0;
                cu_ctl(dl.f0c = 2);
                if (dl.fillsLeft > 0 && pos <= a_7eaap.money)
                    gm_message(0x18dd);
                goto both;
            }
            if (dl.fillsLeft == 0 || pos > a_7eaap.money)
                gm_message(0x10b5);
            else {
                show_tank_cap(&dl, 0x471);
                dl.f12 = 1;
                cu_ctl(dl.f0c = 1);
                vi_anim_start(2);
                so_pump(1);
                mn_pop(1);
                spots_cap();
                mn_push(KM_GAS_STATION);
                g_waitUntil = g_tick + 0x4c;
                dl.f1a = 1;
            }
            break;
        case 3:
            gm_message(0x10f6);
            continue;
        case 5:
            break;
        case 6:
            break;
        }
    }
}

/* replace_tires @ 0000:BED4 (linear 0x0BED4, 192 bytes) */

static int near replace_tires(void)
{
    struct Node *tbl[142];
    int ret;

    ret = 0;
    if (a_7eaap.currentCar == 0)
        return 0;
    a_7eaap.currentCar->value -= gm_take_part(a_7eaap.currentCar->tires, 0);
    a_7eaap.currentCar->tires = 0;
    ga_spares(4, tbl, 0, 0);
    if (g_list[0]) {
        ga_install(ga_unlink(tbl[1]), &a_7eaap.currentCar->tires);
    } else {
        a_7eaap.money -= 0x32;
        if (a_7eaap.money < 0) {
            ret = (int)-a_7eaap.money;
            a_7eaap.money = 0;
        }
        gm_add_part(0x28, g_parts[40].value);
        tbl[0] = a_7eaap.spareParts;
        a_7eaap.spareParts = tbl[0]->next;
        ga_install(tbl[0], &a_7eaap.currentCar->tires);
    }
    return ret;
}

int g_winMsgs[6] = {4973, 5015, 5073, 5043, 5099, 5130};   /* DS:53C8 */
struct Spot g_crashPics[3] = {{1264, 64, 84}, {1266, 208, 82}, {1265, 272, 123}};   /* DS:53D4 */
int g_statsDone = 0;   /* DS:53E6 */

/* show_ticket @ 0000:BF94 (linear 0x0BF94, 580 bytes) */

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
    a = di_bm(0x456, ARENA_LOW_STACK);
    b = di_bm(0x4ef, ARENA_LOW_STACK);
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
    desc3 = g7d86n[a_7eaap.currentCar->specIndex].nameOffset;
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

/* show_crash @ 0000:C1D8 (linear 0x0C1D8, 559 bytes) */

static void near show_crash(void)
{
    struct Bitmap far *a, far *b;
    struct Blt pkt;
    int fast, i;

    fast = r_vid.mode == VID_EGA && g_vga == 0;
    so_crash();
    ma_clear();
    a = di_bm(0x4eb, ARENA_LOW_STACK);
    b = di_bm(0x4ec, ARENA_LOW_STACK);
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
            da_wait_retrace(0x14 - 8 * i);
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

/* ra_finish_stats @ 0000:C407 (linear 0x0C407, 144 bytes) */

void ra_finish_stats(void)
{
    if (g_statsDone == 0) {
        if ((ar7e8av.roadIndex - ao7d2ev.roadIndex) * 8 + ar7e8av.roadFraction <= ao7d2ev.roadFraction)
            g_finishAheadPlayerState = 1;
        else
            g_finishAheadPlayerState = 0;
        g_finishMeanSpeedVal = g_mphAccumulator / g_driveEng.elapsedTicks;
        g_raceSecsElapsedValue = g_driveEng.elapsedTicks / 6.068827333333334;
        dz82c8x = (int)((ao7d2ev.roadIndex - 200) * 100.0 / (ar7e8av.roadIndex - 200)) > 99 ? 99 : (int)((ao7d2ev.roadIndex - 200) * 100.0 / (ar7e8av.roadIndex - 200));
        g_statsDone = 1;
    }
}

/* show_race_stats @ 0000:C497 (linear 0x0C497, 149 bytes) */

static void near show_race_stats(void)
{
    if (g_resultWin) {
        sprintf(g_raceResult1, "Your``winning``time %2.1f sec.,`avg. speed %2d mph", g_raceSecsElapsedValue, g_finishMeanSpeedVal);
        strcpy(g_raceResult2, "Congratulations !");
    } else {
        sprintf(g_raceResult1, "In %2.1f sec you covered only %2d%%", g_raceSecsElapsedValue, dz82c8x);
        sprintf(g_raceResult2, "of the distance. Your avg. speed %2d mph.", g_finishMeanSpeedVal);
    }
    mn_push(KM_RACE_STATS);
    mn_pop(1);
}

/* blow_engine @ 0000:C52C (linear 0x0C52C, 128 bytes) */

static void near blow_engine(int n)
{
    struct Node **p;
    char *q;
    int i;

    if (a_7eaap.currentCar) {
        for (i = 0, p = a_7eaap.currentCar->rack.parts; i < 5; i++, p++) {
            a_7eaap.currentCar->value -= gm_take_part(*p, 0);
            *p = 0;
        }
        for (i = 0, q = a_7eaap.currentCar->engineBolts; i < 9; i++)
            *q++ = 0;
        a_7eaap.currentCar->wireState = 0;
    }
    sy_reset_queues();
    so_noise();
    gm_popup(-n, -1, 0x49);
}

/* drop_transmission @ 0000:C5AC (linear 0x0C5AC, 103 bytes) */

static void near drop_transmission(int n)
{
    char *q;
    int i;

    if (a_7eaap.currentCar) {
        a_7eaap.currentCar->value -= gm_take_part(a_7eaap.currentCar->transmission, 0);
        a_7eaap.currentCar->transmission = 0;
        for (i = 0, q = a_7eaap.currentCar->transmissionBolts; i < 2; i++)
            *q++ = 0;
    }
    sy_reset_queues();
    so_noise();
    gm_popup(-n, -1, 0x49);
}

int g_tireSlip[3][5] = {{2, 5, 8, 11, 14}, {0, 2, 4, 6, 8}, {0, 0, 1, 2, 4}};   /* DS:54B2 */
int g_lastTicks = 0;   /* DS:54D0 */
int g_bumpAlternate = 0;   /* DS:54D2 */
int g_rivalLeft = 1;   /* DS:54D4 */
int g_rivalCurveBrake = 0;   /* DS:54D6 */
int g_contactFrames = 0;   /* DS:54D8 */
int g_brakeTicks = 0;   /* DS:54DA */

/* ra_outcome @ 0000:C613 (linear 0x0C613, 1948 bytes) */

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
    engine = g_driveEng.stopFlags & 4;
    opt = g_driveEng.stopFlags & 0x10;
    stop = g_driveEng.stopFlags == 5 || g_driveEng.stopFlags == 0x11 || g_driveEng.stopFlags == 0x15;
    if (a_7eaap.currentCar->transmission->wear > 9900)
        a_7eaap.currentCar->transmission->wear = 9900;
    if (a_7eaap.currentCar->rack.parts[0]->wear > 9900)
        a_7eaap.currentCar->rack.parts[0]->wear = 9900;
    if (a_7eaap.currentCar->tires->wear > 9900)
        a_7eaap.currentCar->tires->wear = 9900;
    g_resultWin = 0;
    g_statsDone = 0;
    g_mphAccumulator = 0;
    g_fireLatch = 0;
    g_buttons = 0;
    if (g_driveEng.automatic)
        g_driveEng.autoSelector = 1;
    if (g_failureTire)
        g_driveEng.stopFlags |= 8;
    mn_push(KM_RACE_OUTCOME);
    i = a_7eaap.raceType == 0 ? 5 : 15;
    if (a_7eaap.currentCar->fuel > i)
        a_7eaap.currentCar->fuel -= i;
    else
        a_7eaap.currentCar->fuel = 0;
    (*g_videoOps.show)();
    if (g_oppEngState.stopFlags & 1) {
        g_resultWin = 1;
        show_race_stats();
        cu_ctl(CURSOR_SHOW_NOW);
        gm_message(g_isKingState == 0 ? (i = g_winMsgs[kind]) : a_7eaap.raceType == 0 ? 0x14bd : 0x14d6);
        sc_add_money(v);
        if (kind == BET_CAR) {
            found = gm_make_car(bn7648, g_oppSpecPointer->price, 0);
            found->tires->wear = rn_int(500);
            found->appearance = ((g_selectedRacerPointer->sticker & 0x1f) << 8) | (found->appearance & 0xe0ff);
            found->appearance = (((g_selectedRacerPointer->bodyMods & 4) != 0 & 1) << 13) | (found->appearance & 0xdfff);
            found->appearance = (((g_selectedRacerPointer->bodyMods & 2) != 0 & 1) << 14) | (found->appearance & 0xbfff);
            found->appearance = (((g_selectedRacerPointer->bodyMods & 1) != 0 & 1) << 15) | (found->appearance & 0x7fff);
            found->paintScheme = g_selectedRacerPointer->paintScheme;
            if (found->rank > a_7eaap.currentCar->rank) {
                a_7eaap.currentCar->next = a_7eaap.otherCars->next;
                a_7eaap.otherCars = a_7eaap.currentCar;
                a_7eaap.currentCar = found;
                pf_drivetrain(a_7eaap.currentCar, &g_driveEng);
            }
            if (g_demo == 0 || g_driverIdxSelVal != g_demoRacer) {
                if (g_selectedRacerPointer->lostCarOnce != 0 || g_isKingState != 0)
                    g_selectedRacerPointer->status = RACER_EMPTY;
                else {
                    for (i = 0, units = 0; i < 25; i++)
                        if (g7d86n[i].rank < g_oppSpecPointer->rank)
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
        if (g_driveEng.stopFlags & 8)
            if ((i = replace_tires()) != 0)
                goto ret2;
        if (kind == BET_CAR)
            return 1;
        return 3;
    } else if (g_driveEng.stopFlags & 0x40) {
        so_siren(0);
        show_ticket();
        i = (g_driveEng.stopFlags & 8) ? replace_tires() : 0;
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
        if (i <= a_7eaap.money)
            goto pay;
        gm_message(0x1793);
        mn_pop(1);
        mn_pop(1);
        sc_add_money(-a_7eaap.money);
        sc_status_text(-1, " ", SCREEN_WIDTH - g_statusLine.messageX);
        return 4;
pay:
        sc_add_money(-i);
        cost = g_waitUntil;
        gm_pause(g_msgTicks);
        g_waitUntil = cost;
        mn_pop(1);
        sc_status_text(-1, "", SCREEN_WIDTH - g_statusLine.messageX);
    } else if (g_driveEng.stopFlags == 0x81) {
        cu_ctl(CURSOR_SHOW_NOW);
        gm_message(0xecb2);
    } else {
        if (kind == BET_CAR) {
            gm_scrap_car(a_7eaap.currentCar, 0);
            a_7eaap.currentCar = 0;
        }
        sc_add_money(-v);
        if (g_driveEng.stopFlags == 1) {
            show_race_stats();
            cu_ctl(CURSOR_SHOW_NOW);
            i = kind == BET_CAR ? 0x143a : 0x1427;
            gm_message(g_isKingState == 0 ? i : a_7eaap.raceType == 0 ? 0x1450 : 0x146c);
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
                if (g_driveEng.stopFlags & 8) {
                    gm_message(0x16f6);
                    replace_tires();
                } else
                    gm_message(0x1658);
            } else if (g_driveEng.speed > 0x5a) {
                v = gm_scrap_car(a_7eaap.currentCar, 0) / 10 + 0x13;
                a_7eaap.currentCar = 0;
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
                    if (g_driveEng.stopFlags & 8) {
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
                all = g_driveEng.stopFlags & 8;
                if (all)
                    strcpy(g_msgPlusTires, "Plus tires !");
                else
                    g_msgPlusTires[0] = 0;
                sel = i ? KM_GET_RUNNING : KM_REPAIR;
                if ((long)v > a_7eaap.money) {
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
                    sc_add_money((int)(a_7eaap.currentCar->value * 0.4));
                    a_7eaap.currentCar = 0;
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

struct State70C0 al70c0x;
int g_idxAtCop;
int a_7f88d;
int bz8630z;

/* ra_reset_eng @ 0000:CDAF (linear 0x0CDAF, 429 bytes) */

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
    p->stopFlags = p->fbe = p->elapsedTicks = 0;
    if (p->automatic) {
        p->gear[2].bands[1].maxRpm = p->gear[2].bands[2].minRpm = p->shiftRpm;
        p->autoSelector = 1;
    }
    p->f28 = p->f26 = -1;
    p->rpm = 0x5dc;
    p->rpmPerSpeed = p->f2a;
    p->accelStep = p->gear[0].bands[0].accelStep;
    p->decelStep = p->gear[0].bands[0].decelStep;
    p->maxRpm = p->gear[0].bands[0].maxRpm;
    p->minRpm = p->gear[0].bands[0].minRpm;
    g_05d6 = 0;
    g_05d8 = g_05da = 0;
    bz8630z = -1;
    g_finishLineIndex = a_7eaap.raceType == 0 ? 0x17c : 0x3fc;
    car = a_7eaap.currentCar->tires;
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
    g_rivalPace.topSpeed = pf_vmax(bn7648);
    g_rivalPace.slowSpeed = (int)(g_rivalPace.topSpeed * 0.7);
    g_rivalPace.fastSpeed = (int)(g_rivalPace.topSpeed * 0.8);
    g_rivalPace.speedMargin = bu8acc < 4 ? 10 : 16;
    g_bumpAlternate = 0;
    g_statsDone = 0;
}

/* init_rival_eng @ 0000:CF5C (linear 0x0CF5C, 213 bytes) */

static void near init_rival_eng(void)
{
    struct Range *r;

    pf_drivetrain_spec(bn7648, &g_oppEngState);
    g_shiftPenalty = g_selectedRacerPointer->shiftPenalty;
    ra_reset_eng(&g_oppEngState);
    g_oppEngState.gearIndex = 0;
    g_oppEngState.rpmPerSpeed = g_oppEngState.f34[0];
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
    ar7e8av.roadIndex = 200;
    ar7e8av.roadFraction = 0;
    g_targetLane = ar7e8av.lateralX = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200;
}

/* ra_run @ 0000:D031 (linear 0x0D031, 429 bytes) */

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
    if (a_7eaap.raceType == 1 && g_isKingState == 0) {
        if (a_7f88d = !rn_int(4))
            g_idxAtCop = rn_int(255) + 0x1fe;
    } else
        a_7f88d = 0;
    g_phaseDriveRunState = 0;
    bw8aca = 1;
    cr_drive();
    sy_drive_stop();
    g_waitUntil = 0;
    sy_reset_queues();
    sc_status_label(g_bankrollLabel);
    if (ao7d2ev.roadIndex > 200) {
        if (rn_int(3) > 0) {
            if (a_7eaap.currentCar->timing > 0)
                a_7eaap.currentCar->timing = a_7eaap.currentCar->timing < rn_int(4) ? rn_int(4) : a_7eaap.currentCar->timing;
            else if (a_7eaap.currentCar->timing < 0)
                a_7eaap.currentCar->timing = -rn_int(8) < a_7eaap.currentCar->timing ? -rn_int(8) : a_7eaap.currentCar->timing;
            else
                a_7eaap.currentCar->timing = rn_int(8) - 4;
        }
        slot = a_7eaap.currentCar->tires;
        slot->wear += (g_parts[slot->idx].variant == 0 ? (a_7eaap.raceType == 1 ? 700 : 300) : (a_7eaap.raceType == 1 ? 400 : 200)) + rn_int(300);
        if (slot->wear > 10000)
            slot->wear = 10000;
    }
}

/* fit_gear @ 0000:D1DE (linear 0x0D1DE, 191 bytes)

   Steps p->f18 up from 1 until gear[f18].r[1].f2 exceeds f34[f18] * rpm or
   f18 reaches f1c. The four pointers the loop advances in memory are /Ol induction temps for the p->f18 indexing. */

static void near fit_gear(int rpm, struct Eng *p)
{
    struct Range *r;

    for (p->gearIndex = 1; ; p->gearIndex++) {
        p->rpmPerSpeed = p->f34[p->gearIndex];
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

/* rival_accelerate @ 0000:D29D (linear 0x0D29D, 679 bytes) */

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
        if (g_terrain2[ar7e8av.roadIndex] != g_terrain2[ar7e8av.roadIndex + 1]) {
            if (g_rivalCurveBrake == 0) {
                g_rivalCurveBrake = 1;
                vl = g_rivalPace.topSpeed - (ar7e8av.roadIndex > ao7d2ev.roadIndex ? g_selectedRacerPointer->f2 : g_selectedRacerPointer->f2 >> 1);
                if (vl < g_oppEngState.speed)
                    fit_gear(vl, &g_oppEngState);
            }
            continue;
        }
        g_rivalCurveBrake = 0;
        d = ar7e8av.roadIndex - ao7d2ev.roadIndex;
        if (d > 12 && (a_7eaap.raceType != 1 || g_isKingState == 0)) {
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
            if (ao7d2ev.roadIndex >= 30 && vl < g_rivalPace.slowSpeed)
                vl = g_rivalPace.slowSpeed;
            goto chk;
        }
        if (a_7f88d >= 2 && g_driveEng.speed > 50 && g_oppEngState.speed > g_driveEng.speed + 32)
            fit_gear(g_driveEng.speed + 32, &g_oppEngState);
        else {
            g_oppEngState.rpm += g_oppEngState.accelStep;
            if (g_oppEngState.rpm > g_shiftPoint)
                g_oppEngState.rpm = g_shiftPoint;
            if (g_oppEngState.rpm > g_oppEngState.maxRpm) {
                g_oppEngState.rpmBand++;
                g_oppEngState.maxRpm = (r = &g_oppEngState.gear[g_oppEngState.gearIndex].bands[g_oppEngState.rpmBand])->maxRpm;
                g_oppEngState.accelStep = r->accelStep;
            }
            if (g_oppEngState.gearIndex < g_oppEngState.gearCount &&
                ((g_oppEngState.autoSelector && g_oppEngState.rpm >= g_oppEngState.autoShiftRpm) || g_oppEngState.rpm >= g_shiftPoint)) {
                g_oppEngState.gearIndex++;
                if (g_oppEngState.autoSelector == 0)
                    g_shiftPoint = g_oppEngState.gearIndex < g_oppEngState.gearCount ? g_oppEngState.shiftRpm - 100 * g_shiftPenalty
                           : bu8acc < 4 && r_vid.mode != VID_TANDY ? 0x14b4 : 0x1518;
                g_oppEngState.rpmPerSpeed = g_oppEngState.f34[g_oppEngState.gearIndex];
                g_oppEngState.rpm = g_oppEngState.speed * g_oppEngState.rpmPerSpeed;
                if ((r = &g_oppEngState.gear[g_oppEngState.gearIndex].bands[g_rpmBand = 1])->maxRpm <= g_oppEngState.rpm) {
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
    if (ar7e8av.roadIndex >= g_finishLineIndex) {
        liml = ao7d2ev.roadIndex + 5 < ar7e8av.roadIndex ? 5 : 10;
        g_oppEngState.speed = old3;
        if (liml < old3)
            g_oppEngState.speed -= liml;
        else
            g_oppEngState.speed = 0;
    }
}

/* ra_speed_step @ 0000:D544 (linear 0x0D544, 224 bytes) */

int ra_speed_step(int rpm)
{
    if (rpm <= 0)
        return 0;
    if (bu8acc < 4) {
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

struct Bitmap far *ab7526x[13];
int ap7d7cv;
int g_ramAllowed;
int g_leftEdgeX;
int g_swapArmedRivalValue;
struct Bitmap far *g8acew[13];
int g_edgeXR;

/* move_player @ 0000:D624 (linear 0x0D624, 89 bytes) */

static void near move_player(void)
{
    ao7d2ev.roadFraction += ao7d2ev.distanceStep = g_driveEng.speed ? ra_speed_step(g_driveEng.speed) : 0;
    if (ao7d2ev.roadFraction >= 8)
        rd_advance();
    if (ao7d2ev.roadIndex >= g_finishLineIndex && g_statsDone && g_driveEng.speed <= 0) {
        if (g_finishAheadPlayerState)
            g_oppEngState.stopFlags |= 1;
        else
            g_driveEng.stopFlags |= 1;
        sy_stop_drive();
    }
}

/* move_rival @ 0000:D67D (linear 0x0D67D, 789 bytes) */

static void near move_rival(void)
{
    int flag;
    int slope;
    int t;
    int dist;

    ar7e8av.distanceStep = g_oppEngState.speed ? ra_speed_step(g_oppEngState.speed) : 0;
    if (g_oppEngState.speed == 0)
        ar7e8av.distanceStep = 0;
    ar7e8av.roadFraction += ar7e8av.distanceStep;
    if (ar7e8av.roadFraction >= 8 && ar7e8av.distanceStep > 0) {
        ar7e8av.roadIndex += ar7e8av.roadFraction >> 3;
        ar7e8av.roadFraction &= 7;
    }
    if (ar7e8av.roadIndex >= g_finishLineIndex && g_statsDone && g_driveEng.speed <= 0) {
        if (g_finishAheadPlayerState == 0)
            g_driveEng.stopFlags |= 1;
        else
            g_oppEngState.stopFlags |= 1;
        sy_stop_drive();
        return;
    }
    flag = g_terrain3[ar7e8av.roadIndex + 4] & 0xe0;
    dist = ar7e8av.roadIndex - ao7d2ev.roadIndex;
    slope = g_terrain2[ao7d2ev.roadIndex + 4] - g_terrain2[ao7d2ev.roadIndex + 5];
    if (dist > 12) {
        g_targetLane = (al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1;
        if (slope)
            g_rivalLeft = slope < 0;
        g_swapArmedRivalValue = 1;
    } else if (dist > 6) {
        g_ramAllowed = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag ? (al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1 : g_rivalLeft ? ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200 : ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
    } else if ((bu8acc < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else if (flag)
            g_targetLane = (al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1;
        else if (g_swapArmedRivalValue) {
            if (g_rivalLeft)
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
            else
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200;
            g_swapArmedRivalValue = 0;
            g_rivalLeft = !g_rivalLeft;
        }
    } else if (dist > -4) {
        if (ar7e8av.distanceStep > ao7d2ev.distanceStep) {
            if (ap7d7cv >= 9) {
                if ((t = ao7d2ev.lateralX - 330) > g_leftEdgeX + 480) {
                    g_rivalLeft = 1;
                    g_targetLane = t;
                    if (ao7d2ev.lateralX - ar7e8av.lateralX < 280) {
                        ar7e8av.roadIndex = ao7d2ev.roadIndex - 1;
                        fit_gear(g_driveEng.speed, &g_oppEngState);
                    }
                } else {
                    if ((t = ao7d2ev.lateralX + 430) < g_edgeXR - 30) {
                        g_rivalLeft = 0;
                        g_targetLane = t + 200;
                        if (ar7e8av.lateralX - ao7d2ev.lateralX < 580) {
                            ar7e8av.roadIndex = ao7d2ev.roadIndex - 2;
                            fit_gear(g_driveEng.speed, &g_oppEngState);
                        }
                    } else {
                        ar7e8av.roadIndex = ao7d2ev.roadIndex - 2;
                        fit_gear(g_driveEng.speed, &g_oppEngState);
                    }
                }
            } else if (ap7d7cv == 0 || ap7d7cv == 2) {
                g_targetLane = ao7d2ev.lateralX > ar7e8av.lateralX ? ao7d2ev.lateralX - 330 : ao7d2ev.lateralX + 530;
            }
        } else if (ap7d7cv == 0)
            g_targetLane = ao7d2ev.lateralX - 330;
    } else if (dist > -6) {
        g_ramAllowed = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag ? (al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1 : g_rivalLeft ? ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) - 200 : ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) + 300;
    } else {
        g_targetLane = (al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1;
        g_swapArmedRivalValue = 1;
    }
    if (ar7e8av.lateralX > g_targetLane) {
        dist = (ar7e8av.lateralX - g_targetLane) >> 3;
        if (ar7e8av.lateralX - dist > g_targetLane)
            ar7e8av.lateralX -= dist;
    } else {
        dist = (g_targetLane - ar7e8av.lateralX) >> 3;
        if (dist + ar7e8av.lateralX < g_targetLane)
            ar7e8av.lateralX += dist;
    }
}

/* cop_appears @ 0000:D992 (linear 0x0D992, 147 bytes) */

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
        g8acew[n1] = ab7526x[n1];
    g_rivalPace.topSpeed = pf_vmax(0x19);
    g_rivalPace.slowSpeed = (int)(g_rivalPace.topSpeed * 0.6);
    g_rivalPace.fastSpeed = (int)(g_rivalPace.topSpeed * 0.8);
}

int by72f2;
int ay8bc2z;

/* ra_tick @ 0000:DA25 (linear 0x0DA25, 664 bytes) */

void ra_tick(void)
{
    int a1, b, c1, d;

    if (g_driveEng.autoSelector) {
        if (g_driveEng.autoSelector != bz8630z)
            sc_show_gear((bz8630z = g_driveEng.autoSelector) + 4);
    } else if (g_driveEng.gearIndex != bz8630z)
        sc_show_gear(bz8630z = g_driveEng.gearIndex);
    if (g_driveEng.tireFailureIndex && ao7d2ev.roadIndex > g_driveEng.tireFailureIndex)
        g_failureTire = 1;
    if (g_squealOn == 0) {
        if (abs(g_driveEng.steering) >= 12)
            g_squealOn = 1;
    } else if (abs(g_driveEng.steering) < 10)
        g_squealOn = 0;
    rival_accelerate();
    switch (a_7f88d) {
    case 1:
        if (ao7d2ev.roadIndex >= g_idxAtCop) {
            if (ar7e8av.roadIndex + 5 <= ao7d2ev.roadIndex) {
                a_7f88d++;
                cop_appears();
            } else
                a_7f88d = 0;
        }
        break;
    case 2:
        if (ar7e8av.roadIndex + 15 >= ao7d2ev.roadIndex) {
            a_7f88d++;
            so_siren(1);
        }
        break;
    case 3:
        if (ar7e8av.roadIndex - 2 > ao7d2ev.roadIndex) {
            g_driveEng.stopFlags |= 0x41;
            sy_stop_drive();
            return;
        }
        break;
    }
    a1 = ao7d2ev.roadIndex * 8 + ao7d2ev.roadFraction;
    b = ar7e8av.roadIndex * 8 + ar7e8av.roadFraction;
    c1 = ao7d2ev.lateralX;
    d = ar7e8av.lateralX;
    move_player();
    move_rival();
    if (ao7d2ev.roadIndex >= g_finishLineIndex || ar7e8av.roadIndex >= g_finishLineIndex)
        ra_finish_stats();
    if (g_driveEng.stopFlags || g_oppEngState.stopFlags) {
        if (a_7f88d >= 2) {
            if (g_driveEng.stopFlags)
                g_driveEng.stopFlags |= 0x41;
            so_siren(0);
        }
        sy_stop_drive();
        return;
    }
    if (ay8bc2z == 0)
        return;
    if (g_demo)
        return;
    if ((a_7eaap.raceType == 0 ? 0x17c : 0x3fc) <= ao7d2ev.roadIndex)
        return;
    if (by72f2 && a_7f88d <= 1) {
        g_driveEng.stopFlags |= 3;
        sy_stop_drive();
        return;
    }
    if (ap7d7cv == 0 && ao7d2ev.distanceStep > 1) {
        g_contactFrames++;
        if (g_driveEng.steeringInput < 0) {
            if (g_terrain2[ao7d2ev.roadIndex] - g_terrain2[ao7d2ev.roadIndex + 1] >= 0 || ((al70c0x.leftEdge + ao7d2ev.rightEdge) >> 1) >= ao7d2ev.lateralX) {
                so_bump();
                if (g_bumpAlternate == 0) {
                    ar7e8av.roadIndex--;
                    g_bumpAlternate = 1;
                } else
                    g_bumpAlternate = 0;
            }
            return;
        }
        if (g_ramAllowed && g_contactFrames > 3 && a_7f88d < 2) {
            if (rn_int((g_selectedRacerPointer->f2 >> 2) + 2) == 0) {
                so_bump();
                so_bump();
                ao7d2ev.lateralX += 300;
                if (g_driveEng.speed > 20)
                    g_brakeTicks = 2;
                g_ramAllowed = 0;
                ao7d2ev.m7d3c += rn_int(2) ? 15 : -15;
                so_bump();
            }
            g_contactFrames = 0;
        }
    } else if (ap7d7cv)
        g_contactFrames = 0;
}
