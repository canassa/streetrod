#include <dos.h>
#include <string.h>
#include <conio.h>
#include "STRUCTS.H"
#include "MAIN.H"
#include "ARENA.H"
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
static int near show_video_menu(int mode);
static int near cpu_speed(void);
static int near choose_video(int mode);
static void near init_screens(int mode);
static void near set_video_mode(int mode);
static void near check_slow_cpu(void);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Video r_vid;

int g_hercules = 0;   /* DS:0042 */
int g_sysHooked = 0;   /* DS:0044 */
int g_0046 = 0;   /* DS:0046 */
int g_videoSet = 0;   /* DS:0048 */
int g_004a = 0;   /* DS:004A */
int g_004c = 0;   /* DS:004C */

/* dos_print @ 0000:0000 (linear 0x00000, 130 bytes) */

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

char *g_videoMenu[6] = {"                    1 - CGA      320 x 200 x  4 colors", "                    2 - EGA      320 x 200 x 16 colors", "                    3 - VGA      320 x 200 x 16 colors", "                    4 - Hercules monochrome", "                    5 - Tandy    320 x 200 x 16 colors", "\n                  Esc - Back to DOS"};   /* DS:0244 */
int g_egaCard = 0;   /* DS:0250 */
int g_forceEga = 0;   /* DS:0252 */
int g_vga = 0;   /* DS:0254 */

/* show_video_menu @ 0000:0082 (linear 0x00082, 265 bytes) */

static int near show_video_menu(int mode)
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
        sys_exit();
    if (choice <= 0 || choice > VIDEO_CHOICE_TANDY)
        return 0;
    return choice;
}

/* is_vga @ 0000:018B (linear 0x0018B, 70 bytes) */

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

/* main_set_mode @ 0000:01D1 (linear 0x001D1, 33 bytes) */

void main_set_mode(int ax)
{
    union REGS r;

    r.x.ax = ax;
    int86(BIOS_VIDEO, &r, &r);
}

/* main_set_palette @ 0000:01F2 (linear 0x001F2, 52 bytes) */

void main_set_palette(unsigned char color)
{
    union REGS r;

    r.x.ax = VIDEO_SET_PALETTE << 8;
    r.h.bh = CGA_SELECT_PALETTE;
    r.h.bl = color;
    if (r_vid.mode != -4)
        int86(BIOS_VIDEO, &r, &r);
}

int g_useMouse = 1;   /* DS:0262 */

/* choose_video @ 0000:0226 (linear 0x00226, 240 bytes) */

static int near choose_video(int mode)
{
    int herc;
    int pick;

    herc = 0;
    viddet_detect();
    if (g_forceEga)
        g_egaCard = 1;
    if (hercdet_bios_mono() && (g_egaCard == 1 || hercdet_detect())) {
        g_egaCard = 0;
        herc = 1;
    }
    if (g_egaCard)
        mode = g_egaMemory == 3 ? VIDEO_CHOICE_EGA : 6;
    else
        mode = VIDEO_CHOICE_CGA;
    if (herc)
        mode = VIDEO_CHOICE_HERCULES;
    if (memstr_find("Tandy", BIOS_ROM, 0xffff) ||
        memstr_find("TANDY", BIOS_ROM, 0xffff))
        mode = VIDEO_CHOICE_TANDY;
    if (is_vga())
        mode = VIDEO_CHOICE_VGA;
    pick = show_video_menu(mode);
    if (pick)
        mode = pick;
    if (mode == VIDEO_CHOICE_CGA && g_egaCard)
        return 6;
    if (mode == VIDEO_CHOICE_VGA) {
        g_vga = 1;
        return VIDEO_CHOICE_EGA;
    }
    return mode;
}

/* init_screens @ 0000:0316 (linear 0x00316, 478 bytes) */

static void near init_screens(int mode)
{
    switch (mode) {
    case VIDEO_CHOICE_VGA:
        g_vga = 1;
    case VIDEO_CHOICE_EGA:
        r_vid.mode = -2;
        SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80);
        ega_install();
        return;
    case VIDEO_CHOICE_TANDY:
        r_vid.mode = g_screens[0].flags = 0xfa;
        tandy_install();
        g_screens[0].bits.p = r_vid.mode == -4 ? MONO_VRAM : CGA_VRAM;
        g_screens[0].planesize = g_screens[1].planesize = g_screens[2].planesize = 0x8000;
        g_screens[0].pitch = g_screens[1].pitch = g_screens[2].pitch = 0xa0;
        g_screens[1].flags = g_screens[2].flags = 0xfa;
        g_screens[1].bits.p = TANDY_SCREEN1_BITS;
        g_screens[2].bits.p = mem_alloc(0x8000, 1);
        dash_fmemset(g_screens[1].bits.p, 0, 0x8000);
        dash_fmemset(g_screens[2].bits.p, 0, 0x8000);
        return;
    case VIDEO_CHOICE_HERCULES:
        g_screens[0].flags = 0xfc;
        r_vid.mode = -4;
        break;
    case VIDEO_CHOICE_CGA:
        g_screens[0].flags = 0xfd;
        break;
    case 6:
        g_screens[0].flags = 0xfd;
        SET_EQUIP_VIDEO(EQUIP_VIDEO_COLOR80);
        break;
    }
    cga_install();
    g_screens[0].bits.p = r_vid.mode == -4 ? MONO_VRAM : CGA_VRAM;
    g_screens[0].planesize = g_screens[1].planesize = 0x4000;
    g_screens[0].pitch = g_screens[1].pitch = 0x50;
    g_screens[1].flags = 0xfd;
    g_screens[1].bits.p = mem_alloc(0x4000, 1);
    dash_fmemset(g_screens[1].bits.p, 0, 0x4000);
    g_screens[2].bits.p = mem_alloc(0x4000, 1);
    dash_fmemset(g_screens[2].bits.p, 0, 0x4000);
}

