#include "STRUCTS.H"
#include "M_10B7C.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_0F38A.H"
#include "M_11E9C.H"
#include "M_13682.H"
#include "M_139DE.H"
#include "M_15396.H"
#include "M_163BE.H"
#include "M_174A6.H"
#include "M_1947A.H"
#include "M_19ED0.H"
#include "SCRATCH.H"
#include "SCREENS.H"
#include <conio.h>
#include <dos.h>

static struct Cmd *g_cmd;                  /* DS:6C38 */
static struct Obj *g_obj;                  /* DS:6C3A */
static struct Box *g_box;                  /* DS:6C3C */
static int g_6c3ea;                        /* DS:6C3E */
static void near F_10DDF(void);
static void near F_10E3D(int n);

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02). */
struct Colors x_colors;
struct Bitmap far *bp7316[4];
struct Obj bm7568[5];
struct Style *bustyle;
int (far *ab78aat)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
struct Bitmap far *bu825a;
int ac825et;
int aw8260y;
int d_8266;
int ap8268t;

struct Style g_03a0 = {{{""}, {{13, 9, 9, 15, 1}}, {{9, 13, 13, 15, 1}}, {{1, 5, 5, 1, 1}}, {{1, 5, 5, 4, 1}}, {{15, 9, 9, 15, 1}}, {{14, 15, 15, 10, 1}}}, {{1, 4, 4, 1, 1}}, {{1, 3, 3, 4, 2}}, {{15, 9, 9, 15, 1}}, {{13, 9, 9, 15, 1}}, {{11, 2, 2, 11, 1}}, {1, 4, 4, 11, 1}, {{1, 4, 4, 1, 1}}, {{1, 4, 4, 1, 1}}, {{1, 3, 3, 4, 1}}};   /* DS:03A0 */
struct Style g_03f0 = {{{""}, {{1, 2, 2, 3, 1}}, {{2, 1, 1, 3, 1}}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 2, 1}}, {{3, 2, 2, 3, 1}}, {{2, 1, 1, 1, 1}}}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 2, 2}}, {{3, 2, 2, 3, 1}}, {{1, 2, 2, 3, 1}}, {{0, 2, 2, 0, 1}}, {0, 3, 3, 1, 1}, {{0, 3, 3, 0, 1}}, {{0, 3, 3, 0, 1}}, {{1, 2, 2, 3, 1}}};   /* DS:03F0 */
unsigned char g_palette[16] = {0, 0, 8, 7, 15, 7, 9, 11, 3, 2, 4, 12, 5, 10, 6, 14};   /* DS:0440 */
int g_0450 = 0;   /* DS:0450 */
char g_0452[16] = "";   /* DS:0452 */
unsigned char g_0462[16] = {9, 1, 8, 0, 15, 7, 11, 3, 14, 2, 4, 12, 13, 10, 6, 14};   /* DS:0462 */
int g_0472 = 0;   /* DS:0472 */

/* F_10B7C @ 0F38:17FC (linear 0x10B7C, 16 bytes) */

void F_10B7C(void)
{
    F_112CB(12, 5);
}

/* F_10B8C @ 0F38:180C (linear 0x10B8C, 16 bytes) */

void F_10B8C(void)
{
    F_112CB(12, 13);
}

/* F_10B9C @ 0F38:181C (linear 0x10B9C, 16 bytes) */

void F_10B9C(void)
{
    g_palette[6] = 9;
    g_palette[7] = 1;
    g_palette[8] = 3;
}

/* F_10BAC @ 0F38:182C (linear 0x10BAC, 45 bytes) */

void F_10BAC(void)
{
    int i;
    unsigned char save;

    for (i = 0; i < 17; i++) {
        save = g_palette[i];
        g_palette[i] = g_palette[i + 34];
        g_palette[i + 34] = save;
    }
}

/* F_10BD9 @ 0F38:1859 (linear 0x10BD9, 403 bytes) */

#pragma intrinsic(outp)

void F_10BD9(struct Bitmap far *front, struct Bitmap far *back)
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
        bustyle = &g_03a0;
        outp(0x3d4, 0xc);
        outp(0x3d5, 0);
        outp(0x3d4, 0xd);
        outp(0x3d5, 0);
        break;
    default:
        r_vid.planes = 1;
        r_vid.shift = 2;
        r_vid.bpp = 2;
        r_vid.mask = -4;
        bustyle = &g_03f0;
        r_vid.front->flags = -3;
        break;
    case -6:
        r_vid.planes = 1;
        r_vid.shift = 1;
        r_vid.bpp = 4;
        r_vid.mask = -2;
        bustyle = &g_03a0;
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
    F_13F7F();
    ap8247u = 1;
    bp7316[0] = r_vid.front;
    bp7316[1] = r_vid.back;
    bp7316[2] = &g_screens[2];
    if (r_vid.mode == -2) {
        *bp7316[2] = *r_vid.back;
        bp7316[2]->bits.w.off = 0x4000;
        bp7316[2]->bits.w.seg = 0xa000;
        bp7316[3] = &g_2e68_0000;
        *bp7316[3] = *r_vid.back;
        bp7316[3]->bits.w.off = 0x6000;
        bp7316[3]->bits.w.seg = 0xa000;
    }
}

