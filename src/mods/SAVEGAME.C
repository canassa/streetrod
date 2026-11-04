#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "STRUCTS.H"
#include "SAVEGAME.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "EDIT.H"
#include "FAME.H"
#include "GAME.H"
#include "GARAGE.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "M_02B9A.H"
#include "M_265BA.H"
#include "POOLS.H"
#include "POPUP.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

static void near F_05584(void);
static void near F_056A9(void);
static int near F_05795(int handle, struct Node *p);
static int near F_057DC(int handle, struct Node **head);
static int near F_0590F(int handle, struct Car **head, int flag);
static void near F_059F6(int create);
static int near F_05EB3(char *p);
static int near F_0586E(int handle, struct Car *c, int flag);

char g_4fa6[] = "The game must have a name !";   /* DS:4FA6 */
char g_4fc2[16] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   /* DS:4FC2 */
int g_4fd2[16] = {6661, 6678, 6695, 6712, 6729, 6746, 6763, 6780, 6797, 6814, 6831, 6848, 6865, 6882, 6899, 6916};   /* DS:4FD2 */
char *g_4ff2 = g_4fa6;   /* DS:4FF2 */
int g_4ff4 = 0;   /* DS:4FF4 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */

/* F_0542E @ 0000:542E (linear 0x0542E, 342 bytes) */

void F_0542E(int k, int *list)
{
    int x, i, y, n;

    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    av7ff8[k].fa = i;
    av7ff8[k].fb = 0;
    av7ff8[k].fc = 0;
    av7ff8[k].fd = 0;
    av7ff8[k].fe = 0;
    av7ff8[k].ff = 0;
    av7ff8[k].f10 = 0;
    n = list[0];
    do {
        i = random_int(n);
    } while (list[i + 1] == -1);
    av7ff8[k].f8 = list[i + 1];
    list[i + 1] = -1;
    x = av7ff8[k].f8;
    y = (g7d86n[x].f2 >> 1) + 1;
    av7ff8[k].fe = ((g7d86n[x].f4 >> 8 & 7) == 2) ? 8 : 0;
    av7ff8[k].ff = (random_int(20) < y) ? random_int(9) : 0;
    if (w_8df0[x].f6 > 0 && w_8df0[x].f8 > 0)
        av7ff8[k].fe |= (random_int(25) < y) ? 3 : 0;
    if (w_8df0[x].f2 > 0)
        av7ff8[k].fe |= (random_int(30) < y) ? 4 : 0;
}

/* F_05584 @ 0000:5584 (linear 0x05584, 293 bytes) */

static void near F_05584(void)
{
    int nb, na1, nc, v, i, k;
    int cl[11], bm[11], an[11];

    i = na1 = nb = nc = 0;
    for (; i < 25; i++) {
        v = g7d86n[i].f2;
        if (v >= 6)
            cl[++nc] = i;
        else if (v < 3)
            an[++na1] = i;
        else
            bm[++nb] = i;
    }
    an[0] = na1;
    bm[0] = nb;
    cl[0] = nc;
    if (g_0610)
        an[0] = --na1;
    k = 0;
    for (i = 0; i < 7; i++)
        F_0542E(k++, an);
    for (i = 0; i < 7; i++)
        F_0542E(k++, bm);
    for (i = 0; i < 7; i++)
        F_0542E(k++, cl);
    av7ff8[21].fb = 0;
    if (g_0610) {
        an[0] = 1;
        an[1] = 0x18;
        F_0542E(g_4ff4, an);
        av7ff8[g_4ff4].f6 = 0x629;
    }
}

/* F_056A9 @ 0000:56A9 (linear 0x056A9, 99 bytes) */

static void near F_056A9(void)
{
    F_03614(1);
    a_7eaap.m7eaa = 750L;
    if (g_0610)
        a_7eaap.m7eaa = 10000L;
    a_7eaap.m7eae = 10;
    F_26FE1(100);
    F_05584();
    g_05fc = 0L;
    g_0604 = g_0600;
    g_4970 = g_4972 = g_4974 = g_4976 = 0;
}

