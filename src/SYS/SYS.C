#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include "SYS.H"
#include "BITMAP.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "FAME.H"
#include "GAME.H"
#include "GARAGE.H"
#include "HOTSPOT.H"
#include "INPUTDAT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "KBDOFF.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "PC.H"
#include "PERF.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SONG.H"
#include "SOUND.H"
#include "STRUCTS.H"
#include "VIDDET.H"
#include "VIDEO.H"

static struct CursorDraw g_drawnCursor;
static enum ScreenId g_keyGroup;
static char g_lastClick;
static int g_demoChoice;
static struct Vibrato g_vibrato;

struct MusicVoice g_songBgState;
struct MusicVoice g_secondaryMusicStateData;
void(interrupt far *g_clockPrev)(void);
char g_strOpp[80];
int g_menuLevel;
struct HoverBounds g_boxSpotHitData;
struct Hotspot *g_curHoverPointer;
int g_cursHoverCodeVal;
int g_wRivBanner;
struct TimerPriorities g_sysTimerPriorities;

static struct TimerQueue g_timerQueue = {-1, -1, 0, TIMER_NO_DEADLINE, 0, 0};
static struct TimerLink g_timers[4] = {{0, 0, 0L, 0, TIMER_CANCELLED, 0, ""},
                                       {0, 0, 0L, 0, TIMER_CANCELLED, 0, ""},
                                       {0, 0, 0L, 0, TIMER_CANCELLED, 0, ""},
                                       {0, 0, 0L, 0, TIMER_CANCELLED, 0, ""}};
static int g_unused3 = 0;
static int g_unused4[8] = {0, 0, 0, 0, 0, 0, 0, 0};
long g_waitUntil = 0L;
struct InputState g_input = {FALSE, 0};
static int g_unused5 = 32767;
int g_unused6 = 0;
int g_unused7 = 0;
int g_unused8 = 0;
int g_msgTicks = 350;
int g_clickEnds = 0;
int g_kbdHooked = 0;
struct Mouse g_mouse = {0, 0, 0, 0, 0, 0, FALSE, 10, 3};
static int g_unused9[2] = {0, 0};
long g_tick = 0L;
struct GameClock g_gameClock = {0L, 0L, 0L};
static char g_tickDiv = 0;
static char g_driveDiv = 0;
static int g_unused10 = 0;
static char *g_errRead = "cannot read drive ?";
static char *g_errWriteProt = "disk ? is write protected";
int g_demo = 0;
int g_autoDrive = 0;


void sy_hover_reset(void)
{
    g_drawnCursor.dirty = 1;
    g_boxSpotHitData.x2 = -1;
    g_cursHoverCodeVal = 0x7d00;
    g_click = 0;
    g_click2 = 0;
    kg_key();
}


void sy_music_pause(void)
{
    if (g_voice->playing) {
        g_voice->playing = FALSE;
        outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
        g_speakerOn = 0;
        g_voice->speakerPending = TRUE;
        g_hots[ga_hotspot(KR_MUSIC)].textOffset = g_offTunes;
        sy_hover_reset();
    }
}


void sy_music_resume(void)
{
    if (g_musicOn && !g_voice->playing && g_voice->phraseStart != g_voice->stream) {
        g_voice->playing = TRUE;
        g_voice->speakerPending = TRUE;
        g_hots[ga_hotspot(KR_MUSIC)].textOffset = g_offSquelch;
        sy_hover_reset();
    }
}


void sy_reset_queues(void)
{
    int i;
    long deadline;

    g_timerQueue.earliest = g_timerQueue.head = -1;
    g_timerQueue.free = 0;
    g_timerQueue.nextDeadline = TIMER_NO_DEADLINE;
    g_timerQueue.unused = g_timerQueue.busy = 0;
    for (i = 0; i < 4; i++)
        g_timers[i].next = i + 1;
    g_unused3 = -1;
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
        g_mouse.mickeysX = g_mouse.mickeysY = 0;
    }
    g_click = 0;
    for (i = 0; i < 8; i++)
        g_dirHeld[i] = 0;
    g_buttons = g_keyDx = g_keyDy = g_lastDx = g_lastDy = 0;
}