void F_10D6C(int n)
{
    struct Obj *o;

    o = &bm7568[g_58f0[g_59f8[n]].a];
    if (o->img)
        o->img = 0;
    if (o->fc)
        o->fc = 0;
    g_58f0[g_59f8[n]].a = -1;
    o->id = -1;
    if (o->f4)
        F_019BC(n);
}

static void near F_10DDF(void)
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

/* F_10E3D @ 0F38:1ABD (linear 0x10E3D, 780 bytes): run the script of one Obj slot */

static void near F_10E3D(int n)
{
    int k;
    int wait;
    int x1, y, tx, ty;
    struct Rect res;
    struct Blt blt;

    wait = 1;
    F_120FE(-3);
    g_obj = &bm7568[n];
    g_cmd = &g_58f0[k = g_obj->f2];
    g_box = &g_obj->box;
    for (; k >= 0; k++) {
        switch (g_cmd->op) {
        case 1:
        case 3:
            break;
        case 2:
            F_10DDF();
            (*b_78a2)(g_obj->img, r_vid.back, (struct Blt *)g_box, 1);
            break;
        case 4:
            F_10DDF();
            (*ab78aat)(g_obj->img, r_vid.back, (struct Blt *)g_box, 1);
            break;
        case 13:
            if (!F_19518(g_box->x2, g_box->y2, g_box->w, g_box->h, &res))
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
            if (!F_11213(g_obj, g_cmd))
                break;
        case 6:
            F_10D6C(g_obj->id);
            F_120FE(-1);
            return;
        case 7:
            g_cmd = &g_58f0[g_cmd->a];
            goto next;
        case 8:
            g_obj->img = F_15BBE(g_cmd->a, 2);
            g_cmd++;
            continue;
        case 9:
            F_10DDF();
            break;
        case 10:
            g_box->w = g_cmd->a;
            g_box->h = g_cmd->b;
            break;
        case 11:
            if (!(g_obj->f2 = g_cmd->b))
                g_obj->f2 = ++k;
            F_0198A(g_obj->id, g_cmd->a);
            g_obj->f4++;
            if (wait)
                F_120FE(-1);
            if (g_cmd->sub == 4 && r_vid.f16)
                F_1A92A(r_vid.back, r_vid.front, g_box->x2, g_box->y2,
                        g_box->x2 + g_box->w - 1, g_box->y2 + g_box->h - 1);
            return;
        case 12:
            F_120FE(-1);
            wait = 0;
            if (!r_vid.f16)
                break;
            F_1A92A(r_vid.back, r_vid.front, g_box->x2, g_box->y2,
                    g_box->x2 + g_box->w - 1, g_box->y2 + g_box->h - 1);
            break;
        }
        g_cmd++;
    next: ;
    }
}

int F_11149(int n)
{
    int i;

    for (i = 0; i < 5; i++) {
        if (bm7568[i].id == -1) {
            g_58f0[g_59f8[n]].a = i;
            bm7568[i].id = n;
            bm7568[i].f2 = g_59f8[n] + 1;
            bm7568[i].f4 = 0;
            F_10E3D(i);
            break;
        }
    }
    return i;
}

void F_111C1(int n)
{
    int k;

    k = g_58f0[g_59f8[n]].a;
    bm7568[k].f4--;
    F_10E3D(k);
}

int F_111F2(int n)
{
    return g_58f0[g_59f8[n]].a >= 0;
}

int F_11213(struct Obj *o, struct Cmd *c)
{
    int old;

    switch (o->id) {
    case 2:
        if (c->a) {
            g_6c3ea = c->a;
            F_16993(1);
            return 0;
        }
        if (g_6c3ea--) {
            int t1, t2, t3;
            old = x_colors.c[3];
            x_colors.c[3] = (g_6c3ea & 1) ? 4 : 0;
            F_1377F(r_vid.back, 0x33, 0x6f, 2);
            if (!(g_6c3ea & 3)) {
                x_colors.c[3] = (g_6c3ea & 4) ? 4 : 0;
                F_1377F(r_vid.back, 0x30, 0x6f, 2);
            }
            return 0;
        }
        F_16993(0);
        return 1;
    }
}

/* F_112CB: set EGA palette register reg to color (BIOS int 10h function 10h), waiting for vertical retrace */

#pragma intrinsic(inp)

void F_112CB(int reg, char color)
{
    union REGS r;
    struct SREGS s;

    if (color > 7)
        color |= 0x10;
    g_palette[reg] = color;
    r.h.ah = 0x10;
    r.h.al = reg > 15;
    r.h.bl = reg;
    r.h.bh = color;
    while (!(inp(0x3da) & 8))
        ;
    int86x(0x10, &r, &r, &s);
}

char iz8248;

/* F_11324: load all 16 EGA palette registers plus the overscan color (BIOS int 10h function 10h, al = 2) */

void F_11324(char *table)
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
    r.h.ah = 0x10;
    r.h.al = 2;
    int86x(0x10, &r, &r, &s);
}
