#include <conio.h>
#include "STRUCTS.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "SOUND.H"
#include "SYS.H"

int g_toneShapes[3][8] = {{0, 1, 2, 1, 0, -1, -2, -1}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 0, 0, 0, 0, 0, 0}};   /* DS:57F6 */
struct Tone g_tones[5] = {{0, WAVE_TRIANGLE, 0, 0, 0}, {0, WAVE_TRIANGLE, 0, 8, 10}, {0, WAVE_TRIANGLE, 4, 8, 1}, {0, WAVE_TRIANGLE, 6, 8, 1}, {0, WAVE_PULSE, 5, 2, 1}};   /* DS:5826 */

/* mu_play @ 0F38:6F0A (linear 0x1628A, 179 bytes)

   Start the sound song with its tone list tones. Song g_songBgState's own song restarts the
   first voice (g_voiceNum = 1); any other song goes into the second voice g_secondaryMusicStateData (g_voiceNum = 2), the
   first voice's on/off state g_musicOn is saved in g_savedMusicOn meanwhile */

void mu_play(void far *song, void *tones)
{
    if (song == g_songBgState.song) {
        if (g_savedMusicOn != -1) {
            g_musicOn = g_savedMusicOn;
            g_savedMusicOn = -1;
        }
        if (g_musicOn == 0) {
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
            g_speakerOn = 0;
        }
        g_voiceNum = 1;
        g_voice = &g_songBgState;
        g_toneShapePlaybackPointer = g_tones + *g_songBgState.toneCursor;
        g_vibratoActive = 1;
    } else {
        if (g_savedMusicOn == -1) {
            g_savedMusicOn = g_musicOn;
            g_musicOn = 1;
        }
        g_voiceNum = 2;
        g_secondaryMusicStateData.cur = g_secondaryMusicStateData.phraseStart = g_secondaryMusicStateData.song = song;
        g_secondaryMusicStateData.repeatCursor = g_secondaryMusicStateData.repeatTable = tones;
        g_voice = &g_secondaryMusicStateData;
        sy_music_start(2, 0);
    }
}

/* mu_set_vibrato @ 0F38:6FBD (linear 0x1633D, 47 bytes) */

void mu_set_vibrato(int on, enum Waveform waveform, int length, int depth, int delay)
{
    if (on) {
        g_tones[TONE_PROGRAMMABLE].phase = 0;
        g_tones[TONE_PROGRAMMABLE].waveform = waveform;
        g_tones[TONE_PROGRAMMABLE].depth = depth;
        g_tones[TONE_PROGRAMMABLE].length = length;
        g_tones[TONE_PROGRAMMABLE].delay = delay;
    }
    g_vibratoActive = on;
}

/* mu_stop @ 0F38:6FEC (linear 0x1636C, 21 bytes) */

void mu_stop(void)
{
    g_vibratoActive = 0;
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    sy_music_stop();
}

/* mu_suspend @ 0F38:7001 (linear 0x16381, 61 bytes)

   Toggle between two states (calls sy_music_pause on entering, sy_music_resume on leaving) */

void mu_suspend(int on)
{
    if (on) {
        if (g_musicOn) {
            sy_music_pause();
            g_musicOn = 0;
            g_musicSuspended = 1;
        } else
            g_musicSuspended = 0;
    } else {
        if (g_musicSuspended) {
            g_musicOn = 1;
            sy_music_resume();
            g_musicSuspended = 0;
        }
    }
}
