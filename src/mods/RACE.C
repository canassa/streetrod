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
#include "HERCDET.H"
#include "HOTSPOT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "M_02B9A.H"
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
static int g_6ca4rz;                       /* DS:6CA4 */
static int g_6ca6qz;                       /* DS:6CA6 */
static int g_6ca8pz;                       /* DS:6CA8 */
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
static int g_6caaaa;                       /* DS:6CAA */
static int g_rivalTop;                       /* DS:6CAC */
static int g_rivalLead;                       /* DS:6CAE */
static int g_rivalFast;                         /* DS:6CB0 */
static int g_rivalSlow;                         /* DS:6CB2 */
static int g_rivalShiftPen;                         /* DS:6CB4 */
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
    n = ((unsigned)g_1a44 - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    for (i = 0; i < n; i++)
        if (g_hots[base + i].f6 == id)
            return base + i;
}

/* sort_gas_spots @ 0000:B425 (linear 0x0B425, 49 bytes) */

static void near sort_gas_spots(void)
{
    int base, n;

    base = ((unsigned)g_hotGasStation - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_1a44 - (unsigned)g_hotGasStation) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int g7644m;
char at787cu;
int h_8264;
int g826af;
int ae826cv;
int ar826e;
int q_8270;
int bp82c2;
int dz82c8x;
int s_8632;
float p_8bc4;

/* clear_gas_spots @ 0000:B456 (linear 0x0B456, 70 bytes) */

static void near clear_gas_spots(void)
{
    int k;

    g_6ca6qz = 0;
    g_6ca8pz = 0;
    g_6ca4rz = 0;
    for (k = 0; k < 6; k++)
        g_hots[find_gas_spot(g_gasSpotIds[k])].f0 = 14;
}

/* place_gas_spot @ 0000:B49C (linear 0x0B49C, 66 bytes) */

static void near place_gas_spot(int id, int x, int y, int w, int h)
{
    struct Hot *p;
    p = &g_hots[find_gas_spot(id)];
    p->x2 = (p->x1 = x) + w - 1;
    p->y2 = (p->y1 = y) + h - 1;
    p->f0 = 11;
}

/* spots_idle @ 0000:B4DE (linear 0x0B4DE, 71 bytes) */

static void near spots_idle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x3f, 100, 12, 23);
    place_gas_spot(1, 0x54, 0xa8, 0x1f, 15);
    sort_gas_spots();
    g_6ca6qz = 1;
    g_6ca8pz = 1;
}

/* spots_cap @ 0000:B525 (linear 0x0B525, 45 bytes) */

static void near spots_cap(void)
{
    clear_gas_spots();
    place_gas_spot(4, ar826e - 10, q_8270 - 5, 18, 12);
    sort_gas_spots();
    g_6ca4rz = 1;
}

/* spots_nozzle @ 0000:B552 (linear 0x0B552, 73 bytes) */

static void near spots_nozzle(void)
{
    clear_gas_spots();
    place_gas_spot(2, 0x34, 100, 16, 23);
    place_gas_spot(4, ar826e - 1, q_8270 - 1, 10, 9);
    sort_gas_spots();
    g_6ca8pz = 1;
    g_6ca4rz = 1;
}

/* spots_rag @ 0000:B59B (linear 0x0B59B, 124 bytes) */

static void near spots_rag(void)
{
    clear_gas_spots();
    if (h_8264)
        place_gas_spot(6, g826af, ae826cv, 20, 19);
    place_gas_spot(5, d_8266, ap8268t, 22, 19);
    place_gas_spot(1, 0x3e, 0xa5, 0x35, 18);
    place_gas_spot(3, 0xc4, 0x5a, 16, 14);
    sort_gas_spots();
    g_6ca6qz = 1;
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
    cursor_ctl(-3);
    label = F_15BBE(id, 2);
    cursor_ctl(-1);
    work = F_1934E(back->width, back->height, 0, 2);
    bitmap_copy(back, work, 1);
    box.w = label->width;
    box.h = box.fc = label->height;
    box.sx = box.sy = 0;
    box.dx = (~r_vid.mask & ar826e) - box.w + 24;
    box.dy = 12 - label->height;
    bitmap_draw_masked(label, work, &box, 0);
    popup_show(work, ar826e - 16, q_8270 - 5);
    F_19438(2);
}

/* leave_station @ 0000:B717 (linear 0x0B717, 394 bytes) */

static void near leave_station(struct Dlg *d)
{
    struct Bitmap far *p;

    if (d->f0c != 1)
        cursor_ctl(1);
    if (d->f12) {
        show_tank_cap(d, 0x472);
        if (d->f18) {
            F_11540(a_7eaap.m7eaa >= 10 ? -10 : -(int)a_7eaap.m7eaa);
            d->f18 = 0;
        }
    }
    if (d->f14 == 0) {
        popup_show(g_pumpWithNozzle, 0x40, 0x64);
        if (d->f18)
            F_11540(a_7eaap.m7eaa >= 10 ? -10 : -(int)a_7eaap.m7eaa);
    }
    if (d->f10 == 0) {
        cursor_ctl(-3);
        p = F_15BBE(0x4d1, 2);
        cursor_ctl(-1);
        carpic_patch_bg(p, 0x60, 0, 0x30, 0x40, 0xc8, 0x51, 0xa9, 2);
    }
    a_7eaap.m7eb0->f22 = a_7eaap.m7eb0->f22 > 0xaa ? 0xaa : a_7eaap.m7eb0->f22;
    F_181C8(2, d->f0, d->f2, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[g_4806[a_7eaap.m7eb0->slot12->idx].f2]);
    menu_pop(1);
    marks_clear();
    g_scene = 1;
    cruise_go();
}

/* race_gas_station @ 0000:B8A1 (linear 0x0B8A1, 1587 bytes) */

