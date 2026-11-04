#include "STRUCTS.H"
#include "M_0914C.H"
#include "HOTSPOT.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02B9A.H"
#include "M_039C0.H"
#include "M_06476.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11A72.H"
#include "M_139DE.H"
#include "M_15236.H"
#include "M_15396.H"
#include "M_193D4.H"

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
int as79a2z;
int d_79a4;

static void near F_09A27(int, int, int);
static void near F_092A4(int, int, int, int, int, int *);
static void near F_09A83(int *, int *, int *);
static int near F_09524(int);
static int near F_09556(void);
static int near F_09BC5(int kind);
static void near F_0D031(void);
static struct Car * near F_04036(int idx, int weight, struct Car *old);
static void near F_081AB(struct Car *c);

int g_51fc[2][5] = {{4779, 4803, 4822, 4840, 4663}, {4663, 4675, 4694, 4715, 4727}};   /* DS:51FC */
int g_5210[2][3] = {{4859, 4867, 4875}, {4743, 4756, 4767}};   /* DS:5210 */

/* F_09AF4 @ 0000:9AF4 (linear 0x09AF4, 209 bytes) */



static int near F_09AF4(int n)
{
    int list[24], sel[24], kinds[24], choice;

    F_11421(-1, "", 0);
    F_09A83(list, sel, kinds);
    choice = F_142CF(0x24, 1, kinds, list, 0, 0);
    switch (choice) {
    case -4:
        return 0;
    case -3:
        if (bm7f86) {
            F_11421(-1, "", 0);
            F_092A4(2, bn7648, 1, 0xa3, 0, g_5030[m_8272.m827e]);
        }
        choice = F_14C8B();
        F_09A27(sel[choice - 1], n, 0);
        return 1;
    case 0x3ea:
        F_03AB8(0x198c);
        return 0;
    }
}

/* F_09BC5 @ 0000:9BC5 (linear 0x09BC5, 424 bytes) */



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
    chm = g7d84o->f0;
    p18 = F_09524(bn7648);
    p27 = F_09524(a_7eaap.m7eb0->f2);
    clsl = (ar7d7au->f4 >> 8) & 7;
    item1 = g_4806[a_7eaap.m7eb0->rack.s[0]->idx].f2;
    v11 = ag8baey[chm][row];
    v2 = g7d68m[p18][clsl];
    v3 = g7d68m[p27][item1];
    if (v11 + v2 < v3)
        c6 = 0;
    else if (v2 > v3)
        c6 = 2;
    else
        c6 = 1;
    scorel = h_8dd8[col][c6];
    if ((a_7eaap.m7eb0->f24 & 0x1f00) == 0)
        bonus2 = 0;
    else if (a_7eaap.m7eb0->f24 & 0x100)
        bonus2 = 4;
    else
        bonus2 = 6;
    adj9 = (2 * g7d84o->fd - g7d84o->fc) * scorel / 4;
    scorel += adj9 + bonus2;
    if (g_0610)
        scorel += (scorel >> 2) + 10;
    return random_int(100) <= scorel;
}

/* F_09D6D @ 0000:9D6D (linear 0x09D6D, 907 bytes) */



