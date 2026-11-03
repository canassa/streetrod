#include <time.h>
#include "GLOBALS.H"
#include "PROTOS.H"

/* Wichmann-Hill style generator: three 15-bit LCG state words.
   n <= 0 seeds from the clock and returns -1, n > 0 returns a value in 0..n-1. */


int random_int(int n)
{
    if (n <= 0) {
        long t;
        register int x;

        time(&t);
        x = g_rngC = g_rngB = g_rngA = (int)t % 30000 + 1;
        do {
            x = 171 * (x % 177) - 2 * (x / 177);
            if (x < 0)
                x += 30269;
        } while (x > 30000);
        g_rngB = x;
        x = g_rngC;
        do {
            x = 170 * (x % 178) - 63 * (x / 178);
            if (x < 0)
                x += 30323;
        } while (x > 30000);
        g_rngC = x;
        return -1;
    } else {
        int r;

        g_rngA = 171 * (g_rngA % 177) - 2 * (g_rngA / 177);
        if (g_rngA < 0)
            g_rngA += 30269;
        g_rngB = 172 * (g_rngB % 176) - 35 * (g_rngB / 176);
        if (g_rngB < 0)
            g_rngB += 30307;
        g_rngC = 170 * (g_rngC % 178) - 63 * (g_rngC / 178);
        if (g_rngC < 0)
            g_rngC += 30323;
        r = (g_rngA % n + g_rngB % n + g_rngC % n) % n;
        return r;
    }
}
