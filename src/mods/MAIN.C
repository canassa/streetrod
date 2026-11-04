#include <dos.h>
#include <string.h>
#include <conio.h>
#include "STRUCTS.H"
#include "MAIN.H"
#include "ARENA.H"
#include "CGADRV.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "EGADRV.H"
#include "GAME.H"
#include "HERCDET.H"
#include "HERCGFX.H"
#include "MEM.H"
#include "MEMSTR.H"
#include "SCREENS.H"
#include "SOUND.H"
#include "SYS.H"
#include "TANDYDRV.H"
#include "VIDDET.H"
#include "VIDEO.H"

static void near F_00000(char *s);
static int near F_0018B(void);
static int near F_00082(int mode);
static int near F_005C6(void);
static int near F_00226(int mode);
static void near F_00316(int mode);
static void near F_004F4(int mode);
static void near F_00659(void);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Video r_vid;

int g_0042 = 0;   /* DS:0042 */
int g_0044 = 0;   /* DS:0044 */
int g_0046 = 0;   /* DS:0046 */
int g_0048 = 0;   /* DS:0048 */
int g_004a = 0;   /* DS:004A */
int g_004c = 0;   /* DS:004C */

/* F_00000 @ 0000:0000 (linear 0x00000, 130 bytes) */

static void near F_00000(char *s)
{
    union REGS inr, outr;
    char crlf[4];
    char text[84];

    crlf[0] = '\n';
    crlf[1] = '\r';
    crlf[2] = '$';
    crlf[3] = 0;
    strcpy(text, s);
    strcat(text, crlf);
    inr.h.ah = 9;
    inr.x.dx = (int)text;
    int86(0x21, &inr, &outr);
}

char *g_videoMenu[6] = {"                    1 - CGA      320 x 200 x  4 colors", "                    2 - EGA      320 x 200 x 16 colors", "                    3 - VGA      320 x 200 x 16 colors", "                    4 - Hercules monochrome", "                    5 - Tandy    320 x 200 x 16 colors", "\n                  Esc - Back to DOS"};   /* DS:0244 */
int g_0250 = 0;   /* DS:0250 */
int g_0252 = 0;   /* DS:0252 */
int g_0254 = 0;   /* DS:0254 */

/* F_00082 @ 0000:0082 (linear 0x00082, 265 bytes) */

static int near F_00082(int mode)
{
    union REGS r;
    char rec[84];
    int choice;

    if (mode < 1 || mode > 5)
        mode = 1;
    r.x.ax = 3;
    int86(0x10, &r, &r);
    memcpy(rec, "\n           Recommended mode: #", 32);
    strcat(rec, g_videoMenu[mode - 1] + 20);
    F_00000("\n\n\n\n\n\n                    STREET ROD:  Video Options\n");
    F_00000("\n              Use numeric keys to select graphic mode");
    F_00000("           or press any other key to accept recommendation\n");

    for (choice = 0; choice < 6; choice++)
        F_00000(g_videoMenu[choice]);
    F_00000(rec);
    while (kbhit())
        getch();
    while (!kbhit())
        ;
    choice = getch() - '0';
    if (choice == -21)
        F_00FEA();
    if (choice <= 0 || choice > 5)
        return 0;
    return choice;
}

/* F_0018B @ 0000:018B (linear 0x0018B, 70 bytes) */

static int near F_0018B(void)
{
    union REGS r;
    int s;
    int c;

    r.h.ah = 0x1a;
    r.h.al = 0;
    int86(0x10, &r, &r);
    s = r.h.al;
    c = r.h.bl;
    return s && c == 8;
}

/* F_001D1 @ 0000:01D1 (linear 0x001D1, 33 bytes) */

void F_001D1(int ax)
{
    union REGS r;

    r.x.ax = ax;
    int86(0x10, &r, &r);
}

/* F_001F2 @ 0000:01F2 (linear 0x001F2, 52 bytes) */