static int near F_09D6D(int arg)
{
    int num;
    struct Bitmap far *pict;
    unsigned price;
    int kind;
    int x;
    int ch;
    int step;
    int sy;
    int prev;

    F_193FE();
    if (arg) {
        ch = F_09556();
        switch (ch) {
        case 2:
            F_03AB8(0x1113);
            break;
        case 3:
            F_03AB8(0x113d);
            break;
        case 4:
            F_03AB8(0x117d);
            break;
        case 5:
            F_03AB8(0x11a7);
            break;
        case 0:
            if (g_4976 == 0) {
                if (a_7eaap.m7eb0->f22 < 22) {
                    F_03AB8(0x1761);
                    return 0;
                }
                a_7eaap.m7ec0 = 0;
                F_03AB8(0x11d1);
                as79a2z = 0;
            } else {
                a_7eaap.m7ec0 = 1;
                F_03AB8(0x11f7);
                as79a2z = 5;
            }
            d_79a4 = 0;
            F_0D031();
            g_05fc += a_7eaap.m7ec0 == 0 ? 0x444u : 0x777u;
            return 2;
        }
        F_193FE();
        return 0;
    }
    if (a_7eaap.m7eb0->f22 < 7) {
        F_03AB8(0x1761);
        return 0;
    }
    pict = F_14090(0x12, &x, &sy, 1);
    F_13DE6(0x16);
    g_05cc = g_tick + 800;
    while ((ch = F_01417(3000)) == 0)
        ;
    switch (ch) {
    case 1:
        num = 0x12;
        a_7eaap.m7ec0 = 0;
        break;
    case 2:
        num = 0x13;
        a_7eaap.m7ec0 = 1;
        break;
    case -23:
    case 1000:
        F_03AB8(0x122c);
        goto quit;
    }
    if (a_7eaap.m7ec0 == 1 && a_7eaap.m7eb0->f22 < 17) {
        F_03AB8(0x1761);
        goto quit;
    }
    F_13F21(1);
    prev = -1;
    F_13DE6(num);
    for (step = 0; step < 2; step++) {
        g_05cc = g_tick + 800;
        while ((ch = F_01417(3000)) == 0)
            ;
        m_8272.m8278 = ch;
        switch (ch) {
        case -23:
        case 1000:
            F_03AB8(0x122c);
            goto quit;
        case 1:
            price = 0;
            kind = 0;
            break;
        case 2:
            price = 10;
            kind = 1;
            break;
        case 3:
            price = 50;
            kind = 2;
            break;
        case 7:
            price = 25;
            kind = 3;
            break;
        case 8:
            price = 100;
            kind = 4;
            break;
        case 9:
            price = 0;
            kind = 5;
            break;
        }
        if ((long)price <= a_7eaap.m7eaa) {
            if (F_09BC5(kind)) {
                as79a2z = kind;
                d_79a4 = price;
                F_13F21(1);
                F_11BD3(pict, x, sy);
                pict = 0;
                F_03AB8(g_5210[a_7eaap.m7ec0][random_int(3)]);
                F_0D031();
                g_05fc += a_7eaap.m7ec0 == 0 ? 0x444u : 0x777u;
                return 2;
            }
            while ((ch = random_int(5)) == prev)
                ;
            F_03AB8(g_51fc[a_7eaap.m7ec0][prev = ch]);
        } else
            F_03AB8(0x131b);
    }
quit:
    F_13F21(1);
    F_11BD3(pict, x, sy);
    pict = 0;
    F_193FE();
    return 0;
}

/* F_0A0F8 @ 0000:A0F8 (linear 0x0A0F8, 365 bytes) */


static void near F_0A0F8(int a, int b)
{
    struct Hot save;
    struct Hot *hot;
    int y1, x1, y, x;

    hot = g_20aa;
    if (g_20aa[4].f6 != 4) {
        save = g_20aa[3];
        g_20aa[3] = g_20aa[4];
        g_20aa[4] = save;
    }
    hot[4].x1 = 0xc2;
    hot[4].x2 = 0xd8;
    hot[4].y1 = 0x49;
    hot[4].y2 = 0x77;
    hot[2].f0 = (b == 1) ? 11 : 14;
    if (a == 0) {
        hot[1].f0 = 14;
        hot[3].f0 = 14;
    } else {
        x1 = ai70e4t[bn7648].r[1].x + ac825et;
        y1 = ai70e4t[bn7648].r[1].y + aw8260y;
        x = ai70e4t[bn7648].r[1].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].f0 = 11;
        x1 = ai70e4t[bn7648].r[3].x + ac825et;
        y1 = ai70e4t[bn7648].r[3].y + aw8260y;
        x = ai70e4t[bn7648].r[3].w + x1 - 1;
        y = ai70e4t[bn7648].r[3].h + y1 - 1;
        if (hot[1].x2 >= x1)
            x1 = hot[1].x2 + 1;
        hot[3].x1 = x1;
        hot[3].x2 = x;
        hot[3].y1 = y1;
        hot[3].y2 = y;
        hot[3].f0 = 11;
        if (hot[4].y2 >= hot[1].y1)
            hot[4].y2 = hot[1].y1 - 1;
        if (hot[4].x1 < hot[3].x1) {
            save = hot[3];
            hot[3] = hot[4];
            hot[4] = save;
        }
    }
}

