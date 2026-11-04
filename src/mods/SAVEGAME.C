#include <dos.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "STRUCTS.H"
#include "SAVEGAME.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "BOBS.H"
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
#include "PC.H"
#include "POOLS.H"
#include "POPUP.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SYS.H"
#include "VIDEO.H"

static void near init_racers(void);
static void near init_new_game(void);
static int near write_nodes(int handle, struct Node *p);
static int near read_nodes(int handle, struct Node **head);
static int near read_cars(int handle, struct Car **head, int flag);
static void near scan_save_slots(int create);
static int near is_blank(char *p);
static int near write_cars(int handle, struct Car *c, int flag);

char g_noNameMsg[] = "The game must have a name !";   /* DS:4FA6 */
char g_saveSlots[16] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   /* DS:4FC2 */
int g_slotNameOfs[16] = {6661, 6678, 6695, 6712, 6729, 6746, 6763, 6780, 6797, 6814, 6831, 6848, 6865, 6882, 6899, 6916};   /* DS:4FD2 */
char *g_noNameMsgPtr = g_noNameMsg;   /* DS:4FF2 */
int g_demoRacer = 0;   /* DS:4FF4 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */

/* savegame_init_racer @ 0000:542E (linear 0x0542E, 342 bytes) */

void savegame_init_racer(int k, int *list)
{
    int x, i, y, n;

    while ((i = random_int(6)) == 5)
        if (random_int(2) == 1)
            break;
    g_racerRecordsArray[k].paintScheme = i;
    g_racerRecordsArray[k].status = RACER_NOT_MET;
    g_racerRecordsArray[k].betRaces = 0;
    g_racerRecordsArray[k].betWins = 0;
    g_racerRecordsArray[k].bodyMods = 0;
    g_racerRecordsArray[k].sticker = 0;
    g_racerRecordsArray[k].lostCarOnce = 0;
    n = list[0];
    do {
        i = random_int(n);
    } while (list[i + 1] == -1);
    g_racerRecordsArray[k].specIndex = list[i + 1];
    list[i + 1] = -1;
    x = g_racerRecordsArray[k].specIndex;
    y = (g7d86n[x].rank >> 1) + 1;
    g_racerRecordsArray[k].bodyMods = ((g7d86n[x].parts >> 8 & 7) == 2) ? 8 : 0;
    g_racerRecordsArray[k].sticker = (random_int(20) < y) ? random_int(9) : 0;
    if (w_8df0[x].frontBumperPic > 0 && w_8df0[x].rearBumperPic > 0)
        g_racerRecordsArray[k].bodyMods |= (random_int(25) < y) ? 3 : 0;
    if (w_8df0[x].roofPic > 0)
        g_racerRecordsArray[k].bodyMods |= (random_int(30) < y) ? 4 : 0;
}

/* init_racers @ 0000:5584 (linear 0x05584, 293 bytes) */

static void near init_racers(void)
{
    int nb, na1, nc, v, i, k;
    int cl[11], bm[11], an[11];

    i = na1 = nb = nc = 0;
    for (; i < 25; i++) {
        v = g7d86n[i].rank;
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
    if (g_demo)
        an[0] = --na1;
    k = 0;
    for (i = 0; i < RACERS_PER_GROUP; i++)
        savegame_init_racer(k++, an);
    for (i = 0; i < RACERS_PER_GROUP; i++)
        savegame_init_racer(k++, bm);
    for (i = 0; i < RACERS_PER_GROUP; i++)
        savegame_init_racer(k++, cl);
    g_racerRecordsArray[RACER_KING].status = RACER_NOT_MET;
    if (g_demo) {
        an[0] = 1;
        an[1] = 0x18;
        savegame_init_racer(g_demoRacer, an);
        g_racerRecordsArray[g_demoRacer].nameOffset = 0x629;
    }
}

/* init_new_game @ 0000:56A9 (linear 0x056A9, 99 bytes) */

static void near init_new_game(void)
{
    pools_init(1);
    a_7eaap.money = 750L;
    if (g_demo)
        a_7eaap.money = 10000L;
    a_7eaap.m7eae = 10;
    road_build_layout(100);
    init_racers();
    g_addT = 0L;
    g_tRef = g_sysT;
    g_betRaces = g_dragWins = g_roadWins = g_kingDragWon = 0;
}

/* savegame_new @ 0000:570C (linear 0x0570C, 137 bytes) */

int near savegame_new(void)
{
    int sel;

    r_vid.copyToFront = 0;
    F_119B8(0x3f8, 0x10, 6, 0);
    r_vid.copyToFront = 1;
    video_name_pal();
    bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), (SCREEN_BODY_HEIGHT - 1));
    (*g_videoOps.show)();
    switch (sel = edit_text_box(2, 15, g_driverName)) {
    case -3:
    case 0:
        init_new_game();
        return 0;
    case -2:
        return -2;
    }
}

