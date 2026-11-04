#include <stdlib.h>
#include "STRUCTS.H"
#include "DRIVE.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CRTC.H"
#include "DASH.H"
#include "DISK.H"
#include "DRVTICK.H"
#include "MAIN.H"
#include "MARKS.H"
#include "POPUP.H"
#include "RACE.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIEW.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap far *g8188w;

/* drive_run @ 0F38:7B22 (linear 0x16EA2, 1540 bytes)

   Gearbox/drive screen loop: draws the dashboard, then redraws the changing parts until
   drvtick_step reports an input. Returns that value. */

int drive_run(void)
{
    struct Bitmap far *pict;
    struct Blt blt;
    int w;
    struct Bitmap far *src;
    int sv;
    int oldf05;
    int cur;
    int oldfe;
    int shiftx5;
    int ret;

    sv = r_vid.f16;
    do {
        dash_set_view(3);
        marks_clear();
        (*bl78be)();
        g_dashBuffered = r_vid.mode == -3 && g_egaCard;
        if (g_dashBuffered) {
            popup_grab(0, 0x64, 0x140, 0x64, g755cu);
            r_vid.f16 = 1;
            screen_fill_rect(0, 0, 0x140, 0xc8, 0);
            r_vid.f16 = sv;
        }
        if (r_vid.mode == -2)
            g_driveRebuild = 1;
        else
            g_driveRebuild = 0;
        src = F_15BBE(0x3f6, 2);
        if (g_autoTrans) {
            pict = F_15BBE(0x4d8, 2);
            blt.w = pict->width;
            blt.h = pict->height;
            blt.sx = blt.sy = 0;
            blt.dx = 0x78;
            blt.dy = 0x7a;
            (*b_78a2)(pict, src, &blt, 1);
            F_19438(1);
        }
        w = blt.w = src->width;
        blt.h = 0x65;
        blt.sx = 0;
        blt.sy = blt.dx = blt.dy = 0;
        (*b_78a2)(src, g8188w, &blt, 1);
        if (r_vid.mode == -2)
            (*b_78a2)(src, g82b4s, &blt, 1);
        blt.w = src->width;
        blt.h = 0x5a;
        blt.sx = blt.dx = 0;
        blt.sy = 0x64;
        blt.dy = (r_vid.mode == -2 || g_dashBuffered) ? 0 : 0x64;
        (*b_78a2)(src, g_dashBuffered ? g755cu : bm7678, &blt, 1);
        F_19438(1);
        view_load();
        dash_init_shifter(e_78e0.f24, e_78e0.f1c);
        dash_draw_shifter(g_dashBuffered ? g755cu : bm7678, &e_78e0);
        shiftx5 = 0;
        bp82c2 = 0x64;
        dash_load_wheel();
        dash_draw_wheel(0, bp82c2, g_dashBuffered ? g755cu : bm7678);
        bp82c2 = 0;
        ret = 0;
        oldfe = e_78e0.fe;
        dash_speedo_init(oldf05 = e_78e0.f0);
        if (!g_dashBuffered)
            oldf05 = dash_move_needle(e_78e0.f0, oldf05, 0);
        if (!g_autoTrans) {
            dash_tach_init(oldfe);
            if (!g_dashBuffered)
                oldfe = dash_move_needle(e_78e0.fe, oldfe, 1);
        } else if (!g_dashBuffered)
            dash_draw_clock();
    } while (g_driveRebuild > 1);
    g_driveRebuild = 0;
    if (r_vid.mode == -2) {
        crtc_set_split(0x63);
        af824ax = 0x64;
    } else
        af824ax = 0;
    road_start();
    if (r_vid.mode != -2) {
        blt.w = 0x140;
        blt.h = 0x12;
        blt.dy = blt.sy = blt.sx = blt.dx = 0;
        (*b_78a2)(g8188w, bm7678, &blt, 1);
    }
    if (g_dashBuffered) {
        g_dashBuffered = 0;
        blt.w = g755cu->width;
        blt.h = 0x64;
        blt.sy = blt.sx = blt.dx = 0;
        blt.dy = 0x64;
        (*b_78a2)(g755cu, bm7678, &blt, 1);
        oldf05 = dash_move_needle(e_78e0.f0, oldf05, 0);
        if (!g_autoTrans)
            oldfe = dash_move_needle(e_78e0.fe, oldfe, 1);
        else
            dash_draw_clock();
        dash_draw_wheel(shiftx5, bp82c2, bm7678);
    }
    (*m_present)();
    for (;;) {
        road_update();
        if (g_demo == 0x63) {
            g_demo--;
            sys_exit();
        }
        if (ret = drvtick_step())
            break;
        shiftx5 = e_78e0.f4;
        if (e_78e0.f1a)
            dash_draw_shifter(bm7678, &e_78e0);
        if (cur != ao8228u)
            cur = ao8228u;
        if (abs(e_78e0.f0 - oldf05) > 1)
            oldf05 = dash_move_needle(e_78e0.f0, oldf05, 0);
        if (!g_autoTrans && abs(e_78e0.fe - oldfe) > 1)
            oldfe = dash_move_needle(e_78e0.fe, oldfe, 1);
        if (shiftx5 != bp82c2) {
            dash_draw_wheel(shiftx5, bp82c2, bm7678);
            bp82c2 = shiftx5;
        }
    }
    marks_clear();
    if (r_vid.mode == -2) {
        blt.w = w;
        blt.h = 0x64;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = g_vga ? 0x64 : 0x65;
        (*b_78a2)(bm7678, g_drawPage == 1 ? g82b4s : g8188w, &blt, 1);
        blt.w = w;
        blt.h = 1;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = 0x64;
        (*b_78a2)(bm7678, g_drawPage == 1 ? g82b4s : g8188w, &blt, 1);
        crtc_set_split(-1);
        af824ax = 0;
        (*dz78b6)(g_drawPage == 1 ? g82b4s->bits.p : g8188w->bits.p, bm7678->bits.p);
        (*dz78b6)(g_drawPage == 1 ? g82b4s->bits.p : g8188w->bits.p, g_drawPage == 1 ? g8188w->bits.p : g82b4s->bits.p);
    } else
        (*dz78b6)(bm7678->bits.p, g8188w->bits.p);
    dash_set_view(1);
    return ret;
}
