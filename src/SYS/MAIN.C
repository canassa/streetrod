#include <dos.h>
#include <string.h>
#include <conio.h>
#include "MAIN.H"
#include "ARENA.H"
#include "CARPIC.H"
#include "CGA.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "EGA.H"
#include "GAME.H"
#include "HERCDET.H"
#include "HERCGFX.H"
#include "MEM.H"
#include "MEMSTR.H"
#include "PC.H"
#include "SCREENS.H"
#include "SOUND.H"
#include "SYS.H"
#include "TANDY.H"
#include "VIDDET.H"
#include "VIDEO.H"

#define TANDY_SCREEN1_BITS ((char far *)0x90000000L)

static void near dos_print(char *s);
static int near is_vga(void);
static int near show_video_menu(enum VideoChoice mode);
static int near cpu_speed(void);
static int near choose_video(enum VideoChoice mode);
static void near init_screens(enum VideoChoice mode);
static void near set_video_mode(enum VideoChoice mode);
static void near check_slow_cpu(void);

struct Video r_vid;

int g_hercules = 0;
int g_sysHooked = 0;
int g_hercModeSet = 0;
int g_videoSet = 0;
int g_unused1 = 0;
int g_dashViewSaved = 0;


static void near dos_print(char *s)
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
    inr.h.ah = DOS_WRITE_STRING;
    inr.x.dx = (int)text;
    int86(DOS_INT, &inr, &outr);
}

char *g_videoMenu[6] = {
    "                    1 - CGA      320 x 200 x  4 colors", "                    2 - EGA      320 x 200 x 16 colors",
    "                    3 - VGA      320 x 200 x 16 colors", "                    4 - Hercules monochrome",
    "                    5 - Tandy    320 x 200 x 16 colors", "\n                  Esc - Back to DOS"};
int g_egaCard = 0;
int g_forceEga = 0;
int g_vga = 0;


static int near show_video_menu(enum VideoChoice mode)
{
    union REGS r;
    char rec[84];
    int choice;

    if (mode < VIDEO_CHOICE_CGA || mode > VIDEO_CHOICE_TANDY)
        mode = VIDEO_CHOICE_CGA;
    r.x.ax = (VIDEO_SET_MODE << 8) | MODE_TEXT80;
    int86(BIOS_VIDEO, &r, &r);
    memcpy(rec, "\n           Recommended mode: #", 32);
    strcat(rec, g_videoMenu[mode - 1] + 20);
    dos_print("\n\n\n\n\n\n                    STREET ROD:  Video Options\n");
    dos_print("\n              Use numeric keys to select graphic mode");
    dos_print("           or press any other key to accept recommendation\n");

    for (choice = 0; choice < 6; choice++)
        dos_print(g_videoMenu[choice]);
    dos_print(rec);
    while (kbhit())
        getch();
    while (!kbhit())
        ;
    choice = getch() - '0';
    if (choice == -21)
        sy_exit();
    if (choice <= 0 || choice > VIDEO_CHOICE_TANDY)
        return 0;
    return choice;
}


static int near is_vga(void)
{
    union REGS r;
    int s;
    int c;

    r.h.ah = VIDEO_GET_DCC;
    r.h.al = DCC_READ;
    int86(BIOS_VIDEO, &r, &r);
    s = r.h.al;
    c = r.h.bl;
    return s && c == DCC_VGA_COLOR;
}


void mi_set_mode(int ax)
{
    union REGS r;

    r.x.ax = ax;
    int86(BIOS_VIDEO, &r, &r);
}


void mi_set_palette(unsigned char color)
{
    union REGS r;

    r.x.ax = VIDEO_SET_PALETTE << 8;
    r.h.bh = CGA_SELECT_PALETTE;
    r.h.bl = color;
    if (r_vid.mode != VID_HERCULES)
        int86(BIOS_VIDEO, &r, &r);
}

int g_useMouse = 1;