/* write_nodes @ 0000:5795 (linear 0x05795, 71 bytes) */

static int near write_nodes(int handle, struct Node *p)
{
    unsigned written;

    for (; p; p = p->next) {
        if (_dos_write(handle, p, 8, &written) != 0 || written != 8)
            return 1;
    }
    return 0;
}

/* read_nodes @ 0000:57DC (linear 0x057DC, 146 bytes) */

static int near read_nodes(int handle, struct Node **head)
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
        p = game_new_part();
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

/* write_cars @ 0000:586E (linear 0x0586E, 161 bytes) */

static int near write_cars(int handle, struct Car *c, int flag)
{
    int i;
    unsigned n;

    while (c) {
        if (_dos_write(handle, c, 0x28, &n) != 0 || n != 0x28 || write_nodes(handle, c->transmission))
            return 1;
        for (i = 0; i < 5; i++)
            if (write_nodes(handle, c->rack.parts[i]))
                return 1;
        if (write_nodes(handle, c->tires))
            return 1;
        if (flag)
            return 0;
        c = c->next;
    }
    return 0;
}

/* read_cars @ 0000:590F (linear 0x0590F, 231 bytes) */

static int near read_cars(int handle, struct Car **head, int flag)
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
        if (_dos_read(handle, &buf, 0x28, &n) != 0 || n != 0x28 || read_nodes(handle, &buf.transmission))
            return 1;
        for (m = 0; m < 5; m++)
            if (read_nodes(handle, &buf.rack.parts[m]))
                return 1;
        if (read_nodes(handle, &buf.tires))
            return 1;
        car = (struct Car *)game_new_car();
        *car = buf;
        if (prev) {
            prev->next = car;
            prev = car;
        } else
            *head = prev = car;
        if (prev->next == 0)
            break;
        if (flag)
            break;
        prev->next = 0;
    }
    return 0;
}

/* scan_save_slots @ 0000:59F6 (linear 0x059F6, 340 bytes) */

static void near scan_save_slots(int create)
{
    int fd7;
    char hdr3[36];
    char name[20];
    int slot;
    int cnt;
    unsigned n;

    name[0] = disk_data_drive();
    strcpy(name + 1, ":HOTROD");
    for (slot = 1, cnt = 0; slot <= 15; slot++) {
        itoa(slot, name + 8, 10);
        strcat(name, ".SAV");
        if (_dos_open(name, O_RDONLY, &fd7)) {
            g_saveSlots[slot] = 0;
        } else {
            if (_dos_read(fd7, hdr3, 0x24, &n) != 0 || n != 0x24)
                g_saveSlots[slot] = 0;
            else {
                g_saveSlots[slot] = slot;
                strncpy(g_desc + (g_list[++cnt] = g_slotNameOfs[slot]), hdr3, 15);
            }
            _dos_close(fd7);
        }
        if (create && g_saveSlots[slot] == 0) {
            strcpy(g_desc + (g_list[++cnt] = g_slotNameOfs[slot]), g_blankName);
        }
    }
    g_list[0] = cnt;
}

/* savegame_load @ 0000:5B4A (linear 0x05B4A, 873 bytes) */

