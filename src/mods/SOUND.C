#include <conio.h>
#include "STRUCTS.H"
#include "SOUND.H"
#include "CARPIC.H"
#include "EFFECTS.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PC.H"
#include "RANDOM.H"
#include "SONG.H"
#include "SPEAKER.H"
#include "SYS.H"

static void near play_tone(long freq, int n);
static void near pulse_train(int a, int *p, int count, int reps);
static void near pulse_train_back(int a, int *p, int count, int reps);
static int near fill_range(int *buf, int start, int end, int step);
static void near noise(int range, int bits, int count);
static void near warble(long freq, int period, int hi, int step, int cycles, int reps);
static void near sweep_up_down(int a, int b, int c, int d, int e);
static void near tone_burst(int a, int b, int c, int d);
static void near noise_sweep_down(int a, int b, int c, int d, int e);
static void near noise_sweep_up(int a, int b, int c, int d, int e);
static void near sweep_down(int start, int end, int step, int delay);

int g_songRepeats[19] = {2, 1, 0, 2, 2, 0, 2, 1, 0, 1, 0, 0, 2, 2, 0, 2, 1, 0, -1};   /* DS:5858 */
char g_jingleRepeats[4] = {0, 0, -1, -1};   /* DS:587E */
int g_songTones[37] = {1, 2, 1, 2, 3, 3, 1, 2, 1, 3, 1, 4, 4, 1, 2, 1, 3, 1, 2, 1, 3, 4, 4, 1, 2, 1, 3, 4, 1, 1, 1, 2, 1, 4, 3, 3, -1};   /* DS:5882 */
int g_jingleTones[2] = {0, -1};   /* DS:58CC */
int g_musicOn = 1;   /* DS:58D0 */
struct Dev *g_voice = &bs79aa;   /* DS:58D2 */
int g_voiceNum = 1;   /* DS:58D4 */
int g_savedMusicOn = -1;   /* DS:58D6 */
int g_musicSuspended = 0;   /* DS:58D8 */
int g_squealOn = 0;   /* DS:58DA */
int g_sirenOn = 0;   /* DS:58DC */

/* sound_calibrate @ 0F38:703E (linear 0x163BE, 151 bytes)

   Calibrate the speed of the machine: counts how often speaker_delay(n) can be called within
   one BIOS timer tick (the long at 0040:006C). Counts above 848 are scaled by 3/4. Also sets
   g_delayScale to 256 on the way */

int sound_calibrate(int n)
{
    long tick;
    int count;

    count = 0;
    g_delayScale = 0x100;
    tick = *BIOS_TICKS;
    while (tick == *BIOS_TICKS)
        ;
    tick = *BIOS_TICKS;
    while (tick == *BIOS_TICKS) {
        speaker_delay(n);
        count++;
    }
    if (count > 0x350)
        count = 3 * count / 4;
    return count;
}

/* sound_set_freq @ 0F38:70D5 (linear 0x16455, 44 bytes)

   Program PIT channel 2 for a tone of the given frequency */

void sound_set_freq(long freq)
{
    union Word div;

    div.w = (int)(1193180L / freq);
    outp(PIT_CH2, div.b[0]);
    outp(PIT_CH2, div.b[1]);
}

/* sound_init @ 0F38:7101 (linear 0x16481, 33 bytes)

   Set up the PIT for sound (channel 2, mode 3) and program a tone (divisor from 25000) */

void sound_init(void)
{
    g_delayScale = sound_calibrate(10);
    outp(PIT_CMD, PIT_CH2_LOHI_MODE3);
    sound_set_freq(25000L);
}

/* sound_on @ 0F38:7122 (linear 0x164A2, 27 bytes)

   Speaker on: program the tone for 25000 and set bits 0 and 1 of port 61h */

void sound_on(void)
{
    sound_set_freq(25000L);
    outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_MASK);
    g_speakerOn = 1;
}

/* sound_off @ 0F38:713D (linear 0x164BD, 15 bytes)

   Speaker off: clear bits 0 and 1 of port 61h */

void sound_off(void)
{
    outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_MASK));
    g_speakerOn = 0;
}

/* play_tone @ 0F38:714C (linear 0x164CC, 29 bytes)

   Play the tone of frequency freq for n ticks (speaker_delay) */

static void near play_tone(long freq, int n)
{
    sound_set_freq(freq);
    speaker_delay(n);
}

/* tone_burst @ 0F38:7169 (linear 0x164E9, 73 bytes)

   Play count random tones with frequencies from lo up to hi, each held for dur ticks */

static void near tone_burst(int lo, int hi, int dur, int count)
{
    int span;
    int i;

    span = hi - lo;
    for (i = 0; i < count; i++)
        play_tone((long)(random_int(span) + lo), dur);
}

/* sweep_down @ 0F38:71B2 (linear 0x16532, 50 bytes)

   Sweep a tone downwards from start while it is >= end, in steps of step */