/* F_0570C @ 0000:570C (linear 0x0570C, 137 bytes) */

int near F_0570C(void)
{
    int sel;

    r_vid.f16 = 0;
    F_119B8(0x3f8, 0x10, 6, 0);
    r_vid.f16 = 1;
    F_10B9C();
    bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xbd);
    (*m_present)();
    switch (sel = F_151D4(2, 15, atnewName)) {
    case -3:
    case 0:
        F_056A9();
        return 0;
    case -2:
        return -2;
    }
}

/* F_05795 @ 0000:5795 (linear 0x05795, 71 bytes) */

static int near F_05795(int handle, struct Node *p)
{
    unsigned written;

    for (; p; p = p->next) {
        if (_dos_write(handle, p, 8, &written) != 0 || written != 8)
            return 1;
    }
    return 0;
}

/* F_057DC @ 0000:57DC (linear 0x057DC, 146 bytes) */

static int near F_057DC(int handle, struct Node **head)
{
    struct Node *p;
    struct Node *prev;
    struct Node buf;
    unsigned n;

    prev = 0;
    if (*head == 0)
        return 0;
    *head = 0;
    for (;;) {
        if (_dos_read(handle, &buf, 8, &n) != 0 || n != 8)
            return 1;
        p = F_03C1B();
        *p = buf;
        if (prev) {
            prev->next = p;
            prev = p;
        } else
            *head = prev = p;
        if (prev->next == 0)
            break;
        prev->next = 0;
    }
    return 0;
}

/* F_0586E @ 0000:586E (linear 0x0586E, 161 bytes) */

static int near F_0586E(int handle, struct Car *c, int flag)
{
    int i;
    unsigned n;

    while (c) {
        if (_dos_write(handle, c, 0x28, &n) != 0 || n != 0x28 || F_05795(handle, c->slot6))
            return 1;
        for (i = 0; i < 5; i++)
            if (F_05795(handle, c->rack.s[i]))
                return 1;
        if (F_05795(handle, c->slot12))
            return 1;
        if (flag)
            return 0;
        c = c->f26;
    }
    return 0;
}

/* F_0590F @ 0000:590F (linear 0x0590F, 231 bytes) */

static int near F_0590F(int handle, struct Car **head, int flag)
{
    struct Car *car;
    struct Car *prev;
    int m;
    struct Car buf;
    unsigned n;

    prev = 0;
    if (*head == 0)
        return 0;
    *head = 0;
    for (;;) {
        if (_dos_read(handle, &buf, 0x28, &n) != 0 || n != 0x28 || F_057DC(handle, &buf.slot6))
            return 1;
        for (m = 0; m < 5; m++)
            if (F_057DC(handle, &buf.rack.s[m]))
                return 1;
        if (F_057DC(handle, &buf.slot12))
            return 1;
        car = (struct Car *)F_03FB1();
        *car = buf;
        if (prev) {
            prev->f26 = car;
            prev = car;
        } else
            *head = prev = car;
        if (prev->f26 == 0)
            break;
        if (flag)
            break;
        prev->f26 = 0;
    }
    return 0;
}

/* F_059F6 @ 0000:59F6 (linear 0x059F6, 340 bytes) */

static void near F_059F6(int create)
{
    int fd7;
    char hdr3[36];
    char name[20];
    int slot;
    int cnt;
    unsigned n;

    name[0] = F_15FBD();
    strcpy(name + 1, ":HOTROD");
    for (slot = 1, cnt = 0; slot <= 15; slot++) {
        itoa(slot, name + 8, 10);
        strcat(name, ".SAV");
        if (_dos_open(name, 0, &fd7)) {
            g_4fc2[slot] = 0;
        } else {
            if (_dos_read(fd7, hdr3, 0x24, &n) != 0 || n != 0x24)
                g_4fc2[slot] = 0;
            else {
                g_4fc2[slot] = slot;
                strncpy(g_2c02 + (g_49e0[++cnt] = g_4fd2[slot]), hdr3, 15);
            }
            _dos_close(fd7);
        }
        if (create && g_4fc2[slot] == 0) {
            strcpy(g_2c02 + (g_49e0[++cnt] = g_4fd2[slot]), g_4607);
        }
    }
    g_49e0[0] = cnt;
}

