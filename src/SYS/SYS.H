/**
 * @file SYS.H
 * @brief Timers, input, and PC-speaker playback services.
 */

#ifndef SYS_H
#define SYS_H
/** PIT input clocks per game tick; four game ticks per BIOS tick. */
#define SYSTEM_TIMER_RELOAD 0x4000

/** Queue sentinel later than any pending game-tick deadline. */
#define TIMER_NO_DEADLINE 0x7fffffffL

#include "PERF.H"
#include "ROAD.H"
#include "RACE.H"
#include "MENU.H"
#include "SOUND.H"


#include "MUSIC.H"
struct Hotspot;

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
    /** Next duration, control, or divisor word to read. */
    int far *nextWord;
    /** Near base of extra-pass counts; negative ends the list. */
    int *repeatTable;
    int *repeatCursor; /**< Extra-pass count for the current phrase. */
    /** Near base of tone selections for phrase passes. */
    enum ToneSlot *toneTable;
    /** Tone selection for the current phrase pass. */
    enum ToneSlot *toneCursor;
};


/** @brief Work performed when a queued deadline expires. */
enum TimerAction {
    TIMER_CANCELLED = 0,  /**< Discard the timer without running a callback. */
    TIMER_RESUME_ANIM = 1 /**< Resume the timer argument's animation object. */
};

/** @brief Accumulated mouse motion and the latest driver button masks. */
struct Mouse {
    int dx;       /**< Signed horizontal steps from the latest poll. */
    int dy;       /**< Signed vertical steps from the latest poll. */
    int mickeysX; /**< Horizontal mickeys retained until a step is produced. */
    int mickeysY; /**< Vertical mickeys retained until a step is produced. */
    int shiftX;   /**< Right shift converting horizontal mickeys to steps. */
    int shiftY;   /**< Right shift converting vertical mickeys to steps. */
    BOOL left;    /**< TRUE while the left button is down; retains its mask. */
    BOOL right;   /**< TRUE while the right button is down; retains its mask. */
    char present; /**< Mouse polling is enabled while set. */
};

/** @brief Indexed timer node awaiting an animation continuation. */
struct TimerLink {
    /** Next active or free node; negative ends an active list. */
    int next;
    /** Active neighbor toward later deadlines; negative at head. */
    int prev;
    /** Absolute game timer tick when the action becomes due. */
    long deadline;
    /** Larger ranks run first among equal deadlines. */
    int priority;
    enum TimerAction action; /**< Work to perform when due. */
    int animationId;         /**< Animation script ID to resume or cancel. */
    char unused[4];          /**< Reserved bytes. */
};

/** @brief Raw-key capture request and the last captured character. */
struct InputState {
    BOOL captureKeys; /**< TRUE returns positive keys as any-input events. */
    int key;          /**< Captured character, or zero for a click/reset. */
};

/**
 * @brief Calendar time from action costs and elapsed BIOS-rate ticks.
 *
 * Elapsed game time is action ticks plus BIOS ticks minus the time base.
 */
struct GameClock {
    long actionTicks; /**< Extra BIOS-rate ticks charged for player actions. */
    long biosTicks;   /**< Previous timer-handler calls since startup. */
    /** BIOS-tick offset preserving elapsed time across loads. */
    long timeBase;
};

#include "KEYMAPS.H"

/** @brief Whether demo-exit handling has already been entered. */
enum DemoExitState {
    DEMO_EXIT_READY = 0,     /**< Exit handling has not been entered. */
    DEMO_EXIT_ENTERED = 0x63 /**< Suppress recursive demo-exit requests. */
};

/** @brief Simulated menu key sequencing and automatic-driving progress. */
struct DemoControl {
    enum DemoExitState exitState; /**< Demo-exit reentry guard. */
    /** A simulated make event awaits its break event. */
    char keyDown;
    /** Retain the selected action until its cursor arrives. */
    char moving;
    /** Reverse garage-choice step, also used by parts menus. */
    int garageStep;
    /** Screen of the preceding simulated action. */
    enum ScreenId lastGroup;
    /** Automatic upshift requests since the last garage visit. */
    int autoUpshifts;
};

/** @brief Current note's waveform scale and modulated speaker divisor. */
struct Vibrato {
    /** Base-divisor/depth product, wrapped to a word then divided by 1000. */
    int sampleScale;
    int divisor; /**< Base divisor plus the scaled waveform sample. */
};

/** @brief Pending cursor position snapshot for drawing and hover updates. */
struct CursorDraw {
    int x;      /**< Clamped screen x used for drawing; initially invalid. */
    int y;      /**< Clamped screen y used for drawing; initially invalid. */
    char dirty; /**< A movement or hover reset requires a cursor update. */
};

/** @brief Timer-list endpoints and the cached earliest deadline. */
struct TimerQueue {
    int earliest; /**< Active tail index, or negative when empty. */
    int head;     /**< Active head toward later deadlines, or negative. */
    /** First free node index; negative is treated as exhausted. */
    int free;
    /** Earliest tick, or @ref TIMER_NO_DEADLINE when empty. */
    long nextDeadline;
    char unused; /**< Reserved byte, cleared on queue reset. */
    char busy;   /**< Set during insertion; currently never tested. */
};

/** @brief Timer priority settings, with only animation currently consumed. */
struct TimerPriorities {
    int animation; /**< Equal-deadline rank for animation continuations. */
    int unused2;   /**< Initialized spare priority; never read. */
    int unused4;   /**< Initialized spare priority; never read. */
    int unused6;   /**< Initialized spare priority; never read. */
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
extern struct HoverBounds g_boxSpotHitData;
extern struct Hotspot *g_curHoverPointer;
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
