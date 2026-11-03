#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdlib.h>

/* protect_check @ 0000:2FC8 (linear 0x02FC8, 403 bytes)

   The manual lookup copy protection: asks for the colour of the car key or car
   icon on a random manual page and returns 1 if the answer matches. */


int protect_check(void)
{
    int f;
    int e;
    int d;
    int c;
    char b[26];
    char a[50];

    d = random_int(0x33);
    f = d < 0x12;
    F_11421(-1, "", 0);
    g_vid.f16 = 0;
    F_119B8(8, 8, 0, 0);
    F_158DE(8);
    switch (f) {
    case 0:
        sprintf(b, "What`is`the`color`of");
        sprintf(a, "the`car`key`on`page`%d?", g_manual[d].page);
        c = 0x72;
        break;
    case 1:
        sprintf(b, "What`is`the`color`of");
        sprintf(a, "    `the`car`icon`on`page`%d?  ", g_manual[d].page);
        c = 0x55;
        break;
    }
    F_119D6(0x79, 0x3f, b);
    F_119D6(c, 0x48, a);
    F_120FE(-2);
    (*g_blit)(g_vid.front->bits.p, g_vid.back->bits.p);
    if (g_vid.mode == -2 || g_vid.mode == -6)
        F_11324(g_0692);
    else
        (*g_present)();
    g_8249 = 1;
    g_vid.f16 = 1;
    g_4bc8 = 1;
    e = F_14294(0x27);
    g_4bc8 = 0;
    F_11421(-1, "", 0);
    if (g_manual[d].color != e)
        return 0;
    return 1;
}

int F_0315B(void)
{
    char c01;
    char c1;
    char c21;
    char c3;
    int key;

    F_113A2(190);
    g_vid.f16 = 1;
    c01 = g_colors.c[0];
    c1 = g_colors.c[1];
    c3 = g_colors.c[3];
    c21 = g_8244;
    if (g_vid.mode != -2 && g_vid.mode != -6) {
        g_colors.c[0] = 3;
        g_colors.c[1] = 0;
        g_colors.c[3] = 2;
        g_8244 = 1;
    } else {
        g_colors.c[0] = 15;
        g_colors.c[1] = 1;
        g_colors.c[3] = 13;
        g_8244 = 3;
    }
    F_1169A(0, 0, 320, 190, g_colors.c[1]);
    F_13957(g_vid.back, 100, 50, 120, 90);
    F_13957(g_vid.front, 100, 50, 120, 90);
    F_13957(g_vid.back, 102, 52, 116, 86);
    F_13957(g_vid.front, 102, 52, 116, 86);
    g_colors.c[0] = c01;
    g_colors.c[1] = c1;
    g_colors.c[3] = c3;
    g_8244 = c21;
    F_120FE(-2);
    (*g_present)();
    key = F_14294(41);
    F_120FE(-4);
    switch (key) {
    case -2:
    case -1:
        if (g_4f4c) {
            F_15C4A(g_4c28, 1);
            g_4f4c = 0;
        }
        break;
    }
    return key;
}

/* entry_beats_score @ 0000:32D0 (linear 0x032D0, 36 bytes) */


static int near entry_beats_score(int i)
{
    return atoi(g_table[i].score) > g_score;
}
