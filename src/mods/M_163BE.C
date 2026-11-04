#include <conio.h>
#include "STRUCTS.H"
#include "M_163BE.H"
#include "EFFECTS.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_15236.H"
#include "M_1628A.H"
#include "M_174A6.H"
#include "M_21962.H"
#include "SONG.H"

static void near F_164CC(long freq, int n);
static void near F_16612(int a, int *p, int count, int reps);
static void near F_16686(int a, int *p, int count, int reps);
static int near F_166FA(int *buf, int start, int end, int step);
static void near F_167E4(int range, int bits, int count);
static void near F_1676D(long freq, int period, int hi, int step, int cycles, int reps);
static void near F_16564(int a, int b, int c, int d, int e);
static void near F_164E9(int a, int b, int c, int d);
static void near F_16830(int a, int b, int c, int d, int e);
static void near F_1687C(int a, int b, int c, int d, int e);
static void near F_16532(int start, int end, int step, int delay);

int g_5858[19] = {2, 1, 0, 2, 2, 0, 2, 1, 0, 1, 0, 0, 2, 2, 0, 2, 1, 0, -1};   /* DS:5858 */
char g_587e[4] = {0, 0, -1, -1};   /* DS:587E */
int g_5882[37] = {1, 2, 1, 2, 3, 3, 1, 2, 1, 3, 1, 4, 4, 1, 2, 1, 3, 1, 2, 1, 3, 4, 4, 1, 2, 1, 3, 4, 1, 1, 1, 2, 1, 4, 3, 3, -1};   /* DS:5882 */
int g_58cc[2] = {0, -1};   /* DS:58CC */
int g_58d0 = 1;   /* DS:58D0 */
struct Dev *g_58d2 = &bs79aa;   /* DS:58D2 */
int g_58d4 = 1;   /* DS:58D4 */
int g_58d6 = -1;   /* DS:58D6 */
int g_58d8 = 0;   /* DS:58D8 */
int g_58da = 0;   /* DS:58DA */
int g_58dc = 0;   /* DS:58DC */

/* F_163BE @ 0F38:703E (linear 0x163BE, 151 bytes)

   Calibrate the speed of the machine: counts how often F_21962(n) can be called within
   one BIOS timer tick (the long at 0040:006C). Counts above 848 are scaled by 3/4. Also sets
   g_58e2 to 256 on the way */

int F_163BE(int n)
{
    long tick;
    int count;

    count = 0;
    g_58e2 = 0x100;
    tick = *(long far *)0x46cL;
    while (tick == *(long far *)0x46cL)
        ;
    tick = *(long far *)0x46cL;
    while (tick == *(long far *)0x46cL) {
        F_21962(n);
        count++;
    }
    if (count > 0x350)
        count = 3 * count / 4;
    return count;
}

/* F_16455 @ 0F38:70D5 (linear 0x16455, 44 bytes)

   Program PIT channel 2 for a tone of the given frequency */

void F_16455(long freq)
{
    union Word div;

    div.w = (int)(1193180L / freq);
    outp(0x42, div.b[0]);
    outp(0x42, div.b[1]);
}

/* F_16481 @ 0F38:7101 (linear 0x16481, 33 bytes)

   Set up the PIT for sound (channel 2, mode 3) and program a tone (divisor from 25000) */

void F_16481(void)
{
    g_58e2 = F_163BE(10);
    outp(0x43, 0xb6);
    F_16455(25000L);
}

/* F_164A2 @ 0F38:7122 (linear 0x164A2, 27 bytes)

   Speaker on: program the tone for 25000 and set bits 0 and 1 of port 61h */

void F_164A2(void)
{
    F_16455(25000L);
    outp(0x61, inp(0x61) | 3);
    g_47cf = 1;
}

/* F_164BD @ 0F38:713D (linear 0x164BD, 15 bytes)

   Speaker off: clear bits 0 and 1 of port 61h */

void F_164BD(void)
{
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
}

/* F_164CC @ 0F38:714C (linear 0x164CC, 29 bytes)

   Play the tone of frequency freq for n ticks (F_21962) */

static void near F_164CC(long freq, int n)
{
    F_16455(freq);
    F_21962(n);
}

/* F_164E9 @ 0F38:7169 (linear 0x164E9, 73 bytes)

   Play count random tones with frequencies from lo up to hi, each held for dur ticks */

static void near F_164E9(int lo, int hi, int dur, int count)
{
    int span;
    int i;

    span = hi - lo;
    for (i = 0; i < count; i++)
        F_164CC((long)(random_int(span) + lo), dur);
}

