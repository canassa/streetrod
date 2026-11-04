#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "GLOBALS.H"
#include "PROTOS.H"
#include <stdio.h>

static void near F_03614(int reset);
static void near F_056A9(void);
static int near F_057DC(int handle, struct Node **head);
static int near F_0590F(int handle, struct Car **head, int flag);
static void near F_059F6(int create);
static int near F_05EB3(char *p);
static int near F_0586E(int handle, struct Car *c, int flag);
static int near F_05795(int handle, struct Node *p);

/* F_05B4A @ 0000:5B4A (linear 0x05B4A, 873 bytes) */



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
        || F_0590F(fd7, &g_7eaa.m7eb0, 1)
        || F_0590F(fd7, &g_7eaa.m7eb2, 0)
        || F_057DC(fd7, &g_7eaa.m7eb8)
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
    if (g_7eaa.m7eb0)
        F_067CF(g_7eaa.m7eb0->f4);
    F_26FE1(g_5e0c);
    g_0604 = g_0600 - delta;
    F_120FE(-2);
    return 0;
}

/* F_05EB3 @ 0000:5EB3 (linear 0x05EB3, 55 bytes)  - true if the string is only spaces and underscores */


static int near F_05EB3(char *p)
{
    char ch;

    while ((ch = *p++) != 0)
        if (ch != ' ' && ch != '_')
            return 0;
    return 1;
}

char g_501d = 0;   /* DS:501D */
char g_501e[] = "Buy a car first !";   /* DS:501E */

/* F_05EEA @ 0000:5EEA (linear 0x05EEA, 1332 bytes) */



static void near F_05EEA(void)
{
    int cmp;
    char sv7;
    struct Win *box7;
    struct Colors pal4;
    char name[20];
    unsigned r;
    int fd8;
    int wk;
    int sel;
    int key;
    long delta;
    int i5;
    unsigned n;

    sel = 0;
    F_120FE(-4);
    F_059F6(1);
    r = g_vid.f16;
    g_vid.f16 = 0;
    F_118D4(0x3f7, 0);
    F_119B8(0x3f9, 0x48, 0xa9, -1);
    F_119B8(0x3fb, 0xf0, 100, -1);
    F_119B8(0x3fc, 0xf0, 0x8c, -1);
    g_vid.f16 = r;
    if (r)
        F_1A92A(g_vid.back, g_vid.front, 0, 0, 320, 190);
    strcpy(g_2a2e, g_newName);
again:
    for (r = 1; r <= g_49e0[0] && g_4fc2[r]; r++)
        ;
    if (r > g_49e0[0])
        r = g_49e0[0];
    if (sel == 0)
        sel = r;
    F_120FE(-2);
    switch (r = F_1438E(5, sel, 0, g_49e0, 0, 0)) {
    case -4:
        return;
    default:
        if ((int)r < 1)
            return;
        if (r > g_49e0[0])
            return;
        break;
    case -3:
        r = F_14C8B();
        break;
    }
    sel = r;
    if (F_05EB3(g_2a2e) && F_05EB3(g_2c02 + g_49e0[r])) {
        F_1171C(g_4ff2);
        F_120FE(-4);
        goto again;
    }
    for (i5 = 1; i5 <= 15 && (cmp = strcmp(g_2a2e, g_2c02 + g_49e0[i5])) != 0; i5++)
        ;
    if (cmp == 0) {
        F_120FE(-3);
        box7 = (struct Win *)(0x12 * (int)r + g_8b9a - 0x12);
        pal4 = g_colors;
        g_colors = g_style->pal[((struct Win *)g_8b9a)->lines];
        sv7 = g_colors.c[3];
        g_colors.c[3] = g_colors.c[1];
        wk = ((struct Win *)g_8b9a)->tx - ((struct Win *)g_8b9a)->w;
        F_13957(g_vid.back, box7->w, box7->h, wk, 8);
        F_13957(g_vid.front, box7->w, box7->h, wk, 8);
        g_colors = pal4;
        g_colors.c[3] = sv7;
        F_120FE(-1);
        sel = r = i5;
    }
    strcpy(name + 1, ":HOTROD");
    itoa(r, name + 8, 10);
    strcat(name + 1, ".SAV");
    key = g_4fc2[r] ? F_14241(0x1e) : 0;
    F_120FE(-4);
    switch (key) {
    case -0x15:
        goto again;
    case -0x14:
        unlink(name);
    }
    strcpy(g_newName, F_05EB3(g_2a2e) ? g_2c02 + g_49e0[r] : g_2a2e);
    delta = g_0600 - g_0604;
    name[0] = F_15FBD();
    if (_dos_creat(name, 0, &fd8)
        || _dos_write(fd8, g_newName, 0x24, &n) != 0 || n != 0x24
        || F_0586E(fd8, g_7eaa.m7eb0, 1)
        || F_0586E(fd8, g_7eaa.m7eb2, 0)
        || F_05795(fd8, g_7eaa.m7eb8)
        || _dos_write(fd8, &g_4970, 8, &n) != 0 || n != 8
        || _dos_write(fd8, g_7ff8, 0x18c, &n) != 0 || n != 0x18c
        || _dos_write(fd8, &g_05fc, 4, &n) != 0 || n != 4
        || _dos_write(fd8, &delta, 4, &n) != 0 || n != 4
        || _dos_write(fd8, &g_5e0c, 2, &n) != 0 || n != 2) {
        F_120FE(-2);
        F_14241(6);
        F_120FE(-4);
        if (fd8 > -1) {
            _dos_close(fd8);
            unlink(name);
        }
        F_059F6(1);
    } else {
        _dos_close(fd8);
        F_120FE(-2);
        return;
    }
    goto again;
}
