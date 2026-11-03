/* F_10E3D @ 0F38:1ABD (linear 0x10E3D, 780 bytes): run the script of one Obj slot */

#include "GLOBALS.H"
#include "PROTOS.H"

static void near F_10DDF(void);

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
