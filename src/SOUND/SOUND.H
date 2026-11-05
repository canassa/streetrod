/**
 * @file SOUND.H
 * @brief Speaker effects, music streams and delay calibration.
 */

#ifndef SOUND_H
#define SOUND_H
/**
 * @name Driving effect pitches
 * @brief PIT channel 2 divisors; smaller values produce higher speaker pitches.
 * @{
 */
#define SIREN_HIGH_DIVISOR 0x54b
#define SIREN_LOW_DIVISOR 0x712
#define TIRE_SQUEAL_DIVISOR 0x95
/** @} */
/** Delay calls per BIOS tick above which the speed score is reduced by 1/4. */
#define CALIBRATION_FAST_LIMIT 0x350
/** Fixed-point delay multiplier representing 1.0 with eight fraction bits. */
#define DELAY_SCALE_UNITY 0x100

#include "MUSIC.H"


struct Dev {
    union {
        int w;
        unsigned char b[2];
    } divisor;
    int ticksLeft;
    int repeatCount;
    char active;
    char dirty;
    int far *song;
    int far *phraseStart;
    int far *cur;
    int *repeatTable;
    int *repeatCursor;
    enum ToneSlot *toneTable;
    enum ToneSlot *toneCursor;
};

extern int g_songRepeats[];
extern enum ToneSlot g_songTones[];
extern enum ToneSlot g_jingleTones[];
extern int g_musicOn;
extern struct Dev *g_voice;
extern int g_voiceNum;
extern int g_savedMusicOn;
extern int g_musicSuspended;
extern int g_squealOn;
extern int g_sirenOn;
int so_calibrate(int n);
void so_set_freq(long freq);
void so_init(void);
void so_on(void);
void so_off(void);
void so_wheel_roll(int on);
void so_sticker(int on);
void so_low_warble(int on);
void so_pump(int on);
void so_jack(int on);
void so_low_pulse(int on);
void so_part_move(int on);
void so_siren(int on);
void so_squeal(int on);
void so_buzz(void);
void so_bolt(void);
void so_part_set(void);
void so_noise(void);
void so_crash(void);
void so_bump(void);
void so_bolt_turn(void);

#endif
