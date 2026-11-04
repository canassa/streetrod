#include <conio.h>
#include <dos.h>
#include "STRUCTS.H"
#include "VIDEO.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "DRAW.H"
#include "MAIN.H"
#include "MENU.H"
#include "PC.H"
#include "SCRATCH.H"
#include "SCREENS.H"
#include "SOUND.H"
#include "SPRITE.H"
#include "SYS.H"

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static struct Cmd *g_cmd;                  /* DS:6C38 */
static struct Obj *g_obj;                  /* DS:6C3A */
static struct Box *g_box;                  /* DS:6C3C */
static int g_blinkCount;                        /* DS:6C3E */
static void near anim_set_box(void);
static void near anim_run(int n);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Colors x_colors;
struct Bitmap far *g_pageBmps[4];
struct Obj bm7568[5];
struct Style *bustyle;
int (far *ab78aat)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
struct Bitmap far *bu825a;
int ac825et;
int aw8260y;
int d_8266;
int ap8268t;

struct Style g_style16 = {{{""}, {{13, 9, 9, 15, 1}}, {{9, 13, 13, 15, 1}}, {{1, 5, 5, 1, 1}}, {{1, 5, 5, 4, 1}}, {{15, 9, 9, 15, 1}}, {{14, 15, 15, 10, 1}}}, {{1, 4, 4, 1, 1}}, {{1, 3, 3, 4, 2}}, {{15, 9, 9, 15, 1}}, {{13, 9, 9, 15, 1}}, {{11, 2, 2, 11, 1}}, {1, 4, 4, 11, 1}, {{1, 4, 4, 1, 1}}, {{1, 4, 4, 1, 1}}, {{1, 3, 3, 4, 1}}};   /* DS:03A0 */
struct Style g_style4 = {{{""}, {{1, 2, 2, 3, 1}}, {{2, 1, 1, 3, 1}}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 2, 1}}, {{3, 2, 2, 3, 1}}, {{2, 1, 1, 1, 1}}}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 2, 2}}, {{3, 2, 2, 3, 1}}, {{1, 2, 2, 3, 1}}, {{0, 2, 2, 0, 1}}, {0, 3, 3, 1, 1}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 0, 1}}, {{1, 2, 2, 3, 1}}};   /* DS:03F0 */
unsigned char g_palette[16] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14};   /* DS:0440 */
int g_palTail = 0;   /* DS:0450 */
char g_blackPalette[16] = "";   /* DS:0452 */
unsigned char g_altPalette[16] = {9, 1, 8, 0, 15, 7, 11, 3, 14, 2, 4, 12, 13, 10, 6, 14};   /* DS:0462 */
int g_altPalTail = 0;   /* DS:0472 */

/* F_10B7C @ 0F38:17FC (linear 0x10B7C, 16 bytes) */

void F_10B7C(void)
{
    F_112CB(12, 5);
}

/* video_bobs_pal @ 0F38:180C (linear 0x10B8C, 16 bytes) */

void video_bobs_pal(void)
{
    F_112CB(12, 13);
}

/* video_name_pal @ 0F38:181C (linear 0x10B9C, 16 bytes) */

void video_name_pal(void)
{
    g_palette[6] = 9;
    g_palette[7] = 1;
    g_palette[8] = 3;
}

/* video_swap_pal @ 0F38:182C (linear 0x10BAC, 45 bytes) */

void video_swap_pal(void)
{
    int i;
    unsigned char save;

    for (i = 0; i < 17; i++) {
        save = g_palette[i];
        g_palette[i] = g_palette[i + 34];
        g_palette[i + 34] = save;
    }
}

/* video_init @ 0F38:1859 (linear 0x10BD9, 403 bytes) */

