#include "STRUCTS.H"
#include "MARKS.H"
#include "ARENA.H"
#include "MAIN.H"

/* ma_clear @ 0F38:A054 (linear 0x193D4, 42 bytes) */

void ma_clear(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit = 0;
    g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal;
    g_0294 = 0;
}

/* ma_release @ 0F38:A07E (linear 0x193FE, 32 bytes) */

void ma_release(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit;
    g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal;
}

/* ma_pop_low @ 0F38:A09E (linear 0x1941E, 26 bytes) */

void ma_pop_low(int n)
{
    if (n == 0)
        g_lowArenaMarker = 0;
    else
        g_lowArenaMarker -= n;
}

/* ma_pop_high @ 0F38:A0B8 (linear 0x19438, 66 bytes) */

void ma_pop_high(int n)
{
    if (n == 0)
        g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal;
    else {
        g_markerIndexHigh += n;
        if (g_markerIndexHigh == 60)
            g_offsetArenaMarksList[g_markerIndexHigh] = g_arenaTopVal;
        else
            g_offsetArenaMarksList[g_markerIndexHigh] = g_offsetArenaMarksList[g_markerIndexHigh + 1] - 1;
    }
}
