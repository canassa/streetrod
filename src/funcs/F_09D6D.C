/* F_09D6D @ 0000:9D6D (linear 0x09D6D, 907 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"
#include "STRUCTS.H"

static int near F_09556(void);
static int near F_09BC5(int kind);
static void near F_0D031(void);

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
