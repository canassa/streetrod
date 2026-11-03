/* F_16EA2 @ 0F38:7B22 (linear 0x16EA2, 1540 bytes)

   Gearbox/drive screen loop: draws the dashboard, then redraws the changing parts until
   F_08EA8 reports an input. Returns that value. */

#include <stdlib.h>
#pragma intrinsic(abs)
#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

int F_16EA2(void)
{
    struct Bitmap far *pict;
    struct Blt blt;
    int w;
    struct Bitmap far *src;
    int sv;
    int oldf05;
    int cur;
    int oldfe;
    int shiftx5;
    int ret;

    sv = g_vid.f16;
    do {
        F_1013D(3);
        F_193D4();
        (*g_78be)();
        g_58e4 = g_vid.mode == -3 && g_0250;
        if (g_58e4) {
            F_11A72(0, 0x64, 0x140, 0x64, g_755c);
            g_vid.f16 = 1;
            F_1169A(0, 0, 0x140, 0xc8, 0);
            g_vid.f16 = sv;
        }
        if (g_vid.mode == -2)
            g_58ee = 1;
        else
            g_58ee = 0;
        src = F_15BBE(0x3f6, 2);
        if (g_58e6) {
            pict = F_15BBE(0x4d8, 2);
            blt.w = pict->width;
            blt.h = pict->height;
            blt.sx = blt.sy = 0;
            blt.dx = 0x78;
            blt.dy = 0x7a;
            (*g_78a2)(pict, src, &blt, 1);
            F_19438(1);
        }
        w = blt.w = src->width;
        blt.h = 0x65;
        blt.sx = 0;
        blt.sy = blt.dx = blt.dy = 0;
        (*g_78a2)(src, g_8188, &blt, 1);
        if (g_vid.mode == -2)
            (*g_78a2)(src, g_82b4, &blt, 1);
        blt.w = src->width;
        blt.h = 0x5a;
        blt.sx = blt.dx = 0;
        blt.sy = 0x64;
        blt.dy = (g_vid.mode == -2 || g_58e4) ? 0 : 0x64;
        (*g_78a2)(src, g_58e4 ? g_755c : g_7678, &blt, 1);
        F_19438(1);
        F_28879();
        F_10677(g_78e0.f24, g_78e0.f1c);
        F_10872(g_58e4 ? g_755c : g_7678, &g_78e0);
        shiftx5 = 0;
        g_82c2 = 0x64;
        F_102D3();
        F_104B0(0, g_82c2, g_58e4 ? g_755c : g_7678);
        g_82c2 = 0;
        ret = 0;
        oldfe = g_78e0.fe;
        F_0F552(oldf05 = g_78e0.f0);
        if (!g_58e4)
            oldf05 = F_0F7A5(g_78e0.f0, oldf05, 0);
        if (!g_58e6) {
            F_0F6C9(oldfe);
            if (!g_58e4)
                oldfe = F_0F7A5(g_78e0.fe, oldfe, 1);
        } else if (!g_58e4)
            F_0F972();
    } while (g_58ee > 1);
    g_58ee = 0;
    if (g_vid.mode == -2) {
        F_21A14(0x63);
        g_824a = 0x64;
    } else
        g_824a = 0;
    F_28564();
    if (g_vid.mode != -2) {
        blt.w = 0x140;
        blt.h = 0x12;
        blt.dy = blt.sy = blt.sx = blt.dx = 0;
        (*g_78a2)(g_8188, g_7678, &blt, 1);
    }
    if (g_58e4) {
        g_58e4 = 0;
        blt.w = g_755c->width;
        blt.h = 0x64;
        blt.sy = blt.sx = blt.dx = 0;
        blt.dy = 0x64;
        (*g_78a2)(g_755c, g_7678, &blt, 1);
        oldf05 = F_0F7A5(g_78e0.f0, oldf05, 0);
        if (!g_58e6)
            oldfe = F_0F7A5(g_78e0.fe, oldfe, 1);
        else
            F_0F972();
        F_104B0(shiftx5, g_82c2, g_7678);
    }
    (*g_present)();
    for (;;) {
        F_2858D();
        if (g_0610 == 0x63) {
            g_0610--;
            F_00FEA();
        }
        if (ret = F_08EA8())
            break;
        shiftx5 = g_78e0.f4;
        if (g_78e0.f1a)
            F_10872(g_7678, &g_78e0);
        if (cur != g_8228)
            cur = g_8228;
        if (abs(g_78e0.f0 - oldf05) > 1)
            oldf05 = F_0F7A5(g_78e0.f0, oldf05, 0);
        if (!g_58e6 && abs(g_78e0.fe - oldfe) > 1)
            oldfe = F_0F7A5(g_78e0.fe, oldfe, 1);
        if (shiftx5 != g_82c2) {
            F_104B0(shiftx5, g_82c2, g_7678);
            g_82c2 = shiftx5;
        }
    }
    F_193D4();
    if (g_vid.mode == -2) {
        blt.w = w;
        blt.h = 0x64;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = g_0254 ? 0x64 : 0x65;
        (*g_78a2)(g_7678, g_5e3e == 1 ? g_82b4 : g_8188, &blt, 1);
        blt.w = w;
        blt.h = 1;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = 0x64;
        (*g_78a2)(g_7678, g_5e3e == 1 ? g_82b4 : g_8188, &blt, 1);
        F_21A14(-1);
        g_824a = 0;
        (*g_78b6)(g_5e3e == 1 ? g_82b4->bits.p : g_8188->bits.p, g_7678->bits.p);
        (*g_78b6)(g_5e3e == 1 ? g_82b4->bits.p : g_8188->bits.p, g_5e3e == 1 ? g_8188->bits.p : g_82b4->bits.p);
    } else
        (*g_78b6)(g_7678->bits.p, g_8188->bits.p);
    F_1013D(1);
    return ret;
}
