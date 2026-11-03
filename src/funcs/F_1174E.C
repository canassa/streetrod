/* F_1174E: show a message and wait for a key ("Hit Return when ready ...") */

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "GLOBALS.H"
#include "PROTOS.H"

void F_1174E(char *msg)
{
    int saved;
    char prev;
    int dx;
    int i;
    struct Bitmap far *back;

    saved = g_vid.f16;
    strcpy(g_293e, msg);
    strcpy(g_2967, "Hit Return when ready ...");
    if (g_8247 >= 1) {
        if (g_58ee) {
            F_1169A(0, 0, 320, 200, 0);
            g_58ee++;
        }
        prev = g_8249;
        if (!prev) {
            g_8249 = 1;
            (*g_present)();
        }
        dx = g_824a ? 5 : 0x50;
        for (i = 0; i < 5; i++) {
            g_1d8a[i].x1 += dx;
            g_1d8a[i].x2 += dx;
        }
        if (g_58ee)
            F_11324(g_04ac);
        g_vid.f16 = 1;
        F_00E24();
        back = g_vid.back;
        g_vid.back = g_vid.front;
        F_14241(0x20);
        g_vid.back = back;
        for (i = 0; i < 5; i++) {
            g_1d8a[i].x1 -= dx;
            g_1d8a[i].x2 -= dx;
        }
        g_vid.f16 = saved;
        g_8249 = prev;
        if (!prev)
            (*g_78be)();
    } else {
        puts(" ");
        puts(g_293e);
        puts(g_2967);
        if (g_05e0) {
            while (!F_02DB6())
                ;
        } else {
            while (!kbhit() == 0)
                ;
            getch();
        }
    }
}