static void near sweep_down(int start, int end, int step, int delay)
{
    int n;

    for (n = 0; start >= end; start -= step, n++)
        play_tone((long)start, delay);
}

/* sweep_up_down @ 0F38:71E4 (linear 0x16564, 174 bytes)

   Siren: reps times sweep the tone from centre - width up to centre + width and back down
   again, in steps of step, each tone held for dur */

static void near sweep_up_down(int centre, int width, int step, int dur, int reps)
{
    int j;
    int k;

    for (k = 0; k < reps; k++) {
        for (j = 0; j * step < width * 2; j++)
            play_tone((long)(centre - width + j * step), dur);
        for (j = 0; j * step < width * 2; j++)
            play_tone((long)(centre + width - j * step), dur);
    }
}

/* pulse_train @ 0F38:7292 (linear 0x16612, 116 bytes)

   Square wave on the speaker: for each of count tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */

static void near pulse_train(int a, int *p, int count, int reps)
{
    int i;
    int j;

    for (i = 0; i < count; i++) {
        for (j = 0; j < reps; j++) {
            outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_DATA);
            speaker_delay(*p);
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_DATA));
            speaker_delay(a - *p);
        }
        p++;
    }
}

/* pulse_train_back @ 0F38:7306 (linear 0x16686, 116 bytes)

   Same as pulse_train with the table walked backwards: square wave on the speaker: for each of count
   tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */

static void near pulse_train_back(int a, int *p, int count, int reps)
{
    int i;
    int j;

    for (i = 0; i < count; i++) {
        for (j = 0; j < reps; j++) {
            outp(PPI_B, inp(PPI_B) | PPI_SPEAKER_DATA);
            speaker_delay(*p);
            outp(PPI_B, inp(PPI_B) & (0xff ^ PPI_SPEAKER_DATA));
            speaker_delay(a - *p);
        }
        p--;
    }
}

/* fill_range @ 0F38:737A (linear 0x166FA, 115 bytes)

   Fill buf with the values from start to end in steps of step (up or down); returns how many */

static int near fill_range(int *buf, int start, int end, int step)
{
    int n;
    int count;

    if (start < end) {
        for (n = 0; start <= end; n++)
            start = (buf[n] = start) + step;
    } else {
        for (n = 0; start >= end; n++)
            start = (buf[n] = start) - step;
    }
    count = n;
    return count;
}

/* warble @ 0F38:73ED (linear 0x1676D, 119 bytes)

   Warble: set the tone to freq, then reps times play the square wave half made by
   fill_range (from 1 to hi in steps of step) forwards and backwards */

static void near warble(long freq, int period, int hi, int step, int cycles, int reps)
{
    int count;
    int half[30];
    int k;

    count = fill_range(half, 1, hi, step);
    sound_set_freq(freq);
    for (k = 0; k < reps; k++) {
        pulse_train(period, half, count, cycles);
        pulse_train_back(period, &half[count - 1], count, cycles);
    }
}

/* noise @ 0F38:7464 (linear 0x167E4, 76 bytes)

   Noise: speaker off, then count times flip bit 1 of port 61h and wait for a random time
   (random_int(range) | bits) */

static void near noise(int range, int bits, int count)
{
    int i;
    int delay;

    sound_off();
    for (i = 0; i < count; i++) {
        outp(PPI_B, inp(PPI_B) ^ PPI_SPEAKER_DATA);
        delay = random_int(range) | bits;
        speaker_delay(delay);
    }
}

/* noise_sweep_down @ 0F38:74B0 (linear 0x16830, 76 bytes)

   Sweep noise bursts downwards from start while it is > end, in steps of step */

static void near noise_sweep_down(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start - n * step > end; n++) {
        x = start - n * step;
        noise(x, x - gap, dur);
    }
}

/* noise_sweep_up @ 0F38:74FC (linear 0x1687C, 76 bytes)

   Sweep noise bursts upwards from start while it is < end, in steps of step */

static void near noise_sweep_up(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start + n * step < end; n++) {
        x = start + n * step;
        noise(x, x - gap, dur);
    }
}

/* sound_wheel_roll @ 0F38:7548 (linear 0x168C8, 43 bytes)

   Starts one of two sounds through music_play; `on` selects which. */

void sound_wheel_roll(int on)
{
    if (on)
        music_play(g_wheelRollSound, g_jingleRepeats);
    else
        music_play(g_song, g_songRepeats);
}

