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
static int g_6c82;                         /* DS:6C82 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Eng g79e2g;

int g_51d8[5] = {42, 26, 12, 8, 4};   /* DS:51D8 */
int g_51e2[5] = {0, 200, 125, 71, 56};   /* DS:51E2 */
int g_51ec = 0;   /* DS:51EC */

/* F_08EA8 @ 0000:8EA8 (linear 0x08EA8, 675 bytes) */

int F_08EA8(void)
{
    char key;

    if (bw8aca) {
        switch (jz79a6) {
        case 0:
            g_05d6 = 0;
            if (g_51ec == 0)
                F_16381(1);
            sys_drive_start();
            sys_input_reset();
            jz79a6 = 1;
            F_11421(-1, "Get ready !", 0);
            g_05cc = g_tick + random_int(103) + 110;
            return e_78e0.fbc | g79e2g.fbc;
        case 1:
            if (e_78e0.f0 > 0) {
                sys_drive_stop();
                e_78e0.fbc |= 0x81;
                return 1;
            }
            if (g_05cc <= g_tick) {
                jz79a6 = 2;
                F_11421(-1, "Go !", 0);
                g7d66n = 1;
            }
            return e_78e0.fbc | g79e2g.fbc;
        case 2:
            F_0DA25();
            if (e_78e0.fbc || g79e2g.fbc)
                return 1;
            if (e_78e0.f0 > 0) {
                F_11421(-1, "    ", 0);
                jz79a6 = 3;
            }
            return 0;
        case 3:
            if (e_78e0.fbc || g79e2g.fbc)
                return 1;
            F_0DA25();
            return 0;
        }
    } else {
        switch (jz79a6) {
        case 0:
            g_05d6 = 0;
            if (g_51ec == 0)
                F_16381(1);
            sys_drive_start();
            sys_input_reset();
            jz79a6 = 1;
            F_11421(-1, "Cruising in town...", 0);
            e_78e0.f1a = 1;
            e_78e0.f18 = 1;
            if (e_78e0.f24 == 1)
                e_78e0.f24 = 2;
            g_6c82 = g_51d8[0];
            return 0;
        case 1:
            if (g_0610 == 0) {
                key = kbdget_key();
                if (key == 0x1b || key == 0x1f)
                    g_47ee = 1;
                if (g_05f2) {
                    F_16D57();
                    g_47ee |= g_05ee;
                }
                if (g_471d) {
                    joy_read();
                    g_47ee |= g_471c;
                }
            }
            if (g_0610 == 99) {
                g_0610--;
                sys_exit();
            }
            if ((g_terrain0[ao7d2ev.m7d30 + 1] & 1) || (g_terrain1[ao7d2ev.m7d30 + 1] & 1) || g_47ee) {
                g_4736 = 0;
                g_4732 = 0;
                sys_drive_stop();
                return 1;
            }
            e_78e0.fe += g_6c82;
            if (e_78e0.fe > 4700)
                e_78e0.fe = 4700;
            if (e_78e0.f18)
                e_78e0.f0 = e_78e0.fe / g_51e2[e_78e0.f18];
            if (e_78e0.fe > 4500) {
                if (e_78e0.f18 < e_78e0.f1c) {
                    e_78e0.f1a = 1;
                    e_78e0.f18++;
                    g_6c82 = g_51d8[e_78e0.f18];
                    e_78e0.fe = g_51e2[e_78e0.f18] * e_78e0.f0;
                } else
                    g_6c82 = 0;
            }
            return 0;
        }
    }
}