static int near choose_video(enum VideoChoice mode)
{
    int herc;
    int pick;

    herc = 0;
    vd_detect();
    if (g_forceEga)
        g_egaCard = 1;
    if (hd_bios_mono() && (g_egaCard == 1 || hd_detect())) {
        g_egaCard = 0;
        herc = 1;
    }
    if (g_egaCard)
        mode = g_egaMemory == 3 ? VIDEO_CHOICE_EGA : VIDEO_CHOICE_CGA_ON_EGA;
    else
        mode = VIDEO_CHOICE_CGA;
    if (herc)
        mode = VIDEO_CHOICE_HERCULES;
    if (ms_find("Tandy", BIOS_ROM, 0xffff) || ms_find("TANDY", BIOS_ROM, 0xffff))
        mode = VIDEO_CHOICE_TANDY;
    if (is_vga())
        mode = VIDEO_CHOICE_VGA;
    pick = show_video_menu(mode);
    if (pick)
        mode = pick;
    if (mode == VIDEO_CHOICE_CGA && g_egaCard)
        return VIDEO_CHOICE_CGA_ON_EGA;
    if (mode == VIDEO_CHOICE_VGA) {
        g_vga = 1;
        return VIDEO_CHOICE_EGA;
    }
    return mode;
}


static void near init_screens(enum VideoChoice mode)
{
    switch (mode) {
    case VIDEO_CHOICE_VGA:
        g_vga = 1;
    case VIDEO_CHOICE_EGA:
        r_vid.mode = VID_EGA;
        SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80);
        eg_install();
        return;
    case VIDEO_CHOICE_TANDY:
        r_vid.mode = g_screens[0].flags = VID_TANDY;
        ta_install();
        g_screens[0].bits.p = r_vid.mode == VID_HERCULES ? MONO_VRAM : CGA_VRAM;
        g_screens[0].planesize = g_screens[1].planesize = g_screens[2].planesize = 0x8000;
        g_screens[0].pitch = g_screens[1].pitch = g_screens[2].pitch = 0xa0;
        g_screens[1].flags = g_screens[2].flags = VID_TANDY;
        g_screens[1].bits.p = TANDY_SCREEN1_BITS;
        g_screens[2].bits.p = me_alloc(0x8000, 1);
        da_fmemset(g_screens[1].bits.p, 0, 0x8000);
        da_fmemset(g_screens[2].bits.p, 0, 0x8000);
        return;
    case VIDEO_CHOICE_HERCULES:
        g_screens[0].flags = VID_HERCULES;
        r_vid.mode = VID_HERCULES;
        break;
    case VIDEO_CHOICE_CGA:
        g_screens[0].flags = VID_CGA;
        break;
    case VIDEO_CHOICE_CGA_ON_EGA:
        g_screens[0].flags = VID_CGA;
        SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80);
        break;
    }
    cg_install();
    g_screens[0].bits.p = r_vid.mode == VID_HERCULES ? MONO_VRAM : CGA_VRAM;
    g_screens[0].planesize = g_screens[1].planesize = 0x4000;
    g_screens[0].pitch = g_screens[1].pitch = 0x50;
    g_screens[1].flags = VID_CGA;
    g_screens[1].bits.p = me_alloc(0x4000, 1);
    da_fmemset(g_screens[1].bits.p, 0, 0x4000);
    g_screens[2].bits.p = me_alloc(0x4000, 1);
    da_fmemset(g_screens[2].bits.p, 0, 0x4000);
}


static void near set_video_mode(enum VideoChoice mode)
{
    int far *p;

    switch (mode) {
    case VIDEO_CHOICE_EGA:
    case VIDEO_CHOICE_VGA:
        mi_set_mode(MODE_EGA_320_200_16);
        break;
    case VIDEO_CHOICE_TANDY:
        mi_set_mode(MODE_TANDY_320_200_16);
        outp(TANDY_PAGE, 0xf6);
        break;
    case VIDEO_CHOICE_HERCULES:
        hg_set_mode();
        p = r_vid.mode == VID_HERCULES ? (int far *)MONO_VRAM : (int far *)CGA_VRAM;
        while (p < (int far *)((r_vid.mode == VID_HERCULES ? (long)MONO_VRAM : (long)CGA_VRAM) + 0x8000L))
            *p++ = 0;
        outp(MONO_MODE, (MODE_CTRL_GRAPHICS | MODE_CTRL_VIDEO | MODE_CTRL_BLINK));
        g_hercModeSet = 1;
        g_hercules = 1;
        break;
    case VIDEO_CHOICE_CGA:
    case VIDEO_CHOICE_CGA_ON_EGA:
        mi_set_mode(MODE_CGA_320_200_4);
        mi_set_palette(CGA_PALETTE_CYAN_MAGENTA_WHITE);
        break;
    }
    g_videoSet = 1;
}


