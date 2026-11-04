/* MUSIC.H - declarations of what MUSIC.C defines. */

#ifndef MUSIC_H
#define MUSIC_H

enum ToneSlot {
    TONE_END = -1,
    TONE_PROGRAMMABLE = 0,
    TONE_PLAIN = 1,
    TONE_LIGHT_VIBRATO = 2,
    TONE_WIDE_VIBRATO = 3,
    TONE_TRILL = 4
};

enum Waveform {
    WAVE_TRIANGLE = 0,
    WAVE_SAWTOOTH = 1,
    WAVE_PULSE = 2
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
