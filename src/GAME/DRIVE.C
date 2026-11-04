#include <stdlib.h>
#include "STRUCTS.H"
#include "DRIVE.H"
#include "ARENA.H"
#include "CARPIC.H"
#include "CRTC.H"
#include "DASH.H"
#include "DISK.H"
#include "DRVTICK.H"
#include "MAIN.H"
#include "MARKS.H"
#include "PC.H"
#include "POPUP.H"
#include "RACE.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIEW.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap far *g8188w;

/* dv_run @ 0F38:7B22 (linear 0x16EA2, 1540 bytes)

   Gearbox/drive screen loop: draws the dashboard, then redraws the changing parts until
   dt_step reports an input. Returns that value. */

int dv_run(void)
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

    sv = r_vid.copyToFront;
    do {
        da_set_view(3);
        ma_clear();
        (*g_videoOps.blank)();
        g_dashBuffered = r_vid.mode == VID_CGA && g_egaCard;
        if (g_dashBuffered) {
            pu_grab(0, DASH_Y, SCREEN_WIDTH, DASH_HEIGHT, g_bufBackgroundDashPtr);
            r_vid.copyToFront = 1;
            sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
            r_vid.copyToFront = sv;
        }
        if (r_vid.mode == VID_EGA)
            g_driveRebuild = 1;
        else
            g_driveRebuild = 0;
        src = di_bm(0x3f6, ARENA_HIGH_STACK);
        if (g_autoTrans) {
            pict = di_bm(0x4d8, ARENA_HIGH_STACK);
            blt.w = pict->width;
            blt.h = pict->height;
            blt.sx = blt.sy = 0;
            blt.dx = 0x78;
            blt.dy = 0x7a;
            (*g_videoOps.copy)(pict, src, &blt, 1);
            ma_pop_high(1);
        }
        w = blt.w = src->width;
        blt.h = 0x65;
        blt.sx = 0;
        blt.sy = blt.dx = blt.dy = 0;
        (*g_videoOps.copy)(src, g8188w, &blt, 1);
        if (r_vid.mode == VID_EGA)
            (*g_videoOps.copy)(src, g82b4s, &blt, 1);
        blt.w = src->width;
        blt.h = 0x5a;
        blt.sx = blt.dx = 0;
        blt.sy = DASH_Y;
        blt.dy = (r_vid.mode == VID_EGA || g_dashBuffered) ? 0 : DASH_Y;
        (*g_videoOps.copy)(src, g_dashBuffered ? g_bufBackgroundDashPtr : bm7678, &blt, 1);
        ma_pop_high(1);
        vw_load();
        da_init_shifter(g_driveEng.autoSelector, g_driveEng.gearCount);
        da_draw_shifter(g_dashBuffered ? g_bufBackgroundDashPtr : bm7678, &g_driveEng);
        shiftx5 = 0;
        g_steeringOld = 0x64;
        da_load_wheel();
        da_draw_wheel(0, g_steeringOld, g_dashBuffered ? g_bufBackgroundDashPtr : bm7678);
        g_steeringOld = 0;
        ret = 0;
        oldfe = g_driveEng.rpm;
        da_speedo_init(oldf05 = g_driveEng.speed);
        if (!g_dashBuffered)
            oldf05 = da_move_needle(g_driveEng.speed, oldf05, 0);
        if (!g_autoTrans) {
            da_tach_init(oldfe);
            if (!g_dashBuffered)
                oldfe = da_move_needle(g_driveEng.rpm, oldfe, 1);
        } else if (!g_dashBuffered)
            da_draw_clock();
    } while (g_driveRebuild > 1);
    g_driveRebuild = 0;
    if (r_vid.mode == VID_EGA) {
        ct_set_split(DASH_Y - 1);
        g_displaySplitYVal = DASH_Y;
    } else
        g_displaySplitYVal = 0;
    rd_start();
    if (r_vid.mode != VID_EGA) {
        blt.w = SCREEN_WIDTH;
        blt.h = 0x12;
        blt.dy = blt.sy = blt.sx = blt.dx = 0;
        (*g_videoOps.copy)(g8188w, bm7678, &blt, 1);
    }
    if (g_dashBuffered) {
        g_dashBuffered = 0;
        blt.w = g_bufBackgroundDashPtr->width;
        blt.h = DASH_HEIGHT;
        blt.sy = blt.sx = blt.dx = 0;
        blt.dy = DASH_Y;
        (*g_videoOps.copy)(g_bufBackgroundDashPtr, bm7678, &blt, 1);
        oldf05 = da_move_needle(g_driveEng.speed, oldf05, 0);
        if (!g_autoTrans)
            oldfe = da_move_needle(g_driveEng.rpm, oldfe, 1);
        else
            da_draw_clock();
        da_draw_wheel(shiftx5, g_steeringOld, bm7678);
    }
    (*g_videoOps.show)();
    for (;;) {
        rd_update();
        if (g_demo == 0x63) {
            g_demo--;
            sy_exit();
        }
        if (ret = dt_step())
            break;
        shiftx5 = g_driveEng.steering;
        if (g_driveEng.shifterDirty)
            da_draw_shifter(bm7678, &g_driveEng);
        if (cur != ao8228u)
            cur = ao8228u;
        if (abs(g_driveEng.speed - oldf05) > 1)
            oldf05 = da_move_needle(g_driveEng.speed, oldf05, 0);
        if (!g_autoTrans && abs(g_driveEng.rpm - oldfe) > 1)
            oldfe = da_move_needle(g_driveEng.rpm, oldfe, 1);
        if (shiftx5 != g_steeringOld) {
            da_draw_wheel(shiftx5, g_steeringOld, bm7678);
            g_steeringOld = shiftx5;
        }
    }
    ma_clear();
    if (r_vid.mode == VID_EGA) {
        blt.w = w;
        blt.h = DASH_HEIGHT;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = g_vga ? DASH_Y : 0x65;
        (*g_videoOps.copy)(bm7678, g_viewPages.draw == 1 ? g82b4s : g8188w, &blt, 1);
        blt.w = w;
        blt.h = 1;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = DASH_Y;
        (*g_videoOps.copy)(bm7678, g_viewPages.draw == 1 ? g82b4s : g8188w, &blt, 1);
        ct_set_split(-1);
        g_displaySplitYVal = 0;
        (*g_videoOps.copyPage)(g_viewPages.draw == 1 ? g82b4s->bits.p : g8188w->bits.p, bm7678->bits.p);
        (*g_videoOps.copyPage)(g_viewPages.draw == 1 ? g82b4s->bits.p : g8188w->bits.p, g_viewPages.draw == 1 ? g8188w->bits.p : g82b4s->bits.p);
    } else
        (*g_videoOps.copyPage)(bm7678->bits.p, g8188w->bits.p);
    da_set_view(1);
    return ret;
}
