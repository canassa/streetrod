#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>

static void near F_164CC(long freq, int n);
static void near F_164CC(long freq, int delay);
static void near F_16612(int a, int *p, int count, int reps);
static void near F_16686(int a, int *p, int count, int reps);
static int near F_166FA(int *buf, int start, int end, int step);
static void near F_167E4(int range, int bits, int count);

/* F_163BE: calibrate the speed of the machine: counts how often F_21962(n) can be called within
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

/* F_16455 @ 0F38:70D5: program PIT channel 2 for a tone of the given frequency */


#pragma intrinsic(outp)

void F_16455(long freq)
{
    union Word div;

    div.w = (int)(1193180L / freq);
    outp(0x42, div.b[0]);
    outp(0x42, div.b[1]);
}

/* F_16481 @ 0F38:7101: set up the PIT for sound (channel 2, mode 3) and program a tone (divisor from 25000) */


#pragma intrinsic(outp)

void F_16481(void)
{
    g_58e2 = F_163BE(10);
    outp(0x43, 0xb6);
    F_16455(25000L);
}

/* F_164A2: speaker on: program the tone for 25000 and set bits 0 and 1 of port 61h */


#pragma intrinsic(inp, outp)

void F_164A2(void)
{
    F_16455(25000L);
    outp(0x61, inp(0x61) | 3);
    g_47cf = 1;
}

/* F_164BD: speaker off: clear bits 0 and 1 of port 61h */


#pragma intrinsic(inp, outp)

void F_164BD(void)
{
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
}

/* F_164CC: play the tone of frequency freq for n ticks (F_21962) */


static void near F_164CC(long freq, int n)
{
    F_16455(freq);
    F_21962(n);
}

/* F_164E9: play count random tones with frequencies from lo up to hi, each held for dur ticks */



static void near F_164E9(int lo, int hi, int dur, int count)
{
    int span;
    int i;

    span = hi - lo;
    for (i = 0; i < count; i++)
        F_164CC((long)(random_int(span) + lo), dur);
}

/* F_16532 @ 0F38:71B2 (near): sweep a tone downwards from start while it is >= end, in steps of step */



static void near F_16532(int start, int end, int step, int delay)
{
    int n;

    for (n = 0; start >= end; start -= step, n++)
        F_164CC((long)start, delay);
}

/* F_16564: siren: reps times sweep the tone from centre - width up to centre + width and back down
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

/* F_16612: square wave on the speaker: for each of count tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */


#pragma intrinsic(inp, outp)

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

/* F_16686: same as F_16612 with the table walked backwards: square wave on the speaker: for each of count tones in table, flip bit 1 of port 61h
   with delays *p and a - *p, reps times */


#pragma intrinsic(inp, outp)

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

/* F_166FA: fill buf with the values from start to end in steps of step (up or down); returns how many */

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

/* F_1676D: warble: set the tone to freq, then reps times play the square wave half made by
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

/* F_167E4: noise: speaker off, then count times flip bit 1 of port 61h and wait for a random time
   (random_int(range) | bits) */


#pragma intrinsic(inp, outp)

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

/* F_16830: sweep noise bursts downwards from start while it is > end, in steps of step */



static void near F_16830(int start, int end, int gap, int step, int dur)
{
    int n;
    int x;

    for (n = 0; start - n * step > end; n++) {
        x = start - n * step;
        F_167E4(x, x - gap, dur);
    }
}

/* F_1687C: sweep noise bursts upwards from start while it is < end, in steps of step */



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

/* F_168F3: starts one of two sounds through F_1628A; `on` selects which */


void F_168F3(int on)
{
    if (on) {
        F_1628A(g_2f93_00a0, g_587e);
        F_1633D(1, 0, 8, 10, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16943: starts one of two sounds through F_1628A; `on` selects which */


void F_16943(int on)
{
    if (on) {
        F_1628A(g_2f93_00b0, g_587e);
        F_1633D(1, 0, 8, 5, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16993: starts one of two sounds through F_1628A; `on` selects which */


void F_16993(int on)
{
    if (on) {
        F_1628A(g_2f93_0000, g_587e);
        F_1633D(1, 1, 8, 30, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_169E1: starts one of two sounds through F_1628A; `on` selects which */


void F_169E1(int on)
{
    if (on) {
        F_1628A(g_2f93_002c, g_587e);
        F_1633D(1, 1, 8, 20, 1);
    } else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16A2F: starts one of two sounds through F_1628A; `on` selects which */


void F_16A2F(int on)
{
    if (on)
        F_1628A(g_2f93_0050, g_587e);
    else
        F_1628A(g_2e6b_0000, g_5858);
}

/* F_16A5A: starts one of two sounds through F_1628A; `on` selects which */


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
