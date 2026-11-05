#include <dos.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
#include "INPUTDAT.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
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

static char g_noNameMsg[] = "The game must have a name !";
static char g_saveSlots[16] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int g_slotNameOfs[16] = {DESC_SAVE_SLOT_00, DESC_SAVE_SLOT_01, DESC_SAVE_SLOT_02, DESC_SAVE_SLOT_03,
                                DESC_SAVE_SLOT_04, DESC_SAVE_SLOT_05, DESC_SAVE_SLOT_06, DESC_SAVE_SLOT_07,
                                DESC_SAVE_SLOT_08, DESC_SAVE_SLOT_09, DESC_SAVE_SLOT_10, DESC_SAVE_SLOT_11,
                                DESC_SAVE_SLOT_12, DESC_SAVE_SLOT_13, DESC_SAVE_SLOT_14, DESC_SAVE_SLOT_15};
static char *g_noNameMsgPtr = g_noNameMsg;
int g_demoRacer = 0;


void sg_init_racer(int k, int *list)
{
    int x, i, y, n;

    while ((i = rn_int(6)) == 5)
        if (rn_int(2) == 1)
            break;
    g_racerRecordsArray[k].paintScheme = i;
    g_racerRecordsArray[k].status = RACER_NOT_MET;
    g_racerRecordsArray[k].betRaces = 0;
    g_racerRecordsArray[k].rivalBetWins = 0;
    g_racerRecordsArray[k].bodyMods = 0;
    g_racerRecordsArray[k].sticker = 0;
    g_racerRecordsArray[k].lostCarOnce = 0;
    n = list[0];
    do {
        i = rn_int(n);
    } while (list[i + 1] == -1);
    g_racerRecordsArray[k].specIndex = list[i + 1];
    list[i + 1] = -1;
    x = g_racerRecordsArray[k].specIndex;
    y = (g_carData[x].rank >> 1) + 1;
    g_racerRecordsArray[k].bodyMods = ((g_carData[x].parts >> 8 & 7) == ENGINE_V8_LARGE) ? BODY_LARGE_ENGINE : 0;
    g_racerRecordsArray[k].sticker = (rn_int(RIVAL_STICKER_CHANCE) < y) ? rn_int(STICKER_CHOICE_COUNT) : 0;
    if (g_models[x].frontBumperPic > 0 && g_models[x].rearBumperPic > 0)
        g_racerRecordsArray[k].bodyMods |= (rn_int(RIVAL_BUMPER_CHANCE) < y) ? BODY_FRONT_BUMPER | BODY_REAR_BUMPER : 0;
    if (g_models[x].roofPic > 0)
        g_racerRecordsArray[k].bodyMods |= (rn_int(RIVAL_ROOF_CHANCE) < y) ? BODY_ROOF : 0;
}


