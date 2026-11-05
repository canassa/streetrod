#include "DRVTICK.H"
#include "CRUISE.H"
#include "DASH.H"
#include "INPUTDAT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "PERF.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "STRUCTS.H"
#include "SYS.H"

static int g_cruiseRpmStep;

struct DrivingState g_oppEngState;

static int g_cruiseRpmSteps[DRIVE_GEAR_SLOTS] = {42, 26, 12, 8, 4};
static int g_cruiseRatios[DRIVE_GEAR_SLOTS] = {0, 200, 125, 71, 56};
int g_keepCruiseMusic = 0;


int dt_step(void)
{
    char key;

    if (g_runRacingFlag) {
        switch (g_phaseDriveRunState) {
        case DRIVE_START:
            g_unused6 = 0;
            if (g_keepCruiseMusic == 0)
                mu_suspend(1);
            sy_drive_start();
            sy_reset_queues();
            g_phaseDriveRunState = DRIVE_STAGING;
            sc_status_text(-1, "Get ready !", 0);
            g_waitUntil = g_tick + rn_int(START_WAIT_RANDOM_TICKS) + START_WAIT_BASE_TICKS;
            return g_driveEng.stopFlags | g_oppEngState.stopFlags;
        case DRIVE_STAGING:
            if (g_driveEng.speed > 0) {
                sy_drive_stop();
                g_driveEng.stopFlags |= (STOP_LOST | STOP_JUMP_START);
                return 1;
            }
            if (g_waitUntil <= g_tick) {
                g_phaseDriveRunState = DRIVE_WAIT_FOR_MOTION;
                sc_status_text(-1, "Go !", 0);
                g_startingGoActive = 1;
            }
            return g_driveEng.stopFlags | g_oppEngState.stopFlags;
        case DRIVE_WAIT_FOR_MOTION:
            ra_tick();
            if (g_driveEng.stopFlags || g_oppEngState.stopFlags)
                return 1;
            if (g_driveEng.speed > 0) {
                sc_status_text(-1, "    ", 0);
                g_phaseDriveRunState = DRIVE_RUNNING;
            }
            return 0;
        case DRIVE_RUNNING:
            if (g_driveEng.stopFlags || g_oppEngState.stopFlags)
                return 1;
            ra_tick();
            return 0;
        }
    } else {
        switch (g_phaseDriveRunState) {
        case DRIVE_START:
            g_unused6 = 0;
            if (g_keepCruiseMusic == 0)
                mu_suspend(1);
            sy_drive_start();
            sy_reset_queues();
            g_phaseDriveRunState = DRIVE_CRUISING;
            sc_status_text(-1, "Cruising in town...", 0);
            g_driveEng.shifterDirty = TRUE;
            g_driveEng.gearIndex = DRIVE_FIRST;
            if (g_driveEng.autoSelector == SELECTOR_NEUTRAL)
                g_driveEng.autoSelector = SELECTOR_DRIVE;
            g_cruiseRpmStep = g_cruiseRpmSteps[DRIVE_NEUTRAL];
            return 0;
        case DRIVE_CRUISING:
            if (g_demo == 0) {
                key = kg_key();
                if (key == ASCII_ESCAPE || key == KEY_FIRE)
                    g_firePressed = 1;
                if (g_mouse.present) {
                    mo_buttons();
                    g_firePressed |= g_mouse.left;
                }
                if (g_joystick) {
                    jy_read();
                    g_firePressed |= g_joyButtons;
                }
            }
            if (g_demo == 99) {
                g_demo--;
                sy_exit();
            }
            if ((g_terrain0[g_trackViewMotion.roadIndex + 1] & ROADSIDE_DESTINATION) ||
                (g_terrain1[g_trackViewMotion.roadIndex + 1] & ROADSIDE_DESTINATION) || g_firePressed) {
                g_click = 0;
                g_buttons = 0;
                sy_drive_stop();
                return 1;
            }
            g_driveEng.rpm += g_cruiseRpmStep;
            if (g_driveEng.rpm > CRUISE_MAX_RPM)
                g_driveEng.rpm = CRUISE_MAX_RPM;
            if (g_driveEng.gearIndex)
                g_driveEng.speed = g_driveEng.rpm / g_cruiseRatios[g_driveEng.gearIndex];
            if (g_driveEng.rpm > CRUISE_SHIFT_RPM) {
                if (g_driveEng.gearIndex < g_driveEng.topGear) {
                    g_driveEng.shifterDirty = TRUE;
                    g_driveEng.gearIndex++;
                    g_cruiseRpmStep = g_cruiseRpmSteps[g_driveEng.gearIndex];
                    g_driveEng.rpm = g_cruiseRatios[g_driveEng.gearIndex] * g_driveEng.speed;
                } else
                    g_cruiseRpmStep = 0;
            }
            return 0;
        }
    }
}
