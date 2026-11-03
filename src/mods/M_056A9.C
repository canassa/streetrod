#include "GLOBALS.H"
#include "PROTOS.H"
#include <dos.h>
#include <string.h>
#include <stdlib.h>

static void near F_03614(int reset);
static void near F_05584(void);
static void near F_056A9(void);
static struct Node * near F_03C1B(void);
static int near F_05795(int handle, struct Node *p);
static int near F_057DC(int handle, struct Node **head);
static struct Node2 * near F_03FB1(void);

/* F_056A9 @ 0000:56A9 (linear 0x056A9, 99 bytes) */



static void near F_056A9(void)
{
    F_03614(1);
    g_7eaa = 750L;
    if (g_0610)
        g_7eaa = 10000L;
    g_7eae = 10;
    F_26FE1(100);
    F_05584();
    g_05fc = 0L;
    g_0604 = g_0600;
    g_4970 = g_4972 = g_4974 = g_4976 = 0;
}

/* F_0570C @ 0000:570C (linear 0x0570C, 137 bytes) */



static int near F_0570C(void)
{
    int sel;

    g_vid.f16 = 0;
    F_119B8(0x3f8, 0x10, 6, 0);
    g_vid.f16 = 1;
    F_10B9C();
    F_1A92A(g_vid.back, g_vid.front, 0, 0, 0x13f, 0xbd);
    (*g_present)();
    switch (sel = F_151D4(2, 15, g_newName)) {
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