void video_init(struct Bitmap far *front, struct Bitmap far *back)
{
    int i;

    r_vid.front = front;
    r_vid.back = back;
    r_vid.f8 = 0;
    r_vid.f16 = 1;
    switch (r_vid.mode = front->flags) {
    case -2:
        r_vid.planes = 4;
        r_vid.shift = 3;
        r_vid.bpp = 1;
        r_vid.mask = -8;
        bustyle = &g_style16;
        outp(CRTC_INDEX, CRTC_START_HI);
        outp(CRTC_DATA, 0);
        outp(CRTC_INDEX, CRTC_START_LO);
        outp(CRTC_DATA, 0);
        break;
    default:
        r_vid.planes = 1;
        r_vid.shift = 2;
        r_vid.bpp = 2;
        r_vid.mask = -4;
        bustyle = &g_style4;
        r_vid.front->flags = -3;
        break;
    case -6:
        r_vid.planes = 1;
        r_vid.shift = 1;
        r_vid.bpp = 4;
        r_vid.mask = -2;
        bustyle = &g_style16;
        break;
    }
    x_colors = bustyle->pal[1];
    d_8266 = ap8268t = -1;
    i = 5;
    do {
        i--;
        bm7568[i].id = bm7568[i].f2 = -1;
    } while (i);
    (*m_present)();
    menu_reset();
    g_graphicsUp = 1;
    g_pageBmps[0] = r_vid.front;
    g_pageBmps[1] = r_vid.back;
    g_pageBmps[2] = &g_screens[2];
    if (r_vid.mode == -2) {
        *g_pageBmps[2] = *r_vid.back;
        g_pageBmps[2]->bits.w.off = 0x4000;
        g_pageBmps[2]->bits.w.seg = 0xa000;
        g_pageBmps[3] = &g_egaPage3;
        *g_pageBmps[3] = *r_vid.back;
        g_pageBmps[3]->bits.w.off = 0x6000;
        g_pageBmps[3]->bits.w.seg = 0xa000;
    }
}

/* video_anim_stop @ 0F38:19EC (linear 0x10D6C, 115 bytes) */

void video_anim_stop(int n)
{
    struct Obj *o;

    o = &bm7568[g_animScripts[g_animScriptStart[n]].a];
    if (o->img)
        o->img = 0;
    if (o->fc)
        o->fc = 0;
    g_animScripts[g_animScriptStart[n]].a = -1;
    o->id = -1;
    if (o->f4)
        sys_timer_cancel(n);
}

/* anim_set_box @ 0F38:1A5F (linear 0x10DDF, 94 bytes) */

static void near anim_set_box(void)
{
    switch (g_cmd->sub) {
    case 0:
        break;
    case 1:
        g_box->x1 = g_cmd->a;
        g_box->y1 = g_cmd->b;
        break;
    case 3:
        g_box->x1 = g_cmd->a;
        g_box->y1 = g_cmd->b;
    case 2:
        g_box->x2 = g_cmd->a;
        g_box->y2 = g_cmd->b;
        break;
    }
}

/* anim_run @ 0F38:1ABD (linear 0x10E3D, 780 bytes)

   Run the script of one Obj slot */

static void near anim_run(int n)
{
    int k;
    int wait;
    int x1, y, tx, ty;
    struct Rect res;
    struct Blt blt;

    wait = 1;
    cursor_ctl(-3);
    g_obj = &bm7568[n];
    g_cmd = &g_animScripts[k = g_obj->f2];
    g_box = &g_obj->box;
    for (; k >= 0; k++) {
        switch (g_cmd->op) {
        case 1:
        case 3:
            break;
        case 2:
            anim_set_box();
            (*b_78a2)(g_obj->img, r_vid.back, (struct Blt *)g_box, 1);
            break;
        case 4:
            anim_set_box();
            (*ab78aat)(g_obj->img, r_vid.back, (struct Blt *)g_box, 1);
            break;
        case 13:
            if (!sprite_car_clip(g_box->x2, g_box->y2, g_box->w, g_box->h, &res))
                break;
            blt.w = res.x2 - res.x1;
            blt.h = res.y2 - res.y1 + 1;
            blt.sx = res.x1 - ac825et;
            blt.sy = res.y1 - aw8260y;
            tx = res.x1 - g_box->x2;
            ty = res.y1 - g_box->y2;
            for (y = 0; y < g_obj->img->height; y += g_box->h) {
                for (x1 = 0; x1 < g_obj->img->width; x1 += g_box->w) {
                    blt.dx = tx + x1;
                    blt.dy = ty + y;
                    (*ab78aat)(bu825a, g_obj->img, &blt, 1);
                }
            }
            break;
        case 5:
            if (!video_anim_hook(g_obj, g_cmd))
                break;
        case 6:
            video_anim_stop(g_obj->id);
            cursor_ctl(-1);
            return;
        case 7:
            g_cmd = &g_animScripts[g_cmd->a];
            goto next;
        case 8:
            g_obj->img = F_15BBE(g_cmd->a, 2);
            g_cmd++;
            continue;
        case 9:
            anim_set_box();
            break;
        case 10:
            g_box->w = g_cmd->a;
            g_box->h = g_cmd->b;
            break;
        case 11:
            if (!(g_obj->f2 = g_cmd->b))
                g_obj->f2 = ++k;
            sys_timer_anim(g_obj->id, g_cmd->a);
            g_obj->f4++;
            if (wait)
                cursor_ctl(-1);
            if (g_cmd->sub == 4 && r_vid.f16)
                bitmap_copy_box(r_vid.back, r_vid.front, g_box->x2, g_box->y2,
                        g_box->x2 + g_box->w - 1, g_box->y2 + g_box->h - 1);
            return;
        case 12:
            cursor_ctl(-1);
            wait = 0;
            if (!r_vid.f16)
                break;
            bitmap_copy_box(r_vid.back, r_vid.front, g_box->x2, g_box->y2,
                    g_box->x2 + g_box->w - 1, g_box->y2 + g_box->h - 1);
            break;
        }
        g_cmd++;
    next: ;
    }
}

