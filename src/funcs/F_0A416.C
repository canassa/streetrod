/* F_0A416 @ 0000:A416 (linear 0x0A416, 302 bytes) */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_0A35C(struct Bitmap far *, int, struct Bitmap far *);

static void near F_0A416(void)
{
    struct Bitmap far *pict;
    int going;
    int i, key;

    F_193FE();
    pict = F_15BBE(0x4c9, 2);
    i = 0;
    F_13DE6(0x18);
    g_05cc = g_tick + 10;
    going = 1;
    do {
        while ((key = F_01417(3000)) == 0)
            ;
        switch (key) {
        case -26:
            going = 0;
            break;
        case 1000:
            if (i >= 31)
                going = 0;
            else {
                F_10AFD(1);
                F_0A35C(pict, g_525b[g_521e[i].frame], g_vid.front);
                g_05cc = g_tick + (int)g_521e[i].delay;
                i++;
            }
            break;
        }
    } while (going);
    F_10AFD(1);
    F_0A35C(pict, g_525b[1], g_vid.front);
    F_0A35C(pict, g_525b[1], g_vid.back);
    F_13F21(1);
    F_193FE();
}