void race_gas_station(void)
{
    struct Bitmap far *label;
    struct Dlg dl;
    int pos;
    int wait1;
    int done;
    int key;

    F_11421(-1, "\0", 0);
    g_scene = 3;
    F_118D4(0x442, -1);
    F_11593(g_49dc);
    r_vid.f16 = 1;
    (*m_present)();
    dl.f0c = 1;
    cursor_ctl(1);
    dl.f0 = a_7eaap.m7eb0->f2;
    dl.f2 = game_car_look(a_7eaap.m7eb0);
    F_181C8(4, dl.f0, dl.f2, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0xa9, 1,
            g_5030[g_4806[a_7eaap.m7eb0->slot12->idx].f2]);
    F_16381(0);
    label = popup_grab(0x50, 0xaa, 0x28, 0xc, 0);
    dl.bm1 = popup_grab(ar826e - 0x10, q_8270 - 5, 0x18, 0xc, 0);
    r_vid.f16 = 0;
    popup_show(g_pumpNoNozzle, 0x40, 0x64);
    dl.bm2 = popup_grab(ar826e - 0x10, q_8270 - 5, 0x18, 0xc, 0);
    popup_show(g_pumpWithNozzle, 0x40, 0x64);
    r_vid.f16 = 1;
    pos = 1;
    dl.f1a = 0;
    wait1 = 0x28;
    done = 0;
    dl.f12 = 0;
    dl.f14 = 1;
    dl.f10 = 1;
    dl.f16 = 0;
    dl.f18 = 1;
    dl.f0e = (0xbe - a_7eaap.m7eb0->f22) / 0x1e;
    show_tank_cap(&dl, 0x472);
    spots_idle();
    menu_push(0x17);
    cursor_ctl(-2);
top:
    g_waitUntil = g_tick + 20;
    for (;;) {
        while ((key = sys_wait_input(3000)) == 0)
            ;
        switch (key) {
        case 1000:
            if (dl.f0c == 3)
                goto top;
            if (dl.f1a) {
                dl.f0e--;
                a_7eaap.m7eb0->f22 += 0x1e;
                done = 1;
                F_11540(-(pos <= a_7eaap.m7eaa ? pos : (int)a_7eaap.m7eaa));
                if (dl.f0e == 0 || pos > a_7eaap.m7eaa) {
                    if (video_anim_active(2)) {
                        sound_pump(0);
                        video_anim_stop(2);
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
                menu_pop(1);
                spots_none();
                menu_push(0x17);
                pos <<= 1;
                dl.f16 = 1;
                game_play_anim(3);
                game_message(0x19e1);
                dl.f10 = 0;
                if (done)
                    goto fin;
                dl.f18 = 0;
                if (dl.f0c == 1) {
                    popup_show(g_pumpNoNozzle, 0x40, 0x64);
                    cursor_ctl(dl.f0c = 2);
                    dl.f14 = 0;
                }
                show_tank_cap(&dl, 0x471);
                dl.f12 = 1;
                cursor_ctl(dl.f0c = 1);
                video_anim_start(2);
                sound_pump(1);
                dl.f1a = 1;
            }
            g_waitUntil = g_tick + 0x4c;
            break;
        case 1:
            if (g_6ca6qz == 0)
                break;
            if (dl.f0c == 1) {
                cursor_ctl(dl.f0c = 3);
                F_119B8(0x473, 0x50, 0xaa, -1);
                menu_pop(1);
                spots_rag();
        redraw:
                menu_push(0x17);
                goto top;
            } else {
                cursor_ctl(dl.f0c = 1);
                popup_show(label, 0x50, 0xaa);
                menu_pop(1);
                spots_idle();
                goto redraw;
            }
        case 2:
            if (g_6ca8pz == 0)
                break;
            if (dl.f0c == 1) {
                cursor_ctl(dl.f0c = 2);
                popup_show(g_pumpNoNozzle, 0x40, 0x64);
                dl.f14 = 0;
            both:
                menu_pop(1);
                spots_nozzle();
            } else {
                if (pos > a_7eaap.m7eaa || done) {
                    dl.f18 = 0;
                    goto fin;
                }
                cursor_ctl(dl.f0c = 1);
                popup_show(g_pumpWithNozzle, 0x40, 0x64);
                dl.f14 = 1;
                menu_pop(1);
                spots_idle();
            }
            menu_push(0x17);
        tail:
            g_waitUntil = g_tick + 20;
            dl.f1a = 0;
            break;
        case 4:
            if (g_6ca4rz == 0)
                break;
            if (dl.f0c == 1) {
                if (video_anim_active(2)) {
                    sound_pump(0);
                    video_anim_stop(2);
                }
                show_tank_cap(&dl, 0x472);
                dl.f12 = 0;
                cursor_ctl(dl.f0c = 2);
                if (dl.f0e > 0 && pos <= a_7eaap.m7eaa)
                    game_message(0x18dd);
                goto both;
            }
            if (dl.f0e == 0 || pos > a_7eaap.m7eaa)
                game_message(0x10b5);
            else {
                show_tank_cap(&dl, 0x471);
                dl.f12 = 1;
                cursor_ctl(dl.f0c = 1);
                video_anim_start(2);
                sound_pump(1);
                menu_pop(1);
                spots_cap();
                menu_push(0x17);
                g_waitUntil = g_tick + 0x4c;
                dl.f1a = 1;
            }
            break;
        case 3:
            game_message(0x10f6);
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
    if (a_7eaap.m7eb0 == 0)
        return 0;
    a_7eaap.m7eb0->weight -= game_take_part(a_7eaap.m7eb0->slot12, 0);
    a_7eaap.m7eb0->slot12 = 0;
    F_076A7(4, tbl, 0, 0);
    if (g_list[0]) {
        F_07741(F_077B7(tbl[1]), &a_7eaap.m7eb0->slot12);
    } else {
        a_7eaap.m7eaa -= 0x32;
        if (a_7eaap.m7eaa < 0) {
            ret = (int)-a_7eaap.m7eaa;
            a_7eaap.m7eaa = 0;
        }
        game_add_part(0x28, g_4806[40].f0);
        tbl[0] = a_7eaap.m7eb8;
        a_7eaap.m7eb8 = tbl[0]->next;
        F_07741(tbl[0], &a_7eaap.m7eb0->slot12);
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

    marks_clear();
    a = F_15BBE(0x456, 1);
    b = F_15BBE(0x4ef, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.dx = pkt.sx = pkt.sy = 0;
    pkt.dy = 0x45;
    (*ab78aat)(b, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height;
    pkt.sx = pkt.sy = 0;
    pkt.dx = b->width;
    pkt.dy = 0x14;
    (*b_78a2)(a, r_vid.back, &pkt, 1);
    maxw2 = a->width - 8;
    r_vid.f16 = 0;
    x_colors = bustyle->hilite;
    for (p2 = g_driverName, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    (*a_78aem)(r_vid.back, 0x5e, 0x48, g_driverName);
    *p2 = d;
    desc3 = g7d86n[a_7eaap.m7eb0->f2].f6;
    endk = desc3 + strcspn(g_descHdr + desc3 + 8, " `.") + 7;
    for (p2 = g_descHdr + desc3 + 1, wk = 0; *p2 && wk < maxw2; p2++)
        wk += g_font[*p2 - 32].w;
    if (wk >= maxw2)
        p2--;
    d = cl = *p2;
    *p2 = 0;
    e = g_descHdr[endk + 1];
    g_descHdr[endk + 1] = 0;
    (*a_78aem)(r_vid.back, 0x5e, 0x63, g_descHdr + desc3 + 1);
    g_descHdr[endk + 1] = e;
    *p2 = d;
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    r_vid.f16 = 1;
    cursor_ctl(-2);
    sys_reset_queues();
}

/* show_crash @ 0000:C1D8 (linear 0x0C1D8, 559 bytes) */

static void near show_crash(void)
{
    struct Bitmap far *a, far *b;
    struct Blt pkt;
    int fast, i;

    fast = r_vid.mode == -2 && g_vga == 0;
    sound_crash();
    marks_clear();
    a = F_15BBE(0x4eb, 1);
    b = F_15BBE(0x4ec, 1);
    pkt.w = a->width;
    pkt.h = 0x5a;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = 0xb;
    (*ab78aat)(a, r_vid.back, &pkt, 1);
    pkt.w = a->width;
    pkt.h = a->height - 0x59;
    pkt.sy = 0x59;
    pkt.sx = pkt.dx = 0;
    pkt.dy = (fast ? 0x5a : 0x59) + 0xb;
    (*ab78aat)(a, r_vid.back, &pkt, 1);
    pkt.w = b->width;
    pkt.h = b->height;
    pkt.sx = pkt.sy = pkt.dx = 0;
    pkt.dy = fast ? 0x78 : 0x77;
    (*b_78a2)(b, r_vid.back, &pkt, 1);
    dash_steer_marker(bp82c2);
    (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
    if (e_78e0.f0 > 0x5a) {
        for (i = 0; i < 3; i++) {
            a = F_15BBE(g_crashPics[i].id, 1);
            pkt.w = a->width;
            pkt.h = a->height;
            pkt.sx = pkt.sy = 0;
            pkt.dx = g_crashPics[i].x;
            pkt.dy = g_crashPics[i].y;
            F_10AFD(0x14 - 8 * i);
            (*ab78aat)(a, r_vid.back, &pkt, 1);
            (*dz78b6)(r_vid.back->bits.p, r_vid.front->bits.p);
        }
    }
    cursor_ctl(-2);
    (*m_present)();
    sys_reset_queues();
    game_pause(0x64);
    sys_reset_queues();
    marks_clear();
}

/* race_finish_stats @ 0000:C407 (linear 0x0C407, 144 bytes) */

void race_finish_stats(void)
{
    if (g_statsDone == 0) {
        if ((ar7e8av.m7e8a - ao7d2ev.m7d30) * 8 + ar7e8av.m7e8c <= ao7d2ev.m7d32)
            s_8632 = 1;
        else
            s_8632 = 0;
        g7644m = b_82bc / e_78e0.fc0;
        p_8bc4 = e_78e0.fc0 / 6.068827333333334;
        dz82c8x = (int)((ao7d2ev.m7d30 - 200) * 100.0 / (ar7e8av.m7e8a - 200)) > 99 ? 99 : (int)((ao7d2ev.m7d30 - 200) * 100.0 / (ar7e8av.m7e8a - 200));
        g_statsDone = 1;
    }
}

/* show_race_stats @ 0000:C497 (linear 0x0C497, 149 bytes) */

static void near show_race_stats(void)
{
    if (bo7676) {
        sprintf(g_raceResult1, "Your``winning``time %2.1f sec.,`avg. speed %2d mph", p_8bc4, g7644m);
        strcpy(g_raceResult2, "Congratulations !");
    } else {
        sprintf(g_raceResult1, "In %2.1f sec you covered only %2d%%", p_8bc4, dz82c8x);
        sprintf(g_raceResult2, "of the distance. Your avg. speed %2d mph.", g7644m);
    }
    menu_push(0x21);
    menu_pop(1);
}

/* blow_engine @ 0000:C52C (linear 0x0C52C, 128 bytes) */

static void near blow_engine(int n)
{
    struct Node **p;
    char *q;
    int i;

    if (a_7eaap.m7eb0) {
        for (i = 0, p = a_7eaap.m7eb0->rack.s; i < 5; i++, p++) {
            a_7eaap.m7eb0->weight -= game_take_part(*p, 0);
            *p = 0;
        }
        for (i = 0, q = a_7eaap.m7eb0->b16; i < 9; i++)
            *q++ = 0;
        a_7eaap.m7eb0->f1f = 0;
    }
    sys_reset_queues();
    F_16BC1();
    game_popup(-n, -1, 0x49);
}

/* drop_transmission @ 0000:C5AC (linear 0x0C5AC, 103 bytes) */

static void near drop_transmission(int n)
{
    char *q;
    int i;

    if (a_7eaap.m7eb0) {
        a_7eaap.m7eb0->weight -= game_take_part(a_7eaap.m7eb0->slot6, 0);
        a_7eaap.m7eb0->slot6 = 0;
        for (i = 0, q = a_7eaap.m7eb0->b14; i < 2; i++)
            *q++ = 0;
    }
    sys_reset_queues();
    F_16BC1();
    game_popup(-n, -1, 0x49);
}

int g_tireSlip[3][5] = {{2, 5, 8, 11, 14}, {0, 2, 4, 6, 8}, {0, 0, 1, 2, 4}};   /* DS:54B2 */
int g_lastTicks = 0;   /* DS:54D0 */
int g_bumpAlternate = 0;   /* DS:54D2 */
int g_rivalLeft = 1;   /* DS:54D4 */
int g_rivalCurveBrake = 0;   /* DS:54D6 */
int g_contactFrames = 0;   /* DS:54D8 */
int g_brakeTicks = 0;   /* DS:54DA */

/* race_outcome @ 0000:C613 (linear 0x0C613, 1948 bytes) */

int near race_outcome(void)
{
    int units;
    char opt;
    int sel;
    int kind;
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

    kind = as79a2z;
    v = d_79a4;
    engine = e_78e0.fbc & 4;
    opt = e_78e0.fbc & 0x10;
    stop = e_78e0.fbc == 5 || e_78e0.fbc == 0x11 || e_78e0.fbc == 0x15;
    if (a_7eaap.m7eb0->slot6->b > 9900)
        a_7eaap.m7eb0->slot6->b = 9900;
    if (a_7eaap.m7eb0->rack.s[0]->b > 9900)
        a_7eaap.m7eb0->rack.s[0]->b = 9900;
    if (a_7eaap.m7eb0->slot12->b > 9900)
        a_7eaap.m7eb0->slot12->b = 9900;
    bo7676 = 0;
    g_statsDone = 0;
    b_82bc = 0;
    g_fireLatch = 0;
    g_buttons = 0;
    if (e_78e0.f20)
        e_78e0.f24 = 1;
    if (at787cu)
        e_78e0.fbc |= 8;
    menu_push(1);
    i = a_7eaap.m7ec0 == 0 ? 5 : 15;
    if (a_7eaap.m7eb0->f22 > i)
        a_7eaap.m7eb0->f22 -= i;
    else
        a_7eaap.m7eb0->f22 = 0;
    (*m_present)();
    if (g79e2g.fbc & 1) {
        bo7676 = 1;
        show_race_stats();
        cursor_ctl(-2);
        game_message(g7d40q == 0 ? (i = g_winMsgs[kind]) : a_7eaap.m7ec0 == 0 ? 0x14bd : 0x14d6);
        F_11540(v);
        if (kind == 5) {
            found = game_make_car(bn7648, ar7d7au->f0, 0);
            found->slot12->b = random_int(500);
            found->f24 = ((g7d84o->ff & 0x1f) << 8) | (found->f24 & 0xe0ff);
            found->f24 = (((g7d84o->fe & 4) != 0 & 1) << 13) | (found->f24 & 0xdfff);
            found->f24 = (((g7d84o->fe & 2) != 0 & 1) << 14) | (found->f24 & 0xbfff);
            found->f24 = (((g7d84o->fe & 1) != 0 & 1) << 15) | (found->f24 & 0x7fff);
            found->f4 = g7d84o->fa;
            if (found->f5 > a_7eaap.m7eb0->f5) {
                a_7eaap.m7eb0->f26 = a_7eaap.m7eb2->f26;
                a_7eaap.m7eb2 = a_7eaap.m7eb0;
                a_7eaap.m7eb0 = found;
                perf_drivetrain(a_7eaap.m7eb0, &e_78e0);
            }
            if (g_demo == 0 || g7f82r != g_demoRacer) {
                if (g7d84o->f10 != 0 || g7d40q != 0)
                    g7d84o->fb = 0xff;
                else {
                    for (i = 0, units = 0; i < 25; i++)
                        if (g7d86n[i].f2 < ar7d7au->f2)
                            list[++units] = i;
                    if (units > 0) {
                        list[0] = units;
                        savegame_init_racer(g7f82r, list);
                        g7d84o->f10 = 1;
                    } else
                        g7d84o->fb = 0xff;
                    goto done;
                }
            }
        }
done:
        menu_pop(1);
        if (e_78e0.fbc & 8)
            if ((i = replace_tires()) != 0)
                goto ret2;
        if (kind == 5)
            return 1;
        return 3;
    } else if (e_78e0.fbc & 0x40) {
        sound_siren(0);
        show_ticket();
        i = (e_78e0.fbc & 8) ? replace_tires() : 0;
        if (e_78e0.f0 > 0x50)
            i += 0x4b;
        else if (e_78e0.f0 > 0x32)
            i += 0x32;
        else
            i += 0x14;
        menu_push(0x18);
        sys_wait_input(0);
        sprintf(temp, "You have been fined $%2d for speeding", i);
        F_11421(-1, temp, 0x140 - o_8250);
        game_pause(0xfa);
        if (i <= a_7eaap.m7eaa)
            goto pay;
        game_message(0x1793);
        menu_pop(1);
        menu_pop(1);
        F_11540(-a_7eaap.m7eaa);
        F_11421(-1, " ", 0x140 - o_8250);
        return 4;
pay:
        F_11540(-i);
        cost = g_waitUntil;
        game_pause(g_msgTicks);
        g_waitUntil = cost;
        menu_pop(1);
        F_11421(-1, "", 0x140 - o_8250);
    } else if (e_78e0.fbc == 0x81) {
        cursor_ctl(-2);
        game_message(0xecb2);
    } else {
        if (kind == 5) {
            game_scrap_car(a_7eaap.m7eb0, 0);
            a_7eaap.m7eb0 = 0;
        }
        F_11540(-v);
        if (e_78e0.fbc == 1) {
            show_race_stats();
            cursor_ctl(-2);
            i = kind == 5 ? 0x143a : 0x1427;
            game_message(g7d40q == 0 ? i : a_7eaap.m7ec0 == 0 ? 0x1450 : 0x146c);
            menu_pop(1);
            if (kind != 5)
                return 3;
ret2:
            return 2;
        } else if (stop) {
            cursor_ctl(-2);
            if (engine) {
                blow_engine(kind == 5 ? 0x16bd : 0x1634);
                goto finishstop;
            } else if (opt) {
                drop_transmission(kind == 5 ? 0x1678 : 0x1608);
                goto finishstop;
            }
finishstop:
            goto fin;
        } else {
            show_crash();
            if (kind == 5) {
                if (e_78e0.fbc & 8) {
                    game_message(0x16f6);
                    replace_tires();
                } else
                    game_message(0x1658);
            } else if (e_78e0.f0 > 0x5a) {
                v = game_scrap_car(a_7eaap.m7eb0, 0) / 10 + 0x13;
                a_7eaap.m7eb0 = 0;
                F_11540(v);
                ptr = g_msgTotaled;
                while (*ptr++ != '$')
                    ;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                sys_reset_queues();
                game_message(0x1513);
            } else {
                if (e_78e0.f0 > 0x3c) {
                    i = 0;
                    v = random_int(0x28) + 100;
                } else if (e_78e0.f0 > 0x1e) {
                    i = 1;
                    v = random_int(0x14) + 100;
                } else {
                    if (e_78e0.fbc & 8) {
                        game_message(0x1590);
                        all = replace_tires();
                        goto fin;
                    }
                    game_message(0x1561);
                    menu_pop(1);
                    return 1;
                }
                ptr = strchr(i ? g_msgRunCost : g_msgRepairCost, '$') + 1;
                ptr[1] = ' ';
                ptr[2] = ' ';
                itoa(v, ptr, 10);
                ptr[strlen(ptr)] = ' ';
                all = e_78e0.fbc & 8;
                if (all)
                    strcpy(g_msgPlusTires, "Plus tires !");
                else
                    g_msgPlusTires[0] = 0;
                sel = i ? 0x1a : 0x19;
                if ((long)v > a_7eaap.m7eaa) {
                    g_hotRepair[0].f0 = g_hotRepair[7].f0 = 0xe;
                    g_keyMaps[sel][0].val = 0xd0;
                    g_keyMaps[sel][1].key = 1;
                    g_keyMaps[sel][1].val = 0xd0;
                } else {
                    g_hotRepair[0].f0 = g_hotRepair[7].f0 = 0xa;
                    g_keyMaps[sel][0].val = 0xcf;
                    g_keyMaps[sel][1].key = 0x21;
                    g_keyMaps[sel][1].val = 0xcf;
                }
                menu_push(sel);
                sys_reset_queues();
                while ((i = sys_wait_input(3000)) == 0)
                    ;
                menu_pop(1);
                switch (i) {
                case 1:
                    F_11540(-v);
                    if (all)
                        replace_tires();
                    menu_pop(1);
                    return 1;
                case 2:
                    F_11540((int)(a_7eaap.m7eb0->weight * 0.4));
                    a_7eaap.m7eb0 = 0;
                    break;
                default:
                    return;
                }
            }
        }
fin:
        menu_pop(1);
        goto ret2;
    }
    menu_pop(1);
    return 3;
}

struct State70C0 al70c0x;
int g755av;
int a_7f88d;
int bz8630z;

/* race_reset_eng @ 0000:CDAF (linear 0x0CDAF, 429 bytes) */

void near race_reset_eng(struct Eng *p)
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
    p->f0 = p->f18 = p->f42 = p->f2 = p->f4 = 0;
    p->fbc = p->fbe = p->fc0 = 0;
    if (p->f20) {
        p->gear[2].r[1].f2 = p->gear[2].r[2].f0 = p->f1e;
        p->f24 = 1;
    }
    p->f28 = p->f26 = -1;
    p->fe = 0x5dc;
    p->f40 = p->f2a;
    p->f14 = p->gear[0].r[0].f4;
    p->f16 = p->gear[0].r[0].f6;
    p->f12 = p->gear[0].r[0].f2;
    p->f10 = p->gear[0].r[0].f0;
    g_05d6 = 0;
    g_05d8 = g_05da = 0;
    bz8630z = -1;
    g_6caaaa = a_7eaap.m7ec0 == 0 ? 0x17c : 0x3fc;
    car = a_7eaap.m7eb0->slot12;
    p->fc = g_tireSlip[g_4806[car->idx].f2][car->b / 2000];
    if (car->b < 8000)
        p->fa = 0;
    else {
        if (car->b + random_int(2000) <= 9500)
            p->fa = 0;
        else {
            t = (g_6caaaa - 200) >> 1;
            p->fa = random_int(t >> 1) + t + 200;
        }
        goto nx;
    }
nx:
    at787cu = 0;
    g_lastTicks = 0;
    g_rivalTop = F_0E218(bn7648);
    g_rivalSlow = (int)(g_rivalTop * 0.7);
    g_rivalFast = (int)(g_rivalTop * 0.8);
    g_rivalLead = bu8acc < 4 ? 10 : 16;
    g_bumpAlternate = 0;
    g_statsDone = 0;
}

/* init_rival_eng @ 0000:CF5C (linear 0x0CF5C, 213 bytes) */

static void near init_rival_eng(void)
{
    struct Range *r;

    perf_drivetrain_spec(bn7648, &g79e2g);
    g_rivalShiftPen = g7d84o->f1;
    race_reset_eng(&g79e2g);
    g79e2g.f18 = 0;
    g79e2g.f40 = g79e2g.f34[0];
    g79e2g.fe = 0x5dc;
    g79e2g.f0 = 0x5dc / g79e2g.f40;
    if ((r = g79e2g.gear[0].r + (g_rpmBand = 1))->f2 <= 0x5dc) {
        g_rpmBand = 2;
        r++;
    } else if (r->f0 > g79e2g.fe) {
        g_rpmBand = 0;
        r--;
    }
    g79e2g.f42 = g_rpmBand;
    g79e2g.f12 = r->f2;
    g79e2g.f14 = r->f4;
    g_shiftPoint = g79e2g.f24 ? 0x14b4 : g79e2g.f1e - 100 * g_rivalShiftPen;
    ar7e8av.m7e8a = 200;
    ar7e8av.m7e8c = 0;
    g_targetLane = ar7e8av.m7e90 = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
}

/* race_run @ 0000:D031 (linear 0x0D031, 429 bytes) */

void near race_run(void)
{
    int one;
    struct Node *slot;

    one = 1;
    cursor_ctl(-4);
    cursor_ctl(1);
    sys_reset_queues();
    race_reset_eng(&e_78e0);
    init_rival_eng();
    if (a_7eaap.m7ec0 == 1 && g7d40q == 0) {
        if (a_7f88d = !random_int(4))
            g755av = random_int(255) + 0x1fe;
    } else
        a_7f88d = 0;
    jz79a6 = 0;
    bw8aca = 1;
    cruise_drive();
    sys_drive_stop();
    g_waitUntil = 0;
    sys_reset_queues();
    F_11593(g_49dc);
    if (ao7d2ev.m7d30 > 200) {
        if (random_int(3) > 0) {
            if (a_7eaap.m7eb0->f20 > 0)
                a_7eaap.m7eb0->f20 = a_7eaap.m7eb0->f20 < random_int(4) ? random_int(4) : a_7eaap.m7eb0->f20;
            else if (a_7eaap.m7eb0->f20 < 0)
                a_7eaap.m7eb0->f20 = -random_int(8) < a_7eaap.m7eb0->f20 ? -random_int(8) : a_7eaap.m7eb0->f20;
            else
                a_7eaap.m7eb0->f20 = random_int(8) - 4;
        }
        slot = a_7eaap.m7eb0->slot12;
        slot->b += (g_4806[slot->idx].f2 == 0 ? (a_7eaap.m7ec0 == 1 ? 700 : 300) : (a_7eaap.m7ec0 == 1 ? 400 : 200)) + random_int(300);
        if (slot->b > 10000)
            slot->b = 10000;
    }
}

/* fit_gear @ 0000:D1DE (linear 0x0D1DE, 191 bytes)

   Steps p->f18 up from 1 until gear[f18].r[1].f2 exceeds f34[f18] * rpm or
   f18 reaches f1c. The four pointers the loop advances in memory are /Ol induction temps for the p->f18 indexing. */

static void near fit_gear(int rpm, struct Eng *p)
{
    struct Range *r;

    for (p->f18 = 1; ; p->f18++) {
        p->f40 = p->f34[p->f18];
        p->fe = p->f40 * rpm;
        if ((r = &p->gear[p->f18].r[1])->f2 > p->fe) {
            p->f42 = g_rpmBand = p->fe >= r->f0;
            if (g_rpmBand == 0)
                r = &p->gear[p->f18].r[0];
            break;
        }
        if (p->f18 == p->f1c) {
            r = &p->gear[p->f18].r[2];
            p->f42 = g_rpmBand = 2;
            break;
        }
    }
    p->f14 = r->f4;
    p->f0 = p->fe / p->f40;
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

    old3 = g79e2g.f0;
    _disable();
    ticks = e_78e0.fc0 - g_lastTicks;
    g_lastTicks = e_78e0.fc0;
    _enable();
    r = &g79e2g.gear[g79e2g.f18].r[g_rpmBand];
    for (i5 = 0; i5 < ticks; i5++) {
        if (g_terrain2[ar7e8av.m7e8a] != g_terrain2[ar7e8av.m7e8a + 1]) {
            if (g_rivalCurveBrake == 0) {
                g_rivalCurveBrake = 1;
                vl = g_rivalTop - (ar7e8av.m7e8a > ao7d2ev.m7d30 ? g7d84o->f2 : g7d84o->f2 >> 1);
                if (vl < g79e2g.f0)
                    fit_gear(vl, &g79e2g);
            }
            continue;
        }
        g_rivalCurveBrake = 0;
        d = ar7e8av.m7e8a - ao7d2ev.m7d30;
        if (d > 12 && (a_7eaap.m7ec0 != 1 || g7d40q == 0)) {
            if (e_78e0.f0 >= 0x61)
                vl = g_rivalFast;
            else if (g7d40q)
                vl = g_rivalFast;
            else
                vl = g_rivalSlow;
        chk:
            if (vl < g79e2g.f0) {
                fit_gear(vl, &g79e2g);
                continue;
            }
        } else if (d > -5 && d < 5) {
            vl = e_78e0.f0 + g_rivalLead;
            if (ao7d2ev.m7d30 >= 30 && vl < g_rivalSlow)
                vl = g_rivalSlow;
            goto chk;
        }
        if (a_7f88d >= 2 && e_78e0.f0 > 50 && g79e2g.f0 > e_78e0.f0 + 32)
            fit_gear(e_78e0.f0 + 32, &g79e2g);
        else {
            g79e2g.fe += g79e2g.f14;
            if (g79e2g.fe > g_shiftPoint)
                g79e2g.fe = g_shiftPoint;
            if (g79e2g.fe > g79e2g.f12) {
                g79e2g.f42++;
                g79e2g.f12 = (r = &g79e2g.gear[g79e2g.f18].r[g79e2g.f42])->f2;
                g79e2g.f14 = r->f4;
            }
            if (g79e2g.f18 < g79e2g.f1c &&
                ((g79e2g.f24 && g79e2g.fe >= g79e2g.f22) || g79e2g.fe >= g_shiftPoint)) {
                g79e2g.f18++;
                if (g79e2g.f24 == 0)
                    g_shiftPoint = g79e2g.f18 < g79e2g.f1c ? g79e2g.f1e - 100 * g_rivalShiftPen
                           : bu8acc < 4 && r_vid.mode != -6 ? 0x14b4 : 0x1518;
                g79e2g.f40 = g79e2g.f34[g79e2g.f18];
                g79e2g.fe = g79e2g.f0 * g79e2g.f40;
                if ((r = &g79e2g.gear[g79e2g.f18].r[g_rpmBand = 1])->f2 <= g79e2g.fe) {
                    g_rpmBand = 2;
                    r++;
                } else if (r->f0 > g79e2g.fe) {
                    g_rpmBand = 0;
                    r--;
                }
                g79e2g.f42 = g_rpmBand;
                g79e2g.f12 = r->f2;
                g79e2g.f14 = r->f4;
            }
            g79e2g.f0 = g79e2g.fe / g79e2g.f40;
        }
    }
    if (ar7e8av.m7e8a >= g_6caaaa) {
        liml = ao7d2ev.m7d30 + 5 < ar7e8av.m7e8a ? 5 : 10;
        g79e2g.f0 = old3;
        if (liml < old3)
            g79e2g.f0 -= liml;
        else
            g79e2g.f0 = 0;
    }
}

/* race_speed_step @ 0000:D544 (linear 0x0D544, 224 bytes) */

int race_speed_step(int rpm)
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
int gz7d7e;
int g7d82p;
int g_8648;
struct Bitmap far *g8acew[13];
int cz8bd0y;

/* move_player @ 0000:D624 (linear 0x0D624, 89 bytes) */

static void near move_player(void)
{
    ao7d2ev.m7d32 += ao7d2ev.m7d34 = e_78e0.f0 ? race_speed_step(e_78e0.f0) : 0;
    if (ao7d2ev.m7d32 >= 8)
        road_advance();
    if (ao7d2ev.m7d30 >= g_6caaaa && g_statsDone && e_78e0.f0 <= 0) {
        if (s_8632)
            g79e2g.fbc |= 1;
        else
            e_78e0.fbc |= 1;
        sys_stop_drive();
    }
}

/* move_rival @ 0000:D67D (linear 0x0D67D, 789 bytes) */

static void near move_rival(void)
{
    int flag;
    int slope;
    int t;
    int dist;

    ar7e8av.m7e8e = g79e2g.f0 ? race_speed_step(g79e2g.f0) : 0;
    if (g79e2g.f0 == 0)
        ar7e8av.m7e8e = 0;
    ar7e8av.m7e8c += ar7e8av.m7e8e;
    if (ar7e8av.m7e8c >= 8 && ar7e8av.m7e8e > 0) {
        ar7e8av.m7e8a += ar7e8av.m7e8c >> 3;
        ar7e8av.m7e8c &= 7;
    }
    if (ar7e8av.m7e8a >= g_6caaaa && g_statsDone && e_78e0.f0 <= 0) {
        if (s_8632 == 0)
            e_78e0.fbc |= 1;
        else
            g79e2g.fbc |= 1;
        sys_stop_drive();
        return;
    }
    flag = g_terrain3[ar7e8av.m7e8a + 4] & 0xe0;
    dist = ar7e8av.m7e8a - ao7d2ev.m7d30;
    slope = g_terrain2[ao7d2ev.m7d30 + 4] - g_terrain2[ao7d2ev.m7d30 + 5];
    if (dist > 12) {
        g_targetLane = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        if (slope)
            g_rivalLeft = slope < 0;
        g_8648 = 1;
    } else if (dist > 6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag ? (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1 : g_rivalLeft ? ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200 : ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
    } else if ((bu8acc < 4 ? 4 : 2) < dist) {
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else if (flag)
            g_targetLane = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        else if (g_8648) {
            if (g_rivalLeft)
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
            else
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
            g_8648 = 0;
            g_rivalLeft = !g_rivalLeft;
        }
    } else if (dist > -4) {
        if (ar7e8av.m7e8e > ao7d2ev.m7d34) {
            if (ap7d7cv >= 9) {
                if ((t = ao7d2ev.m7d36 - 330) > g7d82p + 480) {
                    g_rivalLeft = 1;
                    g_targetLane = t;
                    if (ao7d2ev.m7d36 - ar7e8av.m7e90 < 280) {
                        ar7e8av.m7e8a = ao7d2ev.m7d30 - 1;
                        fit_gear(e_78e0.f0, &g79e2g);
                    }
                } else {
                    if ((t = ao7d2ev.m7d36 + 430) < cz8bd0y - 30) {
                        g_rivalLeft = 0;
                        g_targetLane = t + 200;
                        if (ar7e8av.m7e90 - ao7d2ev.m7d36 < 580) {
                            ar7e8av.m7e8a = ao7d2ev.m7d30 - 2;
                            fit_gear(e_78e0.f0, &g79e2g);
                        }
                    } else {
                        ar7e8av.m7e8a = ao7d2ev.m7d30 - 2;
                        fit_gear(e_78e0.f0, &g79e2g);
                    }
                }
            } else if (ap7d7cv == 0 || ap7d7cv == 2) {
                g_targetLane = ao7d2ev.m7d36 > ar7e8av.m7e90 ? ao7d2ev.m7d36 - 330 : ao7d2ev.m7d36 + 530;
            }
        } else if (ap7d7cv == 0)
            g_targetLane = ao7d2ev.m7d36 - 330;
    } else if (dist > -6) {
        gz7d7e = 1;
        if (slope) {
            if (slope < 0) {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200;
                g_rivalLeft = 1;
            } else {
                g_targetLane = ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
                g_rivalLeft = 0;
            }
        } else
            g_targetLane = flag ? (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1 : g_rivalLeft ? ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) - 200 : ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) + 300;
    } else {
        g_targetLane = (al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1;
        g_8648 = 1;
    }
    if (ar7e8av.m7e90 > g_targetLane) {
        dist = (ar7e8av.m7e90 - g_targetLane) >> 3;
        if (ar7e8av.m7e90 - dist > g_targetLane)
            ar7e8av.m7e90 -= dist;
    } else {
        dist = (g_targetLane - ar7e8av.m7e90) >> 3;
        if (dist + ar7e8av.m7e90 < g_targetLane)
            ar7e8av.m7e90 += dist;
    }
}

/* cop_appears @ 0000:D992 (linear 0x0D992, 147 bytes) */

static void near cop_appears(void)
{
    int n1;
    int old;

    old = g79e2g.f0;
    g7d84o = &av7ff8[10];
    perf_drivetrain_spec(0x19, &g79e2g);
    g_rivalShiftPen = g7d84o->f1;
    fit_gear(old, &g79e2g);
    cruise_rival_paint(5);
    for (n1 = 0; n1 < 12; n1++)
        g8acew[n1] = ab7526x[n1];
    g_rivalTop = F_0E218(0x19);
    g_rivalSlow = (int)(g_rivalTop * 0.6);
    g_rivalFast = (int)(g_rivalTop * 0.8);
}

int by72f2;
int ay8bc2z;

/* race_tick @ 0000:DA25 (linear 0x0DA25, 664 bytes) */

void race_tick(void)
{
    int a1, b, c1, d;

    if (e_78e0.f24) {
        if (e_78e0.f24 != bz8630z)
            screen_show_gear((bz8630z = e_78e0.f24) + 4);
    } else if (e_78e0.f18 != bz8630z)
        screen_show_gear(bz8630z = e_78e0.f18);
    if (e_78e0.fa && ao7d2ev.m7d30 > e_78e0.fa)
        at787cu = 1;
    if (g_squealOn == 0) {
        if (abs(e_78e0.f4) >= 12)
            g_squealOn = 1;
    } else if (abs(e_78e0.f4) < 10)
        g_squealOn = 0;
    rival_accelerate();
    switch (a_7f88d) {
    case 1:
        if (ao7d2ev.m7d30 >= g755av) {
            if (ar7e8av.m7e8a + 5 <= ao7d2ev.m7d30) {
                a_7f88d++;
                cop_appears();
            } else
                a_7f88d = 0;
        }
        break;
    case 2:
        if (ar7e8av.m7e8a + 15 >= ao7d2ev.m7d30) {
            a_7f88d++;
            sound_siren(1);
        }
        break;
    case 3:
        if (ar7e8av.m7e8a - 2 > ao7d2ev.m7d30) {
            e_78e0.fbc |= 0x41;
            sys_stop_drive();
            return;
        }
        break;
    }
    a1 = ao7d2ev.m7d30 * 8 + ao7d2ev.m7d32;
    b = ar7e8av.m7e8a * 8 + ar7e8av.m7e8c;
    c1 = ao7d2ev.m7d36;
    d = ar7e8av.m7e90;
    move_player();
    move_rival();
    if (ao7d2ev.m7d30 >= g_6caaaa || ar7e8av.m7e8a >= g_6caaaa)
        race_finish_stats();
    if (e_78e0.fbc || g79e2g.fbc) {
        if (a_7f88d >= 2) {
            if (e_78e0.fbc)
                e_78e0.fbc |= 0x41;
            sound_siren(0);
        }
        sys_stop_drive();
        return;
    }
    if (ay8bc2z == 0)
        return;
    if (g_demo)
        return;
    if ((a_7eaap.m7ec0 == 0 ? 0x17c : 0x3fc) <= ao7d2ev.m7d30)
        return;
    if (by72f2 && a_7f88d <= 1) {
        e_78e0.fbc |= 3;
        sys_stop_drive();
        return;
    }
    if (ap7d7cv == 0 && ao7d2ev.m7d34 > 1) {
        g_contactFrames++;
        if (e_78e0.f2 < 0) {
            if (g_terrain2[ao7d2ev.m7d30] - g_terrain2[ao7d2ev.m7d30 + 1] >= 0 || ((al70c0x.m70c0 + ao7d2ev.m7d2e) >> 1) >= ao7d2ev.m7d36) {
                sound_bump();
                if (g_bumpAlternate == 0) {
                    ar7e8av.m7e8a--;
                    g_bumpAlternate = 1;
                } else
                    g_bumpAlternate = 0;
            }
            return;
        }
        if (gz7d7e && g_contactFrames > 3 && a_7f88d < 2) {
            if (random_int((g7d84o->f2 >> 2) + 2) == 0) {
                sound_bump();
                sound_bump();
                ao7d2ev.m7d36 += 300;
                if (e_78e0.f0 > 20)
                    g_brakeTicks = 2;
                gz7d7e = 0;
                ao7d2ev.m7d3c += random_int(2) ? 15 : -15;
                sound_bump();
            }
            g_contactFrames = 0;
        }
    } else if (ap7d7cv)
        g_contactFrames = 0;
}
