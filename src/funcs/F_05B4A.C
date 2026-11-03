/* F_05B4A @ 0000:5B4A (linear 0x05B4A, 873 bytes) */

#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_03614(int reset);
static void near F_056A9(void);
static int near F_057DC(int handle, struct Node **head);
static int near F_0590F(int handle, struct Car **head, int flag);
static void near F_059F6(int create);

static int near F_05B4A(void)
{
    int fd7;
    char name[20];
    int cntl;
    int r;
    int i;
    long delta;
    unsigned n;

    F_120FE(-4);
    F_059F6(0);
    r = g_vid.f16;
    g_vid.f16 = 0;
    F_118D4(0x3f7, 0);
    if (g_49e0[0]) {
        F_119B8(0x3fa, 0xf0, 100, -1);
        F_119B8(0x3fc, 0xf0, 0x8c, -1);
    }
    g_vid.f16 = r;
    if (r)
        F_1A92A(g_vid.back, g_vid.front, 0, 0, 320, 190);
    F_120FE(-2);
    if (g_49e0[0] == 0) {
        F_14294(15);
        return -1;
    }
    r = F_142CF(3, 1, 0, g_49e0, 0, 0);
    switch (r) {
    case -4:
        return -1;
    case -3:
        r = F_14C8B();
        break;
    default:
        if (r >= 1)
            g_49e0[0] = g_49e0[0];
        break;
    }
    for (i = 1, cntl = 0; cntl < r; i++)
        if (g_4fc2[i])
            cntl++;
    r = i - 1;
    F_120FE(-4);
    name[0] = F_15FBD();
    strcpy(name + 1, ":HOTROD");
    itoa(g_4fc2[r], name + 8, 10);
    strcat(name, ".SAV");
    if (_dos_open(name, 0, &fd7)
        || _dos_read(fd7, g_newName, 0x24, &n) != 0 || n != 0x24
        || (F_03614(0), 0)
        || F_0590F(fd7, &g_7eb0, 1)
        || F_0590F(fd7, &g_7eb2, 0)
        || F_057DC(fd7, &g_7eb8)
        || _dos_read(fd7, &g_4970, 8, &n) != 0 || n != 8
        || _dos_read(fd7, g_7ff8, 0x18c, &n) != 0 || n != 0x18c
        || _dos_read(fd7, &g_05fc, 4, &n) != 0 || n != 4
        || _dos_read(fd7, &delta, 4, &n) != 0 || n != 4
        || _dos_read(fd7, &g_5e0c, 2, &n) != 0 || n != 2) {
        F_120FE(-2);
        F_14294(0x1f);
        F_120FE(-3);
        if (fd7 > -1) {
            _dos_close(fd7);
            F_056A9();
        }
        F_120FE(-1);
        return -666;
    }
    _dos_close(fd7);
    if (g_7eb0)
        F_067CF(g_7eb0->f4);
    F_26FE1(g_5e0c);
    g_0604 = g_0600 - delta;
    F_120FE(-2);
    return 0;
}
