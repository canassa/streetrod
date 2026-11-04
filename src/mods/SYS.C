#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "SYS.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "FAME.H"
#include "GAME.H"
#include "GARAGE.H"
#include "HERCDET.H"
#include "HOTSPOT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "KBDOFF.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SONG.H"
#include "SOUND.H"
#include "VIDDET.H"
#include "VIDEO.H"

#pragma intrinsic(inp, outp, _enable, _disable, abs)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_cursorDrawX;                         /* DS:6C46 */
static int g_cursorDrawY;                         /* DS:6C48 */
static char g_cursorDirty;                        /* DS:6C4A */
static int g_keyGroup;                         /* DS:6C4C */
static char g_6c4e;                        /* DS:6C4E */
static int g_demoChoice;                        /* DS:6C50 */
static int g_vibGain;                        /* DS:6C52 */
static int g_vibDivisor;                        /* DS:6C54 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Dev bs79aa;
struct Dev ao79c6t;
void (interrupt far *w_7aaa)(void);
char g7f32s[80];
int g8b8eb;
struct Lims ao8b90v;
struct Hot *at8ba8v;
int az8baa;
int cz8bc0y;
int g8ef6q;
int g_8ef8;
int aj8efax;
int bw8efc;

int g_timerFirst = -1;   /* DS:0566 */
int g_timerHead = -1;   /* DS:0568 */
int g_timerFree = 0;   /* DS:056A */
long g_timerNext = 2147483647L;   /* DS:056C */
char g_0570 = 0;   /* DS:0570 */
char g_timerBusy = 0;   /* DS:0571 */
struct Link g_timers[4] = {{0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}};   /* DS:0572 */
int g_05ba = 0;   /* DS:05BA */
int g_05bc[8] = {0, 0, 0, 0, 0, 0, 0, 0};   /* DS:05BC */
long g_waitUntil = 0L;   /* DS:05CC */
int g_anyInput = 0;   /* DS:05D0 */
int g_inputKey = 0;   /* DS:05D2 */
int g_05d4 = 32767;   /* DS:05D4 */
int g_05d6 = 0;   /* DS:05D6 */
int g_05d8 = 0;   /* DS:05D8 */
int g_05da = 0;   /* DS:05DA */
int g_msgTicks = 350;   /* DS:05DC */
int g_clickEnds = 0;   /* DS:05DE */
int g_kbdHooked = 0;   /* DS:05E0 */
int g_mouseDx = 0;   /* DS:05E2 */
int g_mouseDy = 0;   /* DS:05E4 */
int g_mouseAccX = 0;   /* DS:05E6 */
int g_mouseAccY = 0;   /* DS:05E8 */
int g_mouseShiftX = 0;   /* DS:05EA */
int g_mouseShiftY = 0;   /* DS:05EC */
int g_mouseLeft = 0;   /* DS:05EE */
int g_mouseRight = 10;   /* DS:05F0 */
char g_mousePresent = 3;   /* DS:05F2 */
char g_05f3 = 0;   /* DS:05F3 */
int g_05f4[2] = {0, 0};   /* DS:05F4 */
long g_tick = 0L;   /* DS:05F8 */
long g_05fc = 0L;   /* DS:05FC */
long g_0600 = 0L;   /* DS:0600 */
long g_0604 = 0L;   /* DS:0604 */
char g_tickDiv = 0;   /* DS:0608 */
char g_driveDiv = 0;   /* DS:0609 */
int g_060a = 0;   /* DS:060A */
char *g_errRead = "cannot read drive ?";   /* DS:060C */
char *g_errWriteProt = "disk ? is write protected";   /* DS:060E */
int g_demo = 0;   /* DS:0610 */
int g_autoDrive = 0;   /* DS:0612 */

/* sys_hover_reset @ 0000:0D78 (linear 0x00D78, 33 bytes) */

void sys_hover_reset(void)
{
    g_cursorDirty = 1;
    ao8b90v.x2 = -1;
    az8baa = 0x7d00;
    g_click = 0;
    g_click2 = 0;
    kbdget_key();
}

/* sys_music_pause @ 0000:0D99 (linear 0x00D99, 63 bytes) */

void sys_music_pause(void)
{
    if (g_voice->off) {
        g_voice->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_speakerOn = 0;
        g_voice->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_offTunes;
        sys_hover_reset();
    }
}

/* sys_music_resume @ 0000:0DD8 (linear 0x00DD8, 76 bytes) */

void sys_music_resume(void)
{
    if (g_musicOn && !g_voice->off && g_voice->lc != g_voice->l8) {
        g_voice->off = 1;
        g_voice->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_offSquelch;
        sys_hover_reset();
    }
}

/* sys_input_reset @ 0000:0E24 (linear 0x00E24, 231 bytes) */

void sys_input_reset(void)
{
    int i;
    long deadline;

    g_timerFirst = g_timerHead = -1;
    g_timerFree = 0;
    g_timerNext = 0x7fffffffL;
    g_0570 = g_timerBusy = 0;
    for (i = 0; i < 4; i++)
        g_timers[i].nxt = i + 1;
    g_05ba = -1;
    deadline = g_tick + 10;
    while (deadline > g_tick) {
        for (i = 0; kbdget_key(); i++)
            ;
        if (i)
            deadline = g_tick + 10;
    }
    g_waitUntil = 0;
    if (g_demo == 0 && g_mousePresent) {
        mouse_poll();
        g_mouseAccX = g_mouseAccY = 0;
    }
    g_click = 0;
    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_buttons = g_keyDx = g_keyDy = g_lastDx = g_lastDy = 0;
}

