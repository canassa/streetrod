/* F_27198 @ 2645:0D48 (linear 0x27198, 2517 bytes)

   Builds the terrain flag arrays for a new track: start/finish area, hills,
   then the random features, and finally calls the track builder. */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_265BA(int n);

void F_27198(void)
{
    int cnt, i, j;

    if (g_vid.mode == -2) {
        g_7678 = &g_2fa3_0000;
        g_8188 = &g_5dac;
        g_82b4 = &g_5ddc;
    } else {
        g_7678 = g_vid.front;
        g_8188 = g_vid.back;
        g_82b4 = &g_screens[2];
    }
    cnt = g_82c6;
    for (i = 0; i < 0xb4; i++) {
        g_ter56f0[i] = 0;
        g_ter4570[i] = 0;
        g_ter4e30[i] = 0;
        g_ter3cb0[i] = 0;
        g_ter33f0[i] = 0;
        g_ter4e30[i] |= 1;
        if ((i + 1) % (cnt * 4 + 4) == 0)
            g_ter4e30[i] |= 8;
        if ((i + 2) % 16 == 0) {
            g_ter4e30[i + 2] |= 4;
            g_ter4e30[i + 1] |= 4;
            g_ter4e30[i] |= 4;
            g_ter3cb0[i - 1] |= 0x40;
            g_ter33f0[i + 3] |= 0x40;
            g_ter4e30[i + 1] |= 1;
            g_ter4e30[i + 2] |= 1;
            g_ter4e30[i + 3] |= 1;
            g_ter56f0[i + 1] = 0;
            g_ter4570[i + 1] = 0;
            g_ter56f0[i + 2] = 0;
            g_ter4570[i + 2] = 0;
            g_ter56f0[i + 3] = 0;
            g_ter4570[i + 3] = 0;
            i += 3;
        } else {
            if ((i + 4) % 16 == 0 || (i + 8) % 16 == 0 || (i + 12) % 16 == 0) {
                if (F_265BA(2) == 0) {
                    g_ter33f0[i - 1] |= 4;
                    g_ter33f0[i] |= 4;
                    if (F_265BA(3) == 0) {
                        g_ter33f0[i - 1] |= 8;
                        g_ter33f0[i] |= 8;
                    }
                    g_ter33f0[i] |= 0x10;
                } else {
                    g_ter33f0[i - 2] |= 4;
                    g_ter33f0[i] |= 4;
                    g_ter33f0[i - 1] |= 4;
                    if (F_265BA(3) == 0) {
                        g_ter33f0[i - 2] |= 8;
                        g_ter33f0[i] |= 8;
                        g_ter33f0[i - 1] |= 8;
                    }
                    g_ter33f0[i - 1] |= 0x10;
                }
            } else
                g_ter33f0[i] |= 0x80;
            if ((i + 5) % 16 == 0 || (i + 9) % 16 == 0 || (i + 12) % 16 == 0) {
                if (F_265BA(3) != 0) {
                    g_ter3cb0[i - 1] |= 4;
                    g_ter3cb0[i] |= 4;
                    if (F_265BA(2) == 0) {
                        g_ter3cb0[i - 1] |= 8;
                        g_ter3cb0[i] |= 8;
                    }
                    g_ter3cb0[i] |= 0x10;
                } else {
                    g_ter3cb0[i - 2] |= 4;
                    g_ter3cb0[i] |= 4;
                    g_ter3cb0[i - 1] |= 4;
                    if (F_265BA(4) == 0) {
                        g_ter3cb0[i - 2] |= 8;
                        g_ter3cb0[i] |= 8;
                        g_ter3cb0[i - 1] |= 8;
                    }
                    g_ter3cb0[i - 1] |= 0x10;
                }
            } else
                g_ter3cb0[i] |= 0x80;
            if (F_265BA(cnt * 40 + 6) == 0 && !(g_ter33f0[i] & 0x10))
                g_ter33f0[i] |= 0x20;
            if (F_265BA(cnt * 40 + 7) == 0 && !(g_ter3cb0[i] & 0x10))
                g_ter3cb0[i] |= 0x20;
            if (F_265BA(cnt * 40 + 3) == 0 && !(g_ter33f0[i] & 0x10))
                g_ter33f0[i] |= 0x100;
            if (F_265BA(cnt * 40 + 4) == 0 && !(g_ter3cb0[i] & 0x10))
                g_ter3cb0[i] |= 0x100;
        }
    }
    g_ter4e30[F_265BA(45) + 10] |= 0x4000;
    g_ter4e30[F_265BA(45) + 10] |= 0x8000;
    g_ter4e30[F_265BA(55) + 65] |= 0x4000;
    g_ter4e30[F_265BA(55) + 65] |= 0x8000;
    g_ter4e30[F_265BA(55) + 115] |= 0x4000;
    g_ter4e30[F_265BA(55) + 115] |= 0x8000;
    g_ter3cb0[44] |= 1;
    g_ter33f0[29] |= 0x1000;
    g_ter33f0[24] |= 0x800;
    g_ter33f0[108] |= 1;
    g_ter33f0[87] |= 0x1000;
    g_ter33f0[82] |= 0x800;
    g_ter33f0[156] |= 1;
    g_ter33f0[135] |= 0x1000;
    g_ter33f0[130] |= 0x800;
    for (i = 0xb4; i < 0x460; i++) {
        g_ter56f0[i] = 0;
        g_ter4570[i] = 0;
        g_ter4e30[i] = 0;
        g_ter33f0[i] = 0;
        g_ter3cb0[i] = 0;
        if ((i + 1) % (g_82c6 * 4 + 4) == 0)
            g_ter4e30[i] |= 8;
        if (i % (cnt * 4 + 4) == 0) {
            g_ter33f0[i] |= 1;
            g_ter3cb0[i] |= 1;
        }
        if ((i + 2) % (cnt * 8 + 8) == 0)
            g_ter4e30[i] |= 2;
        if (F_265BA(cnt * 200 + 7) == 0)
            g_ter33f0[i] |= 0x20;
        if (F_265BA(cnt * 200 + 9) == 0)
            g_ter3cb0[i] |= 0x20;
        if (cnt == 0) {
            if (F_265BA(35) == 0) {
                g_ter33f0[i] |= 0x40;
                g_ter33f0[i - 1] |= 0x40;
                g_ter33f0[i - 2] |= 0x40;
                g_ter33f0[i] &= ~0x20;
                g_ter33f0[i - 1] &= ~0x20;
                g_ter33f0[i - 2] &= ~0x20;
            }
            if (F_265BA(25) == 0) {
                g_ter3cb0[i] |= 0x40;
                g_ter3cb0[i - 2] |= 0x40;
                g_ter3cb0[i] &= ~0x20;
                g_ter3cb0[i - 2] &= ~0x20;
            }
        } else {
            if (F_265BA(cnt * 100 + 35) == 0) {
                g_ter33f0[i] |= 0x40;
                g_ter33f0[i] &= ~0x20;
            }
            if (F_265BA(cnt * 100 + 25) == 0) {
                g_ter3cb0[i] |= 0x40;
                g_ter3cb0[i] &= ~0x20;
            }
        }
        if (F_265BA(cnt * 100 + 7) == 0 && !(g_ter33f0[i] & 0x80))
            g_ter33f0[i] |= 0x80;
        if (F_265BA(cnt * 100 + 9) == 0 && !(g_ter3cb0[i] & 0x80))
            g_ter3cb0[i] |= 0x80;
        if (F_265BA(cnt * 100 + 21) == 0)
            g_ter33f0[i] |= 4;
        if (F_265BA(cnt * 100 + 25) == 0)
            g_ter3cb0[i] |= 4;
        if (F_265BA(cnt * 100 + 23) == 0 && !(g_ter33f0[i] & 4))
            g_ter33f0[i] |= 8;
        if (F_265BA(cnt * 100 + 30) == 0 && !(g_ter3cb0[i] & 4))
            g_ter3cb0[i] |= 8;
        if (F_265BA(cnt * 50 + 4) == 0) {
            g_ter33f0[i] |= 0x100;
            if (F_265BA(2) == 0)
                g_ter33f0[i] |= 0x200;
        }
        if (F_265BA(cnt * 50 + 4) == 0) {
            g_ter3cb0[i] |= 0x100;
            if (F_265BA(2) == 0)
                g_ter3cb0[i] |= 0x200;
        }
    }
    g_ter33f0[379] &= 0x1801;
    g_ter3cb0[379] &= 0x1801;
    g_ter4e30[379] |= 0x1010;
    g_ter33f0[1019] &= 0x1801;
    g_ter3cb0[1019] &= 0x1801;
    g_ter4e30[1019] |= 0x2010;
    {
        i = F_265BA(140) + 220;
        j = (g_ter3cb0[i] & 2) ? i - 1 : i;
        g_ter33f0[j] &= 0x1801;
        g_ter3cb0[j] &= 0x1801;
        g_ter4e30[j] |= 0x8000;
        i = F_265BA(600) + 390;
        j = (g_ter3cb0[i] & 2) ? i - 1 : i;
        g_ter33f0[j] &= 0x1801;
        g_ter3cb0[j] &= 0x1801;
        g_ter4e30[j] |= 0x8000;
    }
    i = F_265BA(600) + 390;
    j = (g_ter3cb0[i] & 2) ? i - 1 : i;
    g_ter33f0[j] &= 0x1801;
    g_ter3cb0[j] &= 0x1801;
    g_ter4e30[j] |= 0x4000;
    i = 220;
    do {
        for (j = 0; j < 17; j++)
            g_ter56f0[i++] = g_5c98[j];
        i += 20;
        for (j = 0; j < 17; j++)
            g_ter56f0[i++] = -g_5c98[j];
        i += 30;
    } while (i + 90 < 0x17c);
    g_ter4e30[385] |= 0x100;
    F_26FE1(100);
}