/* F_16532 @ 0F38:71B2 (linear 0x16532, 50 bytes)

   Sweep a tone downwards from start while it is >= end, in steps of step */

static void near F_16532(int start, int end, int step, int delay)
{
    int n;

    for (n = 0; start >= end; start -= step, n++)
        F_164CC((long)start, delay);
}

/* F_16564 @ 0F38:71E4 (linear 0x16564, 174 bytes)

   Siren: reps times sweep the tone from centre - width up to centre + width and back down
   again, in steps of step, each tone held for dur */

static void near F_16564(int centre, int width, int step, int dur, int reps)
{
    int j;
    int k;

    for (k = 0; k < reps; k++) {
        for (j = 0; j * step < width * 2; j++)
            F_164CC((long)(centre - width + j * step), dur);
        for (j = 0; j * step < width * 2; j++)
            F_164CC((long)(centre + width - j * step), dur);
    }
}

/* F_16612 @ 0F38:7292 (linear 0x16612, 116 bytes)

   Square wave on the speaker: for each of count tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */

static void near F_16612(int a, int *p, int count, int reps)
{
    int i;
    int j;

    for (i = 0; i < count; i++) {
        for (j = 0; j < reps; j++) {
            outp(0x61, inp(0x61) | 2);
            F_21962(*p);
            outp(0x61, inp(0x61) & 0xfd);
            F_21962(a - *p);
        }
        p++;
    }
}

/* F_16686 @ 0F38:7306 (linear 0x16686, 116 bytes)

   Same as F_16612 with the table walked backwards: square wave on the speaker: for each of count
   tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */

static void near F_16686(int a, int *p, int count, int reps)
{
    int i;
    int j;

    for (i = 0; i < count; i++) {
        for (j = 0; j < reps; j++) {
            outp(0x61, inp(0x61) | 2);
            F_21962(*p);
            outp(0x61, inp(0x61) & 0xfd);
            F_21962(a - *p);
        }
        p--;
    }
}

/* F_166FA @ 0F38:737A (linear 0x166FA, 115 bytes)

   Fill buf with the values from start to end in steps of step (up or down); returns how many */

static int near F_166FA(int *buf, int start, int end, int step)
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

/* F_1676D @ 0F38:73ED (linear 0x1676D, 119 bytes)

   Warble: set the tone to freq, then reps times play the square wave half made by
   F_166FA (from 1 to hi in steps of step) forwards and backwards */

static void near F_1676D(long freq, int period, int hi, int step, int cycles, int reps)
{
    int count;
    int half[30];
    int k;

    count = F_166FA(half, 1, hi, step);
    F_16455(freq);
    for (k = 0; k < reps; k++) {
        F_16612(period, half, count, cycles);
        F_16686(period, &half[count - 1], count, cycles);
    }
}

/* F_167E4 @ 0F38:7464 (linear 0x167E4, 76 bytes)

   Noise: speaker off, then count times flip bit 1 of port 61h and wait for a random time
   (random_int(range) | bits) */

static void near F_167E4(int range, int bits, int count)
{
    int i;
    int delay;

    F_164BD();
    for (i = 0; i < count; i++) {
        outp(0x61, inp(0x61) ^ 2);
        delay = random_int(range) | bits;
        F_21962(delay);
    }
}

/* F_16830 @ 0F38:74B0 (linear 0x16830, 76 bytes)

   Sweep noise bursts downwards from start while it is > end, in steps of step */

static void near F_16830(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start - n * step > end; n++) {
        x = start - n * step;
        F_167E4(x, x - gap, dur);
    }
}

/* F_1687C @ 0F38:74FC (linear 0x1687C, 76 bytes)

   Sweep noise bursts upwards from start while it is < end, in steps of step */

static void near F_1687C(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start + n * step < end; n++) {
        x = start + n * step;
        F_167E4(x, x - gap, dur);
    }
}

/* F_168C8 @ 0F38:7548 (linear 0x168C8, 43 bytes)

   Starts one of two sounds through F_1628A; `on` selects which. */