void F_001F2(unsigned char color)
{
    union REGS r;

    r.x.ax = 0x0b00;
    r.h.bh = 1;
    r.h.bl = color;
    if (r_vid.mode != -4)
        int86(0x10, &r, &r);
}

int g_0262 = 1;   /* DS:0262 */

/* F_00226 @ 0000:0226 (linear 0x00226, 240 bytes) */

static int near F_00226(int mode)
{
    int herc;
    int pick;

    herc = 0;
    F_0E62F();
    if (g_0252)
        g_0250 = 1;
    if (F_0E5C3() && (g_0250 == 1 || F_0E584())) {
        g_0250 = 0;
        herc = 1;
    }
    if (g_0250)
        mode = g_5af1 == 3 ? 2 : 6;
    else
        mode = 1;
    if (herc)
        mode = 4;
    if (memstr_find("Tandy", (char far *)0xF0000000L, 0xffff) ||
        memstr_find("TANDY", (char far *)0xF0000000L, 0xffff))
        mode = 5;
    if (F_0018B())
        mode = 3;
    pick = F_00082(mode);
    if (pick)
        mode = pick;
    if (mode == 1 && g_0250)
        return 6;
    if (mode == 3) {
        g_0254 = 1;
        return 2;
    }
    return mode;
}

/* F_00316 @ 0000:0316 (linear 0x00316, 478 bytes) */

static void near F_00316(int mode)
{
    switch (mode) {
    case 3:
        g_0254 = 1;
    case 2:
        r_vid.mode = -2;
        *(unsigned char far *)0x410L &= 0xcf;
        *(unsigned char far *)0x410L |= 0x20;
        egadrv_install();
        return;
    case 5:
        r_vid.mode = g_screens[0].flags = 0xfa;
        tandydrv_install();
        g_screens[0].bits.p = r_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
        g_screens[0].planesize = g_screens[1].planesize = g_screens[2].planesize = 0x8000;
        g_screens[0].pitch = g_screens[1].pitch = g_screens[2].pitch = 0xa0;
        g_screens[1].flags = g_screens[2].flags = 0xfa;
        g_screens[1].bits.p = (char far *)0x90000000L;
        g_screens[2].bits.p = F_03709(0x8000, 1);
        F_109C0(g_screens[1].bits.p, 0, 0x8000);
        F_109C0(g_screens[2].bits.p, 0, 0x8000);
        return;
    case 4:
        g_screens[0].flags = 0xfc;
        r_vid.mode = -4;
        break;
    case 1:
        g_screens[0].flags = 0xfd;
        break;
    case 6:
        g_screens[0].flags = 0xfd;
        *(unsigned char far *)0x410L &= 0xcf;
        *(unsigned char far *)0x410L |= 0x20;
        break;
    }
    cgadrv_install();
    g_screens[0].bits.p = r_vid.mode == -4 ? (char far *)0xb0000000L : (char far *)0xb8000000L;
    g_screens[0].planesize = g_screens[1].planesize = 0x4000;
    g_screens[0].pitch = g_screens[1].pitch = 0x50;
    g_screens[1].flags = 0xfd;
    g_screens[1].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[1].bits.p, 0, 0x4000);
    g_screens[2].bits.p = F_03709(0x4000, 1);
    F_109C0(g_screens[2].bits.p, 0, 0x4000);
}

/* F_004F4 @ 0000:04F4 (linear 0x004F4, 210 bytes) */

static void near F_004F4(int mode)
{
    int far *p;

    switch (mode) {
    case 2:
    case 3:
        F_001D1(0xd);
        break;
    case 5:
        F_001D1(9);
        outp(0x3df, 0xf6);
        break;
    case 4:
        F_0E5D2();
        p = r_vid.mode == -4 ? (int far *)0xb0000000L : (int far *)0xb8000000L;
        while (p < (int far *)((r_vid.mode == -4 ? 0xb0000000L : 0xb8000000L) + 0x8000L))
            *p++ = 0;
        outp(0x3b8, 0x2a);
        g_0046 = 1;
        g_0042 = 1;
        break;
    case 1:
    case 6:
        F_001D1(4);
        F_001F2(1);
        break;
    }
    g_0048 = 1;
}

