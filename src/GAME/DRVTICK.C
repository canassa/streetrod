#include "STRUCTS.H"
#include "DRVTICK.H"
#include "CRUISE.H"
#include "DASH.H"
#include "JOY.H"
#include "KBDGET.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "RACE.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SLOTS.H"
#include "SYS.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_cruiseRpmStep;                         /* DS:6C82 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Eng g_oppEngState;

int g_cruiseRpmSteps[5] = {42, 26, 12, 8, 4};   /* DS:51D8 */
int g_cruiseRatios[5] = {0, 200, 125, 71, 56};   /* DS:51E2 */
int g_keepCruiseMusic = 0;   /* DS:51EC */

/* dt_step @ 0000:8EA8 (linear 0x08EA8, 675 bytes) */

int dt_step(void)
{
    char key;

    if (g_runRacingFlag) {
        switch (g_phaseDriveRunState) {
        case DRIVE_START:
            g_05d6 = 0;
            if (g_keepCruiseMusic == 0)
                mu_suspend(1);
            sy_drive_start();
            sy_reset_queues();
            g_phaseDriveRunState = DRIVE_STAGING;
            sc_status_text(-1, "Get ready !", 0);
            g_waitUntil = g_tick + rn_int(103) + 110;
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
            g_05d6 = 0;
            if (g_keepCruiseMusic == 0)
                mu_suspend(1);
            sy_drive_start();
            sy_reset_queues();
            g_phaseDriveRunState = DRIVE_CRUISING;
            sc_status_text(-1, "Cruising in town...", 0);
            g_driveEng.shifterDirty = 1;
            g_driveEng.gearIndex = 1;
            if (g_driveEng.autoSelector == SELECTOR_NEUTRAL)
                g_driveEng.autoSelector = SELECTOR_DRIVE;
            g_cruiseRpmStep = g_cruiseRpmSteps[0];
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
            if ((g_terrain0[g_trackViewMotion.roadIndex + 1] & 1) || (g_terrain1[g_trackViewMotion.roadIndex + 1] & 1) || g_firePressed) {
                g_click = 0;
                g_buttons = 0;
                sy_drive_stop();
                return 1;
            }
            g_driveEng.rpm += g_cruiseRpmStep;
            if (g_driveEng.rpm > 4700)
                g_driveEng.rpm = 4700;
            if (g_driveEng.gearIndex)
                g_driveEng.speed = g_driveEng.rpm / g_cruiseRatios[g_driveEng.gearIndex];
            if (g_driveEng.rpm > 4500) {
                if (g_driveEng.gearIndex < g_driveEng.gearCount) {
                    g_driveEng.shifterDirty = 1;
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
