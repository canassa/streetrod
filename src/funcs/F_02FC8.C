/* protect_check @ 0000:2FC8 (linear 0x02FC8, 403 bytes)

   The manual lookup copy protection: asks for the colour of the car key or car
   icon on a random manual page and returns 1 if the answer matches. */

#include <stdio.h>
#include "GLOBALS.H"
#include "PROTOS.H"

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