/* sys_timer_remove @ 0000:0F0B (linear 0x00F0B, 172 bytes) */

void sys_timer_remove(int n)
{
    register int i;

    i = g_timers[n].nxt;
    if (i >= 0)
        g_timers[i].prv = g_timers[n].prv;
    else {
        i = g_timers[n].prv;
        g_timerFirst = i;
        if (i < 0)
            g_timerNext = 0x7fffffffL;
        else
            g_timerNext = g_timers[i].key;
    }
    i = g_timers[n].prv;
    if (i >= 0)
        g_timers[i].nxt = g_timers[n].nxt;
    else
        g_timerHead = g_timers[n].nxt;
    g_timers[n].nxt = g_timerFree;
    g_timerFree = n;
}

/* sys_unhook @ 0000:0FB7 (linear 0x00FB7, 51 bytes) */

void sys_unhook(void)
{
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);
    _dos_setvect(8, w_7aaa);
    _enable();
    kbdoff_unhook();
    g_kbdHooked = 0;
    g_sysHooked = 0;
}

/* sys_exit @ 0000:0FEA (linear 0x00FEA, 113 bytes) */

void sys_exit(void)
{
    if (g_demo) {
        g_demo = (g_demo == 0x62) ? 0 : 1;
        screen_wait_msg("Quit demo");
        screen_fill_rect(0, 0, 320, 200, 0);
    }
    if (g_sysHooked)
        sys_unhook();
    if (g_videoSet) {
        (*m_present)();
        viddet_restore();
    }
    outp(0x61, inp(0x61) & 0xfc);
    g_speakerOn = 0;
    exit(5);
}

char g_064d = 0;   /* DS:064D */
char g_keymapGroup = 0;   /* DS:064E */
struct KeyMap g_keymapSaved[30] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};   /* DS:0650 */

/* sys_crit_error @ 0000:105B (linear 0x0105B, 182 bytes) */

void sys_crit_error(unsigned deverr, unsigned char errcode)
{
    int disk;
    int action;

    disk = (deverr & 0x8000) == 0;
    if (disk) {
        if (deverr & 0x1000)
            action = 1;
        else
            action = 2;
    } else
        action = 1;
    switch (errcode) {
    case 0:
        g_errWriteProt[5] = deverr + 'A';
        screen_wait_msg(g_errWriteProt);
        break;
    case 2:
        g_errRead[strlen(g_errRead) - 1] = deverr + 'A';
        screen_wait_msg(g_errRead);
        break;
    case 9:
        screen_wait_msg("printer out of paper");
        break;
    default:
        if (disk)
            screen_wait_msg("disk error");
        else {
            screen_wait_msg("hardware error");
            action = 2;
        }
    }
    if (action == 2)
        sys_exit();
    _hardresume(action);
}

/* sys_init @ 0000:1111 (linear 0x01111, 461 bytes) */

void sys_init(void)
{
    int sumx;
    int sumy;
    int first;
    int v;
    int j;

    sumx = 0;
    sumy = 0;
    g8ef6q = 10;
    g_8ef8 = 2;
    aj8efax = 5;
    bw8efc = 5;
    outp(0x201, 0);
    outp(0x201, 0);
    first = inp(0x201);
    for (j = 1; j; j++)
        if (((v = inp(0x201)) & 0xf3) == 0xf0)
            break;
    if (g_demo == 0 && j != 0 && v != first) {
        g_joystick = 1;
        joy_read();
        sumx = g_joyX;
        sumy = g_joyY;
        joy_read();
        sumx += g_joyX;
        sumy += g_joyY;
        joy_read();
        sumx += g_joyX;
        sumy += g_joyY;
        g_joyCenterX = sumx / 3;
        g_joyCenterY = sumy / 3;
        g_joyMinX = -(g_joyCenterX >> 1);
        g_joyMaxX = g_joyCenterX >> 1;
        g_joyMinY = -(g_joyCenterY >> 1);
        g_joyMaxY = g_joyCenterY >> 1;
    }
    if (g_demo == 0 && g_useMouse && mouse_reset() == -1) {
        g_mousePresent = 1;
        g_mouseAccX = g_mouseAccY = 0;
        g_mouseShiftX = 1;
        g_mouseShiftY = 1;
    } else
        g_mousePresent = 0;
    g_cursorDrawX = -1;
    g_cursorDrawY = -1;
    bs79aa.off = 0;
    bs79aa.l8 = g_song;
    bs79aa.cur = bs79aa.lc = bs79aa.l8;
    bs79aa.f16 = bs79aa.f14 = g_songRepeats;
    bs79aa.f1a = bs79aa.f18 = g_songTones;
    ao79c6t.off = 0;
    ao79c6t.f1a = ao79c6t.f18 = g_jingleTones;
    _harderr(sys_crit_error);
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0x40);
    w_7aaa = _dos_getvect(8);
    _dos_setvect(8, sys_timer_isr);
    _enable();
    kbdget_hook();
    g_kbdHooked = 1;
    outp(0x43, 0xb6);
    sys_input_reset();
    g_sysHooked = 1;
}

/* sys_keymap_set @ 0000:12DE (linear 0x012DE, 179 bytes) */

