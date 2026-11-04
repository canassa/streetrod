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
#include "HOTSPOT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "KBDOFF.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SONG.H"
#include "SOUND.H"
#include "VIDDET.H"
#include "VIDEO.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct CursorDraw g_drawnCursor; /* DS:6C46 */
static int g_keyGroup;                         /* DS:6C4C */
static char g_lastClick;                        /* DS:6C4E */
static int g_demoChoice;                        /* DS:6C50 */
static struct Vibrato g_vibrato; /* DS:6C52 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Dev g_songBgState;
struct Dev g_secondaryMusicStateData;
void (interrupt far *g_clockPrev)(void);
char g_strOpp[80];
int g_menuLevel;
struct Lims g_boxSpotHitData;
struct Hot *g_curHoverPointer;
int g_cursHoverCodeVal;
int g_wRivBanner;
struct TimerPriorities g_sysTimerPriorities; /* DS:8EF6 */

struct TimerQueue g_timerQueue = {-1, -1, 0, 2147483647L, 0, 0}; /* DS:0566 */
struct Link g_timers[4] = {{0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}};   /* DS:0572 */
int g_05ba = 0;   /* DS:05BA */
int g_05bc[8] = {0, 0, 0, 0, 0, 0, 0, 0};   /* DS:05BC */
long g_waitUntil = 0L;   /* DS:05CC */
struct InputState g_input = {0, 0}; /* DS:05D0 */
int g_05d4 = 32767; /* DS:05D4 */
int g_05d6 = 0; /* DS:05D6 */
int g_05d8 = 0; /* DS:05D8 */
int g_05da = 0; /* DS:05DA */
int g_msgTicks = 350;   /* DS:05DC */
int g_clickEnds = 0;   /* DS:05DE */
int g_kbdHooked = 0;   /* DS:05E0 */
struct Mouse g_mouse = {0, 0, 0, 0, 0, 0, 0, 10, 3};   /* DS:05E2 */
int g_05f4[2] = {0, 0};   /* DS:05F4 */
long g_tick = 0L;   /* DS:05F8 */
struct GameClock g_gameClock = {0L, 0L, 0L}; /* DS:05FC */
char g_tickDiv = 0;   /* DS:0608 */
char g_driveDiv = 0;   /* DS:0609 */
int g_060a = 0;   /* DS:060A */
char *g_errRead = "cannot read drive ?";   /* DS:060C */
char *g_errWriteProt = "disk ? is write protected";   /* DS:060E */
int g_demo = 0;   /* DS:0610 */
int g_autoDrive = 0;   /* DS:0612 */

/* sy_hover_reset @ 0000:0D78 (linear 0x00D78, 33 bytes) */

void sy_hover_reset(void)
{
    g_drawnCursor.dirty = 1;
    g_boxSpotHitData.x2 = -1;
    g_cursHoverCodeVal = 0x7d00;
    g_click = 0;
    g_click2 = 0;
    kg_key();
}

/* sy_music_pause @ 0000:0D99 (linear 0x00D99, 63 bytes) */

void sy_music_pause(void)
{
    if (g_voice->active) {
        g_voice->active = 0;
        outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
        g_speakerOn = 0;
        g_voice->dirty = 1;
        g_hots[ga_hotspot(-40)].textOffset = g_offTunes;
        sy_hover_reset();
    }
}

/* sy_music_resume @ 0000:0DD8 (linear 0x00DD8, 76 bytes) */

void sy_music_resume(void)
{
    if (g_musicOn && !g_voice->active && g_voice->phraseStart != g_voice->song) {
        g_voice->active = 1;
        g_voice->dirty = 1;
        g_hots[ga_hotspot(-40)].textOffset = g_offSquelch;
        sy_hover_reset();
    }
}

/* sy_reset_queues @ 0000:0E24 (linear 0x00E24, 231 bytes) */

void sy_reset_queues(void)
{
    int i;
    long deadline;

    g_timerQueue.earliest = g_timerQueue.head = -1;
    g_timerQueue.free = 0;
    g_timerQueue.nextDeadline = 0x7fffffffL;
    g_timerQueue.unused = g_timerQueue.busy = 0;
    for (i = 0; i < 4; i++)
        g_timers[i].nxt = i + 1;
    g_05ba = -1;
    deadline = g_tick + 10;
    while (deadline > g_tick) {
        for (i = 0; kg_key(); i++)
            ;
        if (i)
            deadline = g_tick + 10;
    }
    g_waitUntil = 0;
    if (g_demo == 0 && g_mouse.present) {
        mo_poll();
        g_mouse.accX = g_mouse.accY = 0;
    }
    g_click = 0;
    for (i = 0; i < 8; i++)
        g_dirHeld[i] = 0;
    g_buttons = g_keyDx = g_keyDy = g_lastDx = g_lastDy = 0;
}

/* sy_timer_remove @ 0000:0F0B (linear 0x00F0B, 172 bytes) */

