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
        x = g_random.state30323 = g_random.state30307 = g_random.state30269 = (int)t % 30000 + 1;
        do {
            x = 171 * (x % 177) - 2 * (x / 177);
            if (x < 0)
                x += 30269;
        } while (x > 30000);
        g_random.state30307 = x;
        x = g_random.state30323;
        do {
            x = 170 * (x % 178) - 63 * (x / 178);
            if (x < 0)
                x += 30323;
        } while (x > 30000);
        g_random.state30323 = x;
        return -1;
    } else {
        int r;

        g_random.state30269 = 171 * (g_random.state30269 % 177) - 2 * (g_random.state30269 / 177);
        if (g_random.state30269 < 0)
            g_random.state30269 += 30269;
        g_random.state30307 = 172 * (g_random.state30307 % 176) - 35 * (g_random.state30307 / 176);
        if (g_random.state30307 < 0)
            g_random.state30307 += 30307;
        g_random.state30323 = 170 * (g_random.state30323 % 178) - 63 * (g_random.state30323 / 178);
        if (g_random.state30323 < 0)
            g_random.state30323 += 30323;
        r = (g_random.state30269 % n + g_random.state30307 % n + g_random.state30323 % n) % n;
        return r;
    }
}
