/* F_09BC5 @ 0000:9BC5 (linear 0x09BC5, 424 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static int near F_09524(int);

static int near F_09BC5(int kind)
{
    int item1;
    unsigned v11, v2;
    int adj9, chm, scorel, row, p27, p18, c6, bonus2;
    unsigned v3;
    int clsl, col;

    if (kind == 0) {
        row = 0;
        col = 0;
    } else {
        if (kind == 5) {
            row = 2;
            col = 3;
        } else if (kind == 1 || kind == 2) {
            row = 1;
            col = 1;
        } else if (kind == 3 || kind == 4) {
            row = 1;
            col = 2;
        }
        goto calc;
    }
calc:
    chm = g_7d84->f0;
    p18 = F_09524(g_7648);
    p27 = F_09524(g_7eb0->f2);
    clsl = (g_7d7a->f4 >> 8) & 7;
    item1 = g_4806[g_7eb0->rack.s[0]->idx].f2;
    v11 = g_8bae[chm][row];
    v2 = g_7d68[p18][clsl];
    v3 = g_7d68[p27][item1];
    if (v11 + v2 < v3)
        c6 = 0;
    else if (v2 > v3)
        c6 = 2;
    else
        c6 = 1;
    scorel = g_8dd8[col][c6];
    if ((g_7eb0->f24 & 0x1f00) == 0)
        bonus2 = 0;
    else if (g_7eb0->f24 & 0x100)
        bonus2 = 4;
    else
        bonus2 = 6;
    adj9 = (2 * g_7d84->fd - g_7d84->fc) * scorel / 4;
    scorel += adj9 + bonus2;
    if (g_0610)
        scorel += (scorel >> 2) + 10;
    return random_int(100) <= scorel;
}