void sy_timer_remove(int n)
{
    register int i;

    i = g_timers[n].next;
    if (i >= 0)
        g_timers[i].prev = g_timers[n].prev;
    else {
        i = g_timers[n].prev;
        g_timerQueue.earliest = i;
        if (i < 0)
            g_timerQueue.nextDeadline = TIMER_NO_DEADLINE;
        else
            g_timerQueue.nextDeadline = g_timers[i].deadline;
    }
    i = g_timers[n].prev;
    if (i >= 0)
        g_timers[i].next = g_timers[n].next;
    else
        g_timerQueue.head = g_timers[n].next;
    g_timers[n].next = g_timerQueue.free;
    g_timerQueue.free = n;
}


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

static char g_unused11 = 0;
static char g_keymapGroup = 0;
static struct KeyMap g_keymapSaved[30] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};


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
        g_mouse.mickeysX = g_mouse.mickeysY = 0;
        g_mouse.shiftX = 1;
        g_mouse.shiftY = 1;
    } else
        g_mouse.present = 0;
    g_drawnCursor.x = -1;
    g_drawnCursor.y = -1;
    g_songBgState.playing = FALSE;
    g_songBgState.stream = g_song;
    g_songBgState.nextWord = g_songBgState.phraseStart = g_songBgState.stream;
    g_songBgState.repeatCursor = g_songBgState.repeatTable = g_songRepeats;
    g_songBgState.toneCursor = g_songBgState.toneTable = g_songTones;
    g_secondaryMusicStateData.playing = FALSE;
    g_secondaryMusicStateData.toneCursor = g_secondaryMusicStateData.toneTable = g_jingleTones;
    _harderr(sy_crit_error);
    _disable();
    outp(PIT_CMD, PIT_CH0_LOHI_MODE3);
    outp(PIT_CH0, 0);
    outp(PIT_CH0, SYSTEM_TIMER_RELOAD >> 8);
    g_clockPrev = _dos_getvect(TIMER_INT);
    _dos_setvect(TIMER_INT, sy_timer_isr);
    _enable();
    kg_hook();
    g_kbdHooked = 1;
    outp(PIT_CMD, PIT_CH2_LOHI_MODE3);
    sy_reset_queues();
    g_sysHooked = 1;
}


void sy_keymap_set(enum ScreenId n)
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
    for (; p->scanCode; p++, q++, num++) {
        if (num >= 30)
            break;
        q->mappedByte = g_scanMap[q->scanCode = c = p->scanCode];
        g_scanMap[c] = p->mappedByte;
    }
    p = g_keyMaps[KM_MUSIC];
    q->mappedByte = g_scanMap[q->scanCode = c = p->scanCode];
    g_scanMap[c] = p->mappedByte;
    q[1].scanCode = 0;
}


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
            for (p = g_keymapSaved; p->scanCode; p++)
                if (p->mappedByte >= 0x20)
                    g_scanMap[p->scanCode] = p->mappedByte;
        } else {
            for (p = g_keymapSaved; p->scanCode; p++)
                g_scanMap[p->scanCode] = p->mappedByte;
            g_keymapGroup = 0;
        }
    }
    return i;
}

char g_protectPal[16] = {0, 0, 8, 7, 15, 7, 15, 8, 7, 2, 4, 12, 5, 10, 6, 14};
static int g_unused12 = 0;
static struct DemoControl g_demoControl = {DEMO_EXIT_READY, 0, 0, 0, KM_NONE, 0};
static int g_demoTargetX = 0;
static int g_demoTargetY = 0;
static int g_demoScriptPos = 0;
static int g_demoScript[22] = {-1, 0, -1, 0, 1, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 0, 0, 2};
static char g_cursorBusy = 0;
static int g_vibCount = 1;
static int g_sirenPhase = 0;
static int g_overRev = 0;
static char g_driveBusy = 0;
static char g_lastFire = 0;
static char g_squealPhase = 0;


