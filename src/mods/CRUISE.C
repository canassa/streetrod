#include "STRUCTS.H"
#include "CRUISE.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRIVE.H"
#include "GARAGE.H"
#include "MAIN.H"
#include "M_02B9A.H"
#include "RACE.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
int jz79a6;

char g_savedPal12 = -1;   /* DS:51AC */
char g_savedPal3 = -1;   /* DS:51AD */

/* cruise_go @ 0000:8D26 (linear 0x08D26, 118 bytes) */

void cruise_go(void)
{
    g_firePressed = g_mouseLeft = g_joyButtons = g_buttons = 0;
    if (g_scene) {
        if (a_7eaap.currentCar->fuel > 2)
            a_7eaap.currentCar->fuel -= 2;
        else
            a_7eaap.currentCar->fuel = 0;
        (*bl78be)();
        if (g_firePressed == 0) {
            cursor_ctl(CURSOR_HIDE_NOW);
            g_autoTrans = !g_4806[a_7eaap.currentCar->transmission->idx].variant;
            race_reset_eng(&g_driveEng);
            jz79a6 = 0;
            bw8aca = 0;
            cruise_drive();
        }
    }
}

/* cruise_rival_paint @ 0000:8D9C (linear 0x08D9C, 145 bytes) */

void cruise_rival_paint(int mode)
{
    char a, b;

    if (r_vid.mode == -2 || r_vid.mode == -6) {
        if (mode == -1) {
            a = g_savedPal12;
            b = g_savedPal3;
            g_savedPal12 = g_savedPal3 = -1;
        } else {
            if (g_savedPal12 == -1) {
                g_savedPal12 = g_palette[12];
                g_savedPal3 = g_palette[3];
            }
            a = g_50c2[mode][0];
            b = g_50c2[mode][1];
        }
        if (gz8249) {
            F_112CB(12, a);
            F_112CB(3, b);
        }
        g_palette[12] = a;
        g_palette[3] = b;
    }
}

int g7d66n;

/* cruise_drive @ 0000:8E2D (linear 0x08E2D, 123 bytes) */

int cruise_drive(void)
{
    int r, saved;

    if (bw8aca)
        (*bl78be)();
    ao8228u = g7d66n = 0;
    saved = m_8272.paintScheme;
    F_067CF(a_7eaap.currentCar->paintScheme);
    if (bw8aca)
        cruise_rival_paint(saved);
    r = drive_run();
    sound_squeal(0);
    sound_siren(0);
    g_cursorX = g_savedCursorX;
    g_cursorY = g_savedCursorY;
    g_savedCursorX = 0;
    g_savedCursorY = 0;
    return r;
}