/* F_005C6 @ 0000:05C6 (linear 0x005C6, 147 bytes) */

static int near F_005C6(void)
{
    long count;
    long prev;

    count = 0;
    prev = *(long far *)0x0000046cL;
    while (prev == *(long far *)0x0000046cL)
        ;
    prev = *(long far *)0x0000046cL;
    while (prev == *(long far *)0x0000046cL)
        count++;
    count /= 95;
    count /= 10;
    return count > 1 ? (int)count : 1;
}

int by70e2;
struct Bitmap far *g755cu;
char far *ab7564z;
char ap8247u;
int bl82c6;
int bu8acc;
int ag8bcet;

/* F_00659 @ 0000:0659 (linear 0x00659, 22 bytes) */

static void near F_00659(void)
{
    if (bu8acc < 4)
        bl82c6 = 1;
    else
        bl82c6 = 0;
}

int g_0280 = 0;   /* DS:0280 */
int g_0282 = 0;   /* DS:0282 */
int g_0284 = 0;   /* DS:0284 */
int g_0286 = 0;   /* DS:0286 */
struct Bitmap far *g_0288 = 0;   /* DS:0288 */
struct Bitmap far *g_028c = 0;   /* DS:028C */
struct Bitmap far *g_0290 = 0;   /* DS:0290 */
struct Bitmap far *g_0294 = 0;   /* DS:0294 */
int g_0298[7] = {0, 19, 31, 29, 0, 0, 30};   /* DS:0298 */
int g_02a6[7] = {0, 0, 8, 0, 0, 0, 9};   /* DS:02A6 */
int g_02b4[7] = {0, 2, 0, 2, 0, 0, 0};   /* DS:02B4 */
int g_02c2[7] = {0, 3, 1, 3, 0, 3, 1};   /* DS:02C2 */
int g_02d0 = 0;   /* DS:02D0 */

/* main @ 0000:066F (linear 0x0066F, 580 bytes) */

int main(int argc, char **argv)
{
    int e;
    int b;
    int c;
    int d;
    int a;

    ap8247u = 0;
    bu8acc = F_005C6();
    e = 2;
    b = 0;
    g_0262 = 1;
    ag8bcet = 0;
    g_0612 = 0;
    for (c = 1; c < argc; c++) {
        argv++;
        if (memcmp(*argv, "1", 2) == 0)
            e = 1;
        else if (strcmpi(*argv, "nomouse") == 0)
            g_0262 = 0;
        else if (strcmpi(*argv, "demo") == 0)
            ag8bcet = 1;
        else if (strcmpi(*argv, "auto") == 0)
            g_0612 = 1;
        else if (strcmpi(*argv, "nouemem") == 0)
            g_0280 = 1;
    }
    b = F_00226(b);
    F_036BE();
    F_19269();
    F_00316(b);
    F_004F4(b);
    F_10BD9(g_screens, &g_screen1);
    F_01111();
    if (r_vid.mode == -2 || r_vid.mode == -6)
        F_11324(g_palette);
    F_15CC3(e, b);
    F_00659();
    d = F_1552B(1);
    F_11EA8();
    F_16481();
    F_1013D(0);
    if (r_vid.mode == -3 && g_0250)
        g755cu = F_0376B(320, 100, 0, -1, 0);
    ab7564z = F_03709(r_vid.mode != -2 && r_vid.mode != -6 ? 16000 : 18000, 1);
    a = F_15C4A(g_4c20, 0);
    if (a == 3) {
        by70e2 = 1;
        d = F_1552B(2);
        F_15EF9();
        if (r_vid.mode == -2)
            F_15C07(g_4c28);
        F_15C4A(g_4c28, 1);
    } else {
        by70e2 = 0;
        F_15C95(g_4c20);
    }
    F_0503F();
    F_00FEA();
}
