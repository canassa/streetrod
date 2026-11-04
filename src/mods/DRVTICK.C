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

/* drvtick_step @ 0000:8EA8 (linear 0x08EA8, 675 bytes) */

int drvtick_step(void)
{
    char key;

    if (bw8aca) {
        switch (jz79a6) {
        case 0:
            g_05d6 = 0;
            if (g_keepCruiseMusic == 0)
                F_16381(1);
            sys_drive_start();
            sys_reset_queues();
            jz79a6 = 1;
            F_11421(-1, "Get ready !", 0);
            g_waitUntil = g_tick + random_int(103) + 110;
            return g_driveEng.fbc | g_oppEngState.fbc;
        case 1:
            if (g_driveEng.f0 > 0) {
                sys_drive_stop();
                g_driveEng.fbc |= 0x81;
                return 1;
            }
            if (g_waitUntil <= g_tick) {
                jz79a6 = 2;
                F_11421(-1, "Go !", 0);
                g7d66n = 1;
            }
            return g_driveEng.fbc | g_oppEngState.fbc;
        case 2:
            race_tick();
            if (g_driveEng.fbc || g_oppEngState.fbc)
                return 1;
            if (g_driveEng.f0 > 0) {
                F_11421(-1, "    ", 0);
                jz79a6 = 3;
            }
            return 0;
        case 3:
            if (g_driveEng.fbc || g_oppEngState.fbc)
                return 1;
            race_tick();
            return 0;
        }
    } else {
        switch (jz79a6) {
        case 0:
            g_05d6 = 0;
            if (g_keepCruiseMusic == 0)
                F_16381(1);
            sys_drive_start();
            sys_reset_queues();
            jz79a6 = 1;
            F_11421(-1, "Cruising in town...", 0);
            g_driveEng.f1a = 1;
            g_driveEng.f18 = 1;
            if (g_driveEng.f24 == 1)
                g_driveEng.f24 = 2;
            g_cruiseRpmStep = g_cruiseRpmSteps[0];
            return 0;
        case 1:
            if (g_demo == 0) {
                key = kbdget_key();
                if (key == 0x1b || key == 0x1f)
                    g_firePressed = 1;
                if (g_mousePresent) {
                    mouse_buttons();
                    g_firePressed |= g_mouseLeft;
                }
                if (g_joystick) {
                    joy_read();
                    g_firePressed |= g_joyButtons;
                }
            }
            if (g_demo == 99) {
                g_demo--;
                sys_exit();
            }
            if ((g_terrain0[ao7d2ev.m7d30 + 1] & 1) || (g_terrain1[ao7d2ev.m7d30 + 1] & 1) || g_firePressed) {
                g_click = 0;
                g_buttons = 0;
                sys_drive_stop();
                return 1;
            }
            g_driveEng.fe += g_cruiseRpmStep;
            if (g_driveEng.fe > 4700)
                g_driveEng.fe = 4700;
            if (g_driveEng.f18)
                g_driveEng.f0 = g_driveEng.fe / g_cruiseRatios[g_driveEng.f18];
            if (g_driveEng.fe > 4500) {
                if (g_driveEng.f18 < g_driveEng.f1c) {
                    g_driveEng.f1a = 1;
                    g_driveEng.f18++;
                    g_cruiseRpmStep = g_cruiseRpmSteps[g_driveEng.f18];
                    g_driveEng.fe = g_cruiseRatios[g_driveEng.f18] * g_driveEng.f0;
                } else
                    g_cruiseRpmStep = 0;
            }
            return 0;
        }
    }
}