/* video_anim_start @ 0F38:1DC9 (linear 0x11149, 120 bytes) */

int video_anim_start(int n)
{
    int i;

    for (i = 0; i < 5; i++) {
        if (bm7568[i].id == -1) {
            g_animScripts[g_animScriptStart[n]].a = i;
            bm7568[i].id = n;
            bm7568[i].f2 = g_animScriptStart[n] + 1;
            bm7568[i].f4 = 0;
            anim_run(i);
            break;
        }
    }
    return i;
}

/* video_anim_resume @ 0F38:1E41 (linear 0x111C1, 49 bytes) */

void video_anim_resume(int n)
{
    int k;

    k = g_animScripts[g_animScriptStart[n]].a;
    bm7568[k].f4--;
    anim_run(k);
}

/* video_anim_active @ 0F38:1E72 (linear 0x111F2, 33 bytes) */

int video_anim_active(int n)
{
    return g_animScripts[g_animScriptStart[n]].a >= 0;
}

/* video_anim_hook @ 0F38:1E93 (linear 0x11213, 184 bytes) */

int video_anim_hook(struct Obj *o, struct Cmd *c)
{
    int old;

    switch (o->id) {
    case 2:
        if (c->a) {
            g_blinkCount = c->a;
            sound_pump(1);
            return 0;
        }
        if (g_blinkCount--) {
            struct Colors save;     /* PROVISIONAL: never used; fills the 6-byte frame slot (original local unknown) */
            old = x_colors.c[3];
            x_colors.c[3] = (g_blinkCount & 1) ? 4 : 0;
            draw_hline(r_vid.back, 0x33, 0x6f, 2);
            if (!(g_blinkCount & 3)) {
                x_colors.c[3] = (g_blinkCount & 4) ? 4 : 0;
                draw_hline(r_vid.back, 0x30, 0x6f, 2);
            }
            return 0;
        }
        sound_pump(0);
        return 1;
    }
}

/* F_112CB @ 0F38:1F4B (linear 0x112CB, 89 bytes)

   Set EGA palette register reg to color (BIOS int 10h function 10h), waiting for vertical retrace */

void F_112CB(int reg, char color)
{
    union REGS r;
    struct SREGS s;

    if (color > 7)
        color |= 0x10;
    g_palette[reg] = color;
    r.h.ah = VIDEO_PALETTE_REGS;
    r.h.al = reg > EGA_PALETTE_LAST;
    r.h.bl = reg;
    r.h.bh = color;
    while (!(inp(INPUT_STATUS) & STATUS_VRETRACE))
        ;
    int86x(BIOS_VIDEO, &r, &r, &s);
}

char iz8248;

/* video_set_palette @ 0F38:1FA4 (linear 0x11324, 124 bytes)

   Load all 16 EGA palette registers plus the overscan color (BIOS int 10h function 10h, al = 2) */

void video_set_palette(char *table)
{
    char pal[17];
    char far *dest;
    union REGS r;
    struct SREGS s;
    int n;

    dest = (char far *)pal;
    for (n = 0; n < 16; n++) {
        if (*table > 7)
            *dest++ = *table | 0x10;
        else
            *dest++ = *table;
        table++;
    }
    pal[16] = iz8248;
    dest = (char far *)pal;
    s.es = FP_SEG(dest);
    r.x.dx = FP_OFF(dest);
    r.h.ah = VIDEO_PALETTE_REGS;
    r.h.al = PALETTE_SET_ALL;
    int86x(BIOS_VIDEO, &r, &r, &s);
}