int sy_wait_input(int ticks)
{
    register struct TimerLink *p;
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
    if (g_keyGroup == KM_RACE_TYPE) {
        g_statusLine.needsErase = TRUE;
        sc_status_text(-1, "", 0);
        g_statusLine.needsErase = TRUE;
        sc_status_text(g_bannerX, "", g_wRivBanner);
        g_strOpp[0] = 0;
    }
    flag = g_waitUntil != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_demo == 0x63 && g_demoControl.exitState != DEMO_EXIT_ENTERED) {
            g_demoControl.exitState = DEMO_EXIT_ENTERED;
            sy_exit();
        }
        if (g_protectCycle && (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY) && !(g_tick & 0xf) && g_musicOn &&
            g_speakerOn) {
            old = g_protectPal[8];
            g_protectPal[8] = g_protectPal[7];
            g_protectPal[7] = g_protectPal[6];
            g_protectPal[6] = old;
            vi_palette_reg(6, old);
            vi_palette_reg(7, g_protectPal[7]);
            vi_palette_reg(8, g_protectPal[8]);
        }
        res = kg_key();
        if (res > KEY_NONE) {
            if (g_input.captureKeys) {
                g_input.key = res;
                return KR_ANY_INPUT;
            }
            if (res == ASCII_DELETE)
                sy_exit();
        } else if (res < KEY_NONE) {
            res += 0x100 - KEY_VALUE_BASE;
            if (res == KR_MUSIC) {
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
            case TIMER_CANCELLED:
            default:
                sy_timer_remove(i);
                break;
            case TIMER_RESUME_ANIM:
                res = p->animationId;
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
                return KR_ANY_INPUT;
            }
            if (g_menuLevel) {
                if (i = (x >= g_boxSpotHitData.x1 && x < g_boxSpotHitData.x2 && y >= g_boxSpotHitData.y1 &&
                         y < g_boxSpotHitData.y2)
                            ? g_cursHoverCodeVal
                            : mn_hit(x, y))
                    return i;
            }
            if (g_input.captureKeys) {
                g_input.key = 0;
                return KR_ANY_INPUT;
            }
        } else if (flag && g_waitUntil < g_tick) {
            g_waitUntil = 0L;
            return KR_TIMEOUT;
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
            if (g_menuLevel && (x < g_boxSpotHitData.x1 || x >= g_boxSpotHitData.x2 || y < g_boxSpotHitData.y1 ||
                                y >= g_boxSpotHitData.y2)) {
                res = g_cursHoverCodeVal;
                if (i = mn_hit(x, y)) {
                    if (res != i) {
                        if (g_keyGroup == KM_BOBS) {
                            g_statusLine.needsErase = TRUE;
                            sc_status_text(-1, "", 0);
                            g_statusLine.needsErase = TRUE;
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
                    if (g_keyGroup == KM_BOBS)
                        sc_status_text(g_bannerX, g_strOpp, g_wRivBanner);
                    else if (res)
                        sc_status_text(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}

struct DrivingState g_driveEng;
int g_vibratoActive;
struct Tone *g_toneShapePlaybackPointer;


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
                    cur3 = g_timers[prev = cur3].next;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_timers[cur3].priority >= pri)
                        break;
                    cur3 = g_timers[prev = cur3].next;
                    if (cur3 < 0 || g_timers[cur3].deadline != key)
                        break;
                }
            }
            break;
        }
    }
    g_timers[i].deadline = when;
    g_timers[i].priority = pri;
    g_timerQueue.free = g_timers[i].next;
    g_timers[i].next = cur3;
    if (cur3 < 0) {
        g_timerQueue.earliest = i;
        g_timerQueue.nextDeadline = when;
    } else
        g_timers[cur3].prev = i;
    g_timers[i].prev = prev;
    if (prev < 0)
        g_timerQueue.head = i;
    else
        g_timers[prev].next = i;
    g_timerQueue.busy = 0;
    return i;
}


