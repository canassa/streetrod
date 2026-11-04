#include <conio.h>
#include "STRUCTS.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "SOUND.H"
#include "SYS.H"

#pragma intrinsic(inp, outp)

int g_57f6[3][8] = {{0, 1, 2, 1, 0, -1, -2, -1}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 0, 0, 0, 0, 0, 0}};   /* DS:57F6 */
struct Tone g_tones[5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 8, 10}, {0, 0, 4, 8, 1}, {0, 0, 6, 8, 1}, {0, 2, 5, 2, 1}};   /* DS:5826 */

/* music_play @ 0F38:6F0A (linear 0x1628A, 179 bytes)

   Start the sound song with its tone list tones. Song bs79aa's own song restarts the
   first voice (g_58d4 = 1); any other song goes into the second voice ao79c6t (g_58d4 = 2), the
   first voice's on/off state g_58d0 is saved in g_58d6 meanwhile */

void music_play(void far *song, void *tones)
{
    if (song == bs79aa.l8) {
        if (g_58d6 != -1) {
            g_58d0 = g_58d6;
            g_58d6 = -1;
        }
        if (g_58d0 == 0) {
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        }
        g_58d4 = 1;
        g_58d2 = &bs79aa;
        bn82c4 = g_tones + *bs79aa.f1a;
        t_72ec = 1;
    } else {
        if (g_58d6 == -1) {
            g_58d6 = g_58d0;
            g_58d0 = 1;
        }
        g_58d4 = 2;
        ao79c6t.cur = ao79c6t.lc = ao79c6t.l8 = song;
        ao79c6t.f16 = ao79c6t.f14 = tones;
        g_58d2 = &ao79c6t;
        sys_music_start(2, 0);
    }
}

/* music_set_vibrato @ 0F38:6FBD (linear 0x1633D, 47 bytes)

   If on, fill the first entry of the sound table; t_72ec = on either way */

void music_set_vibrato(int on, int a, int b, int c, int d)
{
    if (on) {
        g_tones[0].f0 = 0;
        g_tones[0].f1 = a;
        g_tones[0].f2 = c;
        g_tones[0].f3 = b;
        g_tones[0].f4 = d;
    }
    t_72ec = on;
}

/* music_stop @ 0F38:6FEC (linear 0x1636C, 21 bytes) */

void music_stop(void)
{
    t_72ec = 0;
    outp(0x61, inp(0x61) & 0xfc);
    sys_music_stop();
}

/* F_16381 @ 0F38:7001 (linear 0x16381, 61 bytes)

   Toggle between two states (calls sys_music_pause on entering, sys_music_resume on leaving) */

void F_16381(int on)
{
    if (on) {
        if (g_58d0) {
            sys_music_pause();
            g_58d0 = 0;
            g_58d8 = 1;
        } else
            g_58d8 = 0;
    } else {
        if (g_58d8) {
            g_58d0 = 1;
            sys_music_resume();
            g_58d8 = 0;
        }
    }
}