void F_168C8(int on)
{
    if (on)
        F_1628A(g_2f93_00c0, g_587e);
    else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_168F3 @ 0F38:7573 (linear 0x168F3, 80 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_168F3(int on)
{
    if (on) {
        F_1628A(g_2f93_00a0, g_587e);
        F_1633D(1, 0, 8, 10, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16943 @ 0F38:75C3 (linear 0x16943, 80 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_16943(int on)
{
    if (on) {
        F_1628A(g_2f93_00b0, g_587e);
        F_1633D(1, 0, 8, 5, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16993 @ 0F38:7613 (linear 0x16993, 78 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_16993(int on)
{
    if (on) {
        F_1628A(g_2f93_0000, g_587e);
        F_1633D(1, 1, 8, 30, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_169E1 @ 0F38:7661 (linear 0x169E1, 78 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_169E1(int on)
{
    if (on) {
        F_1628A(g_2f93_002c, g_587e);
        F_1633D(1, 1, 8, 20, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16A2F @ 0F38:76AF (linear 0x16A2F, 43 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_16A2F(int on)
{
    if (on)
        F_1628A(g_2f93_0050, g_587e);
    else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16A5A @ 0F38:76DA (linear 0x16A5A, 78 bytes)

   Starts one of two sounds through F_1628A; `on` selects which */

void F_16A5A(int on)
{
    if (on) {
        F_1628A(g_2f93_007c, g_587e);
        F_1633D(1, 1, 8, 14, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16AA8 @ 0F38:7728 (linear 0x16AA8, 25 bytes) */

void F_16AA8(int on)
{
    if (on)
        g_58dc = 1;
    else
        g_58dc = 0;
}

/* F_16AC1 @ 0F38:7741 (linear 0x16AC1, 25 bytes) */

void F_16AC1(int on)
{
    if (on)
        g_58da = 1;
    else
        g_58da = 0;
}

/* F_16ADA @ 0F38:775A (linear 0x16ADA, 48 bytes) */

void F_16ADA(void)
{
    if (g_47ce) {
        F_164A2();
        F_1676D(30000L, 300, 100, 6, 1, 2);
        F_164BD();
    }
}

/* F_16B0A @ 0F38:778A (linear 0x16B0A, 48 bytes) */

void F_16B0A(void)
{
    if (g_47ce) {
        F_164A2();
        F_1676D(30000L, 200, 70, 5, 1, 3);
        F_164BD();
    }
}

/* F_16B3A @ 0F38:77BA (linear 0x16B3A, 39 bytes)

   Sound effect, only when sound is on */

void F_16B3A(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(3000, 10, 10, 60, 9);
        F_164BD();
    }
}

/* F_16B61 @ 0F38:77E1 (linear 0x16B61, 54 bytes)

   Sound effect with a random start pitch */

void F_16B61(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(random_int(100) + 6200, 40, 10, 1, 10);
        F_164BD();
    }
}

/* F_16B97 @ 0F38:7817 (linear 0x16B97, 42 bytes)

   Sound effect */

void F_16B97(void)
{
    if (g_47ce) {
        F_164A2();
        F_16564(5200, 40, 10, 1, 10);
        F_164BD();
    }
}

/* F_16BC1 @ 0F38:7841 (linear 0x16BC1, 34 bytes)

   Sound effect */

void F_16BC1(void)
{
    if (g_47ce) {
        F_164A2();
        F_167E4(200, 10, 60);
        F_164BD();
    }
}

/* F_16BE3 @ 0F38:7863 (linear 0x16BE3, 172 bytes)

   Sound effect, three repeats of a noise burst, a click and a blip */

void F_16BE3(void)
{
    register int i;
    int count;

    if (g_47ce) {
        count = 3;
        for (i = count; i; i--) {
            F_164A2();
            F_164E9(3000, 8000, 60, 10);
            F_16830(200, random_int(30) + 18, 15, 5, 2);
            F_167E4(random_int(300) + 40, 2, 30);
            F_1687C(16, random_int(100) + 50, 15, 5, 4);
        }
        F_164BD();
    }
}

/* F_16C8F @ 0F38:790F (linear 0x16C8F, 100 bytes)

   Sound effect, two noise bursts each followed by a click run */

void F_16C8F(void)
{
    if (g_47ce) {
        F_164A2();
        F_164E9(2000, 8000, 100, 10);
        F_167E4(120, 2, 30);
        F_164A2();
        F_164E9(4000, 9000, 100, 10);
        F_167E4(220, 2, 20);
        F_164BD();
    }
}

/* F_16CF3 @ 0F38:7973 (linear 0x16CF3, 38 bytes)

   Sound effect, a falling sweep */

void F_16CF3(void)
{
    if (g_47ce) {
        F_164A2();
        F_16532(130, 90, 4, 1000);
        F_164BD();
    }
}
