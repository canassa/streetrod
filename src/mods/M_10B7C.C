#include "GLOBALS.H"
#include "PROTOS.H"
#include <conio.h>
#include "STRUCTS.H"
#include <dos.h>

static void near F_10DDF(void);
static void near F_10E3D(int n);

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

    g_vid.front = front;
    g_vid.back = back;
    g_vid.f8 = 0;
    g_vid.f16 = 1;
    switch (g_vid.mode = front->flags) {
    case -2:
        g_vid.planes = 4;
        g_vid.shift = 3;
        g_vid.bpp = 1;
        g_vid.mask = -8;
        g_style = &g_03a0;
        outp(0x3d4, 0xc);
        outp(0x3d5, 0);
        outp(0x3d4, 0xd);
        outp(0x3d5, 0);
        break;
    default:
        g_vid.planes = 1;
        g_vid.shift = 2;
        g_vid.bpp = 2;
        g_vid.mask = -4;
        g_style = &g_03f0;
        g_vid.front->flags = -3;
        break;
    case -6:
        g_vid.planes = 1;
        g_vid.shift = 1;
        g_vid.bpp = 4;
        g_vid.mask = -2;
        g_style = &g_03a0;
        break;
    }
    g_colors = g_style->pal[1];
    g_8266 = g_8268 = -1;
    i = 5;
    do {
        i--;
        g_7568[i].id = g_7568[i].f2 = -1;
    } while (i);
    (*g_present)();
    F_13F7F();
    g_8247 = 1;
    g_7316[0] = g_vid.front;
    g_7316[1] = g_vid.back;
    g_7316[2] = &g_screens[2];
    if (g_vid.mode == -2) {
        *g_7316[2] = *g_vid.back;
        g_7316[2]->bits.w.off = 0x4000;
        g_7316[2]->bits.w.seg = 0xa000;
        g_7316[3] = &g_2e68_0000;
        *g_7316[3] = *g_vid.back;
        g_7316[3]->bits.w.off = 0x6000;
        g_7316[3]->bits.w.seg = 0xa000;
    }
}

void F_10D6C(int n)
{
    struct Obj *o;

    o = &g_7568[g_58f0[g_59f8[n]].a];
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
    g_obj = &g_7568[n];
    g_cmd = &g_58f0[k = g_obj->f2];
    g_box = &g_obj->box;
    for (; k >= 0; k++) {
        switch (g_cmd->op) {
        case 1:
        case 3:
            break;
        case 2:
            F_10DDF();
            (*g_78a2)(g_obj->img, g_vid.back, (struct Blt *)g_box, 1);
            break;
        case 4:
            F_10DDF();
            (*g_78aa)(g_obj->img, g_vid.back, (struct Blt *)g_box, 1);
            break;
        case 13:
            if (!F_19518(g_box->x2, g_box->y2, g_box->w, g_box->h, &res))
                break;
            blt.w = res.x2 - res.x1;
            blt.h = res.y2 - res.y1 + 1;
            blt.sx = res.x1 - g_825e;
            blt.sy = res.y1 - g_8260;
            tx = res.x1 - g_box->x2;
            ty = res.y1 - g_box->y2;
            for (y = 0; y < g_obj->img->height; y += g_box->h) {
                for (x1 = 0; x1 < g_obj->img->width; x1 += g_box->w) {
                    blt.dx = tx + x1;
                    blt.dy = ty + y;
                    (*g_78aa)(g_825a, g_obj->img, &blt, 1);
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
            if (g_cmd->sub == 4 && g_vid.f16)
                F_1A92A(g_vid.back, g_vid.front, g_box->x2, g_box->y2,
                        g_box->x2 + g_box->w - 1, g_box->y2 + g_box->h - 1);
            return;
        case 12:
            F_120FE(-1);
            wait = 0;
            if (!g_vid.f16)
                break;
            F_1A92A(g_vid.back, g_vid.front, g_box->x2, g_box->y2,
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
        if (g_7568[i].id == -1) {
            g_58f0[g_59f8[n]].a = i;
            g_7568[i].id = n;
            g_7568[i].f2 = g_59f8[n] + 1;
            g_7568[i].f4 = 0;
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
    g_7568[k].f4--;
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
            g_6c3e = c->a;
            F_16993(1);
            return 0;
        }
        if (g_6c3e--) {
            int t1, t2, t3;
            old = g_colors.c[3];
            g_colors.c[3] = (g_6c3e & 1) ? 4 : 0;
            F_1377F(g_vid.back, 0x33, 0x6f, 2);
            if (!(g_6c3e & 3)) {
                g_colors.c[3] = (g_6c3e & 4) ? 4 : 0;
                F_1377F(g_vid.back, 0x30, 0x6f, 2);
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
