#include "STRUCTS.H"
#include "MARKS.H"
#include "ARENA.H"
#include "MAIN.H"


void ma_clear(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit = 0;
    g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal;
    g_stickerBg = 0;
}


void ma_release(void)
{
    g_offsetArenaMarksList[g_lowArenaMarker = 0] = g_loHeapLimit;
    g_offsetArenaMarksList[g_markerIndexHigh = 60] = g_arenaTopVal;
}


void ma_pop_low(int n)
{
    if (n == 0)
        g_lowArenaMarker = 0;
    else
        g_lowArenaMarker -= n;
}


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