int near savegame_load(void)
{
    int fd7;
    char name[20];
    int cntl;
    int r;
    int i;
    long delta;
    unsigned n;

    cursor_ctl(CURSOR_HIDE_NOW);
    scan_save_slots(0);
    r = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    F_118D4(0x3f7, 0);
    if (g_list[0]) {
        F_119B8(0x3fa, 0xf0, 100, -1);
        F_119B8(0x3fc, 0xf0, 0x8c, -1);
    }
    r_vid.copyToFront = r;
    if (r)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, SCREEN_WIDTH, SCREEN_BODY_HEIGHT);
    cursor_ctl(CURSOR_SHOW_NOW);
    if (g_list[0] == 0) {
        menu_wait(15);
        return -1;
    }
    r = F_142CF(3, 1, 0, g_list, 0, 0);
    switch (r) {
    case -4:
        return -1;
    case -3:
        r = menu_ix();
        break;
    default:
        if (r >= 1)             /* PROVISIONAL: the original loads g_list[0] here and drops it */
            g_list[0] = g_list[0];
        break;
    }
    for (i = 1, cntl = 0; cntl < r; i++)
        if (g_saveSlots[i])
            cntl++;
    r = i - 1;
    cursor_ctl(CURSOR_HIDE_NOW);
    name[0] = disk_data_drive();
    strcpy(name + 1, ":HOTROD");
    itoa(g_saveSlots[r], name + 8, 10);
    strcat(name, ".SAV");
    if (_dos_open(name, O_RDONLY, &fd7)
        || _dos_read(fd7, g_driverName, 0x24, &n) != 0 || n != 0x24
        || (pools_init(0), 0)
        || read_cars(fd7, &a_7eaap.currentCar, 1)
        || read_cars(fd7, &a_7eaap.otherCars, 0)
        || read_nodes(fd7, &a_7eaap.spareParts)
        || _dos_read(fd7, &g_betRaces, 8, &n) != 0 || n != 8
        || _dos_read(fd7, g_racerRecordsArray, 0x18c, &n) != 0 || n != 0x18c
        || _dos_read(fd7, &g_addT, 4, &n) != 0 || n != 4
        || _dos_read(fd7, &delta, 4, &n) != 0 || n != 4
        || _dos_read(fd7, &g_roadLayout, 2, &n) != 0 || n != 2) {
        cursor_ctl(CURSOR_SHOW_NOW);
        menu_wait(0x1f);
        cursor_ctl(CURSOR_HIDE);
        if (fd7 > -1) {
            _dos_close(fd7);
            init_new_game();
        }
        cursor_ctl(CURSOR_SHOW);
        return -666;
    }
    _dos_close(fd7);
    if (a_7eaap.currentCar)
        F_067CF(a_7eaap.currentCar->paintScheme);
    road_build_layout(g_roadLayout);
    g_tRef = g_sysT - delta;
    cursor_ctl(CURSOR_SHOW_NOW);
    return 0;
}

/* is_blank @ 0000:5EB3 (linear 0x05EB3, 55 bytes)

   True if the string is only spaces and underscores */

static int near is_blank(char *p)
{
    char ch;

    while ((ch = *p++) != 0)
        if (ch != ' ' && ch != '_')
            return 0;
    return 1;
}

char g_501d = 0;   /* DS:501D */
char g_buyCarMsg[] = "Buy a car first !";   /* DS:501E */

/* savegame_save @ 0000:5EEA (linear 0x05EEA, 1332 bytes) */

void near savegame_save(void)
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
    cursor_ctl(CURSOR_HIDE_NOW);
    scan_save_slots(1);
    r = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    F_118D4(0x3f7, 0);
    F_119B8(0x3f9, 0x48, 0xa9, -1);
    F_119B8(0x3fb, 0xf0, 100, -1);
    F_119B8(0x3fc, 0xf0, 0x8c, -1);
    r_vid.copyToFront = r;
    if (r)
        bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, SCREEN_WIDTH, SCREEN_BODY_HEIGHT);
    strcpy(g_saveName, g_driverName);
