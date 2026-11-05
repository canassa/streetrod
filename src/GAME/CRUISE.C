#include "CRUISE.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRIVE.H"
#include "GARAGE.H"
#include "INPUTDAT.H"
#include "MAIN.H"
#include "RACE.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

enum DrivePhase g_phaseDriveRunState;

static char g_savedPal12 = -1;
static char g_savedPal3 = -1;


void cr_go(void)
{
    g_firePressed = g_mouse.left = g_joyButtons = g_buttons = 0;
    if (g_scene) {
        if (g_driver.currentCar->fuel > TOWN_TRIP_FUEL)
            g_driver.currentCar->fuel -= TOWN_TRIP_FUEL;
        else
            g_driver.currentCar->fuel = 0;
        (*g_videoOps.blank)();
        if (g_firePressed == 0) {
            cu_ctl(CURSOR_HIDE_NOW);
            g_autoTrans = !g_parts[g_driver.currentCar->transmission->idx].variant;
            ra_reset_eng(&g_driveEng);
            g_phaseDriveRunState = DRIVE_START;
            g_runRacingFlag = 0;
            cr_drive();
        }
    }
}


void cr_rival_paint(enum PaintScheme scheme)
{
    char primary, highlight;

    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) {
        if (scheme == RIVAL_PAINT_RESTORE) {
            primary = g_savedPal12;
            highlight = g_savedPal3;
            g_savedPal12 = g_savedPal3 = -1;
        } else {
            if (g_savedPal12 == -1) {
                g_savedPal12 = g_palette[PALETTE_RIVAL_PRIMARY];
                g_savedPal3 = g_palette[PALETTE_RIVAL_SECONDARY];
            }
            primary = g_paint[scheme][PAINT_BASE];
            highlight = g_paint[scheme][PAINT_HIGHLIGHT];
        }
        if (g_displayEnabled) {
            vi_palette_reg(PALETTE_RIVAL_PRIMARY, primary);
            vi_palette_reg(PALETTE_RIVAL_SECONDARY, highlight);
        }
        g_palette[PALETTE_RIVAL_PRIMARY] = primary;
        g_palette[PALETTE_RIVAL_SECONDARY] = highlight;
    }
}

int g_startingGoActive;


int cr_drive(void)
{
    int r;
    enum PaintScheme savedPaint;

    if (g_runRacingFlag)
        (*g_videoOps.blank)();
    g_startLightGo = g_startingGoActive = 0;
    savedPaint = g_carDraw.paintScheme;
    ga_palette(g_driver.currentCar->paintScheme);
    if (g_runRacingFlag)
        cr_rival_paint(savedPaint);
    r = dv_run();
    so_squeal(0);
    so_siren(0);
    g_cursorX = g_savedCursorX;
    g_cursorY = g_savedCursorY;
    g_savedCursorX = 0;
    g_savedCursorY = 0;
    return r;
}