void sy_timer_remove(int n)
{
    register int i;

    i = g_timers[n].nxt;
    if (i >= 0)
        g_timers[i].prv = g_timers[n].prv;
    else {
        i = g_timers[n].prv;
        g_timerQueue.earliest = i;
        if (i < 0)
            g_timerQueue.nextDeadline = 0x7fffffffL;
        else
            g_timerQueue.nextDeadline = g_timers[i].deadline;
    }
    i = g_timers[n].prv;
    if (i >= 0)
        g_timers[i].nxt = g_timers[n].nxt;
    else
        g_timerQueue.head = g_timers[n].nxt;
    g_timers[n].nxt = g_timerQueue.free;
    g_timerQueue.free = n;
}

/* sy_unhook @ 0000:0FB7 (linear 0x00FB7, 51 bytes) */

void sy_unhook(void)
{
    _disable();
    outp(PIT_CMD, PIT_CH0_LOHI_MODE3);
    outp(PIT_CH0, 0);
    outp(PIT_CH0, 0);
    _dos_setvect(TIMER_INT, g_clockPrev);
    _enable();
    ko_unhook();
    g_kbdHooked = 0;
    g_sysHooked = 0;
}

/* sy_exit @ 0000:0FEA (linear 0x00FEA, 113 bytes) */

void sy_exit(void)
{
    if (g_demo) {
        g_demo = (g_demo == 0x62) ? 0 : 1;
        sc_wait_msg("Quit demo");
        sc_fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    }
    if (g_sysHooked)
        sy_unhook();
    if (g_videoSet) {
        (*g_videoOps.show)();
        vd_restore();
    }
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    exit(5);
}

char g_064d = 0;   /* DS:064D */
char g_keymapGroup = 0;   /* DS:064E */
struct KeyMap g_keymapSaved[30] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};   /* DS:0650 */

/* sy_crit_error @ 0000:105B (linear 0x0105B, 182 bytes) */

void sy_crit_error(unsigned deverr, unsigned char errcode)
{
    int disk;
    int action;

    disk = (deverr & CRIT_DEVICE_ERROR) == 0;
    if (disk) {
        if (deverr & CRIT_RETRY_ALLOWED)
            action = _HARDERR_RETRY;
        else
            action = _HARDERR_ABORT;
    } else
        action = _HARDERR_RETRY;
    switch (errcode) {
    case CRIT_WRITE_PROTECT:
        g_errWriteProt[5] = deverr + 'A';
        sc_wait_msg(g_errWriteProt);
        break;
    case CRIT_NOT_READY:
        g_errRead[strlen(g_errRead) - 1] = deverr + 'A';
        sc_wait_msg(g_errRead);
        break;
    case CRIT_PAPER_OUT:
        sc_wait_msg("printer out of paper");
        break;
    default:
        if (disk)
            sc_wait_msg("disk error");
        else {
            sc_wait_msg("hardware error");
            action = _HARDERR_ABORT;
        }
    }
    if (action == _HARDERR_ABORT)
        sy_exit();
    _hardresume(action);
}

/* sy_init @ 0000:1111 (linear 0x01111, 461 bytes) */

void sy_init(void)
{
    int sumx;
    int sumy;
    int first;
    int v;
    int j;

    sumx = 0;
    sumy = 0;
    g_sysTimerPriorities.animation = 10;
    g_sysTimerPriorities.unused2 = 2;
    g_sysTimerPriorities.unused4 = 5;
    g_sysTimerPriorities.unused6 = 5;
    outp(GAME_PORT, 0);
    outp(GAME_PORT, 0);
    first = inp(GAME_PORT);
    for (j = 1; j; j++)
        if (((v = inp(GAME_PORT)) & (GAME_BUTTON_MASK | GAME_JOY1_AXES)) == GAME_BUTTON_MASK)
            break;
    if (g_demo == 0 && j != 0 && v != first) {
        g_joystick = 1;
        jy_read();
        sumx = g_joyX;
        sumy = g_joyY;
        jy_read();
        sumx += g_joyX;
        sumy += g_joyY;
        jy_read();
        sumx += g_joyX;
        sumy += g_joyY;
        g_joyCenterX = sumx / 3;
        g_joyCenterY = sumy / 3;
        g_joyMinX = -(g_joyCenterX >> 1);
        g_joyMaxX = g_joyCenterX >> 1;
        g_joyMinY = -(g_joyCenterY >> 1);
        g_joyMaxY = g_joyCenterY >> 1;
    }
    if (g_demo == 0 && g_useMouse && mo_reset() == -1) {
        g_mouse.present = 1;
        g_mouse.accX = g_mouse.accY = 0;
        g_mouse.shiftX = 1;
        g_mouse.shiftY = 1;
    } else
        g_mouse.present = 0;
    g_drawnCursor.x = -1;
    g_drawnCursor.y = -1;
    g_songBgState.active = 0;
    g_songBgState.song = g_song;
    g_songBgState.cur = g_songBgState.phraseStart = g_songBgState.song;
    g_songBgState.repeatCursor = g_songBgState.repeatTable = g_songRepeats;
    g_songBgState.toneCursor = g_songBgState.toneTable = g_songTones;
    g_secondaryMusicStateData.active = 0;
    g_secondaryMusicStateData.toneCursor = g_secondaryMusicStateData.toneTable = g_jingleTones;
    _harderr(sy_crit_error);
    _disable();
    outp(PIT_CMD, PIT_CH0_LOHI_MODE3);
    outp(PIT_CH0, 0);
    outp(PIT_CH0, 0x40);
    g_clockPrev = _dos_getvect(TIMER_INT);
    _dos_setvect(TIMER_INT, sy_timer_isr);
    _enable();
    kg_hook();
    g_kbdHooked = 1;
    outp(PIT_CMD, PIT_CH2_LOHI_MODE3);
    sy_reset_queues();
    g_sysHooked = 1;
}

