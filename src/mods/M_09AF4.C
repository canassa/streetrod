#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static void near F_09A27(int, int, int);
static void near F_092A4(int, int, int, int, int, int *);
static void near F_09A83(int *, int *, int *);
static int near F_09524(int);
static int near F_09556(void);
static int near F_09BC5(int kind);
static void near F_0D031(void);
static struct Car * near F_04036(int idx, int weight, struct Car *old);
static void near F_081AB(struct Car *c);

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
        if (g_7f86) {
            F_11421(-1, "", 0);
            F_092A4(2, g_7648, 1, 0xa3, 0, g_5030[g_827e]);
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
                if (g_7eb0->f22 < 22) {
                    F_03AB8(0x1761);
                    return 0;
                }
                g_7ec0 = 0;
                F_03AB8(0x11d1);
                g_79a2 = 0;
            } else {
                g_7ec0 = 1;
                F_03AB8(0x11f7);
                g_79a2 = 5;
            }
            g_79a4 = 0;
            F_0D031();
            g_05fc += g_7ec0 == 0 ? 0x444u : 0x777u;
            return 2;
        }
        F_193FE();
        return 0;
    }
    if (g_7eb0->f22 < 7) {
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
        g_7ec0 = 0;
        break;
    case 2:
        num = 0x13;
        g_7ec0 = 1;
        break;
    case -23:
    case 1000:
        F_03AB8(0x122c);
        goto quit;
    }
    if (g_7ec0 == 1 && g_7eb0->f22 < 17) {
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
        g_8278 = ch;
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
        if ((long)price <= g_7eaa) {
            if (F_09BC5(kind)) {
                g_79a2 = kind;
                g_79a4 = price;
                F_13F21(1);
                F_11BD3(pict, x, sy);
                pict = 0;
                F_03AB8(g_5210[g_7ec0][random_int(3)]);
                F_0D031();
                g_05fc += g_7ec0 == 0 ? 0x444u : 0x777u;
                return 2;
            }
            while ((ch = random_int(5)) == prev)
                ;
            F_03AB8(g_51fc[g_7ec0][prev = ch]);
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
        x1 = g_70e8[g_7648].x + g_825e;
        y1 = g_70e8[g_7648].y + g_8260;
        x = g_70e8[g_7648].w + x1 - 1;
        y = hot[0].y1 - 1;
        hot[1].x1 = x1;
        hot[1].x2 = x;
        hot[1].y1 = y1;
        hot[1].y2 = y;
        hot[1].f0 = 11;
        x1 = g_70e8[g_7648].sx + g_825e;
        y1 = g_70e8[g_7648].sy + g_8260;
        x = g_70e8[g_7648].sw + x1 - 1;
        y = g_70e8[g_7648].sh + y1 - 1;
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
    F_04036(g_7648, 0, &chassis);
    F_081AB(&chassis);
    F_11421(-1, "", 0);
    F_13DE6(0x18);
    g_05cc = g_tick + 800;
    while ((n = F_01417(3000)) == 0)
        ;
    g_vid.f8 = 0;
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
        (*g_78a2)(g_vid.front, bm, &bl, 1);
    }
    (*g_78a2)(bm, dst, &cell, 1);
}