void sys_keymap_set(int n)
{
    struct KeyMap *p;
    int num;
    struct KeyMap *q;
    char c;

    num = 1;
    if (g_demo)
        g_demoKey = 0;
    g_keyGroup = n;
    sys_keymap_restore(0);
    g_keymapGroup = n;
    p = g_keyMaps[n];
    q = g_keymapSaved;
    for (; p->key; p++, q++, num++) {
        if (num >= 30)
            break;
        q->val = g_scanMap[q->key = c = p->key];
        g_scanMap[c] = p->val;
    }
    p = g_keyMaps[42];
    q->val = g_scanMap[q->key = c = p->key];
    g_scanMap[c] = p->val;
    q[1].key = 0;
}

/* sys_keymap_restore @ 0000:1391 (linear 0x01391, 134 bytes) */

int sys_keymap_restore(int keep)
{
    int i;
    struct KeyMap *p;

    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_keyDx = g_keyDy = g_lastDx = g_lastDy = 0;
    i = g_keymapGroup;
    if (i) {
        if (keep) {
            for (p = g_keymapSaved; p->key; p++)
                if (p->val >= 0x20)
                    g_scanMap[p->key] = p->val;
        } else {
            for (p = g_keymapSaved; p->key; p++)
                g_scanMap[p->key] = p->val;
            g_keymapGroup = 0;
        }
    }
    return i;
}

char g_protectPal[16] = {0, 0, 8, 7, 15, 7, 15, 8, 7, 2, 4, 12, 5, 10, 6, 14};   /* DS:0692 */
int g_06a2 = 0;   /* DS:06A2 */
int g_demoQuit = 0;   /* DS:06A4 */
char g_demoKeyDown = 0;   /* DS:06A6 */
char g_demoMoving = 0;   /* DS:06A7 */
int g_demoCount = 0;   /* DS:06A8 */
int g_demoLastGroup = 0;   /* DS:06AA */
int g_06ac = 0;   /* DS:06AC */
int g_demoTargetX = 0;   /* DS:06AE */
int g_demoTargetY = 0;   /* DS:06B0 */
int g_demoScriptPos = 0;   /* DS:06B2 */
int g_demoScript[22] = {-1, 0, -1, 0, 1, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 0, 0, 2};   /* DS:06B4 */
char g_cursorBusy = 0;   /* DS:06E0 */
int g_vibCount = 1;   /* DS:06E2 */
int g_06e4 = 0;   /* DS:06E4 */
int g_overRev = 0;   /* DS:06E6 */
char g_driveBusy = 0;   /* DS:06E8 */
char g_lastFire = 0;   /* DS:06E9 */
char g_06ea = 0;   /* DS:06EA */

/* sys_wait_input @ 0000:1417 (linear 0x01417, 1068 bytes) */

