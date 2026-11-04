#include <stdlib.h>
#include "STRUCTS.H"
#include "M_16EA2.H"
#include "CRTC.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_08EA8.H"
#include "M_0914C.H"
#include "M_0B3CA.H"
#include "M_0F38A.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_15396.H"
#include "M_174A6.H"
#include "M_193D4.H"
#include "M_265BA.H"
#include "M_2869E.H"

#pragma intrinsic(abs)

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Bitmap far *g8188w;

/* F_16EA2 @ 0F38:7B22 (linear 0x16EA2, 1540 bytes)

   Gearbox/drive screen loop: draws the dashboard, then redraws the changing parts until
   F_08EA8 reports an input. Returns that value. */

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

    sv = r_vid.f16;
    do {
        F_1013D(3);
        F_193D4();
        (*bl78be)();
        g_58e4 = r_vid.mode == -3 && g_0250;
        if (g_58e4) {
            F_11A72(0, 0x64, 0x140, 0x64, g755cu);
            r_vid.f16 = 1;
            F_1169A(0, 0, 0x140, 0xc8, 0);
            r_vid.f16 = sv;
        }
        if (r_vid.mode == -2)
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
            (*b_78a2)(pict, src, &blt, 1);
            F_19438(1);
        }
        w = blt.w = src->width;
        blt.h = 0x65;
        blt.sx = 0;
        blt.sy = blt.dx = blt.dy = 0;
        (*b_78a2)(src, g8188w, &blt, 1);
        if (r_vid.mode == -2)
            (*b_78a2)(src, g82b4s, &blt, 1);
        blt.w = src->width;
        blt.h = 0x5a;
        blt.sx = blt.dx = 0;
        blt.sy = 0x64;
        blt.dy = (r_vid.mode == -2 || g_58e4) ? 0 : 0x64;
        (*b_78a2)(src, g_58e4 ? g755cu : bm7678, &blt, 1);
        F_19438(1);
        F_28879();
        F_10677(e_78e0.f24, e_78e0.f1c);
        F_10872(g_58e4 ? g755cu : bm7678, &e_78e0);
        shiftx5 = 0;
        bp82c2 = 0x64;
        F_102D3();
        F_104B0(0, bp82c2, g_58e4 ? g755cu : bm7678);
        bp82c2 = 0;
        ret = 0;
        oldfe = e_78e0.fe;
        F_0F552(oldf05 = e_78e0.f0);
        if (!g_58e4)
            oldf05 = F_0F7A5(e_78e0.f0, oldf05, 0);
        if (!g_58e6) {
            F_0F6C9(oldfe);
            if (!g_58e4)
                oldfe = F_0F7A5(e_78e0.fe, oldfe, 1);
        } else if (!g_58e4)
            F_0F972();
    } while (g_58ee > 1);
    g_58ee = 0;
    if (r_vid.mode == -2) {
        crtc_set_split(0x63);
        af824ax = 0x64;
    } else
        af824ax = 0;
    F_28564();
    if (r_vid.mode != -2) {
        blt.w = 0x140;
        blt.h = 0x12;
        blt.dy = blt.sy = blt.sx = blt.dx = 0;
        (*b_78a2)(g8188w, bm7678, &blt, 1);
    }
    if (g_58e4) {
        g_58e4 = 0;
        blt.w = g755cu->width;
        blt.h = 0x64;
        blt.sy = blt.sx = blt.dx = 0;
        blt.dy = 0x64;
        (*b_78a2)(g755cu, bm7678, &blt, 1);
        oldf05 = F_0F7A5(e_78e0.f0, oldf05, 0);
        if (!g_58e6)
            oldfe = F_0F7A5(e_78e0.fe, oldfe, 1);
        else
            F_0F972();
        F_104B0(shiftx5, bp82c2, bm7678);
    }
    (*m_present)();
    for (;;) {
        F_2858D();
        if (g_0610 == 0x63) {
            g_0610--;
            F_00FEA();
        }
        if (ret = F_08EA8())
            break;
        shiftx5 = e_78e0.f4;
        if (e_78e0.f1a)
            F_10872(bm7678, &e_78e0);
        if (cur != ao8228u)
            cur = ao8228u;
        if (abs(e_78e0.f0 - oldf05) > 1)
            oldf05 = F_0F7A5(e_78e0.f0, oldf05, 0);
        if (!g_58e6 && abs(e_78e0.fe - oldfe) > 1)
            oldfe = F_0F7A5(e_78e0.fe, oldfe, 1);
        if (shiftx5 != bp82c2) {
            F_104B0(shiftx5, bp82c2, bm7678);
            bp82c2 = shiftx5;
        }
    }
    F_193D4();
    if (r_vid.mode == -2) {
        blt.w = w;
        blt.h = 0x64;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = g_0254 ? 0x64 : 0x65;
        (*b_78a2)(bm7678, g_5e3e == 1 ? g82b4s : g8188w, &blt, 1);
        blt.w = w;
        blt.h = 1;
        blt.sx = 0;
        blt.sy = 0;
        blt.dx = 0;
        blt.dy = 0x64;
        (*b_78a2)(bm7678, g_5e3e == 1 ? g82b4s : g8188w, &blt, 1);
        crtc_set_split(-1);
        af824ax = 0;
        (*dz78b6)(g_5e3e == 1 ? g82b4s->bits.p : g8188w->bits.p, bm7678->bits.p);
        (*dz78b6)(g_5e3e == 1 ? g82b4s->bits.p : g8188w->bits.p, g_5e3e == 1 ? g8188w->bits.p : g82b4s->bits.p);
    } else
        (*dz78b6)(bm7678->bits.p, g8188w->bits.p);
    F_1013D(1);
    return ret;
}
