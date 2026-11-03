/* F_104B0 @ 0F38:1130 (linear 0x104B0, 455 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_0F38A(int x, int y, int w, int h, int kind);
static void near F_0F418(int x, int y, int w, int h, int kind);

void F_104B0(int a, int b, struct Bitmap far *dst)
{
    struct Blt pkt1;
    int sel, x, y2;
    unsigned off;
    int ymin2;
    unsigned seg;
    int ymax;

    sel = (a != 0) + (a > 0);
    if (sel < 3 && sel != g_0350) {
        pkt1.w = 0xb8;
        pkt1.h = 0x1a;
        if (g_vid.mode != -2) {
            pkt1.sx = 0x88;
            pkt1.sy = g_6c22[sel];
        } else
            pkt1.sx = pkt1.sy = 0;
        pkt1.dx = 0x50;
        pkt1.dy = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + 0x3f;
        (*g_78a2)(g_6c28[sel], dst, &pkt1, 1);
        g_0350 = sel;
    }
    if (b != 100) {
        x = g_031e[b][0];
        y2 = (g_vid.mode == -2 ? 0 : 100) + g_031e[b][1];
        F_0F38A(x - 2, y2 - 2, 5, 5, 1);
    }
    x = g_031e[a][0];
    y2 = (g_vid.mode == -2 || g_58e4 ? 0 : 100) + g_031e[a][1];
    F_0F418(x - 2, y2 - 2, 5, 5, 1);
    if (g_58e4 == 0) {
        seg = g_787a;
        off = g_767c;
        g_787a = g_vid.mode == -2 ? 0xa000 : g_vid.mode == -4 ? 0xb000 : 0xb800;
        g_767c = 0;
        F_28840();
        if (g_vid.mode != -2) {
            ymax = g_clip_ymax;
            ymin2 = g_clip_ymin;
            g_clip_ymax = 190;
            g_clip_ymin = 100;
        }
        F_29F0F(x, y2);
        if (g_vid.mode != -2) {
            g_clip_ymax = ymax;
            g_clip_ymin = ymin2;
        }
        F_28860();
        g_787a = seg;
        g_767c = off;
    }
}
