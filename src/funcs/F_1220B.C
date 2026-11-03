/* F_1220B: move the saved-background cursor strip to (x, y): restore what it covered, save and draw anew */

#include "GLOBALS.H"
#include "PROTOS.H"

void F_1220B(int x, int y)
{
    int a;
    int b;

    if (g_cursor.state) {
        if (g_cursor.clip.w > 0) {
            if (g_vid.mode == -2)
                F_24EFB(g_cursor.save->bits.p, g_cursor.clip.w >> 3, g_cursor.clip.h, g_vid.back->bits.p + 40 * g_cursor.clip.dy + (g_cursor.clip.dx >> 3), 0x18 >> g_vid.shift);
            else
                (*g_78a2)(g_cursor.save, g_vid.back, (struct Blt *)&g_cursor.clip, 1);
            g_cursor.clip.w = 0;
        } else {
            g_cursor.r.x1 = g_cursor.r.y1 = 1000;
            g_cursor.r.x2 = g_cursor.r.y2 = -1;
        }
        if (g_cursor.state < 0) {
            if (g_cursor.r.x2 < 0)
                return;
            if (g_cursor.state < -1)
                return;
            g_cursor.state = 1;
        } else {
            g_cursor.clip.dx = x & g_vid.mask;
            g_cursor.clip.w = 320 - g_cursor.clip.dx > 0x18 ? 0x18 : 320 - g_cursor.clip.dx;
            g_cursor.clip.h = 200 - y > 16 ? 16 : 200 - y;
            g_cursor.clip.dy = y;
            a = (x & ~g_vid.mask) * (0x18 >> g_vid.shift) + g_cursor.off;
            (*g_78b2)(g_cursor.shape + a, g_cursor.mask + a, g_cursor.save->bits.p, g_vid.back->bits.p, g_cursor.clip.dx >> g_vid.shift, y, g_cursor.clip.w >> g_vid.shift, g_cursor.clip.h);
        }
        a = x + 15;
        b = y + 15;
        if (g_vid.f16 && g_cursor.state == 1)
            F_1A92A(g_vid.back, g_vid.front, g_cursor.r.x1 > x ? x : g_cursor.r.x1, g_cursor.r.y1 > y ? y : g_cursor.r.y1, g_cursor.r.x2 < a ? a : g_cursor.r.x2, g_cursor.r.y2 < b ? b : g_cursor.r.y2);
        g_cursor.r.x1 = x;
        g_cursor.r.y1 = y;
        g_cursor.r.x2 = a;
        g_cursor.r.y2 = b;
    }
}