again:
    for (r = 1; r <= g_list[0] && g_saveSlots[r]; r++)
        ;
    if (r > g_list[0])
        r = g_list[0];
    if (sel == 0)
        sel = r;
    cursor_ctl(CURSOR_SHOW_NOW);
    switch (r = menu_list(5, sel, 0, g_list, 0, 0)) {
    case -4:
        return;
    default:
        if ((int)r < 1)
            return;
        if (r > g_list[0])
            return;
        break;
    case -3:
        r = menu_ix();
        break;
    }
    sel = r;
    if (is_blank(g_saveName) && is_blank(g_desc + g_list[r])) {
        screen_msg_box(g_noNameMsgPtr);
        cursor_ctl(CURSOR_HIDE_NOW);
        goto again;
    }
    for (i5 = 1; i5 <= 15 && (cmp = strcmp(g_saveName, g_desc + g_list[i5])) != 0; i5++)
        ;
    if (cmp == 0) {
        cursor_ctl(CURSOR_HIDE);
        box7 = (struct Win *)(0x12 * (int)r + g_listLineBasePointerValue - 0x12);
        pal4 = x_colors;
        x_colors = bustyle->pal[((struct Win *)g_listLineBasePointerValue)->lines];
        sv7 = x_colors.line;
        x_colors.line = x_colors.textBg;
        wk = ((struct Win *)g_listLineBasePointerValue)->tx - ((struct Win *)g_listLineBasePointerValue)->w;
        draw_frame(r_vid.back, box7->w, box7->h, wk, 8);
        draw_frame(r_vid.front, box7->w, box7->h, wk, 8);
        x_colors = pal4;
        x_colors.line = sv7;
        cursor_ctl(CURSOR_SHOW);
        sel = r = i5;
    }
    strcpy(name + 1, ":HOTROD");
    itoa(r, name + 8, 10);
    strcat(name + 1, ".SAV");
    key = g_saveSlots[r] ? menu_modal(0x1e) : 0;
    cursor_ctl(CURSOR_HIDE_NOW);
    switch (key) {
    case -0x15:
        goto again;
    case -0x14:
        unlink(name);
    }
    strcpy(g_driverName, is_blank(g_saveName) ? g_desc + g_list[r] : g_saveName);
    delta = g_sysT - g_tRef;
    name[0] = disk_data_drive();
    if (_dos_creat(name, _A_NORMAL, &fd8)
        || _dos_write(fd8, g_driverName, 0x24, &n) != 0 || n != 0x24
        || write_cars(fd8, a_7eaap.currentCar, 1)
        || write_cars(fd8, a_7eaap.otherCars, 0)
        || write_nodes(fd8, a_7eaap.spareParts)
        || _dos_write(fd8, &g_betRaces, 8, &n) != 0 || n != 8
        || _dos_write(fd8, g_racerRecordsArray, 0x18c, &n) != 0 || n != 0x18c
        || _dos_write(fd8, &g_addT, 4, &n) != 0 || n != 4
        || _dos_write(fd8, &delta, 4, &n) != 0 || n != 4
        || _dos_write(fd8, &g_roadLayout, 2, &n) != 0 || n != 2) {
        cursor_ctl(CURSOR_SHOW_NOW);
        menu_modal(6);
        cursor_ctl(CURSOR_HIDE_NOW);
        if (fd8 > -1) {
            _dos_close(fd8);
            unlink(name);
        }
        scan_save_slots(1);
    } else {
        _dos_close(fd8);
        cursor_ctl(CURSOR_SHOW_NOW);
        return;
    }
    goto again;
}

/* savegame_menu @ 0000:641E (linear 0x0641E, 87 bytes) */

int near savegame_menu(void)
{
    struct Bitmap far *p;
    int row;
    int x;
    int y;

    p = menu_grab(0x1b, &x, &y, ARENA_HIGH_STACK);
    row = menu_wait(0x1b);
    popup_show(p, x, y);
    F_19438(1);
    return row;
}
