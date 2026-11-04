#include "STRUCTS.H"
#include "M_08EA8.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_02DB6.H"
#include "M_02DF4.H"
#include "M_08D26.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_113A2.H"
#include "M_15236.H"
#include "M_1628A.H"
#include "M_16D1A.H"
#include "SLOTS.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (CLAUDE.md 4f). */
static int g_6c82;                         /* DS:6C82 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (CLAUDE.md 4e). */
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
            g_5d6 = 0;
            if (g_51ec == 0)
                F_16381(1);
            F_01AAD();
            F_00E24();
            jz79a6 = 1;
            F_11421(-1, "Get ready !", 0);
            g_05cc = g_tick + random_int(103) + 110;
            return e_78e0.fbc | g79e2g.fbc;
        case 1:
            if (e_78e0.f0 > 0) {
                F_01B27();
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
            g_5d6 = 0;
            if (g_51ec == 0)
                F_16381(1);
            F_01AAD();
            F_00E24();
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
                key = F_02DB6();
                if (key == 0x1b || key == 0x1f)
                    g_47ee = 1;
                if (g_05f2) {
                    F_16D57();
                    g_47ee |= g_5ee;
                }
                if (g_471d) {
                    F_02DF4();
                    g_47ee |= g_471c;
                }
            }
            if (g_0610 == 99) {
                g_0610--;
                F_00FEA();
            }
            if ((g_terrain0[ao7d2ev.m7d30 + 1] & 1) || (g_terrain1[ao7d2ev.m7d30 + 1] & 1) || g_47ee) {
                g_4736 = 0;
                g_4732 = 0;
                F_01B27();
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