/* set_video_mode @ 0000:04F4 (linear 0x004F4, 210 bytes) */

static void near set_video_mode(int mode)
{
    int far *p;

    switch (mode) {
    case VIDEO_CHOICE_EGA:
    case VIDEO_CHOICE_VGA:
        main_set_mode(MODE_EGA_320_200_16);
        break;
    case VIDEO_CHOICE_TANDY:
        main_set_mode(MODE_TANDY_320_200_16);
        outp(TANDY_PAGE, 0xf6);
        break;
    case VIDEO_CHOICE_HERCULES:
        hercgfx_set_mode();
        p = r_vid.mode == -4 ? (int far *)MONO_VRAM : (int far *)CGA_VRAM;
        while (p < (int far *)((r_vid.mode == -4 ? (long)MONO_VRAM : (long)CGA_VRAM) + 0x8000L))
            *p++ = 0;
        outp(MONO_MODE, (MODE_CTRL_GRAPHICS | MODE_CTRL_VIDEO | MODE_CTRL_BLINK));
        g_0046 = 1;
        g_hercules = 1;
        break;
    case VIDEO_CHOICE_CGA:
    case 6:
        main_set_mode(MODE_CGA_320_200_4);
        main_set_palette(CGA_PALETTE_CYAN_MAGENTA_WHITE);
        break;
    }
    g_videoSet = 1;
}

/* cpu_speed @ 0000:05C6 (linear 0x005C6, 147 bytes) */

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

int by70e2;
struct Bitmap far *g755cu;
char far *g_imageBufferPointer;
char g_graphicsUp;
int bl82c6;
int bu8acc;
int ag8bcet;

/* check_slow_cpu @ 0000:0659 (linear 0x00659, 22 bytes) */

static void near check_slow_cpu(void)
{
    if (bu8acc < 4)
        bl82c6 = 1;
    else
        bl82c6 = 0;
}

int g_noUeMem = 0;   /* DS:0280 */
int g_end = 0;   /* DS:0282 */
int g_scene = 0;   /* DS:0284 */
int g_carAnim = 0;   /* DS:0286 */
struct Bitmap far *g_spritePic = 0;   /* DS:0288 */
struct Bitmap far *g_pumpWithNozzle = 0;   /* DS:028C */
struct Bitmap far *g_pumpNoNozzle = 0;   /* DS:0290 */
struct Bitmap far *g_0294 = 0;   /* DS:0294 */
int g_carBandHeights[7] = {0, 19, 31, 29, 0, 0, 30};   /* DS:0298 */
int g_carBottomCuts[7] = {0, 0, 8, 0, 0, 0, 9};   /* DS:02A6 */
int g_carBg4[7] = {0, 2, 0, 2, 0, 0, 0};   /* DS:02B4 */
int g_carBg16[7] = {0, 3, 1, 3, 0, 3, 1};   /* DS:02C2 */
int g_spriteBottomCut = 0;   /* DS:02D0 */

/* main @ 0000:066F (linear 0x0066F, 580 bytes) */

int main(int argc, char **argv)
{
    int e;
    int b;
    int c;
    int d;
    int a;

    g_graphicsUp = 0;
    bu8acc = cpu_speed();
    e = 2;
    b = 0;
    g_useMouse = 1;
    ag8bcet = 0;
    g_autoDrive = 0;
    for (c = 1; c < argc; c++) {
        argv++;
        if (memcmp(*argv, "1", 2) == 0)
            e = 1;
        else if (strcmpi(*argv, "nomouse") == 0)
            g_useMouse = 0;
        else if (strcmpi(*argv, "demo") == 0)
            ag8bcet = 1;
        else if (strcmpi(*argv, "auto") == 0)
            g_autoDrive = 1;
        else if (strcmpi(*argv, "nouemem") == 0)
            g_noUeMem = 1;
    }
    b = choose_video(b);
    mem_init();
    arena_init();
    init_screens(b);
    set_video_mode(b);
    video_init(g_screens, &g_screen1);
    sys_init();
    if (r_vid.mode == -2 || r_vid.mode == -6)
        video_set_palette(g_palette);
    disk_open_libs(e, b);
    check_slow_cpu();
    d = disk_read_index(1);
    cursor_init();
    sound_init();
    dash_set_view(0);
    if (r_vid.mode == -3 && g_egaCard)
        g755cu = mem_new_bitmap(SCREEN_W, DASH_H, 0, -1, 0);
    g_imageBufferPointer = mem_alloc(r_vid.mode != -2 && r_vid.mode != -6 ? 16000 : 18000, 1);
    a = disk_load_list(g_titlePics, 0);
    if (a == 3) {
        by70e2 = 1;
        d = disk_read_index(2);
        disk_load_data();
        if (r_vid.mode == -2)
            disk_cache_list(g_gamePics);
        disk_load_list(g_gamePics, 1);
    } else {
        by70e2 = 0;
        disk_free_list(g_titlePics);
    }
    game_run();
    sys_exit();
}
