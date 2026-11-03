#include <dos.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>

static void near F_00000(char *s);

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
    if (g_vid.mode != -4)
        int86(0x10, &r, &r);
}