void sy_timer_anim(int fc, int delay)
{
    int n;

    n = sy_timer_add(delay, g_sysTimerPriorities.animation);
    g_timers[n].action = TIMER_RESUME_ANIM;
    g_timers[n].animationId = fc;
}


void sy_timer_cancel(int n)
{
    struct TimerLink *p;
    int i;

    for (i = g_timerQueue.earliest; i >= 0; i = g_timers[i].next)
        if ((p = g_timers + i)->action == TIMER_RESUME_ANIM && p->animationId == n)
            p->action = TIMER_CANCELLED;
}


void sy_music_start(int n, int cnt)
{
    if (g_musicOn) {
        g_voice->playing = FALSE;
        outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
        g_speakerOn = 0;
        g_voice->phrasePasses = 0;
        g_voice->speakerPending = TRUE;
        g_voice->phraseStart = g_voice->nextWord = g_voice->stream + n;
        g_voice->ticksLeft = cnt + 1;
        g_voice->toneCursor = g_voice->toneTable;
        g_toneShapePlaybackPointer = g_tones + *g_voice->toneCursor;
        g_vibratoActive = 1;
        g_voice->playing = TRUE;
    } else {
        g_voice->playing = FALSE;
    }
}


void sy_music_stop(void)
{
    g_voice->playing = FALSE;
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
    g_voice->speakerPending = TRUE;
    g_voice->phraseStart = g_voice->nextWord = g_voice->stream;
}


void sy_drive_start(void)
{
    g_unused10 = 0;
    _dos_setvect(TIMER_INT, sy_drive_isr);
    if (!g_demo && g_mouse.present) {
        mo_poll();
        g_mouse.mickeysX = g_mouse.mickeysY = 0;
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
        g_mouse.mickeysX = g_mouse.mickeysY = 0;
        g_mouse.shiftX = 1;
        g_mouse.shiftY = 1;
    }
    g_lastDx = g_lastDy = g_buttons = g_click = 0;
}


int sy_sign(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}

#pragma loop_opt(off)


