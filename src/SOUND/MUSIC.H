/**
 * @file MUSIC.H
 * @brief Music voice selection and pitch modulation.
 */

#ifndef MUSIC_H
#define MUSIC_H

/** @brief Pitch-modulation presets selected by each music phrase. */
enum ToneSlot {
    TONE_END = -1,          /**< Unused slot at the repeat-sequence boundary. */
    TONE_PROGRAMMABLE = 0,  /**< Preset set by the vibrato controls. */
    TONE_PLAIN = 1,         /**< Zero depth leaves the pitch unchanged. */
    TONE_LIGHT_VIBRATO = 2, /**< Triangle depth: four timer-divisor units. */
    TONE_WIDE_VIBRATO = 3,  /**< Triangle depth: six timer-divisor units. */
    TONE_TRILL = 4          /**< Two-step pulse depth: five divisor units. */
};

/** @brief Sample table used to offset the speaker timer divisor. */
enum Waveform {
    WAVE_TRIANGLE = 0, /**< Eight samples rising and falling through zero. */
    WAVE_SAWTOOTH = 1, /**< Eight samples rising from zero to seven. */
    WAVE_PULSE = 2     /**< One raised sample among seven zero samples. */
};

struct Tone {
    int phase;
    enum Waveform waveform;
    int depth;
    int length;
    int delay;
};

extern int g_toneShapes[][8];
extern struct Tone g_tones[];
void mu_play(void far *song, void *tones);
void mu_set_vibrato(int on, enum Waveform waveform, int length, int depth, int delay);
void mu_suspend(int on);

#endif
