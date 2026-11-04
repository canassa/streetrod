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
    inr.h.ah = 9;
    inr.x.dx = (int)text;
    int86(0x21, &inr, &outr);
}

char *g_videoMenu[6] = {"                    1 - CGA      320 x 200 x  4 colors", "                    2 - EGA      320 x 200 x 16 colors", "                    3 - VGA      320 x 200 x 16 colors", "                    4 - Hercules monochrome", "                    5 - Tandy    320 x 200 x 16 colors", "\n                  Esc - Back to DOS"};   /* DS:0244 */
int g_egaCard = 0;   /* DS:0250 */
int g_0252 = 0;   /* DS:0252 */
int g_vga = 0;   /* DS:0254 */

/* show_video_menu @ 0000:0082 (linear 0x00082, 265 bytes) */

static int near show_video_menu(int mode)
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
    if (choice <= 0 || choice > 5)
        return 0;
    return choice;
}

/* is_vga @ 0000:018B (linear 0x0018B, 70 bytes) */

static int near is_vga(void)
{
    union REGS r;
    int s;
    int c;

    r.h.ah = 0x1a;
    r.h.al = 0;
    int86(0x10, &r, &r);
    s = r.h.al;
    c = r.h.bl;
    return s && c == 8;
}

/* main_set_mode @ 0000:01D1 (linear 0x001D1, 33 bytes) */

void main_set_mode(int ax)
{
    union REGS r;

    r.x.ax = ax;
    int86(0x10, &r, &r);
}

/* main_set_palette @ 0000:01F2 (linear 0x001F2, 52 bytes) */

void main_set_palette(unsigned char color)
{
    union REGS r;

    r.x.ax = 0x0b00;
    r.h.bh = 1;
    r.h.bl = color;
    if (r_vid.mode != -4)
        int86(0x10, &r, &r);
}

int g_useMouse = 1;   /* DS:0262 */

/* choose_video @ 0000:0226 (linear 0x00226, 240 bytes) */

static int near choose_video(int mode)
{
    int herc;
    int pick;

    herc = 0;
    viddet_detect();
    if (g_0252)
        g_egaCard = 1;
    if (hercdet_bios_mono() && (g_egaCard == 1 || hercdet_detect())) {
        g_egaCard = 0;
        herc = 1;
    }
    if (g_egaCard)
        mode = g_egaMemory == 3 ? 2 : 6;
    else
        mode = 1;
    if (herc)
        mode = 4;
    if (memstr_find("Tandy", BIOS_ROM, 0xffff) ||
        memstr_find("TANDY", BIOS_ROM, 0xffff))
        mode = 5;
    if (is_vga())
        mode = 3;
    pick = show_video_menu(mode);
    if (pick)
        mode = pick;
    if (mode == 1 && g_egaCard)
        return 6;
    if (mode == 3) {
        g_vga = 1;
        return 2;
    }
    return mode;
}

/* init_screens @ 0000:0316 (linear 0x00316, 478 bytes) */

static void near init_screens(int mode)
{
    switch (mode) {
    case 3:
        g_vga = 1;
    case 2:
        r_vid.mode = -2;
        *BIOS_EQUIP &= 0xcf;
        *BIOS_EQUIP |= 0x20;
        ega_install();
        return;
    case 5:
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
    case 4:
        g_screens[0].flags = 0xfc;
        r_vid.mode = -4;
        break;
    case 1:
        g_screens[0].flags = 0xfd;
        break;
    case 6:
        g_screens[0].flags = 0xfd;
        *BIOS_EQUIP &= 0xcf;
        *BIOS_EQUIP |= 0x20;
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
    case 2:
    case 3:
        main_set_mode(0xd);
        break;
    case 5:
        main_set_mode(9);
        outp(0x3df, 0xf6);
        break;
    case 4:
        hercgfx_set_mode();
        p = r_vid.mode == -4 ? (int far *)MONO_VRAM : (int far *)CGA_VRAM;
        while (p < (int far *)((r_vid.mode == -4 ? (long)MONO_VRAM : (long)CGA_VRAM) + 0x8000L))
            *p++ = 0;
        outp(0x3b8, 0x2a);
        g_0046 = 1;
        g_hercules = 1;
        break;
    case 1:
    case 6:
        main_set_mode(4);
        main_set_palette(1);
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
char far *ab7564z;
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
int g_0282 = 0;   /* DS:0282 */
int g_0284 = 0;   /* DS:0284 */
int g_carAnim = 0;   /* DS:0286 */
struct Bitmap far *g_0288 = 0;   /* DS:0288 */
struct Bitmap far *g_028c = 0;   /* DS:028C */
struct Bitmap far *g_0290 = 0;   /* DS:0290 */
struct Bitmap far *g_0294 = 0;   /* DS:0294 */
int g_0298[7] = {0, 19, 31, 29, 0, 0, 30};   /* DS:0298 */
int g_02a6[7] = {0, 0, 8, 0, 0, 0, 9};   /* DS:02A6 */
int g_02b4[7] = {0, 2, 0, 2, 0, 0, 0};   /* DS:02B4 */
int g_02c2[7] = {0, 3, 1, 3, 0, 3, 1};   /* DS:02C2 */
int g_02d0 = 0;   /* DS:02D0 */

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
        g755cu = mem_new_bitmap(320, 100, 0, -1, 0);
    ab7564z = mem_alloc(r_vid.mode != -2 && r_vid.mode != -6 ? 16000 : 18000, 1);
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
