/* F_068E8 @ 0000:68E8 (linear 0x068E8, 446 bytes) - rect + hotspot setup for the 5 button rects of panel n */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_068E8(int n, int x, int y)
{
    struct Panel *panel;
    struct Hot *hotspot;

    panel = &g_70e4[n];
    hotspot = &g_09f4[F_06827(11)];
    hotspot->x1 = panel->r[0].x + x;
    hotspot->y1 = panel->r[0].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[0].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[0].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(13)];
    hotspot->x1 = panel->r[1].x + x;
    hotspot->y1 = panel->r[1].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[1].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[1].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(9)];
    hotspot->x1 = panel->r[2].x + x;
    hotspot->y1 = panel->r[2].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[2].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[2].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(10)];
    hotspot->x1 = panel->r[3].x + x;
    hotspot->y1 = panel->r[3].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[3].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[3].h;
    hotspot->f0 = 11;
    hotspot = &g_09f4[F_06827(12)];
    hotspot->x1 = panel->r[4].x + x;
    hotspot->y1 = panel->r[4].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[4].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[4].h;
    hotspot->f0 = 11;
}
