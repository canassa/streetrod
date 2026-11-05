/**
 * @file SYS.H
 * @brief Timers, input, and PC-speaker playback services.
 */

#ifndef SYS_H
#define SYS_H
#define SYSTEM_TIMER_RELOAD 0x4000
#define TIMER_NO_DEADLINE 0x7fffffffL

#include "PERF.H"
#include "ROAD.H"
#include "RACE.H"
#include "MENU.H"
#include "SOUND.H"


#include "MUSIC.H"
struct Hot;

/**
 * @brief Playback state for one PC-speaker music or effect stream.
 *
 * The selected voice advances on timer interrupts; another voice can retain
 * its position while an effect plays.
 */
struct MusicVoice {
    /** Base PIT channel-2 divisor; zero denotes a rest. */
    union {
        int w;              /**< Whole divisor word. */
        unsigned char b[2]; /**< Low byte followed by high byte. */
    } divisor;
    int ticksLeft;    /**< Timer calls remaining before the next note. */
    int phrasePasses; /**< Completed passes of the current phrase. */
    char playing;     /**< The timer advances this voice while set. */
    /** Enable the speaker on the next audible note when set. */
    char speakerPending;
    /** Instruction base, including the initial stop pair. */
    int far *stream;
    int far *phraseStart; /**< First duration word of the current phrase. */
    int far *nextWord;    /**< Next duration, control, or divisor word to read. */
    /** Near base of extra-pass counts; negative ends the list. */
    int *repeatTable;
    int *repeatCursor;         /**< Extra-pass count for the current phrase. */
    enum ToneSlot *toneTable;  /**< Near base of tone selections for phrase passes. */
    enum ToneSlot *toneCursor; /**< Tone selection for the current phrase pass. */
};


enum TimerAction {
    TIMER_CANCELLED = 0,
    TIMER_RESUME_ANIM = 1
};

struct Mouse {
    int dx;
    int dy;
    int accX;
    int accY;
    int shiftX;
    int shiftY;
    int left;
    int right;
    char present;
};

struct Link {
    int nxt;
    int prv;
    long deadline;
    int priority;
    enum TimerAction action;
    int argument;
    char unused[4];
};

struct InputState {
    int any;
    int key;
};

/* Game time from action ticks, BIOS ticks and a saved time base. */
struct GameClock {
    long actionTicks;
    long biosTicks;
    long timeBase;
};
#include "KEYMAPS.H"

struct DemoControl {
    int quit;
    char keyDown;
    char moving;
    int count;
    enum ScreenId lastGroup;
    int autoShifts;
};

struct Vibrato {
    int gain;
    int divisor;
};

struct CursorDraw {
    int x;
    int y;
    char dirty;
};

struct TimerQueue {
    int earliest;
    int head;
    int free;
    long nextDeadline;
    char unused;
    char busy;
};

struct TimerPriorities {
    int animation;
    int unused2;
    int unused4;
    int unused6;
};

extern struct InputState g_input;
extern int g_unused6;
extern int g_unused7;
extern int g_unused8;

extern struct TimerPriorities g_sysTimerPriorities;

extern long g_waitUntil;
extern int g_msgTicks;
extern int g_clickEnds;
extern int g_kbdHooked;
extern struct Mouse g_mouse;
extern long g_tick;
extern struct GameClock g_gameClock;
extern int g_demo;
extern int g_autoDrive;
extern char g_protectPal[];
extern int g_vibratoActive;
extern struct DrivingState g_driveEng;
extern struct MusicVoice g_songBgState;
extern struct MusicVoice g_secondaryMusicStateData;
extern void(interrupt far *g_clockPrev)(void);
extern struct RoadView g_trackViewMotion;
extern struct RivalPosition g_roadOppPos;
extern char g_strOpp[];
extern int g_startLightGo;
extern long g_mphAccumulator;
extern struct Tone *g_toneShapePlaybackPointer;
extern int g_xCoordSprite;
extern int g_menuLevel;
extern struct Lims g_boxSpotHitData;
extern struct Hot *g_curHoverPointer;
extern int g_cursHoverCodeVal;
extern int g_wRivBanner;
void sy_hover_reset(void);
void sy_music_pause(void);
void sy_music_resume(void);
void sy_reset_queues(void);
void sy_timer_remove(int n);
void sy_unhook(void);
void sy_exit(void);
void sy_crit_error(unsigned deverr, unsigned char errcode);
void sy_init(void);
void sy_keymap_set(enum ScreenId n);
int sy_keymap_restore(int keep);
int sy_wait_input(int n);
int sy_timer_add(int delay, int pri);
void sy_timer_anim(int fc, int delay);
void sy_timer_cancel(int n);
void sy_music_start(int n, int cnt);
void sy_music_stop(void);
void sy_drive_start(void);
void sy_drive_stop(void);
int sy_sign(int x);
void sy_demo_step(void);
void interrupt far sy_timer_isr(void);
void sy_stop_drive(void);
void interrupt far sy_drive_isr(void);

#endif
