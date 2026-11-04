#include <time.h>
#include "RANDOM.H"

static struct RandomState g_random;

/* Wichmann-Hill style generator: three 15-bit LCG state words.
   n <= 0 seeds from the clock and returns -1, n > 0 returns a value in 0..n-1. */

int rn_int(int n)
{
    if (n <= 0) {
        long t;
        register int x;

        time(&t);
        x = g_random.c = g_random.b = g_random.a = (int)t % 30000 + 1;
        do {
            x = 171 * (x % 177) - 2 * (x / 177);
            if (x < 0)
                x += 30269;
        } while (x > 30000);
        g_random.b = x;
        x = g_random.c;
        do {
            x = 170 * (x % 178) - 63 * (x / 178);
            if (x < 0)
                x += 30323;
        } while (x > 30000);
        g_random.c = x;
        return -1;
    } else {
        int r;

        g_random.a = 171 * (g_random.a % 177) - 2 * (g_random.a / 177);
        if (g_random.a < 0)
            g_random.a += 30269;
        g_random.b = 172 * (g_random.b % 176) - 35 * (g_random.b / 176);
        if (g_random.b < 0)
            g_random.b += 30307;
        g_random.c = 170 * (g_random.c % 178) - 63 * (g_random.c / 178);
        if (g_random.c < 0)
            g_random.c += 30323;
        r = (g_random.a % n + g_random.b % n + g_random.c % n) % n;
        return r;
    }
}