/* F_05B4A @ 0000:5B4A (linear 0x05B4A, 873 bytes) */

int near F_05B4A(void)
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
    r = r_vid.f16;
    r_vid.f16 = 0;
    F_118D4(0x3f7, 0);
    if (g_49e0[0]) {
        F_119B8(0x3fa, 0xf0, 100, -1);
        F_119B8(0x3fc, 0xf0, 0x8c, -1);
    }
    r_vid.f16 = r;
    if (r)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 320, 190);
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
        if (r >= 1)             /* PROVISIONAL: the original loads g_49e0[0] here and drops it */
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
        || _dos_read(fd7, atnewName, 0x24, &n) != 0 || n != 0x24
        || (F_03614(0), 0)
        || F_0590F(fd7, &a_7eaap.m7eb0, 1)
        || F_0590F(fd7, &a_7eaap.m7eb2, 0)
        || F_057DC(fd7, &a_7eaap.m7eb8)
        || _dos_read(fd7, &g_4970, 8, &n) != 0 || n != 8
        || _dos_read(fd7, av7ff8, 0x18c, &n) != 0 || n != 0x18c
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
    if (a_7eaap.m7eb0)
        F_067CF(a_7eaap.m7eb0->f4);
    F_26FE1(g_5e0c);
    g_0604 = g_0600 - delta;
    F_120FE(-2);
    return 0;
}

/* F_05EB3 @ 0000:5EB3 (linear 0x05EB3, 55 bytes)

   True if the string is only spaces and underscores */

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

void near F_05EEA(void)
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
    r = r_vid.f16;
    r_vid.f16 = 0;
    F_118D4(0x3f7, 0);
    F_119B8(0x3f9, 0x48, 0xa9, -1);
    F_119B8(0x3fb, 0xf0, 100, -1);
    F_119B8(0x3fc, 0xf0, 0x8c, -1);
    r_vid.f16 = r;
    if (r)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 320, 190);
    strcpy(g_2a2e, atnewName);
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
        box7 = (struct Win *)(0x12 * (int)r + a_8b9ap - 0x12);
        pal4 = x_colors;
        x_colors = bustyle->pal[((struct Win *)a_8b9ap)->lines];
        sv7 = x_colors.c[3];
        x_colors.c[3] = x_colors.c[1];
        wk = ((struct Win *)a_8b9ap)->tx - ((struct Win *)a_8b9ap)->w;
        F_13957(r_vid.back, box7->w, box7->h, wk, 8);
        F_13957(r_vid.front, box7->w, box7->h, wk, 8);
        x_colors = pal4;
        x_colors.c[3] = sv7;
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
    strcpy(atnewName, F_05EB3(g_2a2e) ? g_2c02 + g_49e0[r] : g_2a2e);
    delta = g_0600 - g_0604;
    name[0] = F_15FBD();
    if (_dos_creat(name, 0, &fd8)
        || _dos_write(fd8, atnewName, 0x24, &n) != 0 || n != 0x24
        || F_0586E(fd8, a_7eaap.m7eb0, 1)
        || F_0586E(fd8, a_7eaap.m7eb2, 0)
        || F_05795(fd8, a_7eaap.m7eb8)
        || _dos_write(fd8, &g_4970, 8, &n) != 0 || n != 8
        || _dos_write(fd8, av7ff8, 0x18c, &n) != 0 || n != 0x18c
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

/* F_0641E @ 0000:641E (linear 0x0641E, 87 bytes) */

int near F_0641E(void)
{
    struct Bitmap far *p;
    int row;
    int x;
    int y;

    p = F_14090(0x1b, &x, &y, 2);
    row = F_14294(0x1b);
    F_11BD3(p, x, y);
    F_19438(1);
    return row;
}
