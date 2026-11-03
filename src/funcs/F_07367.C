/* F_07367 @ 0000:7367 (linear 0x07367, 832 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_07367(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *bm3;
    struct Pos6 *q2;
    struct Bitmap far *bm4;
    struct Blt B1;
    struct Blt A2;
    struct Blt C;
    int x;
    int y;
    int idx6;
    int k;

    if (!g_7eb0)
        F_03AB8(0x0efa);
    else if (g_7eaa < 5L)
        F_03AB8(0x1975);
    else {
        g_vid.f16 = 1;
        x = 24;
        y = 11;
        bm1 = F_1934E(264, 69, 0, 2);
        F_11A72(x, y, 264, 69, bm1);
        F_119B8(1200, x, y, -1);
        F_13DE6(35);
        while ((k = F_01417(3000)) == 0)
            ;
        switch (k) {
        case -38: case -37: case -36: case -35:
        case -34: case -33: case -32: case -31:
            F_120FE(-3);
            F_13F21(1);
            F_11BD3(bm1, x, y);
            F_19438(1);
            idx6 = -30 - k;
            g_7eb0->f24 = ((idx6 & 0x1f) << 8) | (g_7eb0->f24 & 0xe0ff);
            bm3 = F_15BBE(1218, 2);
            bm1 = F_1934E(g_0294->width, g_0294->height, 0, 2);
            F_19ED0(g_0294, bm1, 1);
            q2 = &g_7844[idx6];
            F_1A734(bm1, q2->id, (char)(q2->dx + g_8276), (char)q2->dy, 0);
            bm4 = F_1934E(bm1->width, bm1->height, 0, 2);
            F_19ED0(bm1, bm4, 1);
            A2.sx = B1.sx = C.sx = 0;
            A2.sy = B1.sy = C.sy = 0;
            A2.dy = B1.dy = 0;
            A2.dx = B1.dx = 0;
            C.dx = g_8272;
            C.dy = g_8274;
            B1.h = B1.fc = A2.h = C.h = bm1->height;
            A2.w = B1.w = C.w = 8;
            F_168F3(1);
            for (k = 0; k < bm1->width; k += 8) {
                (*g_78a2)(bm1, bm4, &A2, 1);
                if (bm1->width - 8 > k)
                    (*g_78aa)(bm3, bm4, &B1, 1);
                F_10AFD(2);
                (*g_78a2)(bm4, g_vid.front, &C, 1);
                A2.w += 8;
                C.w += 8;
                B1.dx += 8;
            }
            F_11BD3(bm1, g_8272, g_8274);
            F_19438(3);
            F_168F3(0);
            g_05fc += 0x222L;
            F_11540(-5);
            F_120FE(-1);
            break;
        case -22:
            F_13F21(1);
            F_11BD3(bm1, x, y);
            F_19438(1);
            break;
        }
    }
}
