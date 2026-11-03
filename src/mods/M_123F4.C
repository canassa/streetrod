#include "GLOBALS.H"
#include "PROTOS.H"

/* F_123F4: draw the 64 x 6 bar for slot of the dialog; frame selects the highlighted frame.
   When frame == 1 and the screen is shadowed, the old bar is grabbed and pasted first. */

char g_06fc = 1;   /* DS:06FC */
int g_06fe[2] = {78, 93};   /* DS:06FE */
int g_0702[3][2] = {{24, 68}, {24, 68}, {16, 68}};   /* DS:0702 */
int g_070e[6] = {109, 112, 120, 111, 124, 116};   /* DS:070E */
int g_071a[12] = {75, 113, 64, 122, 83, 110, 72, 119, 83, 118, 72, 127};   /* DS:071A */
int g_0732[8] = {68, 74, 144, 74, 68, 74, 160, 74};   /* DS:0732 */
int g_0742[10][2] = {{90, 70}, {98, 70}, {94, 70}, {64, 70}, {90, 70}, {115, 70}, {72, 71}, {111, 71}, {71, 69}, {115, 69}};   /* DS:0742 */
int g_076a[10] = {10, -1, 21, -1, 11, -1, 29, -1, 11, -1};   /* DS:076A */
char g_077e[14] = {29, 0, -1, -1, 2, 2, 2, 2, 3, 4, 2, 3, 2, 3};   /* DS:077E */
char g_078c[6] = {5, 6, 7, 10, 12, 0};   /* DS:078C */
struct Bitmap far *g_0792 = 0;   /* DS:0792 */
struct NewLink g_0796 = {11, 0, 0, 1, 0, 0, 0, 0, ""};   /* DS:0796 */

void F_123F4(int frame, int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    x = 0xb8;
    y = g_06fe[slot] + 10;
    if (frame == 1 && g_vid.f16) {
        blt.w = 0x10;
        blt.h = 6;
        blt.sx = 0x40;
        blt.sy = 0;
        tmp = F_1A89E(pict, &blt, 2);
        (*g_78ce)(0xe, 0x94, x, 8, y, tmp);
        F_19438(1);
    }
    blt.sx = frame << 4;
    blt.sy = 0;
    blt.dx = x;
    blt.dy = y;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    F_120FE(-1);
    if (g_vid.f16)
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
    F_19438(1);
}

/* F_12525: draw the bar for slot "slot" of the dialog, then (screen shadowed) grab it back and paste the frame */


void F_12525(int slot)
{
    struct Bitmap far *pict;
    struct Bitmap far *tmp;
    struct Blt blt;
    int x;
    int y;

    F_120FE(-3);
    pict = F_15BBE(0x44f, 2);
    blt.sx = blt.sy = 0;
    blt.dx = x = 0xb8;
    blt.dy = y = g_06fe[slot] + 10;
    blt.w = 0x10;
    blt.h = blt.fc = 6;
    (*g_78a2)(pict, g_vid.back, &blt, 1);
    F_120FE(-1);
    if (g_vid.f16) {
        F_1A92A(g_vid.back, g_vid.front, blt.dx, blt.dy, blt.dx + 15, blt.dy + 5);
        F_16CF3();
        F_120FE(-4);
        blt.sx = 0x40;
        tmp = F_1A89E(pict, &blt, 2);
        (*g_78ce)(0xe, 0x94, x, y, 0x94, tmp);
        F_19438(1);
        F_120FE(-2);
    }
    F_19438(1);
}

/* F_1264E: show picture n centered in the dialog area; flag moves it left by 24.
   With the screen shadowed the area under it is saved around the draw and restored afterwards. */


void F_1264E(int n, int flag)
{
    struct Bitmap far *pict;
    struct Bitmap far *saved;
    struct Blt blt;
    int w1;
    int h1;
    int sx;
    int sy;

    F_120FE(-4);
    pict = F_15BBE(n, 2);
    sx = 200 - (w1 = pict->width);
    sy = 125 - (h1 = pict->height);
    if (flag)
        sx -= 0x18;
    if (g_vid.f16) {
        saved = F_11A72(sx - 8, sy, w1 + 8, h1, 0);
        (*g_78ce)(0xe, 0x94, sx - 8, 0x94, sy, pict);
        blt.sx = blt.sy = 0;
        blt.dx = 8;
        blt.dy = 0;
        blt.w = w1 + 8;
        blt.h = h1;
        (*g_78aa)(pict, saved, &blt, 1);
        F_120FE(-2);
        F_11BD3(saved, sx - 8, sy);
        F_19438(1);
    } else {
        (*g_78ce)(0xe, 0x94, sx, sy, sy, pict);
        F_120FE(-2);
    }
    F_19438(1);
}

/* F_12784: paste picture 0x42e at (0x20, 10) */


void F_12784(int n, int flag)
{
    struct Bitmap far *p;

    F_120FE(-3);
    p = F_15BBE(0x42e, 2);
    F_120FE(-1);
    F_11BD3(p, 0x20, 10);
    F_19438(1);
}

/* F_127DB: redraw the dialog picture n and its two bars; s gives the bar states.
   The screen shadow flag is switched off while drawing and the area is copied to the front afterwards. */


void F_127DB(int n, char far *s)
{
    int saved;
    int i;

    saved = g_vid.f16;
    if (n) {
        g_06fc = 0;
        g_vid.f16 = 0;
        F_120FE(-3);
        F_1264E(n, 0);
        for (i = 0; i < 2; i++)
            F_123F4(*s++, i);
        F_120FE(-1);
        if (g_vid.f16 = saved)
            F_1A92A(g_vid.back, g_vid.front, 0x20, 10, 0xe7, 0xb8);
        g_06fc = 1;
    }
}