void sy_demo_step(void)
{
    int num;
    int cx;
    int k;
    int mask;
    int step;
    int cy;

    if (g_keyGroup == KM_START_GAME)
        rn_int(-1);
    if (g_demoControl.keyDown) {
        g_demoKey |= SCAN_BREAK;
        g_demoControl.keyDown = 0;
        jy_demo_key();
        return;
    }
    if (!g_demoControl.moving) {
        if (g_demoControl.autoUpshifts && g_keyGroup == KM_GARAGE) {
            g_demoControl.autoUpshifts = 0;
            g_demoControl.garageStep = 15;
        }
        num = (int)strlen((char *)g_keyMaps[g_keyGroup]) / 2;
        k = 0;
        switch (g_keyGroup) {
        case KM_PAINT:
        case KM_DRAG_WAGER:
        case KM_ROAD_WAGER:
        case KM_RACE_TYPE:
        case KM_CALL_RIVAL:
            num -= 2;
            break;
        case KM_YOUR_CARS:
            num -= 2;
        case KM_BOBS:
            k = 1;
            break;
        case KM_GAS_STATION:
            g_demoControl.garageStep = 4;
            break;
        case KM_ADVERTS:
            k = 4;
            break;
        case KM_TIMING:
        case KM_STICKERS:
            k = 2;
            break;
        case KM_BUY:
            if (g_driver.currentCar)
                break;
        case KM_COUNTER_OFFER:
            num--;
            break;
        }
        if (g_keyGroup == KM_GARAGE && g_demoControl.lastGroup != KM_GARAGE) {
            g_demoChoice = num - g_demoControl.garageStep++ - 1;
            g_demoControl.garageStep = g_demoControl.garageStep % num;
        } else
            g_demoChoice = rn_int(num + k);
        switch (g_keyGroup) {
        case KM_ENGINE:
            if ((g_demoChoice >= 0 && g_demoChoice < 9) || g_demoChoice == 12)
                g_demoChoice = 9;
            if (g_demoControl.lastGroup == KM_TIMING)
                g_demoChoice = 11;
            break;
        case KM_TRANSMISSION:
            g_demoChoice = g_demoScript[g_demoScriptPos++];
            if (g_demoChoice == 2)
                g_demoScriptPos = 0;
            break;
        case KM_GAS_STATION:
            if (g_demoChoice)
                break;
        case KM_START_GAME:
            g_demoChoice = 1;
            break;
        case KM_SPARE_PARTS:
            if (g_demoControl.garageStep < 11)
                g_demoChoice = 0;
            else
                g_demoChoice = num - 1;
            break;
        case KM_BODY_CHANGE:
        case KM_END_MENU:
            g_demoChoice = 0;
            break;
        case KM_CALL_RIVAL:
            if (g_demoControl.lastGroup == KM_CALL_RIVAL)
                g_demoChoice = 0;
            break;
        case KM_YOUR_CARS:
            if (g_demoControl.lastGroup == KM_YOUR_CARS)
                g_demoChoice = 10;
            break;
        case KM_SAVE_RESTART_QUIT:
            g_demoChoice = 5;
            break;
        case KM_NEWSPAPER:
            if (g_driver.currentCar == 0)
                g_demoChoice = 0;
            break;
        }
        if (g_keyGroup == KM_ADVERTS)
            g_demoTargetX = 160;
        if (g_demoChoice >= 0) {
            if (num > g_demoChoice) {
                g_demoKey = g_keyMaps[g_keyGroup][g_demoChoice].scanCode;
                mask = g_keyMaps[g_keyGroup][g_demoChoice].mappedByte + 0x32;
                step = g_groupHot[g_keyGroup];
                for (; step - g_groupHot[g_keyGroup] < 32 && g_hots[step].hitCode != mask; step++)
                    ;
                if (g_hots[step].hitCode == mask) {
                    cx = (g_hots[step].x1 + g_hots[step].x2) / 2;
                    if (cx)
                        g_demoTargetX = cx;
                    if (g_keyGroup == KM_ADVERTS)
                        g_demoTargetX = 160;
                    cy = (g_hots[step].y1 + g_hots[step].y2) / 2;
                    if (cy)
                        g_demoTargetY = cy;
                }
            } else {
                if (num == g_demoChoice)
                    g_demoKey = SC_ENTER;
                if (num + 1 == g_demoChoice)
                    g_demoKey = SC_SPACE;
                if (num + 2 == g_demoChoice)
                    g_demoKey = SC_UP;
                if (num + 3 == g_demoChoice)
                    g_demoKey = SC_DOWN;
                if (num + 4 == g_demoChoice)
                    g_demoKey = SC_LEFT;
                if (num + 5 == g_demoChoice)
                    g_demoKey = SC_RIGHT;
            }
        } else
            g_demoKey = SC_CENTER;
    }
    if ((g_demoTargetX && g_cursorX != g_demoTargetX) || (g_demoTargetY && g_cursorY != g_demoTargetY)) {
        g_demoControl.moving = 1;
        return;
    }
    g_demoControl.moving = 0;
    if (g_keyGroup == KM_BOBS && g_demoControl.autoUpshifts < 10 && g_demoKey == SC_G)
        g_demoKey = SC_ENTER;
    if (g_keyGroup == KM_GARAGE && g_demoControl.lastGroup == KM_GARAGE) {
        g_demoKey = SC_ENTER;
        g_demoControl.lastGroup = KM_NONE;
    } else
        g_demoControl.lastGroup = g_keyGroup;
    if (g_driver.spareParts == 0 && g_keyGroup == KM_NEWSPAPER)
        gm_add_part(0x2a, g_parts[42].basePrice);
    if (g_driver.currentCar == 0) {
        if (g_keyGroup == KM_GARAGE)
            g_demoKey = SC_N;
        if (g_keyGroup == KM_ADVERTS && (g_demoKey == SC_ESC || g_demoKey == SC_G || g_demoKey == SC_P))
            g_demoKey = SC_SPACE;
    }
    if (g_demoKey == SC_SCROLL_LOCK)
        g_demoKey = SC_ENTER;
    g_demoControl.keyDown = 1;
    jy_demo_key();
}


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
    if (g_voice->playing && --g_voice->ticksLeft) {
        if (g_vibratoActive && g_voice->divisor.w && !--g_vibCount) {
            g_toneShapePlaybackPointer->sampleIndex =
                g_toneShapePlaybackPointer->sampleIndex % g_toneShapePlaybackPointer->sampleCount;
            g_vibrato.divisor =
                g_toneShapes[g_toneShapePlaybackPointer->waveform][g_toneShapePlaybackPointer->sampleIndex++] *
                    g_vibrato.sampleScale +
                g_voice->divisor.w;
            outp(PIT_CH2, g_vibrato.divisor);
            outp(PIT_CH2, g_vibrato.divisor >> 8);
            g_vibCount = g_toneShapePlaybackPointer->sampleTicks;
        }
    } else if (g_voice->playing && g_voice->ticksLeft == 0) {
        while (!(g_voice->ticksLeft = *g_voice->nextWord++)) {
            if (*g_voice->nextWord++ == 0) {
                outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
                g_speakerOn = 0;
                g_voice->playing = FALSE;
                g_voice->speakerPending = TRUE;
                g_voice->phraseStart = g_voice->nextWord = g_voice->stream;
                goto tail;
            }
            g_voice->toneCursor++;
            if (*g_voice->repeatCursor < ++g_voice->phrasePasses) {
                if (*++g_voice->repeatCursor < 0) {
                    g_voice->nextWord = g_voice->stream + 2;
                    g_voice->repeatCursor = g_voice->repeatTable;
                    g_voice->toneCursor = g_voice->toneTable;
                }
                g_voice->phraseStart = g_voice->nextWord;
                g_voice->phrasePasses = 0;
            } else
                g_voice->nextWord = g_voice->phraseStart;
            g_toneShapePlaybackPointer = g_tones + *g_voice->toneCursor;
        }
        if (!(g_voice->divisor.w = *g_voice->nextWord++)) {
            g_voice->speakerPending = TRUE;
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
            g_speakerOn = 0;
        } else {
            if (g_vibratoActive) {
                g_vibCount = g_toneShapePlaybackPointer->sampleTicks;
                g_vibrato.sampleScale = (unsigned)g_toneShapePlaybackPointer->depthPermille * g_voice->divisor.w / 1000;
            }
            outp(PIT_CH2, g_voice->divisor.b[0]);
            outp(PIT_CH2, g_voice->divisor.b[1]);
            if (g_soundOn && (g_soundOn != g_speakerOn || g_voice->speakerPending)) {
                outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
                g_speakerOn = 1;
                if (g_voice->speakerPending)
                    g_voice->speakerPending = FALSE;
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
                g_cursorX += g_cursorStepX[x + 1];
                g_cursorY += g_cursorStepY[y + 1];
                g_drawnCursor.dirty = 1;
            }
            g_cursorBusy = 0;
        }
    }
}