static int near cpu_speed(void)
{
    long count;
    long prev;

    count = 0;
    prev = *BIOS_TICKS;
    while (prev == *BIOS_TICKS)
        ;
    prev = *BIOS_TICKS;
    while (prev == *BIOS_TICKS)
        count++;
    count /= 95;
    count /= 10;
    return count > 1 ? (int)count : 1;
}

int g_loadedIntroBmps;
struct Bitmap far *g_bufBackgroundDashPtr;
char far *g_imageBufferPointer;
char g_graphicsUp;
int g_hasSlowCpu;
int g_processorSpeedRating;
int g_attractSwitchStartup;


static void near check_slow_cpu(void)
{
    if (g_processorSpeedRating < 4)
        g_hasSlowCpu = 1;
    else
        g_hasSlowCpu = 0;
}

int g_noUeMem = 0;
int g_end = 0;
enum Scene g_scene = SCENE_NONE;
int g_carAnim = 0;
struct Bitmap far *g_spritePic = 0;
struct Bitmap far *g_pumpWithNozzle = 0;
struct Bitmap far *g_pumpNoNozzle = 0;
struct Bitmap far *g_stickerBg = 0;
int g_carBandHeights[7] = {0, 19, 31, 29, 0, 0, 30};
int g_carBottomCuts[7] = {0, 0, 8, 0, 0, 0, 9};
int g_carBg4[7] = {0, 2, 0, 2, 0, 0, 0};
int g_carBg16[7] = {0, 3, 1, 3, 0, 3, 1};
int g_spriteBottomCut = 0;


int main(int argc, char **argv)
{
    int e;
    int b;
    int c;
    int d;
    int a;

    g_graphicsUp = 0;
    g_processorSpeedRating = cpu_speed();
    e = 2;
    b = 0;
    g_useMouse = 1;
    g_attractSwitchStartup = 0;
    g_autoDrive = 0;
    for (c = 1; c < argc; c++) {
        argv++;
        if (memcmp(*argv, "1", 2) == 0)
            e = 1;
        else if (strcmpi(*argv, "nomouse") == 0)
            g_useMouse = 0;
        else if (strcmpi(*argv, "demo") == 0)
            g_attractSwitchStartup = 1;
        else if (strcmpi(*argv, "auto") == 0)
            g_autoDrive = 1;
        else if (strcmpi(*argv, "nouemem") == 0)
            g_noUeMem = 1;
    }
    b = choose_video(b);
    me_init();
    ar_init();
    init_screens(b);
    set_video_mode(b);
    vi_init(g_screens, &g_screen1);
    sy_init();
    if (r_vid.mode == VID_EGA || r_vid.mode == VID_TANDY)
        vi_set_palette(g_palette);
    di_open_libs(e, b);
    check_slow_cpu();
    d = di_read_index(1);
    cu_init();
    so_init();
    da_set_view(DASH_VIEW_SAVE);
    if (r_vid.mode == VID_CGA && g_egaCard)
        g_bufBackgroundDashPtr = me_new_bitmap(SCREEN_WIDTH, DASH_HEIGHT, 0, -1, 0);
    g_imageBufferPointer = me_alloc(r_vid.mode != VID_EGA && r_vid.mode != VID_TANDY ? 16000 : 18000, 1);
    a = di_load_list(g_titlePics, 0);
    if (a == 3) {
        g_loadedIntroBmps = 1;
        d = di_read_index(2);
        di_load_data();
        if (r_vid.mode == VID_EGA)
            di_cache_list(g_gamePics);
        di_load_list(g_gamePics, 1);
    } else {
        g_loadedIntroBmps = 0;
        di_free_list(g_titlePics);
    }
    gm_run();
    sy_exit();
}