static void near init_racers(void)
{
    int nb, na1, nc, v, i, k;
    int cl[11], bm[11], an[11];

    i = na1 = nb = nc = 0;
    for (; i < SPEC_ORDINARY_COUNT; i++) {
        v = g_carData[i].rank;
        if (v >= RACER_GROUP_HIGH_RANK)
            cl[++nc] = i;
        else if (v < CAR_CLASS_LOW_LIMIT)
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
        sg_init_racer(k++, an);
    for (i = 0; i < RACERS_PER_GROUP; i++)
        sg_init_racer(k++, bm);
    for (i = 0; i < RACERS_PER_GROUP; i++)
        sg_init_racer(k++, cl);
    g_racerRecordsArray[RACER_KING].status = RACER_NOT_MET;
    if (g_demo) {
        an[0] = 1;
        an[1] = 0x18;
        sg_init_racer(g_demoRacer, an);
        g_racerRecordsArray[g_demoRacer].nameOffset = DESC_THE_GEEK;
    }
}


static void near init_new_game(void)
{
    pl_init(1);
    g_driver.money = STARTING_MONEY;
    if (g_demo)
        g_driver.money = DEMO_STARTING_MONEY;
    g_driver.unused = 10;
    rd_build_layout(100);
    init_racers();
    g_gameClock.actionTicks = 0L;
    g_gameClock.timeBase = g_gameClock.biosTicks;
    g_betRaces = g_dragWins = g_roadWins = g_kingDragWon = 0;
}


int near sg_new(void)
{
    int sel;

    r_vid.copyToFront = 0;
    sc_picture_at(PIC_DRIVERS_LICENSE, 0x10, 6, 0);
    r_vid.copyToFront = 1;
    vi_name_pal();
    bm_copy_box(r_vid.back, r_vid.front, 0, 0, (SCREEN_WIDTH - 1), (SCREEN_BODY_HEIGHT - 1));
    (*g_videoOps.show)();
    switch (sel = ed_text_box(KM_START_GAME, 15, g_driverName)) {
    case KR_START_OK:
    case 0:
        init_new_game();
        return 0;
    case KR_OLD_GAME:
        return KR_OLD_GAME;
    }
}


static int near write_nodes(int handle, struct Node *p)
{
    unsigned written;

    for (; p; p = p->next) {
        if (_dos_write(handle, p, 8, &written) != 0 || written != 8)
            return 1;
    }
    return 0;
}


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
        p = gm_new_part();
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
        car = (struct Car *)gm_new_car();
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


static void near scan_save_slots(int create)
{
    int fd7;
    char hdr3[36];
    char name[20];
    int slot;
    int cnt;
    unsigned n;

    name[0] = di_data_drive();
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


enum SaveLoadResult near sg_load(void)
{
    int fd7;
    char name[20];
    int cntl;
    int r;
    int i;
    long delta;
    unsigned n;

    cu_ctl(CURSOR_HIDE_NOW);
    scan_save_slots(0);
    r = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    sc_picture(PIC_SAVE_LOAD_BOOK, 0);
    if (g_list[0]) {
        sc_picture_at(PIC_LOAD_BUTTON, 0xf0, 100, -1);
        sc_picture_at(PIC_CANCEL_BUTTON, 0xf0, 0x8c, -1);
    }
    r_vid.copyToFront = r;
    if (r)
        bm_copy_box(r_vid.back, r_vid.front, 0, 0, SCREEN_WIDTH, SCREEN_BODY_HEIGHT);
    cu_ctl(CURSOR_SHOW_NOW);
    if (g_list[0] == 0) {
        mn_wait(KM_NO_SAVED_GAMES);
        return LOAD_CANCELLED;
    }
    r = mn_modal_list(KM_LOAD, 1, 0, g_list, 0, 0);
    switch (r) {
    case KR_CANCEL:
        return LOAD_CANCELLED;
    case KR_ACCEPT:
        r = mn_sel();
        break;
    default:
        if (r >= 1) /* PROVISIONAL: the original loads g_list[0] here and drops it */
            g_list[0] = g_list[0];
        break;
    }
    for (i = 1, cntl = 0; cntl < r; i++)
        if (g_saveSlots[i])
            cntl++;
    r = i - 1;
    cu_ctl(CURSOR_HIDE_NOW);
    name[0] = di_data_drive();
    strcpy(name + 1, ":HOTROD");
    itoa(g_saveSlots[r], name + 8, 10);
    strcat(name, ".SAV");
    if (_dos_open(name, O_RDONLY, &fd7) || _dos_read(fd7, g_driverName, 0x24, &n) != 0 || n != 0x24 ||
        (pl_init(0), 0) || read_cars(fd7, &g_driver.currentCar, 1) || read_cars(fd7, &g_driver.otherCars, 0) ||
        read_nodes(fd7, &g_driver.spareParts) || _dos_read(fd7, &g_betRaces, 8, &n) != 0 || n != 8 ||
        _dos_read(fd7, g_racerRecordsArray, 0x18c, &n) != 0 || n != 0x18c ||
        _dos_read(fd7, &g_gameClock.actionTicks, 4, &n) != 0 || n != 4 || _dos_read(fd7, &delta, 4, &n) != 0 ||
        n != 4 || _dos_read(fd7, &g_roadLayout, 2, &n) != 0 || n != 2) {
        cu_ctl(CURSOR_SHOW_NOW);
        mn_wait(KM_LOAD_ERROR);
        cu_ctl(CURSOR_HIDE);
        if (fd7 > -1) {
            _dos_close(fd7);
            init_new_game();
        }
        cu_ctl(CURSOR_SHOW);
        return LOAD_ERROR;
    }
    _dos_close(fd7);
    if (g_driver.currentCar)
        ga_palette(g_driver.currentCar->paintScheme);
    rd_build_layout(g_roadLayout);
    g_gameClock.timeBase = g_gameClock.biosTicks - delta;
    cu_ctl(CURSOR_SHOW_NOW);
    return LOAD_OK;
}

/* True if the string is only spaces and underscores */

static int near is_blank(char *p)
{
    char ch;

    while ((ch = *p++) != 0)
        if (ch != ' ' && ch != '_')
            return 0;
    return 1;
}

static char g_buyCarMsg[] = "Buy a car first !";


void near sg_save(void)
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
    cu_ctl(CURSOR_HIDE_NOW);
    scan_save_slots(1);
    r = r_vid.copyToFront;
    r_vid.copyToFront = 0;
    sc_picture(PIC_SAVE_LOAD_BOOK, 0);
    sc_picture_at(PIC_LICENSE_NAME_STRIP, 0x48, 0xa9, -1);
    sc_picture_at(PIC_SAVE_BUTTON, 0xf0, 100, -1);
    sc_picture_at(PIC_CANCEL_BUTTON, 0xf0, 0x8c, -1);
    r_vid.copyToFront = r;
    if (r)
        bm_copy_box(r_vid.back, r_vid.front, 0, 0, SCREEN_WIDTH, SCREEN_BODY_HEIGHT);
    strcpy(g_saveName, g_driverName);
again:
    for (r = 1; r <= g_list[0] && g_saveSlots[r]; r++)
        ;
    if (r > g_list[0])
        r = g_list[0];
    if (sel == 0)
        sel = r;
    cu_ctl(CURSOR_SHOW_NOW);
    switch (r = mn_list(KM_SAVE, sel, 0, g_list, 0, 0)) {
    case KR_CANCEL:
        return;
    default:
        if ((int)r < 1)
            return;
        if (r > g_list[0])
            return;
        break;
    case KR_ACCEPT:
        r = mn_sel();
        break;
    }
    sel = r;
    if (is_blank(g_saveName) && is_blank(g_desc + g_list[r])) {
        sc_msg_box(g_noNameMsgPtr);
        cu_ctl(CURSOR_HIDE_NOW);
        goto again;
    }
    for (i5 = 1; i5 <= 15 && (cmp = strcmp(g_saveName, g_desc + g_list[i5])) != 0; i5++)
        ;
    if (cmp == 0) {
        cu_ctl(CURSOR_HIDE);
        box7 = (struct Win *)(0x12 * (int)r + g_menuList.lineBase - 0x12);
        pal4 = x_colors;
        x_colors = bustyle->pal[((struct Win *)g_menuList.lineBase)->lines];
        sv7 = x_colors.line;
        x_colors.line = x_colors.textBg;
        wk = ((struct Win *)g_menuList.lineBase)->tx - ((struct Win *)g_menuList.lineBase)->w;
        dr_frame(r_vid.back, box7->w, box7->h, wk, 8);
        dr_frame(r_vid.front, box7->w, box7->h, wk, 8);
        x_colors = pal4;
        x_colors.line = sv7;
        cu_ctl(CURSOR_SHOW);
        sel = r = i5;
    }
    strcpy(name + 1, ":HOTROD");
    itoa(r, name + 8, 10);
    strcat(name + 1, ".SAV");
    key = g_saveSlots[r] ? mn_modal(KM_OVERWRITE_SAVE) : 0;
    cu_ctl(CURSOR_HIDE_NOW);
    switch (key) {
    case KR_OVERWRITE_FORGET:
        goto again;
    case KR_OVERWRITE_GO_AHEAD:
        unlink(name);
    }
    strcpy(g_driverName, is_blank(g_saveName) ? g_desc + g_list[r] : g_saveName);
    delta = g_gameClock.biosTicks - g_gameClock.timeBase;
    name[0] = di_data_drive();
    if (_dos_creat(name, _A_NORMAL, &fd8) || _dos_write(fd8, g_driverName, 0x24, &n) != 0 || n != 0x24 ||
        write_cars(fd8, g_driver.currentCar, 1) || write_cars(fd8, g_driver.otherCars, 0) ||
        write_nodes(fd8, g_driver.spareParts) || _dos_write(fd8, &g_betRaces, 8, &n) != 0 || n != 8 ||
        _dos_write(fd8, g_racerRecordsArray, 0x18c, &n) != 0 || n != 0x18c ||
        _dos_write(fd8, &g_gameClock.actionTicks, 4, &n) != 0 || n != 4 || _dos_write(fd8, &delta, 4, &n) != 0 ||
        n != 4 || _dos_write(fd8, &g_roadLayout, 2, &n) != 0 || n != 2) {
        cu_ctl(CURSOR_SHOW_NOW);
        mn_modal(KM_SAVE_ERROR);
        cu_ctl(CURSOR_HIDE_NOW);
        if (fd8 > -1) {
            _dos_close(fd8);
            unlink(name);
        }
        scan_save_slots(1);
    } else {
        _dos_close(fd8);
        cu_ctl(CURSOR_SHOW_NOW);
        return;
    }
    goto again;
}


int near sg_menu(void)
{
    struct Bitmap far *p;
    int row;
    int x;
    int y;

    p = mn_grab(KM_SAVE_RESTART_QUIT, &x, &y, ARENA_HIGH_STACK);
    row = mn_wait(KM_SAVE_RESTART_QUIT);
    pu_show(p, x, y);
    ma_pop_high(1);
    return row;
}