/* sy_keymap_set @ 0000:12DE (linear 0x012DE, 179 bytes) */

void sy_keymap_set(int n)
{
    struct KeyMap *p;
    int num;
    struct KeyMap *q;
    char c;

    num = 1;
    if (g_demo)
        g_demoKey = 0;
    g_keyGroup = n;
    sy_keymap_restore(0);
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

/* sy_keymap_restore @ 0000:1391 (linear 0x01391, 134 bytes) */

int sy_keymap_restore(int keep)
{
    int i;
    struct KeyMap *p;

    for (i = 0; i < 8; i++)
        g_dirHeld[i] = 0;
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
struct DemoControl g_demoControl = {0, 0, 0, 0, 0, 0}; /* DS:06A4 */
int g_demoTargetX = 0;   /* DS:06AE */
int g_demoTargetY = 0;   /* DS:06B0 */
int g_demoScriptPos = 0;   /* DS:06B2 */
int g_demoScript[22] = {-1, 0, -1, 0, 1, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 0, 0, 2};   /* DS:06B4 */
char g_cursorBusy = 0;   /* DS:06E0 */
int g_vibCount = 1;   /* DS:06E2 */
int g_sirenPhase = 0;   /* DS:06E4 */
int g_overRev = 0;   /* DS:06E6 */
char g_driveBusy = 0;   /* DS:06E8 */
char g_lastFire = 0;   /* DS:06E9 */
char g_squealPhase = 0;   /* DS:06EA */

/* sy_wait_input @ 0000:1417 (linear 0x01417, 1068 bytes) */

int sy_wait_input(int ticks)
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
        g_statusLine.shown = 1;
        sc_status_text(-1, "", 0);
        g_statusLine.shown = 1;
        sc_status_text(g_bannerX, "", g_wRivBanner);
        g_strOpp[0] = 0;
    }
    flag = g_waitUntil != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_demo == 0x63 && g_demoControl.quit != 0x63) {
            g_demoControl.quit = 0x63;
            sy_exit();
        }
        if (g_protectCycle && (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY)
            && !(g_tick & 0xf) && g_musicOn && g_speakerOn) {
            old = g_protectPal[8];
            g_protectPal[8] = g_protectPal[7];
            g_protectPal[7] = g_protectPal[6];
            g_protectPal[6] = old;
            vi_palette_reg(6, old);
            vi_palette_reg(7, g_protectPal[7]);
            vi_palette_reg(8, g_protectPal[8]);
        }
        res = kg_key();
        if (res > 0) {
            if (g_input.any) {
                g_input.key = res;
                return 0x3eb;
            }
            if (res == 0x7f)
                sy_exit();
        } else if (res < 0) {
            res += 0x32;
            if (res == -0x28) {
                if (g_voiceNum == 1) {
                    if (g_musicOn) {
                        sy_music_pause();
                        g_musicOn = 0;
                    } else {
                        g_musicOn = 1;
                        sy_music_resume();
                    }
                }
            } else
                return res;
        }
        if (g_timerQueue.nextDeadline <= g_tick) {
            p = &g_timers[i = g_timerQueue.earliest];
            switch (p->action) {
            case 0:
            default:
                sy_timer_remove(i);
                break;
            case 1:
                res = p->argument;
                sy_timer_remove(i);
                vi_anim_resume(res);
                break;
            }
        }
        if (g_demo == 0 && g_mouse.present) {
            mo_poll();
            if (g_drawnCursor.dirty |= (g_mouse.dx != 0 || g_mouse.dy != 0)) {
                g_cursorX += g_mouse.dx;
                g_cursorY += g_mouse.dy;
            }
        }
        if (g_click | g_click2) {
            _disable();
            g_lastClick = g_click;
            x = g_clickX;
            y = g_clickY;
            g_click2 = g_click = 0;
            _enable();
            if (g_clickEnds) {
                g_input.key = 0;
                return 0x3eb;
            }
            if (g_menuLevel) {
                if (i = (x >= g_boxSpotHitData.x1 && x < g_boxSpotHitData.x2 && y >= g_boxSpotHitData.y1 && y < g_boxSpotHitData.y2)
                        ? g_cursHoverCodeVal : mn_hit(x, y))
                    return i;
            }
            if (g_input.any) {
                g_input.key = 0;
                return 0x3eb;
            }
        } else if (flag && g_waitUntil < g_tick) {
            g_waitUntil = 0L;
            return 0x3e8;
        }
        if (g_cVis > 0 && g_drawnCursor.dirty) {
            _disable();
            if ((g_drawnCursor.x = g_cursorX) < 0)
                g_drawnCursor.x = g_cursorX = 0;
            else if (g_drawnCursor.x > 0x13a)
                g_drawnCursor.x = g_cursorX = 0x13a;
            if ((g_drawnCursor.y = g_cursorY) < 0)
                g_drawnCursor.y = g_cursorY = 0;
            else if (g_drawnCursor.y > 0xc2)
                g_drawnCursor.y = g_cursorY = 0xc2;
            g_drawnCursor.dirty = 0;
            _enable();
            cu_draw(x = g_drawnCursor.x, y = g_drawnCursor.y);
            if (g_menuLevel
                && (x < g_boxSpotHitData.x1 || x >= g_boxSpotHitData.x2 || y < g_boxSpotHitData.y1 || y >= g_boxSpotHitData.y2)) {
                res = g_cursHoverCodeVal;
                if (i = mn_hit(x, y)) {
                    if (res != i) {
                        if (g_keyGroup == 0x25) {
                            g_statusLine.shown = 1;
                            sc_status_text(-1, "", 0);
                            g_statusLine.shown = 1;
                            sc_status_text(g_bannerX, "", g_wRivBanner);
                        }
                        sc_status_text(-1, g_txt + g_curHoverPointer->textOffset, 0);
                        bit = 1;
                    }
                } else {
                    if (bit) {
                        sc_status_text(-1, "", 0);
                        bit = 0;
                    }
                    if (g_keyGroup == 0x25)
                        sc_status_text(g_bannerX, g_strOpp, g_wRivBanner);
                    else if (res)
                        sc_status_text(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}

struct Eng g_driveEng;
int g_vibratoActive;
struct Tone *g_toneShapePlaybackPointer;

/* sy_timer_add @ 0000:1843 (linear 0x01843, 327 bytes) */

int sy_timer_add(int delay, int pri)
{
    register int i, cur3;
    int prev;
    long when, key;

    g_timerQueue.busy = 1;
    i = g_timerQueue.free;
    if (i < 0) {
        g_timerQueue.busy = 0;
        return -1;
    }
    cur3 = g_timerQueue.head;
    prev = -1;
    when = g_tick + delay;
    if (delay) {
        for (;;) {
            if (cur3 >= 0) {
                if ((key = g_timers[cur3].deadline) > when) {
                    cur3 = g_timers[prev = cur3].nxt;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_timers[cur3].priority >= pri)
                        break;
                    cur3 = g_timers[prev = cur3].nxt;
                    if (cur3 < 0 || g_timers[cur3].deadline != key)
                        break;
                }
            }
            break;
        }
    }
    g_timers[i].deadline = when;
    g_timers[i].priority = pri;
    g_timerQueue.free = g_timers[i].nxt;
    g_timers[i].nxt = cur3;
    if (cur3 < 0) {
        g_timerQueue.earliest = i;
        g_timerQueue.nextDeadline = when;
    } else
        g_timers[cur3].prv = i;
    g_timers[i].prv = prev;
    if (prev < 0)
        g_timerQueue.head = i;
    else
        g_timers[prev].nxt = i;
    g_timerQueue.busy = 0;
    return i;
}

/* sy_timer_anim @ 0000:198A (linear 0x0198A, 50 bytes) */

void sy_timer_anim(int fc, int delay)
{
    int n;

    n = sy_timer_add(delay, g_sysTimerPriorities.animation);
    g_timers[n].action = 1;
    g_timers[n].argument = fc;
}

/* sy_timer_cancel @ 0000:19BC (linear 0x019BC, 73 bytes) */

void sy_timer_cancel(int n)
{
    struct Link *p;
    int i;

    for (i = g_timerQueue.earliest; i >= 0; i = g_timers[i].nxt)
        if ((p = g_timers + i)->action == 1 && p->argument == n)
            p->action = 0;
}

/* sy_music_start @ 0000:1A05 (linear 0x01A05, 119 bytes) */

void sy_music_start(int n, int cnt)
{
    if (g_musicOn) {
        g_voice->active = 0;
        outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
        g_speakerOn = 0;
        g_voice->repeatCount = 0;
        g_voice->dirty = 1;
        g_voice->phraseStart = g_voice->cur = g_voice->song + n;
        g_voice->ticksLeft = cnt + 1;
        g_voice->toneCursor = g_voice->toneTable;
        g_toneShapePlaybackPointer = g_tones + *g_voice->toneCursor;
        g_vibratoActive = 1;
        g_voice->active = 1;
    } else {
        g_voice->active = 0;
    }
}

/* sy_music_stop @ 0000:1A7C (linear 0x01A7C, 49 bytes) */

void sy_music_stop(void)
{
    g_voice->active = 0;
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    g_voice->dirty = 1;
    g_voice->phraseStart = g_voice->cur = g_voice->song;
}

/* sy_drive_start @ 0000:1AAD (linear 0x01AAD, 122 bytes) */

void sy_drive_start(void)
{
    g_060a = 0;
    _dos_setvect(TIMER_INT, sy_drive_isr);
    if (!g_demo && g_mouse.present) {
        mo_poll();
        g_mouse.accX = g_mouse.accY = 0;
        g_mouse.shiftX = 1;
        g_mouse.shiftY = 6;
    }
    g_lastDx = g_lastDy = g_buttons = g_click = 0;
    g_driveEng.steeringInput = g_driveEng.steering = 0;
    g_driveDiv = 0;
    outp(PIT_CH2, 0x1e);
    outp(PIT_CH2, 0);
    if (g_soundOn) {
        outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
        g_speakerOn = 1;
    }
}

/* sy_drive_stop @ 0000:1B27 (linear 0x01B27, 106 bytes) */

void sy_drive_stop(void)
{
    _dos_setvect(TIMER_INT, sy_timer_isr);
    g_sirenOn = 0;
    g_squealOn = 0;
    g_onShoulder = 0;
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    if (!g_demo && g_mouse.present) {
        mo_poll();
        g_mouse.accX = g_mouse.accY = 0;
        g_mouse.shiftX = 1;
        g_mouse.shiftY = 1;
    }
    g_lastDx = g_lastDy = g_buttons = g_click = 0;
}

/* sy_sign @ 0000:1B91 (linear 0x01B91, 29 bytes) */

int sy_sign(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}

/* Loop optimization is off from here to the end of the file. The file is
   compiled /Oalsi, but in the shipped code the loops of sy_demo_step, sy_timer_isr and
   sy_drive_isr (the timer interrupt handler) are plain /Ol-off loops: counters and
   pointers stay in memory, the test is reached by a jmp, no induction
   variables. No for/while/do formulation reproduces that under /Ol (/Ol takes
   every such loop here); this one pragma reproduces all three functions. */
#pragma loop_opt(off)

/* sy_demo_step @ 0000:1BAE (linear 0x01BAE, 1076 bytes) */

void sy_demo_step(void)
{
    int num;
    int cx;
    int k;
    int mask;
    int step;
    int cy;

    if (g_keyGroup == 2)
        rn_int(-1);
    if (g_demoControl.keyDown) {
        g_demoKey |= 0x80;
        g_demoControl.keyDown = 0;
        jy_demo_key();
        return;
    }
    if (!g_demoControl.moving) {
        if (g_demoControl.autoShifts && g_keyGroup == 10) {
            g_demoControl.autoShifts = 0;
            g_demoControl.count = 15;
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
            g_demoControl.count = 4;
            break;
        case 8:
            k = 4;
            break;
        case 28:
        case 35:
            k = 2;
            break;
        case 9:
            if (a_7eaap.currentCar)
                break;
        case 13:
            num--;
            break;
        }
        if (g_keyGroup == 10 && g_demoControl.lastGroup != 10) {
            g_demoChoice = num - g_demoControl.count++ - 1;
            g_demoControl.count = g_demoControl.count % num;
        } else
            g_demoChoice = rn_int(num + k);
        switch (g_keyGroup) {
        case 21:
            if ((g_demoChoice >= 0 && g_demoChoice < 9) || g_demoChoice == 12)
                g_demoChoice = 9;
            if (g_demoControl.lastGroup == 28)
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
            if (g_demoControl.count < 11)
                g_demoChoice = 0;
            else
                g_demoChoice = num - 1;
            break;
        case 34:
        case 41:
            g_demoChoice = 0;
            break;
        case 36:
            if (g_demoControl.lastGroup == 36)
                g_demoChoice = 0;
            break;
        case 11:
            if (g_demoControl.lastGroup == 11)
                g_demoChoice = 10;
            break;
        case 27:
            g_demoChoice = 5;
            break;
        case 7:
            if (a_7eaap.currentCar == 0)
                g_demoChoice = 0;
            break;
        }
        if (g_keyGroup == 8)
            g_demoTargetX = 160;
        if (g_demoChoice >= 0) {
            if (num > g_demoChoice) {
                g_demoKey = g_keyMaps[g_keyGroup][g_demoChoice].key;
                mask = g_keyMaps[g_keyGroup][g_demoChoice].val + 0x32;
                step = g_groupHot[g_keyGroup];
                for (; step - g_groupHot[g_keyGroup] < 32 && g_hots[step].f6 != mask; step++)
                    ;
                if (g_hots[step].f6 == mask) {
                    cx = (g_hots[step].x1 + g_hots[step].x2) / 2;
                    if (cx)
                        g_demoTargetX = cx;
                    if (g_keyGroup == 8)
                        g_demoTargetX = 160;
                    cy = (g_hots[step].y1 + g_hots[step].y2) / 2;
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
        g_demoControl.moving = 1;
        return;
    }
    g_demoControl.moving = 0;
    if (g_keyGroup == 0x25 && g_demoControl.autoShifts < 10 && g_demoKey == 0x22)
        g_demoKey = 0x1c;
    if (g_keyGroup == 10 && g_demoControl.lastGroup == 10) {
        g_demoKey = 0x1c;
        g_demoControl.lastGroup = 0;
    } else
        g_demoControl.lastGroup = g_keyGroup;
    if (a_7eaap.spareParts == 0 && g_keyGroup == 7)
        gm_add_part(0x2a, g_parts[42].value);
    if (a_7eaap.currentCar == 0) {
        if (g_keyGroup == 10)
            g_demoKey = 0x31;
        if (g_keyGroup == 8 && (g_demoKey == 1 || g_demoKey == 0x22 || g_demoKey == 0x19))
            g_demoKey = 0x39;
    }
    if (g_demoKey == 0x46)
        g_demoKey = 0x1c;
    g_demoControl.keyDown = 1;
    jy_demo_key();
}

/* sy_timer_isr @ 0000:1FE2 (linear 0x01FE2, 914 bytes) */

void interrupt far sy_timer_isr(void)
{
    int x, y;
    register int delta;

    g_tick++;
    if (g_tickDiv--) {
        outp(PIC_CMD, PIC_EOI);
        _enable();
    } else {
        (*g_clockPrev)();
        g_tickDiv = 3;
        g_gameClock.biosTicks++;
    }
    if (!g_soundOn && g_soundOn != g_speakerOn) {
        outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
        g_speakerOn = 0;
    }
    if (g_demo) {
        if (((g_demoControl.keyDown ? 0xf : 0x7f) & g_tick) == 0)
            sy_demo_step();
        if ((g_demoTargetX || g_demoTargetY) && (g_cursorX != g_demoTargetX || g_cursorY != g_demoTargetY)) {
            g_cursorX += sy_sign(g_demoTargetX - g_cursorX);
            g_cursorY += sy_sign(g_demoTargetY - g_cursorY);
            g_drawnCursor.dirty = 1;
        }
    }
    if (g_voice->active && --g_voice->ticksLeft) {
        if (g_vibratoActive && g_voice->divisor.w && !--g_vibCount) {
            g_toneShapePlaybackPointer->phase = g_toneShapePlaybackPointer->phase % g_toneShapePlaybackPointer->length;
            g_vibrato.divisor = g_toneShapes[g_toneShapePlaybackPointer->waveform][g_toneShapePlaybackPointer->phase++] * g_vibrato.gain + g_voice->divisor.w;
            outp(PIT_CH2, g_vibrato.divisor);
            outp(PIT_CH2, g_vibrato.divisor >> 8);
            g_vibCount = g_toneShapePlaybackPointer->delay;
        }
    } else if (g_voice->active && g_voice->ticksLeft == 0) {
        while (!(g_voice->ticksLeft = *g_voice->cur++)) {
            if (*g_voice->cur++ == 0) {
                outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
                g_speakerOn = 0;
                g_voice->active = 0;
                g_voice->dirty = 1;
                g_voice->phraseStart = g_voice->cur = g_voice->song;
                goto tail;
            }
            g_voice->toneCursor++;
            if (*g_voice->repeatCursor < ++g_voice->repeatCount) {
                if (*++g_voice->repeatCursor < 0) {
                    g_voice->cur = g_voice->song + 2;
                    g_voice->repeatCursor = g_voice->repeatTable;
                    g_voice->toneCursor = g_voice->toneTable;
                }
                g_voice->phraseStart = g_voice->cur;
                g_voice->repeatCount = 0;
            } else
                g_voice->cur = g_voice->phraseStart;
            g_toneShapePlaybackPointer = g_tones + *g_voice->toneCursor;
        }
        if (!(g_voice->divisor.w = *g_voice->cur++)) {
            g_voice->dirty = 1;
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
            g_speakerOn = 0;
        } else {
            if (g_vibratoActive) {
                g_vibCount = g_toneShapePlaybackPointer->delay;
                g_vibrato.gain = (unsigned)g_toneShapePlaybackPointer->depth * g_voice->divisor.w / 1000;
            }
            outp(PIT_CH2, g_voice->divisor.b[0]);
            outp(PIT_CH2, g_voice->divisor.b[1]);
            if (g_soundOn && (g_soundOn != g_speakerOn || g_voice->dirty)) {
                outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
                g_speakerOn = 1;
                if (g_voice->dirty)
                    g_voice->dirty = 0;
            }
        }
    }
tail:
    if (g_cVis > 0) {
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
                jy_read();
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
                g_cursorX += g_cursorStepX[x];
                g_cursorY += g_cursorStepY[y];
                g_drawnCursor.dirty = 1;
            }
            g_cursorBusy = 0;
        }
    }
}

struct State7E8A ar7e8av;
struct State7D2E ao7d2ev;
int ao8228u;
long g_mphAccumulator;
int g_xCoordSprite;

/* sy_stop_drive @ 0000:2374 (linear 0x02374, 24 bytes) */

void sy_stop_drive(void)
{
    g_driveEng.accelStep = g_driveEng.decelStep = g_driveEng.steeringInput = g_driveEng.steering = g_05d6 = 0;
    g_driveEng.fbe = g_driveEng.elapsedTicks;
}

/* sy_drive_isr @ 0000:238C (linear 0x0238C, 2062 bytes) */

void interrupt far sy_drive_isr(void)
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
        outp(PIC_CMD, PIC_EOI);
        _enable();
        if (g_soundOn) {
            if (g_soundOn != g_speakerOn) {
                outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
                g_speakerOn = 1;
            }
            if (g_tickDiv & 1) {
                if (g_carAnim == 0) {
                    r = (unsigned)(((long)g_driveEng.rpm * g_driveEng.rpm) >> 10);
                    r += r >> (r + 2) >> 3;
                    val.w = 50000 - ((g_driveEng.rpm == 5700 ? 10000 : 0) + r);
                    outp(PIT_CH2, val.b[0]);
                    outp(PIT_CH2, val.b[1]);
                } else if (g_carMoving) {
                    off = ac825et - g_xCoordSprite;
                    r = off < 0 ? -off : off;
                    r = (r >> 1) * r;
                    if (r > 20000)
                        r = 20000;
                    val.w = 60000 - r;
                    outp(PIT_CH2, val.b[0]);
                    outp(PIT_CH2, val.b[1]);
                }
            } else {
                g_squealPhase ^= 1;
                if (g_sirenOn) {
                    if (g_sirenPhase++ < 10)
                        val.w = 0x54b;
                    else
                        val.w = 0x712;
                    outp(PIT_CH2, val.b[0]);
                    outp(PIT_CH2, val.b[1]);
                    if (g_sirenPhase > 20)
                        g_sirenPhase = 0;
                } else if (g_squealOn || g_onShoulder) {
                    if (g_squealPhase) {
                        val.w = 0x95;
                        outp(PIT_CH2, val.b[0]);
                        outp(PIT_CH2, val.b[1]);
                    }
                }
            }
        } else if (g_soundOn != g_speakerOn) {
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
            g_speakerOn = 0;
        }
    } else {
        g_gameClock.biosTicks++;
        (*g_clockPrev)();
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
                        jy_read();
                        if ((n = g_joyX) < g_joyMinX) {
                            g_drawnCursor.dirty = s = 1;
                            g_cursorX += -1;
                        } else if (g_joyMaxX < n) {
                            g_drawnCursor.dirty = s = 1;
                            g_cursorX++;
                        }
                        if ((n = g_joyY) < g_joyMinY) {
                            g_drawnCursor.dirty = flag = 1;
                            g_cursorY += -1;
                        } else if (g_joyMaxY < n) {
                            g_drawnCursor.dirty = prev = 1;
                            g_cursorY++;
                        }
                    }
                    if (g_demo == 0 && g_mouse.present) {
                        mo_poll();
                        if (g_mouse.dy > 0) {
                            g_drawnCursor.dirty = 1;
                            g_cursorY++;
                            prev = 1;
                        }
                        if (n = g_mouse.dx) {
                            g_drawnCursor.dirty = s = 1;
                            g_cursorX += n > 0 ? 1 : -1;
                        }
                        if (g_mouse.right)
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
                    if (g_driveEng.accelStep * 4 + g_driveEng.rpm < 5700) {
                        if (ar7e8av.roadIndex > ao7d2ev.roadIndex ||
                            (!a_7eaap.raceType ? 0x17c : 0x3fc) - 60 < ao7d2ev.roadIndex)
                            flag = 1;
                    } else if (g_driveEng.gearIndex < g_driveEng.gearCount && ao8228u) {
                        c = 1;
                        flag = 1;
                        g_demoControl.autoShifts++;
                    }
                    if (g_driveEng.rpm < 2000 && g_driveEng.gearIndex > 1 && !flag)
                        c = 1;
                    if (g_driveEng.automatic && g_driveEng.gearIndex > 0)
                        c = 0;
                }
                if (bw8aca) {
                    g_mphAccumulator += g_driveEng.speed;
                    g_driveEng.elapsedTicks++;
                }
                if ((!a_7eaap.raceType ? 0x17c : 0x3fc) < ao7d2ev.roadIndex) {
                    prev = 1;
                    flag = s = 0;
                    if (g_driveEng.speed > 15)
                        g_driveEng.speed -= 15;
                }
                if (g_brakeTicks > 0) {
                    prev = 1;
                    flag = s = 0;
                    g_brakeTicks--;
                }
                if (g_driveEng.rpm > 5000) {
                    a_7eaap.currentCar->rack.parts[0]->wear += !a_7eaap.raceType ? 1 : 1;
                    a_7eaap.currentCar->transmission->wear += !a_7eaap.raceType ? 1 : 1;
                }
                if (!(g_gameClock.biosTicks & (!a_7eaap.raceType ? 0xf : 0xf)))
                    a_7eaap.currentCar->rack.parts[0]->wear++;
                if ((i = a_7eaap.currentCar->rack.parts[0]->wear) >= 9900) {
                    g_driveEng.stopFlags |= 5;
                    sy_stop_drive();
                }
                if (flag) {
                    g_driveEng.rpm += g_driveEng.accelStep;
                    if (g_driveEng.automatic && g_driveEng.rpm > 5690)
                        g_driveEng.rpm = 5690;
                    if (g_driveEng.rpm > 5700) {
                        g_driveEng.rpm = 5700;
                        if (15 - i / 1000 < ++g_overRev) {
                            g_driveEng.stopFlags |= 5;
                            sy_stop_drive();
                        }
                    }
                } else if (g_driveEng.rpm > 1500 ||
                           (g_driveEng.gearIndex > 0 && g_driveEng.rpm > g_driveEng.decelStep))
                    g_driveEng.rpm -= g_driveEng.decelStep;
                if (g_driveEng.rpm < 5700)
                    g_overRev = 0;
                top = g_driveEng.speed < 5 && !flag && g_driveEng.gearIndex > 0;
                if (c || top ||
                    (g_driveEng.automatic && g_driveEng.autoSelector > 1 &&
                     ((flag && g_driveEng.rpm > g_driveEng.autoShiftRpm && g_driveEng.gearIndex < g_driveEng.gearCount) ||
                      (!flag && g_driveEng.gearIndex > 1 && !g_driveEng.rpmBand)))) {
                    a_7eaap.currentCar->transmission->wear += g_driveEng.rpm > 5000 ? 15 : 3;
                    if (g_demo == 0) {
                        i = a_7eaap.currentCar->transmission->wear;
                        if (i >= 9900 ||
                            (g_driveEng.automatic == 0 && flag && g_driveEng.rpm > 5000 &&
                             i / 100 + 100 > rn_int(3277))) {
                            g_driveEng.stopFlags |= 0x11;
                            sy_stop_drive();
                        }
                    }
                    if ((prev || (!g_driveEng.rpmBand && !flag)) && g_driveEng.gearIndex > 0)
                        g_driveEng.gearIndex--;
                    else if ((flag || (g_driveEng.rpmBand > 0 && !prev)) &&
                             g_driveEng.gearIndex < g_driveEng.gearCount)
                        g_driveEng.gearIndex++;
                    if (g_driveEng.automatic) {
                        if (c)
                            g_driveEng.autoSelector = g_driveEng.autoSelector == 1 ? 2 : 1;
                        if (top || g_driveEng.gearIndex == 0)
                            g_driveEng.autoSelector = 1;
                        if (g_driveEng.autoSelector == 1)
                            g_driveEng.gearIndex = 0;
                    } else if (top)
                        g_driveEng.gearIndex = 0;
                    g_driveEng.shifterDirty = 1;
                    g_driveEng.rpmPerSpeed = g_driveEng.f34[g_driveEng.gearIndex];
                    if ((g_driveEng.rpm = g_driveEng.speed * g_driveEng.rpmPerSpeed) == 0)
                        g_driveEng.rpm = 1505;
                }
                i = -1;
                do {
                    sp = &g_driveEng.gear[g_driveEng.gearIndex].bands[++i];
                } while (i < 2 && sp->maxRpm < g_driveEng.rpm);
                if (i != g_driveEng.rpmBand || g_driveEng.shifterDirty) {
                    g_driveEng.rpmBand = i;
                    g_driveEng.accelStep = sp->accelStep;
                    g_driveEng.decelStep = sp->decelStep;
                }
                if (prev || (g_onShoulder && g_driveEng.speed > 8)) {
                    if ((g_driveEng.speed -= (g_driveEng.speed >> 4) + 2) < 0)
                        g_driveEng.speed = 0;
                    if ((g_driveEng.rpm = g_driveEng.speed * g_driveEng.rpmPerSpeed) < 1500)
                        g_driveEng.rpm = 1500;
                } else if (g_driveEng.gearIndex == 0 && g_driveEng.speed) {
                    if (g_driveEng.speed)
                        g_driveEng.speed--;
                    g_driveEng.rpm = 30 * g_driveEng.speed + 1500;
                } else if (g_driveEng.rpmPerSpeed)
                    g_driveEng.speed = g_driveEng.rpm / g_driveEng.rpmPerSpeed;
                if (s && abs(g_driveEng.steeringInput + g_cursorX) > abs(g_driveEng.steeringInput)) {
                    if (abs(g_cursorX) + abs(g_driveEng.steering) == abs(g_driveEng.steering + g_cursorX) &&
                        abs(g_driveEng.steeringInput) < abs(g_driveEng.steering))
                        g_driveEng.steeringInput = g_driveEng.steering;
                    g_driveEng.steeringInput += g_cursorX << 1;
                    if (abs(g_driveEng.steeringInput) > 24)
                        g_driveEng.steeringInput = g_driveEng.steeringInput > 0 ? 24 : -24;
                    g_cursorX = 0;
                    g_driveEng.steering = g_driveEng.steeringInput;
                } else
                    g_driveEng.steeringInput = 0;
                if (abs(g_driveEng.steering) > 0)
                    g_driveEng.steering += g_driveEng.steering > 0 ? -((g_driveEng.steering >> 2) + 1)
                                               : ((-g_driveEng.steering >> 2) + 1);
                if (bw8aca && rd_check_crash()) {
                    g_driveEng.stopFlags |= 0x21;
                    sy_stop_drive();
                }
                g_driveBusy = 0;
            }
        }
    }
}