struct Step g_521e[30] = {{2, 15}, {3, 15}, {4, 22}, {5, 15}, {6, 37}, {5, 15}, {6, 45}, {5, 15}, {4, 15}, {3, 15}, {2, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {8, 75}, {11, 15}, {12, 30}, {13, 15}, {12, 15}, {11, 15}, {8, 90}, {7, 15}, {9, 15}, {10, 30}, {9, 15}, {7, 7}, {14, 15}};   /* DS:521E */
char g_525a[16] = {1, 15, 7, 8, 9, 10, 11, 12, 2, 3, 0, 1, 5, 6, 13, 4};   /* DS:525A */
struct Cell g_526a[20] = {{0, 0, 48, 24, 80, 0}, {48, 0, 32, 24, 88, 0}, {80, 0, 32, 24, 96, 0}, {112, 0, 64, 24, 88, 0}, {0, 24, 40, 44, 96, 0}, {40, 24, 40, 44, 96, 0}, {80, 24, 40, 44, 96, 0}, {120, 24, 40, 44, 96, 0}, {0, 68, 40, 44, 96, 0}, {40, 68, 40, 44, 96, 0}, {80, 68, 40, 44, 96, 0}, {120, 68, 40, 44, 96, 0}, {0, 112, 40, 44, 96, 0}, {40, 112, 40, 44, 96, 0}, {80, 112, 40, 44, 96, 0}, {120, 112, 40, 44, 96, 0}, {0, 156, 48, 44, 88, 0}, {48, 156, 56, 44, 80, 0}, {104, 156, 56, 44, 80, 0}, {160, 156, 40, 44, 96, 0}};   /* DS:526A */
struct Cell g_52e2[9] = {{208, 54, 8, 2, 128, 84}, {200, 54, 8, 3, 112, 88}, {184, 54, 16, 4, 96, 94}, {160, 54, 24, 10, 112, 113}, {160, 86, 40, 10, 128, 113}, {160, 65, 48, 20, 136, 104}, {160, 1, 112, 52, 128, 70}, {0, 1, 160, 97, 112, 52}, {0, 99, 224, 101, 80, 51}};   /* DS:52E2 */
int g_5318[8] = {1201, 1217, 1106, 1107, 1108, 1109, 1073, 0};   /* DS:5318 */
int g_5328[3] = {6, 7, 0};   /* DS:5328 */

/* F_0A265 @ 0000:A265 (linear 0x0A265, 247 bytes) */



static void near F_0A265(void)
{
    struct Bitmap far *pict;
    struct Car chassis;
    int n;
    struct Node nodes[5];
    struct Node n1;
    struct Node na;

    F_193FE();
    pict = F_11A72(0x60, 9, 200, 143, 0);
    chassis.slot6 = &n1;
    chassis.slot12 = &na;
    for (n = 0; n < 5; n++)
        chassis.rack.s[n] = &nodes[n];
    F_04036(bn7648, 0, &chassis);
    F_081AB(&chassis);
    F_11421(-1, "", 0);
    F_13DE6(0x18);
    g_05cc = g_tick + 800;
    while ((n = F_01417(3000)) == 0)
        ;
    r_vid.f8 = 0;
    F_11BD3(pict, 0x60, 9);
    F_193FE();
    F_13F21(1);
}

/* F_0A35C @ 0000:A35C (linear 0x0A35C, 186 bytes) - blit sprite-sheet cell n (7 per row) of bm into dst */


static void near F_0A35C(struct Bitmap far *bm, int n, struct Bitmap far *dst)
{
    struct Blt cell;
    int row;
    struct Blt bl;
    int col;

    if (n < 14) {
        row = n / 7;
        col = n - 7 * row;
    } else {
        row = 0;
        col = 0;
    }
    cell.w = 40;
    cell.h = 50;
    cell.sx = 40 * col;
    cell.sy = 50 * row;
    cell.dx = 184;
    cell.dy = 73;
    if (g_0284 == 6) {
        bl.w = 16;
        bl.h = 50;
        bl.sx = 208;
        bl.sy = 73;
        bl.dx = cell.sx + 24;
        bl.dy = cell.sy;
        (*b_78a2)(r_vid.front, bm, &bl, 1);
    }
    (*b_78a2)(bm, dst, &cell, 1);
}
