/* F_0FF70 @ 0F38:0BF0 (linear 0x0FF70, 370 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

void F_0FF70(void)
{
    int saved;

    saved = g_04c2;
    if (g_vid.mode == -2 || g_vid.mode == -6)
        (*g_78be)();
    g_04c2 = 200;
    if (g_vid.mode != -2 && g_vid.mode != -6)
        F_001F2(0);
    F_119B8(3, 0x28, 0x28, 0);
    F_158DE(3);
    (*g_present)();
    g_vid.f16 = 0;
    F_118D4(1, -1);
    F_158DE(1);
    F_0FED1(400);
    (*g_present)();
    if (g_vid.mode != -2 && g_vid.mode != -6)
        F_001F2(1);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_118D4(4, 0);
    F_158DE(4);
    F_0FED1(400);
    if (g_vid.mode == -2 || g_vid.mode == -6)
        F_11324(g_02d4);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    F_0FED1(400);
    g_04c2 = saved;
    F_1169A(0, 0, 320, 200, 0);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
}
