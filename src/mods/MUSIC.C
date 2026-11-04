#include <conio.h>
#include "STRUCTS.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "SOUND.H"
#include "SYS.H"

int g_toneShapes[3][8] = {{0, 1, 2, 1, 0, -1, -2, -1}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 0, 0, 0, 0, 0, 0}};   /* DS:57F6 */
struct Tone g_tones[5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 8, 10}, {0, 0, 4, 8, 1}, {0, 0, 6, 8, 1}, {0, 2, 5, 2, 1}};   /* DS:5826 */

/* music_play @ 0F38:6F0A (linear 0x1628A, 179 bytes)

   Start the sound song with its tone list tones. Song g_songBgState's own song restarts the
   first voice (g_voiceNum = 1); any other song goes into the second voice g_secondaryMusicStateData (g_voiceNum = 2), the
   first voice's on/off state g_musicOn is saved in g_savedMusicOn meanwhile */

void music_play(void far *song, void *tones)
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
        sys_music_start(2, 0);
    }
}

/* music_set_vibrato @ 0F38:6FBD (linear 0x1633D, 47 bytes)

   If on, fill the first entry of the sound table; g_vibratoActive = on either way */

void music_set_vibrato(int on, int a, int b, int c, int d)
{
    if (on) {
        g_tones[0].phase = 0;
        g_tones[0].waveform = a;
        g_tones[0].depth = c;
        g_tones[0].length = b;
        g_tones[0].delay = d;
    }
    g_vibratoActive = on;
}

/* music_stop @ 0F38:6FEC (linear 0x1636C, 21 bytes) */

void music_stop(void)
{
    g_vibratoActive = 0;
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    sys_music_stop();
}

/* F_16381 @ 0F38:7001 (linear 0x16381, 61 bytes)

   Toggle between two states (calls sys_music_pause on entering, sys_music_resume on leaving) */

void F_16381(int on)
{
    if (on) {
        if (g_musicOn) {
            sys_music_pause();
            g_musicOn = 0;
            g_musicSuspended = 1;
        } else
            g_musicSuspended = 0;
    } else {
        if (g_musicSuspended) {
            g_musicOn = 1;
            sys_music_resume();
            g_musicSuspended = 0;
        }
    }
}
