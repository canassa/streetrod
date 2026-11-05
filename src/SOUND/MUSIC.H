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
    TONE_TRILL = 4,         /**< Two-step pulse depth: five divisor units. */
    TONE_SLOT_COUNT = 5     /**< Number of modulation presets. */
};

/** @brief Sample table used to offset the speaker timer divisor. */
enum Waveform {
    WAVE_TRIANGLE = 0, /**< Eight samples rising and falling through zero. */
    WAVE_SAWTOOTH = 1, /**< Eight samples rising from zero to seven. */
    WAVE_PULSE = 2,    /**< One raised sample among seven zero samples. */
    WAVEFORM_COUNT = 3 /**< Number of waveform rows. */
};

/**
 * @brief Mutable pitch-modulation preset and its current sample position.
 *
 * Depth scales the base speaker divisor in thousandths for each waveform sample.
 */
struct Tone {
    /** Next sample; reduced modulo the sample count. */
    int sampleIndex;
    enum Waveform waveform; /**< Waveform row to sample. */
    /** Per-sample scale in thousandths of the base divisor. */
    int depthPermille;
    /** Positive waveform prefix length when configured. */
    int sampleCount;
    /** Game timer interrupts between waveform updates. */
    int sampleTicks;
};

/** @brief Capacity of a pitch-modulation waveform row. */
enum ToneWaveSize {
    TONE_WAVE_SAMPLES = 8 /**< Samples available in each waveform row. */
};

extern int g_toneShapes[WAVEFORM_COUNT][TONE_WAVE_SAMPLES];
extern struct Tone g_tones[TONE_SLOT_COUNT];
void mu_play(void far *song, void *tones);
void mu_set_vibrato(int on, enum Waveform waveform, int length, int depth, int delay);
void mu_suspend(int on);

#endif