/* F_168F3 @ 0F38:7573 (linear 0x168F3, 80 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void F_168F3(int on)
{
    if (on) {
        music_play(g_stickerSound, g_jingleRepeats);
        music_set_vibrato(1, 0, 8, 10, 1);
    } else
        music_play(g_song, g_songRepeats);
}

/* F_16943 @ 0F38:75C3 (linear 0x16943, 80 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void F_16943(int on)
{
    if (on) {
        music_play(g_2f93_00b0, g_jingleRepeats);
        music_set_vibrato(1, 0, 8, 5, 1);
    } else
        music_play(g_song, g_songRepeats);
}

/* sound_pump @ 0F38:7613 (linear 0x16993, 78 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void sound_pump(int on)
{
    if (on) {
        music_play(g_pumpSound, g_jingleRepeats);
        music_set_vibrato(1, 1, 8, 30, 1);
    } else
        music_play(g_song, g_songRepeats);
}

/* sound_jack @ 0F38:7661 (linear 0x169E1, 78 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void sound_jack(int on)
{
    if (on) {
        music_play(g_jackSound, g_jingleRepeats);
        music_set_vibrato(1, 1, 8, 20, 1);
    } else
        music_play(g_song, g_songRepeats);
}

/* F_16A2F @ 0F38:76AF (linear 0x16A2F, 43 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void F_16A2F(int on)
{
    if (on)
        music_play(g_2f93_0050, g_jingleRepeats);
    else
        music_play(g_song, g_songRepeats);
}

/* F_16A5A @ 0F38:76DA (linear 0x16A5A, 78 bytes)

   Starts one of two sounds through music_play; `on` selects which */

void F_16A5A(int on)
{
    if (on) {
        music_play(g_partMoveSound, g_jingleRepeats);
        music_set_vibrato(1, 1, 8, 14, 1);
    } else
        music_play(g_song, g_songRepeats);
}

/* sound_siren @ 0F38:7728 (linear 0x16AA8, 25 bytes) */

void sound_siren(int on)
{
    if (on)
        g_sirenOn = 1;
    else
        g_sirenOn = 0;
}

/* sound_squeal @ 0F38:7741 (linear 0x16AC1, 25 bytes) */

void sound_squeal(int on)
{
    if (on)
        g_squealOn = 1;
    else
        g_squealOn = 0;
}

/* sound_warble1 @ 0F38:775A (linear 0x16ADA, 48 bytes) */

void sound_warble1(void)
{
    if (g_soundOn) {
        sound_on();
        warble(30000L, 300, 100, 6, 1, 2);
        sound_off();
    }
}

/* sound_warble2 @ 0F38:778A (linear 0x16B0A, 48 bytes) */

void sound_warble2(void)
{
    if (g_soundOn) {
        sound_on();
        warble(30000L, 200, 70, 5, 1, 3);
        sound_off();
    }
}

/* sound_buzz @ 0F38:77BA (linear 0x16B3A, 39 bytes)

   Sound effect, only when sound is on */

void sound_buzz(void)
{
    if (g_soundOn) {
        sound_on();
        sweep_up_down(3000, 10, 10, 60, 9);
        sound_off();
    }
}

/* F_16B61 @ 0F38:77E1 (linear 0x16B61, 54 bytes)

   Sound effect with a random start pitch */

void F_16B61(void)
{
    if (g_soundOn) {
        sound_on();
        sweep_up_down(random_int(100) + 6200, 40, 10, 1, 10);
        sound_off();
    }
}

/* F_16B97 @ 0F38:7817 (linear 0x16B97, 42 bytes)

   Sound effect */

void F_16B97(void)
{
    if (g_soundOn) {
        sound_on();
        sweep_up_down(5200, 40, 10, 1, 10);
        sound_off();
    }
}

/* F_16BC1 @ 0F38:7841 (linear 0x16BC1, 34 bytes)

   Sound effect */

void F_16BC1(void)
{
    if (g_soundOn) {
        sound_on();
        noise(200, 10, 60);
        sound_off();
    }
}

/* sound_crash @ 0F38:7863 (linear 0x16BE3, 172 bytes)

   Sound effect, three repeats of a noise burst, a click and a blip */

void sound_crash(void)
{
    register int i;
    int count;

    if (g_soundOn) {
        count = 3;
        for (i = count; i; i--) {
            sound_on();
            tone_burst(3000, 8000, 60, 10);
            noise_sweep_down(200, random_int(30) + 18, 15, 5, 2);
            noise(random_int(300) + 40, 2, 30);
            noise_sweep_up(16, random_int(100) + 50, 15, 5, 4);
        }
        sound_off();
    }
}

/* sound_bump @ 0F38:790F (linear 0x16C8F, 100 bytes)

   Sound effect, two noise bursts each followed by a click run */

void sound_bump(void)
{
    if (g_soundOn) {
        sound_on();
        tone_burst(2000, 8000, 100, 10);
        noise(120, 2, 30);
        sound_on();
        tone_burst(4000, 9000, 100, 10);
        noise(220, 2, 20);
        sound_off();
    }
}

/* F_16CF3 @ 0F38:7973 (linear 0x16CF3, 38 bytes)

   Sound effect, a falling sweep */

void F_16CF3(void)
{
    if (g_soundOn) {
        sound_on();
        sweep_down(130, 90, 4, 1000);
        sound_off();
    }
}
