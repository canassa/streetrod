/* F_00082 @ 0000:0082 (linear 0x00082, 265 bytes) */

#include <conio.h>
#include <dos.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_00000(char *s);

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