struct RivalPosition g_roadOppPos;
struct RoadView g_trackViewMotion;
int g_startLightGo;
long g_mphAccumulator;
int g_xCoordSprite;


void sy_stop_drive(void)
{
    g_driveEng.accelStep = g_driveEng.decelStep = g_driveEng.steeringInput = g_driveEng.steering = g_unused6 = 0;
    g_driveEng.unused5 = g_driveEng.elapsedTicks;
}


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
                    val.w = 50000 - ((g_driveEng.rpm == ENGINE_REDLINE_RPM ? 10000 : 0) + r);
                    outp(PIT_CH2, val.b[0]);
                    outp(PIT_CH2, val.b[1]);
                } else if (g_carMoving) {
                    off = g_picXDraw - g_xCoordSprite;
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
                        val.w = SIREN_HIGH_DIVISOR;
                    else
                        val.w = SIREN_LOW_DIVISOR;
                    outp(PIT_CH2, val.b[0]);
                    outp(PIT_CH2, val.b[1]);
                    if (g_sirenPhase > 20)
                        g_sirenPhase = 0;
                } else if (g_squealOn || g_onShoulder) {
                    if (g_squealPhase) {
                        val.w = TIRE_SQUEAL_DIVISOR;
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
        if (g_runRacingFlag && !g_carAnim && --g_driveDiv <= 0) {
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
                    if (g_driveEng.accelStep * 4 + g_driveEng.rpm < ENGINE_REDLINE_RPM) {
                        if (g_roadOppPos.roadIndex > g_trackViewMotion.roadIndex ||
                            (!g_driver.raceType ? DRAG_FINISH_INDEX : ROAD_FINISH_INDEX) - 60 <
                                g_trackViewMotion.roadIndex)
                            flag = 1;
                    } else if (g_driveEng.gearIndex < g_driveEng.topGear && g_startLightGo) {
                        c = 1;
                        flag = 1;
                        g_demoControl.autoUpshifts++;
                    }
                    if (g_driveEng.rpm < AUTO_DOWNSHIFT_RPM && g_driveEng.gearIndex > DRIVE_FIRST && !flag)
                        c = 1;
                    if (g_driveEng.automatic && g_driveEng.gearIndex > DRIVE_NEUTRAL)
                        c = 0;
                }
                if (g_runRacingFlag) {
                    g_mphAccumulator += g_driveEng.speed;
                    g_driveEng.elapsedTicks++;
                }
                if ((!g_driver.raceType ? DRAG_FINISH_INDEX : ROAD_FINISH_INDEX) < g_trackViewMotion.roadIndex) {
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
                if (g_driveEng.rpm > ENGINE_WEAR_RPM) {
                    g_driver.currentCar->rack.parts[RACK_ENGINE]->wear += !g_driver.raceType ? 1 : 1;
                    g_driver.currentCar->transmission->wear += !g_driver.raceType ? 1 : 1;
                }
                if (!(g_gameClock.biosTicks & (!g_driver.raceType ? 0xf : 0xf)))
                    g_driver.currentCar->rack.parts[RACK_ENGINE]->wear++;
                if ((i = g_driver.currentCar->rack.parts[RACK_ENGINE]->wear) >= PART_WEAR_LIMIT) {
                    g_driveEng.stopFlags |= (STOP_LOST | STOP_ENGINE);
                    sy_stop_drive();
                }
                if (flag) {
                    g_driveEng.rpm += g_driveEng.accelStep;
                    if (g_driveEng.automatic && g_driveEng.rpm > AUTO_MAX_RPM)
                        g_driveEng.rpm = AUTO_MAX_RPM;
                    if (g_driveEng.rpm > ENGINE_REDLINE_RPM) {
                        g_driveEng.rpm = ENGINE_REDLINE_RPM;
                        if (15 - i / 1000 < ++g_overRev) {
                            g_driveEng.stopFlags |= (STOP_LOST | STOP_ENGINE);
                            sy_stop_drive();
                        }
                    }
                } else if (g_driveEng.rpm > ENGINE_IDLE_RPM ||
                           (g_driveEng.gearIndex > DRIVE_NEUTRAL && g_driveEng.rpm > g_driveEng.decelStep))
                    g_driveEng.rpm -= g_driveEng.decelStep;
                if (g_driveEng.rpm < ENGINE_REDLINE_RPM)
                    g_overRev = 0;
                top = g_driveEng.speed < 5 && !flag && g_driveEng.gearIndex > DRIVE_NEUTRAL;
                if (c || top ||
                    (g_driveEng.automatic && g_driveEng.autoSelector > SELECTOR_NEUTRAL &&
                     ((flag && g_driveEng.rpm > g_driveEng.autoShiftRpm && g_driveEng.gearIndex < g_driveEng.topGear) ||
                      (!flag && g_driveEng.gearIndex > DRIVE_FIRST && !g_driveEng.rpmBand)))) {
                    g_driver.currentCar->transmission->wear += g_driveEng.rpm > ENGINE_WEAR_RPM ? 15 : 3;
                    if (g_demo == 0) {
                        i = g_driver.currentCar->transmission->wear;
                        if (i >= PART_WEAR_LIMIT ||
                            (!g_driveEng.automatic && flag && g_driveEng.rpm > ENGINE_WEAR_RPM &&
                             i / 100 + 100 > rn_int(3277))) {
                            g_driveEng.stopFlags |= (STOP_LOST | STOP_TRANSMISSION);
                            sy_stop_drive();
                        }
                    }
                    if ((prev || (!g_driveEng.rpmBand && !flag)) && g_driveEng.gearIndex > DRIVE_NEUTRAL)
                        g_driveEng.gearIndex--;
                    else if ((flag || (g_driveEng.rpmBand > RPM_BAND_LOW && !prev)) &&
                             g_driveEng.gearIndex < g_driveEng.topGear)
                        g_driveEng.gearIndex++;
                    if (g_driveEng.automatic) {
                        if (c)
                            g_driveEng.autoSelector =
                                g_driveEng.autoSelector == SELECTOR_NEUTRAL ? SELECTOR_DRIVE : SELECTOR_NEUTRAL;
                        if (top || g_driveEng.gearIndex == DRIVE_NEUTRAL)
                            g_driveEng.autoSelector = SELECTOR_NEUTRAL;
                        if (g_driveEng.autoSelector == SELECTOR_NEUTRAL)
                            g_driveEng.gearIndex = DRIVE_NEUTRAL;
                    } else if (top)
                        g_driveEng.gearIndex = DRIVE_NEUTRAL;
                    g_driveEng.shifterDirty = TRUE;
                    g_driveEng.rpmPerSpeed = g_driveEng.gearRpmPerSpeed[g_driveEng.gearIndex];
                    if ((g_driveEng.rpm = g_driveEng.speed * g_driveEng.rpmPerSpeed) == 0)
                        g_driveEng.rpm = 1505;
                }
                i = RPM_BAND_LOW - 1;
                do {
                    sp = &g_driveEng.gear[g_driveEng.gearIndex].bands[++i];
                } while (i < RPM_BAND_HIGH && sp->maxRpm < g_driveEng.rpm);
                if (i != g_driveEng.rpmBand || g_driveEng.shifterDirty) {
                    g_driveEng.rpmBand = i;
                    g_driveEng.accelStep = sp->accelStep;
                    g_driveEng.decelStep = sp->decelStep;
                }
                if (prev || (g_onShoulder && g_driveEng.speed > 8)) {
                    if ((g_driveEng.speed -= (g_driveEng.speed >> 4) + 2) < 0)
                        g_driveEng.speed = 0;
                    if ((g_driveEng.rpm = g_driveEng.speed * g_driveEng.rpmPerSpeed) < ENGINE_IDLE_RPM)
                        g_driveEng.rpm = ENGINE_IDLE_RPM;
                } else if (g_driveEng.gearIndex == DRIVE_NEUTRAL && g_driveEng.speed) {
                    if (g_driveEng.speed)
                        g_driveEng.speed--;
                    g_driveEng.rpm = 30 * g_driveEng.speed + ENGINE_IDLE_RPM;
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
                    g_driveEng.steering +=
                        g_driveEng.steering > 0 ? -((g_driveEng.steering >> 2) + 1) : ((-g_driveEng.steering >> 2) + 1);
                if (g_runRacingFlag && rd_check_crash()) {
                    g_driveEng.stopFlags |= (STOP_LOST | STOP_ROADSIDE_CRASH);
                    sy_stop_drive();
                }
                g_driveBusy = 0;
            }
        }
    }
}