int sys_wait_input(int ticks)
{
    register struct Link *p;
    long end;
    int x;
    register int i;
    int y;
    int res;
    char flag;
    int j;
    char old;
    int bit;

    res = 0;
    j = 0;
    bit = 0;
    if (g_keyGroup == 0x16) {
        aq8256z = 1;
        F_11421(-1, "", 0);
        aq8256z = 1;
        F_11421(g_bannerX, "", cz8bc0y);
        g7f32s[0] = 0;
    }
    flag = g_waitUntil != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_demo == 0x63 && g_demoQuit != 0x63) {
            g_demoQuit = 0x63;
            sys_exit();
        }
        if (g_protectCycle && (r_vid.mode == -2 || r_vid.mode == -6)
            && !(g_tick & 0xf) && g_musicOn && g_speakerOn) {
            old = g_protectPal[8];
            g_protectPal[8] = g_protectPal[7];
            g_protectPal[7] = g_protectPal[6];
            g_protectPal[6] = old;
            F_112CB(6, old);
            F_112CB(7, g_protectPal[7]);
            F_112CB(8, g_protectPal[8]);
        }
        res = kbdget_key();
        if (res > 0) {
            if (g_anyInput) {
                g_inputKey = res;
                return 0x3eb;
            }
            if (res == 0x7f)
                sys_exit();
        } else if (res < 0) {
            res += 0x32;
            if (res == -0x28) {
                if (g_voiceNum == 1) {
                    if (g_musicOn) {
                        sys_music_pause();
                        g_musicOn = 0;
                    } else {
                        g_musicOn = 1;
                        sys_music_resume();
                    }
                }
            } else
                return res;
        }
        if (g_timerNext <= g_tick) {
            p = &g_timers[i = g_timerFirst];
            switch (p->fa) {
            case 0:
            default:
                sys_timer_remove(i);
                break;
            case 1:
                res = p->fc;
                sys_timer_remove(i);
                video_anim_resume(res);
                break;
            }
        }
        if (g_demo == 0 && g_mousePresent) {
            mouse_poll();
            if (g_cursorDirty |= (g_mouseDx != 0 || g_mouseDy != 0)) {
                g_cursorX += g_mouseDx;
                g_cursorY += g_mouseDy;
            }
        }
        if (g_click | g_click2) {
            _disable();
            g_6c4e = g_click;
            x = g_clickX;
            y = g_clickY;
            g_click2 = g_click = 0;
            _enable();
            if (g_clickEnds) {
                g_inputKey = 0;
                return 0x3eb;
            }
            if (g8b8eb) {
                if (i = (x >= ao8b90v.x1 && x < ao8b90v.x2 && y >= ao8b90v.y1 && y < ao8b90v.y2)
                        ? az8baa : menu_hit(x, y))
                    return i;
            }
            if (g_anyInput) {
                g_inputKey = 0;
                return 0x3eb;
            }
        } else if (flag && g_waitUntil < g_tick) {
            g_waitUntil = 0L;
            return 0x3e8;
        }
        if (g_4733 > 0 && g_cursorDirty) {
            _disable();
            if ((g_cursorDrawX = g_cursorX) < 0)
                g_cursorDrawX = g_cursorX = 0;
            else if (g_cursorDrawX > 0x13a)
                g_cursorDrawX = g_cursorX = 0x13a;
            if ((g_cursorDrawY = g_cursorY) < 0)
                g_cursorDrawY = g_cursorY = 0;
            else if (g_cursorDrawY > 0xc2)
                g_cursorDrawY = g_cursorY = 0xc2;
            g_cursorDirty = 0;
            _enable();
            cursor_draw(x = g_cursorDrawX, y = g_cursorDrawY);
            if (g8b8eb
                && (x < ao8b90v.x1 || x >= ao8b90v.x2 || y < ao8b90v.y1 || y >= ao8b90v.y2)) {
                res = az8baa;
                if (i = menu_hit(x, y)) {
                    if (res != i) {
                        if (g_keyGroup == 0x25) {
                            aq8256z = 1;
                            F_11421(-1, "", 0);
                            aq8256z = 1;
                            F_11421(g_bannerX, "", cz8bc0y);
                        }
                        F_11421(-1, g_239e + at8ba8v->f4, 0);
                        bit = 1;
                    }
                } else {
                    if (bit) {
                        F_11421(-1, "", 0);
                        bit = 0;
                    }
                    if (g_keyGroup == 0x25)
                        F_11421(g_bannerX, g7f32s, cz8bc0y);
                    else if (res)
                        F_11421(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}

struct Eng e_78e0;
int t_72ec;
struct Tone *bn82c4;

/* sys_timer_add @ 0000:1843 (linear 0x01843, 327 bytes) */

int sys_timer_add(int delay, int pri)
{
    register int i, cur3;
    int prev;
    long when, key;

    g_timerBusy = 1;
    i = g_timerFree;
    if (i < 0) {
        g_timerBusy = 0;
        return -1;
    }
    cur3 = g_timerHead;
    prev = -1;
    when = g_tick + delay;
    if (delay) {
        for (;;) {
            if (cur3 >= 0) {
                if ((key = g_timers[cur3].key) > when) {
                    cur3 = g_timers[prev = cur3].nxt;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_timers[cur3].f8 >= pri)
                        break;
                    cur3 = g_timers[prev = cur3].nxt;
                    if (cur3 < 0 || g_timers[cur3].key != key)
                        break;
                }
            }
            break;
        }
    }
    g_timers[i].key = when;
    g_timers[i].f8 = pri;
    g_timerFree = g_timers[i].nxt;
    g_timers[i].nxt = cur3;
    if (cur3 < 0) {
        g_timerFirst = i;
        g_timerNext = when;
    } else
        g_timers[cur3].prv = i;
    g_timers[i].prv = prev;
    if (prev < 0)
        g_timerHead = i;
    else
        g_timers[prev].nxt = i;
    g_timerBusy = 0;
    return i;
}

/* sys_timer_anim @ 0000:198A (linear 0x0198A, 50 bytes) */

void sys_timer_anim(int fc, int delay)
{
    int n;

    n = sys_timer_add(delay, g8ef6q);
    g_timers[n].fa = 1;
    g_timers[n].fc = fc;
}

/* sys_timer_cancel @ 0000:19BC (linear 0x019BC, 73 bytes) */

void sys_timer_cancel(int n)
{
    struct Link *p;
    int i;

    for (i = g_timerFirst; i >= 0; i = g_timers[i].nxt)
        if ((p = g_timers + i)->fa == 1 && p->fc == n)
            p->fa = 0;
}

/* sys_music_start @ 0000:1A05 (linear 0x01A05, 119 bytes) */

void sys_music_start(int n, int cnt)
{
    if (g_musicOn) {
        g_voice->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_speakerOn = 0;
        g_voice->f4 = 0;
        g_voice->dirty = 1;
        g_voice->lc = g_voice->cur = g_voice->l8 + n;
        g_voice->f2 = cnt + 1;
        g_voice->f1a = g_voice->f18;
        bn82c4 = g_tones + *g_voice->f1a;
        t_72ec = 1;
        g_voice->off = 1;
    } else {
        g_voice->off = 0;
    }
}

/* sys_music_stop @ 0000:1A7C (linear 0x01A7C, 49 bytes) */

void sys_music_stop(void)
{
    g_voice->off = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_speakerOn = 0;
    g_voice->dirty = 1;
    g_voice->lc = g_voice->cur = g_voice->l8;
}

/* sys_drive_start @ 0000:1AAD (linear 0x01AAD, 122 bytes) */

void sys_drive_start(void)
{
    g_060a = 0;
    _dos_setvect(8, sys_drive_isr);
    if (!g_demo && g_mousePresent) {
        mouse_poll();
        g_mouseAccX = g_mouseAccY = 0;
        g_mouseShiftX = 1;
        g_mouseShiftY = 6;
    }
    g_lastDx = g_lastDy = g_buttons = g_click = 0;
    e_78e0.f2 = e_78e0.f4 = 0;
    g_driveDiv = 0;
    outp(0x42, 0x1e);
    outp(0x42, 0);
    if (g_soundOn) {
        outp(0x61, inp(0x61) | 3);
        g_speakerOn = 1;
    }
}

/* sys_drive_stop @ 0000:1B27 (linear 0x01B27, 106 bytes) */

void sys_drive_stop(void)
{
    _dos_setvect(8, sys_timer_isr);
    g_sirenOn = 0;
    g_squealOn = 0;
    g_onShoulder = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_speakerOn = 0;
    if (!g_demo && g_mousePresent) {
        mouse_poll();
        g_mouseAccX = g_mouseAccY = 0;
        g_mouseShiftX = 1;
        g_mouseShiftY = 1;
    }
    g_lastDx = g_lastDy = g_buttons = g_click = 0;
}

/* sys_sign @ 0000:1B91 (linear 0x01B91, 29 bytes) */

int sys_sign(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}

/* Loop optimization is off from here to the end of the file. The file is
   compiled /Oalsi, but in the shipped code the loops of sys_demo_step, sys_timer_isr and
   sys_drive_isr (the timer interrupt handler) are plain /Ol-off loops: counters and
   pointers stay in memory, the test is reached by a jmp, no induction
   variables. No for/while/do formulation reproduces that under /Ol (/Ol takes
   every such loop here); this one pragma reproduces all three functions. */
#pragma loop_opt(off)

/* sys_demo_step @ 0000:1BAE (linear 0x01BAE, 1076 bytes) */

void sys_demo_step(void)
{
    int num;
    int cx;
    int k;
    int mask;
    int step;
    int cy;

    if (g_keyGroup == 2)
        random_int(-1);
    if (g_demoKeyDown) {
        g_demoKey |= 0x80;
        g_demoKeyDown = 0;
        joy_demo_key();
        return;
    }
    if (!g_demoMoving) {
        if (g_06ac && g_keyGroup == 10) {
            g_06ac = 0;
            g_demoCount = 15;
        }
        num = (int)strlen((char *)g_keyMaps[g_keyGroup]) / 2;
        k = 0;
        switch (g_keyGroup) {
        case 14:
        case 18:
        case 19:
        case 22:
        case 36:
            num -= 2;
            break;
        case 11:
            num -= 2;
        case 37:
            k = 1;
            break;
        case 23:
            g_demoCount = 4;
            break;
        case 8:
            k = 4;
            break;
        case 28:
        case 35:
            k = 2;
            break;
        case 9:
            if (a_7eaap.m7eb0)
                break;
        case 13:
            num--;
            break;
        }
        if (g_keyGroup == 10 && g_demoLastGroup != 10) {
            g_demoChoice = num - g_demoCount++ - 1;
            g_demoCount = g_demoCount % num;
        } else
            g_demoChoice = random_int(num + k);
        switch (g_keyGroup) {
        case 21:
            if ((g_demoChoice >= 0 && g_demoChoice < 9) || g_demoChoice == 12)
                g_demoChoice = 9;
            if (g_demoLastGroup == 28)
                g_demoChoice = 11;
            break;
        case 20:
            g_demoChoice = g_demoScript[g_demoScriptPos++];
            if (g_demoChoice == 2)
                g_demoScriptPos = 0;
            break;
        case 23:
            if (g_demoChoice)
                break;
        case 2:
            g_demoChoice = 1;
            break;
        case 4:
            if (g_demoCount < 11)
                g_demoChoice = 0;
            else
                g_demoChoice = num - 1;
            break;
        case 34:
        case 41:
            g_demoChoice = 0;
            break;
        case 36:
            if (g_demoLastGroup == 36)
                g_demoChoice = 0;
            break;
        case 11:
            if (g_demoLastGroup == 11)
                g_demoChoice = 10;
            break;
        case 27:
            g_demoChoice = 5;
            break;
        case 7:
            if (a_7eaap.m7eb0 == 0)
                g_demoChoice = 0;
            break;
        }
        if (g_keyGroup == 8)
            g_demoTargetX = 160;
        if (g_demoChoice >= 0) {
            if (num > g_demoChoice) {
                g_demoKey = g_keyMaps[g_keyGroup][g_demoChoice].key;
                mask = g_keyMaps[g_keyGroup][g_demoChoice].val + 0x32;
                step = g_screenHot[g_keyGroup];
                for (; step - g_screenHot[g_keyGroup] < 32 && g_09f4[step].f6 != mask; step++)
                    ;
                if (g_09f4[step].f6 == mask) {
                    cx = (g_09f4[step].x1 + g_09f4[step].x2) / 2;
                    if (cx)
                        g_demoTargetX = cx;
                    if (g_keyGroup == 8)
                        g_demoTargetX = 160;
                    cy = (g_09f4[step].y1 + g_09f4[step].y2) / 2;
                    if (cy)
                        g_demoTargetY = cy;
                }
            } else {
                if (num == g_demoChoice)
                    g_demoKey = 0x1c;
                if (num + 1 == g_demoChoice)
                    g_demoKey = 0x39;
                if (num + 2 == g_demoChoice)
                    g_demoKey = 0x48;
                if (num + 3 == g_demoChoice)
                    g_demoKey = 0x50;
                if (num + 4 == g_demoChoice)
                    g_demoKey = 0x4b;
                if (num + 5 == g_demoChoice)
                    g_demoKey = 0x4d;
            }
        } else
            g_demoKey = 0x4c;
    }
    if ((g_demoTargetX && g_cursorX != g_demoTargetX) || (g_demoTargetY && g_cursorY != g_demoTargetY)) {
        g_demoMoving = 1;
        return;
    }
    g_demoMoving = 0;
    if (g_keyGroup == 0x25 && g_06ac < 10 && g_demoKey == 0x22)
        g_demoKey = 0x1c;
    if (g_keyGroup == 10 && g_demoLastGroup == 10) {
        g_demoKey = 0x1c;
        g_demoLastGroup = 0;
    } else
        g_demoLastGroup = g_keyGroup;
    if (a_7eaap.m7eb8 == 0 && g_keyGroup == 7)
        game_add_part(0x2a, g_4806[42].f0);
    if (a_7eaap.m7eb0 == 0) {
        if (g_keyGroup == 10)
            g_demoKey = 0x31;
        if (g_keyGroup == 8 && (g_demoKey == 1 || g_demoKey == 0x22 || g_demoKey == 0x19))
            g_demoKey = 0x39;
    }
    if (g_demoKey == 0x46)
        g_demoKey = 0x1c;
    g_demoKeyDown = 1;
    joy_demo_key();
}

/* sys_timer_isr @ 0000:1FE2 (linear 0x01FE2, 914 bytes) */

void interrupt far sys_timer_isr(void)
{
    int x, y;
    register int delta;

    g_tick++;
    if (g_tickDiv--) {
        outp(0x20, 0x20);
        _enable();
    } else {
        (*w_7aaa)();
        g_tickDiv = 3;
        g_0600++;
    }
    if (!g_soundOn && g_soundOn != g_speakerOn) {
        outp(0x61, inp(0x61) & 0xfc);
        g_speakerOn = 0;
    }
    if (g_demo) {
        if (((g_demoKeyDown ? 0xf : 0x7f) & g_tick) == 0)
            sys_demo_step();
        if ((g_demoTargetX || g_demoTargetY) && (g_cursorX != g_demoTargetX || g_cursorY != g_demoTargetY)) {
            g_cursorX += sys_sign(g_demoTargetX - g_cursorX);
            g_cursorY += sys_sign(g_demoTargetY - g_cursorY);
            g_cursorDirty = 1;
        }
    }
    if (g_voice->off && --g_voice->f2) {
        if (t_72ec && g_voice->f0.w && !--g_vibCount) {
            bn82c4->f0 = bn82c4->f0 % bn82c4->f3;
            g_vibDivisor = g_toneShapes[bn82c4->f1][bn82c4->f0++] * g_vibGain + g_voice->f0.w;
            outp(0x42, g_vibDivisor);
            outp(0x42, g_vibDivisor >> 8);
            g_vibCount = bn82c4->f4;
        }
    } else if (g_voice->off && g_voice->f2 == 0) {
        while (!(g_voice->f2 = *g_voice->cur++)) {
            if (*g_voice->cur++ == 0) {
                outp(0x61, inp(0x61) & 0xfc);
                g_speakerOn = 0;
                g_voice->off = 0;
                g_voice->dirty = 1;
                g_voice->lc = g_voice->cur = g_voice->l8;
                goto tail;
            }
            g_voice->f1a++;
            if (*g_voice->f16 < ++g_voice->f4) {
                if (*++g_voice->f16 < 0) {
                    g_voice->cur = g_voice->l8 + 2;
                    g_voice->f16 = g_voice->f14;
                    g_voice->f1a = g_voice->f18;
                }
                g_voice->lc = g_voice->cur;
                g_voice->f4 = 0;
            } else
                g_voice->cur = g_voice->lc;
            bn82c4 = g_tones + *g_voice->f1a;
        }
        if (!(g_voice->f0.w = *g_voice->cur++)) {
            g_voice->dirty = 1;
            outp(0x61, inp(0x61) & 0xfc);
            g_speakerOn = 0;
        } else {
            if (t_72ec) {
                g_vibCount = bn82c4->f4;
                g_vibGain = (unsigned)bn82c4->f2 * g_voice->f0.w / 1000;
            }
            outp(0x42, g_voice->f0.b[0]);
            outp(0x42, g_voice->f0.b[1]);
            if (g_soundOn && (g_soundOn != g_speakerOn || g_voice->dirty)) {
                outp(0x61, inp(0x61) | 3);
                g_speakerOn = 1;
                if (g_voice->dirty)
                    g_voice->dirty = 0;
            }
        }
    }
tail:
    if (g_4733 > 0) {
        _disable();
        if (!g_cursorBusy) {
            g_cursorBusy = 1;
            _enable();
            x = y = 0;
            if (g_inputLock == 0) {
                if (delta = g_keyDx)
                    x = delta > 0 ? 1 : -1;
                if (delta = g_keyDy)
                    y = delta > 0 ? 1 : -1;
            }
            if (g_demo == 0 && g_joystick) {
                joy_read();
                if ((delta = g_joyX) < g_joyMinX)
                    x = -1;
                else if (g_joyMaxX < delta)
                    x = 1;
                if ((delta = g_joyY) < g_joyMinY)
                    y = -1;
                else if (g_joyMaxY < delta)
                    y = 1;
            }
            if (x | y) {
                g_cursorX += g_474c[x];
                g_cursorY += g_4752[y];
                g_cursorDirty = 1;
            }
            g_cursorBusy = 0;
        }
    }
}

struct State7E8A ar7e8av;
struct State7D2E ao7d2ev;
int ao8228u;
long b_82bc;
int z_8b7c;

/* sys_car_halt @ 0000:2374 (linear 0x02374, 24 bytes) */

void sys_car_halt(void)
{
    e_78e0.f14 = e_78e0.f16 = e_78e0.f2 = e_78e0.f4 = g_05d6 = 0;
    e_78e0.fbe = e_78e0.fc0;
}

/* sys_drive_isr @ 0000:238C (linear 0x0238C, 2062 bytes) */

void interrupt far sys_drive_isr(void)
{
    struct Range *sp;
    char flag = 0;
    char s = 0;
    char prev = 0;
    char c = 0;
    char top = 0;
    union Word val;
    int i;
    int off;
    int n;
    unsigned r;

    g_tick++;
    if (g_tickDiv--) {
        outp(0x20, 0x20);
        _enable();
        if (g_soundOn) {
            if (g_soundOn != g_speakerOn) {
                outp(0x61, inp(0x61) | 3);
                g_speakerOn = 1;
            }
            if (g_tickDiv & 1) {
                if (g_carAnim == 0) {
                    r = (unsigned)(((long)e_78e0.fe * e_78e0.fe) >> 10);
                    r += r >> (r + 2) >> 3;
                    val.w = 50000 - ((e_78e0.fe == 5700 ? 10000 : 0) + r);
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                } else if (g_carMoving) {
                    off = ac825et - z_8b7c;
                    r = off < 0 ? -off : off;
                    r = (r >> 1) * r;
                    if (r > 20000)
                        r = 20000;
                    val.w = 60000 - r;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                }
            } else {
                g_06ea ^= 1;
                if (g_sirenOn) {
                    if (g_06e4++ < 10)
                        val.w = 0x54b;
                    else
                        val.w = 0x712;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                    if (g_06e4 > 20)
                        g_06e4 = 0;
                } else if (g_squealOn || g_onShoulder) {
                    if (g_06ea) {
                        val.w = 0x95;
                        outp(0x42, val.b[0]);
                        outp(0x42, val.b[1]);
                    }
                }
            }
        } else if (g_soundOn != g_speakerOn) {
            outp(0x61, inp(0x61) & 0xfc);
            g_speakerOn = 0;
        }
    } else {
        g_0600++;
        (*w_7aaa)();
        g_tickDiv = 3;
        if (bw8aca && !g_carAnim && --g_driveDiv <= 0) {
            g_driveDiv = 3;
            _disable();
            if (!g_driveBusy) {
                g_driveBusy = 1;
                _enable();
                flag = s = prev = c = 0;
                if (g_inputLock == 0) {
                    if (n = g_keyDx) {
                        s = 1;
                        g_cursorX = n > 0 ? 1 : -1;
                    }
                    if (n = g_keyDy) {
                        if (n > 0)
                            prev = 1;
                        else
                            flag = 1;
                    }
                    if (g_demo == 0 && g_joystick) {
                        joy_read();
                        if ((n = g_joyX) < g_joyMinX) {
                            g_cursorDirty = s = 1;
                            g_cursorX += -1;
                        } else if (g_joyMaxX < n) {
                            g_cursorDirty = s = 1;
                            g_cursorX++;
                        }
                        if ((n = g_joyY) < g_joyMinY) {
                            g_cursorDirty = flag = 1;
                            g_cursorY += -1;
                        } else if (g_joyMaxY < n) {
                            g_cursorDirty = prev = 1;
                            g_cursorY++;
                        }
                    }
                    if (g_demo == 0 && g_mousePresent) {
                        mouse_poll();
                        if (g_mouseDy > 0) {
                            g_cursorDirty = 1;
                            g_cursorY++;
                            prev = 1;
                        }
                        if (n = g_mouseDx) {
                            g_cursorDirty = s = 1;
                            g_cursorX += n > 0 ? 1 : -1;
                        }
                        if (g_mouseRight)
                            flag = 1;
                    }
                }
                c = g_buttons && !g_lastFire;
                g_lastFire = g_buttons;
                if (g_fireLatch) {
                    c = 1;
                    g_fireLatch = 0;
                }
                if (g_demo || g_autoDrive) {
                    if (e_78e0.f14 * 4 + e_78e0.fe < 5700) {
                        if (ar7e8av.m7e8a > ao7d2ev.m7d30 ||
                            (!a_7eaap.m7ec0 ? 0x17c : 0x3fc) - 60 < ao7d2ev.m7d30)
                            flag = 1;
                    } else if (e_78e0.f18 < e_78e0.f1c && ao8228u) {
                        c = 1;
                        flag = 1;
                        g_06ac++;
                    }
                    if (e_78e0.fe < 2000 && e_78e0.f18 > 1 && !flag)
                        c = 1;
                    if (e_78e0.f20 && e_78e0.f18 > 0)
                        c = 0;
                }
                if (bw8aca) {
                    b_82bc += e_78e0.f0;
                    e_78e0.fc0++;
                }
                if ((!a_7eaap.m7ec0 ? 0x17c : 0x3fc) < ao7d2ev.m7d30) {
                    prev = 1;
                    flag = s = 0;
                    if (e_78e0.f0 > 15)
                        e_78e0.f0 -= 15;
                }
                if (g_54da > 0) {
                    prev = 1;
                    flag = s = 0;
                    g_54da--;
                }
                if (e_78e0.fe > 5000) {
                    a_7eaap.m7eb0->rack.s[0]->b += !a_7eaap.m7ec0 ? 1 : 1;
                    a_7eaap.m7eb0->slot6->b += !a_7eaap.m7ec0 ? 1 : 1;
                }
                if (!(g_0600 & (!a_7eaap.m7ec0 ? 0xf : 0xf)))
                    a_7eaap.m7eb0->rack.s[0]->b++;
                if ((i = a_7eaap.m7eb0->rack.s[0]->b) >= 9900) {
                    e_78e0.fbc |= 5;
                    sys_car_halt();
                }
                if (flag) {
                    e_78e0.fe += e_78e0.f14;
                    if (e_78e0.f20 && e_78e0.fe > 5690)
                        e_78e0.fe = 5690;
                    if (e_78e0.fe > 5700) {
                        e_78e0.fe = 5700;
                        if (15 - i / 1000 < ++g_overRev) {
                            e_78e0.fbc |= 5;
                            sys_car_halt();
                        }
                    }
                } else if (e_78e0.fe > 1500 ||
                           (e_78e0.f18 > 0 && e_78e0.fe > e_78e0.f16))
                    e_78e0.fe -= e_78e0.f16;
                if (e_78e0.fe < 5700)
                    g_overRev = 0;
                top = e_78e0.f0 < 5 && !flag && e_78e0.f18 > 0;
                if (c || top ||
                    (e_78e0.f20 && e_78e0.f24 > 1 &&
                     ((flag && e_78e0.fe > e_78e0.f22 && e_78e0.f18 < e_78e0.f1c) ||
                      (!flag && e_78e0.f18 > 1 && !e_78e0.f42)))) {
                    a_7eaap.m7eb0->slot6->b += e_78e0.fe > 5000 ? 15 : 3;
                    if (g_demo == 0) {
                        i = a_7eaap.m7eb0->slot6->b;
                        if (i >= 9900 ||
                            (e_78e0.f20 == 0 && flag && e_78e0.fe > 5000 &&
                             i / 100 + 100 > random_int(3277))) {
                            e_78e0.fbc |= 0x11;
                            sys_car_halt();
                        }
                    }
                    if ((prev || (!e_78e0.f42 && !flag)) && e_78e0.f18 > 0)
                        e_78e0.f18--;
                    else if ((flag || (e_78e0.f42 > 0 && !prev)) &&
                             e_78e0.f18 < e_78e0.f1c)
                        e_78e0.f18++;
                    if (e_78e0.f20) {
                        if (c)
                            e_78e0.f24 = e_78e0.f24 == 1 ? 2 : 1;
                        if (top || e_78e0.f18 == 0)
                            e_78e0.f24 = 1;
                        if (e_78e0.f24 == 1)
                            e_78e0.f18 = 0;
                    } else if (top)
                        e_78e0.f18 = 0;
                    e_78e0.f1a = 1;
                    e_78e0.f40 = e_78e0.f34[e_78e0.f18];
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) == 0)
                        e_78e0.fe = 1505;
                }
                i = -1;
                do {
                    sp = &e_78e0.gear[e_78e0.f18].r[++i];
                } while (i < 2 && sp->f2 < e_78e0.fe);
                if (i != e_78e0.f42 || e_78e0.f1a) {
                    e_78e0.f42 = i;
                    e_78e0.f14 = sp->f4;
                    e_78e0.f16 = sp->f6;
                }
                if (prev || (g_onShoulder && e_78e0.f0 > 8)) {
                    if ((e_78e0.f0 -= (e_78e0.f0 >> 4) + 2) < 0)
                        e_78e0.f0 = 0;
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) < 1500)
                        e_78e0.fe = 1500;
                } else if (e_78e0.f18 == 0 && e_78e0.f0) {
                    if (e_78e0.f0)
                        e_78e0.f0--;
                    e_78e0.fe = 30 * e_78e0.f0 + 1500;
                } else if (e_78e0.f40)
                    e_78e0.f0 = e_78e0.fe / e_78e0.f40;
                if (s && abs(e_78e0.f2 + g_cursorX) > abs(e_78e0.f2)) {
                    if (abs(g_cursorX) + abs(e_78e0.f4) == abs(e_78e0.f4 + g_cursorX) &&
                        abs(e_78e0.f2) < abs(e_78e0.f4))
                        e_78e0.f2 = e_78e0.f4;
                    e_78e0.f2 += g_cursorX << 1;
                    if (abs(e_78e0.f2) > 24)
                        e_78e0.f2 = e_78e0.f2 > 0 ? 24 : -24;
                    g_cursorX = 0;
                    e_78e0.f4 = e_78e0.f2;
                } else
                    e_78e0.f2 = 0;
                if (abs(e_78e0.f4) > 0)
                    e_78e0.f4 += e_78e0.f4 > 0 ? -((e_78e0.f4 >> 2) + 1)
                                               : ((-e_78e0.f4 >> 2) + 1);
                if (bw8aca && road_check_crash()) {
                    e_78e0.fbc |= 0x21;
                    sys_car_halt();
                }
                g_driveBusy = 0;
            }
        }
    }
}
