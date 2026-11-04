#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "STRUCTS.H"
#include "GARAGE.H"
#include "ARENA.H"
#include "BITMAP.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DISK.H"
#include "GAME.H"
#include "GARGFX.H"
#include "HOTSPOT.H"
#include "MAIN.H"
#include "MARKS.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "PERF.H"
#include "POPUP.H"
#include "RANDOM.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "TIRES.H"
#include "VIDEO.H"

static int near day_to_date(int v, int *out);
static void near place_spots(int n, int x, int y);
static void near hide_spots(void);
static void near sort_spots(void);
static int near is_broke(void);
static void near remove_part(struct Node **slot);
static void near tune_timing(struct Car *car);

int g_5030[3][4] = {{1065, 1066, 1067, 1068}, {1028, 1029, 1030, 1031}, {1106, 1107, 1108, 1109}};   /* DS:5030 */
int g_5048 = 33;   /* DS:5048 */
int g_504a[4] = {14, 107, 15, 105};   /* DS:504A */
int g_5052[4] = {0, 5, 9, 14};   /* DS:5052 */

/* garage_day @ 0000:6476 (linear 0x06476, 45 bytes) */

int garage_day(void)
{
    return (int)((g_05fc + g_0600 - g_0604) / 6552) > 91 ? 91 : (int)((g_05fc + g_0600 - g_0604) / 6552);
}

/* F_064A3 @ 0000:64A3 (linear 0x064A3, 134 bytes) */

void F_064A3(int *a, int *b)
{
    long t;
    int r;

    t = g_05fc + g_0600 - g_0604;
    if (t > 0x91908L)
        t = 0x91908L;
    r = (int)(t % 6552);
    *a = (unsigned)r / 546;
    *b = (long)((unsigned long)(unsigned)(r - 546 * *a) * 60) / 546;
}

/* F_06529 @ 0000:6529 (linear 0x06529, 19 bytes) */

void F_06529(int n)
{
    if (garage_day() >= 91)
        g_end |= 1;
}

/* day_to_date @ 0000:653C (linear 0x0653C, 115 bytes) */

static int near day_to_date(int v, int *out)
{
    int r;

    if (v < 15) {
        r = 0;
        *out = v + 16;
    } else if (v < 46) {
        r = 1;
        *out = v - 14;
    } else if (v < 77) {
        r = 2;
        *out = v - 45;
    } else if (v < 91) {
        r = 3;
        *out = v - 76;
    } else {
        r = 3;
        *out = 15;
    }
    return r;
}

/* F_065AF @ 0000:65AF (linear 0x065AF, 417 bytes) */

void F_065AF(int a1)
{
    int x;
    struct Bitmap far *pic;
    int py4, px5, row5, m, id, d9, al, y4;
    struct Bitmap far *img;
    int e9, col, day, bl;

    F_06529(0);
    if (a1 == 0 && g_end)
        return;
    d9 = garage_day();
    F_064A3(&al, &bl);
    m = day_to_date(d9, &day);
    col = (e9 = d9 + 21) % 7;
    row5 = e9 / 7 - g_5052[m];
    x = 18 * col + g_5048;
    y4 = 7 * row5 + g_504a[m];
    r_vid.f16 = 1;
    px5 = 80;
    py4 = 4;
    id = m < 2 ? 0x4c3 : 0x4c5;
    cursor_ctl(-3);
    pic = F_15BBE(id, 2);
    bitmap_draw_pic(pic, 0x4c4, (unsigned char)x, (unsigned char)y4, 0);
    img = F_1934E(pic->width, pic->height, 0, 2);
    popup_grab(px5, py4, pic->width, pic->height, img);
    popup_show(pic, px5, py4);
    F_19438(1);
    cursor_ctl(-1);
    if (a1 == 0) {
        F_11421(-1, "", 0);
        game_pause(500);
        g_05fc += 0L;
        popup_show(img, px5, py4);
    }
    F_19438(1);
}

char *g_5082[4] = {"June", "July", "August", "September"};   /* DS:5082 */
int g_508a[5] = {11, 13, 9, 10, 12};   /* DS:508A */

/* F_06750 @ 0000:6750 (linear 0x06750, 127 bytes)

   Draws the current date ("Month day, 1963") */

void F_06750(int x)
{
    int date;
    int today;
    int mnum;
    char buf[30];
    struct Colors sv;

    today = garage_day();
    mnum = day_to_date(today, &date);
    sprintf(buf, "%s %d, 1963", g_5082[mnum], date);
    sv = x_colors;
    x_colors = bustyle->hilite;
    (*a_78aem)(r_vid.back, 0x61, x, buf);
    x_colors = sv;
}

/* F_067CF @ 0000:67CF (linear 0x067CF, 88 bytes) */

void F_067CF(int a)
{
    if (r_vid.mode == -2 || r_vid.mode == -6) {
        if (gz8249) {
            F_112CB(6, g_50c2[a][0]);
            F_112CB(7, g_50c2[a][1]);
        }
        g_palette[6] = g_50c2[a][0];
        g_palette[7] = g_50c2[a][1];
    }
}

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct State8272 m_8272;

/* F_06827 @ 0000:6827 (linear 0x06827, 91 bytes) */

int F_06827(int id)
{
    int n, k, base;

    base = ((unsigned)g_121c - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    for (k = 0; k < n; k++)
        if (g_hots[base + k].f6 == id)
            return base + k;
}

/* sort_spots @ 0000:6882 (linear 0x06882, 49 bytes) */

static void near sort_spots(void)
{
    int base, n;

    base = ((unsigned)g_121c - (unsigned)g_hots) / sizeof(struct Hot);
    n = ((unsigned)g_134e - (unsigned)g_121c) / sizeof(struct Hot) + 1;
    F_135E5(base, n);
}

/* hide_spots @ 0000:68B3 (linear 0x068B3, 53 bytes) */

static void near hide_spots(void)
{
    int i;

    for (i = 0; i < 5; i++)
        g_hots[F_06827(g_508a[i])].f0 = 14;
}

/* place_spots @ 0000:68E8 (linear 0x068E8, 446 bytes)

   Rect + hotspot setup for the 5 button rects of panel n */

static void near place_spots(int n, int x, int y)
{
    struct Panel *panel;
    struct Hot *hotspot;

    panel = &ai70e4t[n];
    hotspot = &g_hots[F_06827(11)];
    hotspot->x1 = panel->r[0].x + x;
    hotspot->y1 = panel->r[0].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[0].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[0].h;
    hotspot->f0 = 11;
    hotspot = &g_hots[F_06827(13)];
    hotspot->x1 = panel->r[1].x + x;
    hotspot->y1 = panel->r[1].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[1].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[1].h;
    hotspot->f0 = 11;
    hotspot = &g_hots[F_06827(9)];
    hotspot->x1 = panel->r[2].x + x;
    hotspot->y1 = panel->r[2].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[2].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[2].h;
    hotspot->f0 = 11;
    hotspot = &g_hots[F_06827(10)];
    hotspot->x1 = panel->r[3].x + x;
    hotspot->y1 = panel->r[3].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[3].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[3].h;
    hotspot->f0 = 11;
    hotspot = &g_hots[F_06827(12)];
    hotspot->x1 = panel->r[4].x + x;
    hotspot->y1 = panel->r[4].y + y;
    hotspot->x2 = (unsigned)hotspot->x1 + panel->r[4].w;
    hotspot->y2 = (unsigned)hotspot->y1 + panel->r[4].h;
    hotspot->f0 = 11;
}

/* F_06AA6 @ 0000:6AA6 (linear 0x06AA6, 50 bytes) */

void F_06AA6(void)
{
    char *s;

    s = "                                ";
    strcpy(g_29ff, s);
}

/* is_broke @ 0000:6AD8 (linear 0x06AD8, 46 bytes) */

static int near is_broke(void)
{
    if (a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2 == 0 && a_7eaap.m7eb8 == 0 && a_7eaap.m7eaa < 400L)
        return 1;
    return 0;
}

char g_50c2[6][2] = {{4, 12}, {3, 11}, {2, 10}, {5, 13}, {1, 9}, {7, 15}};   /* DS:50C2 */
int g_50ce[7] = {0, 5, 5, 5, 5, 0, 0};   /* DS:50CE */

/* F_06B06 @ 0000:6B06 (linear 0x06B06, 1125 bytes) */

int near F_06B06(int mode)
{
    char *p;
    int r;
    int n;

    if (g_4733 > 0)
        cursor_ctl(-4);
    F_06529(0);
    if (g_end) {
        if (g_end & 1)
            cursor_ctl(-2);
        return 0x29a;
    }
    cursor_ctl(0);
    F_11421(-1, "", 0);
    g_scene = 1;
    switch (mode) {
    case 3:
        if (g_4f48 == 0) {
            cursor_ctl(-2);
            game_message(0x1867);
            cursor_ctl(-4);
            break;
        }
        if (g_4f46) {
            n = game_car_look(g_4f46);
            F_181C8(6, g_4f46->f2, n, (g_4f46->f24 >> 8) & 0x1f,
                    0, 0x98, 0, g_5030[g_4806[g_4f46->slot12->idx].f2]);
            F_06AA6();
            menu_push(10);
            menu_pop(1);
        }
    case 2:
        r_vid.f16 = 0;
    case 1:
        if (mode == 3)
            mode = 1;
        else {
            F_118D4(0x408, -1);
            F_10B7C();
            if (mode == 2 && a_7eaap.m7eb0 == 0 && a_7eaap.m7eb2) {
                a_7eaap.m7eb0 = a_7eaap.m7eb2;
                a_7eaap.m7eb2 = a_7eaap.m7eb0->f26;
                mode = 1;
            }
        }
        r_vid.f16 = 1;
        (*m_present)();
        if (a_7eaap.m7eb0) {
            m_8272.m827a = g7d86n[a_7eaap.m7eb0->f2].f8;
            m_8272.m827c = a_7eaap.m7eb0->f4;
            m_8272.m827e = g_4806[a_7eaap.m7eb0->slot12->idx].f2;
            if (mode == 1)
                F_067CF(m_8272.m827c);
            m_8272.m8280 = 0;
            m_8272.m8282 = a_7eaap.m7eb0->f2;
            m_8272.m8284.shade = (a_7eaap.m7eb0->f24 >> 8) & 0x1f;
            m_8272.m8284.roof = (a_7eaap.m7eb0->f24 >> 13) & 1;
            m_8272.m8284.front = (a_7eaap.m7eb0->f24 >> 14) & 1;
            m_8272.m8284.rear = (a_7eaap.m7eb0->f24 >> 15) & 1;
            for (r = 0; r < 5; r++)
                o_8290[r] = a_7eaap.m7eb0->rack.s[r]->idx;
            n = game_car_look(a_7eaap.m7eb0);
            strcpy(g_29ff, g_desc + g7d86n[a_7eaap.m7eb0->f2].f6);
            if (mode == 2)
                F_181C8(3, m_8272.m8282, n, m_8272.m8284.shade, 0, 0x98, 0, g_5030[m_8272.m827e]);
            else
                F_181C8(4, m_8272.m8282, n, m_8272.m8284.shade, 0, 0x98, 0, g_5030[m_8272.m827e]);
        } else {
            bitmap_copy_box(r_vid.back, r_vid.front, 0, 0, 0x13f, 0xc7);
            g_29ff[0] = 0;
        }
        break;
    case 0:
        break;
    }
    if (a_7eaap.m7eb0)
        place_spots(m_8272.m8282, ac825et, aw8260y);
    else
        hide_spots();
    sort_spots();
    F_11593(g_49dc);
    cursor_ctl(-2);
    if (a_7eaap.m7eb6 > 15)
        game_message(0x0d30);
    else if (a_7eaap.m7ebc > 130) {
        if (a_7eaap.m7ebc == 131)
            game_message(0x0d76);
        else {
            p = g_39c9;
            p += strlen(p) + 1;
            p += strcspn(p, "1234567890");
            itoa(a_7eaap.m7ebc - 130, p, 10);
            p[strlen(p)] = ' ';
            game_message(0x0dc7);
        }
    }
    if (is_broke()) {
        game_message(0x17ff);
        g_end |= 0x10;
    } else {
        F_16381(0);
        sys_reset_queues();
        r = menu_wait(10);
    }
    F_06529(0);
    if (g_end & 0x10)
        g_end = 0x10;
    if (g_end) {
        r = 0x29a;
        if ((g_end & 1) == 0)
            cursor_ctl(-4);
    }
    return r;
}

/* F_06F6B @ 0000:6F6B (linear 0x06F6B, 431 bytes) */

void near F_06F6B(void)
{
    struct Bitmap far *bm1;
    int x;
    int idx4;
    int y;
    int key;

    if (!a_7eaap.m7eb0)
        game_message(0x0efa);
    else if (r_vid.mode != -2 && r_vid.mode != -6)
        game_message(0x18ad);
    else {
        bm1 = menu_grab(14, &x, &y, 2);
        menu_push(14);
        idx4 = a_7eaap.m7eb0->f4;
        for (;;) {
            F_112CB(12, g_50c2[idx4][0]);
            while ((key = sys_wait_input(3000)) == 0)
                ;
            switch (key) {
            case 1:
                if (--idx4 < 0)
                    idx4 = 5;
                continue;
            case 2:
                if (++idx4 >= 6)
                    idx4 = 0;
                continue;
            case 3:
                if (a_7eaap.m7eaa < 20L) {
                    game_message(0x10a1);
                    break;
                }
                r_vid.f16 = 1;
                menu_pop(1);
                popup_show(bm1, x, y);
                F_19438(1);
                popup_show(g_0294, m_8272.m8272, m_8272.m8274);
                a_7eaap.m7eb0->f24 &= 0xe0ff;
                F_067CF(a_7eaap.m7eb0->f4 = idx4);
                F_112CB(12, 5);
                g_05fc += 0x888L;
                F_11540(-20);
                return;
            case -23:
            default:
                break;
            }
            break;
        }
        menu_pop(1);
        F_112CB(12, 5);
        popup_show(bm1, x, y);
        F_19438(1);
    }
}

/* F_0711A @ 0000:711A (linear 0x0711A, 589 bytes) */

void near F_0711A(unsigned mask)
{
    struct Bitmap far *bm1;
    int n9;
    char *msg;
    unsigned val5;
    int x;
    int r7;
    struct Model *p9;
    int y;
    int key;
    int cost;

    p9 = &w_8df0[a_7eaap.m7eb0->f2];
    if (!a_7eaap.m7eb0) {
        game_message(0x0efa);
        return;
    }
    if (mask & 4) {
        if (p9->f2 == 0) {
            game_message(0x1914);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x2000) ? (g_239e + 1299) : (g_239e + 1283);
        cost = 70;
        val5 = 0x2220;
    }
    if (mask & 2) {
        if (p9->f6 == 0) {
            game_message(0x1936);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x4000) ? (g_239e + 1337) : (g_239e + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if (mask & 1) {
        if (p9->f8 == 0) {
            game_message(0x1936);
            return;
        }
        msg = (a_7eaap.m7eb0->f24 & 0x8000) ? (g_239e + 1337) : (g_239e + 1318);
        cost = 15;
        val5 = 0x888;
    }
    if ((long)cost > a_7eaap.m7eaa) {
        game_message(0x1959);
        return;
    }
    bm1 = menu_grab(34, &x, &y, 2);
    g_1e34[0].f4 = msg - g_239e;
    menu_push(34);
    while ((key = sys_wait_input(3000)) == 0)
        ;
    switch (key) {
    case -20:
        if (mask & 4)
            a_7eaap.m7eb0->f24 ^= 0x2000;
        if (mask & 2)
            a_7eaap.m7eb0->f24 ^= 0x4000;
        if (mask & 1)
            a_7eaap.m7eb0->f24 ^= 0x8000;
        cursor_ctl(-4);
        menu_pop(1);
        popup_show(bm1, x, y);
        F_19438(1);
        n9 = a_7eaap.m7eb0->f2;
        r7 = game_car_look(a_7eaap.m7eb0);
        F_181C8(8, n9, r7, (a_7eaap.m7eb0->f24 >> 8) & 0x1f, 0, 0x98, 0,
                g_5030[g_4806[a_7eaap.m7eb0->slot12->idx].f2]);
        F_16BC1();
        cursor_ctl(-2);
        g_05fc += val5;
        F_11540(-cost);
        break;
    case -22:
        menu_pop(1);
        popup_show(bm1, x, y);
        F_19438(1);
        break;
    }
}

/* F_07367 @ 0000:7367 (linear 0x07367, 832 bytes) */

void near F_07367(void)
{
    struct Bitmap far *bm1;
    struct Bitmap far *bm3;
    struct Pos6 *q2;
    struct Bitmap far *bm4;
    struct Blt B1;
    struct Blt A2;
    struct Blt C;
    int x;
    int y;
    int idx6;
    int k;

    if (!a_7eaap.m7eb0)
        game_message(0x0efa);
    else if (a_7eaap.m7eaa < 5L)
        game_message(0x1975);
    else {
        r_vid.f16 = 1;
        x = 24;
        y = 11;
        bm1 = F_1934E(264, 69, 0, 2);
        popup_grab(x, y, 264, 69, bm1);
        F_119B8(1200, x, y, -1);
        menu_push(35);
        while ((k = sys_wait_input(3000)) == 0)
            ;
        switch (k) {
        case -38: case -37: case -36: case -35:
        case -34: case -33: case -32: case -31:
            cursor_ctl(-3);
            menu_pop(1);
            popup_show(bm1, x, y);
            F_19438(1);
            idx6 = -30 - k;
            a_7eaap.m7eb0->f24 = ((idx6 & 0x1f) << 8) | (a_7eaap.m7eb0->f24 & 0xe0ff);
            bm3 = F_15BBE(1218, 2);
            bm1 = F_1934E(g_0294->width, g_0294->height, 0, 2);
            bitmap_copy(g_0294, bm1, 1);
            q2 = &a784als[idx6 - 1];
            bitmap_draw_pic(bm1, q2->id, (char)(q2->dx + m_8272.m8276), (char)q2->dy, 0);
            bm4 = F_1934E(bm1->width, bm1->height, 0, 2);
            bitmap_copy(bm1, bm4, 1);
            A2.sx = B1.sx = C.sx = 0;
            A2.sy = B1.sy = C.sy = 0;
            A2.dy = B1.dy = 0;
            A2.dx = B1.dx = 0;
            C.dx = m_8272.m8272;
            C.dy = m_8272.m8274;
            B1.h = B1.fc = A2.h = C.h = bm1->height;
            A2.w = B1.w = C.w = 8;
            F_168F3(1);
            for (k = 0; k < bm1->width; k += 8) {
                (*b_78a2)(bm1, bm4, &A2, 1);
                if (bm1->width - 8 > k)
                    (*ab78aat)(bm3, bm4, &B1, 1);
                F_10AFD(2);
                (*b_78a2)(bm4, r_vid.front, &C, 1);
                A2.w += 8;
                C.w += 8;
                B1.dx += 8;
            }
            popup_show(bm1, m_8272.m8272, m_8272.m8274);
            F_19438(3);
            F_168F3(0);
            g_05fc += 0x222L;
            F_11540(-5);
            cursor_ctl(-1);
            break;
        case -22:
            menu_pop(1);
            popup_show(bm1, x, y);
            F_19438(1);
            break;
        }
    }
}

/* F_076A7 @ 0000:76A7 (linear 0x076A7, 154 bytes) */

void near F_076A7(int t, struct Node **list, char *buf, int start)
{
    struct Node *p;
    struct Item *ent7;
    int n;

    n = start;
    p = a_7eaap.m7eb8;
    while (p) {
        if ((ent7 = &g_4806[p->idx])->f4 == t) {
            n++;
            list[n] = p;
            g_list[n] = ent7->f6;
            if (p->b != -128)
                buf[n] = p->b / 100;
            else
                buf[n] = 0x80;
        }
        p = p->next;
    }
    g_list[0] = n;
}

/* F_07741 @ 0000:7741 (linear 0x07741, 72 bytes) */

void near F_07741(struct Node *n, struct Node **slot)
{
    if (*slot) {
        (*slot)->next = a_7eaap.m7eb8;
        a_7eaap.m7eb0->weight -= game_part_value(a_7eaap.m7eb8 = *slot);
    }
    *slot = n;
    n->next = 0;
    a_7eaap.m7eb0->weight += game_part_value(n);
}

/* remove_part @ 0000:7789 (linear 0x07789, 46 bytes) */

static void near remove_part(struct Node **slot)
{
    (*slot)->next = a_7eaap.m7eb8;
    a_7eaap.m7eb0->weight -= game_part_value(a_7eaap.m7eb8 = *slot);
    *slot = 0;
}

/* F_077B7 @ 0000:77B7 (linear 0x077B7, 73 bytes) */

struct Node * near F_077B7(struct Node *n)
{
    struct Node *p1, *q;

    for (p1 = a_7eaap.m7eb8, q = 0; p1;)
        if (p1 != n)
            p1 = (q = p1)->next;
        else
            break;
    if (q == 0)
        a_7eaap.m7eb8 = p1->next;
    else
        q->next = p1->next;
    return p1;
}

/* F_07800 @ 0000:7800 (linear 0x07800, 241 bytes) */

void near F_07800(void)
{
    struct Car *car;
    int sel;
    struct Node *tbl[142];
    char line[142];
    int y, cur;

    car = a_7eaap.m7eb0;
    if (car == 0) {
        game_message(0x0efa);
        return;
    }
    F_076A7(4, tbl, line, 0);
    switch (F_142CF(4, 1, 0, g_list, line, 0)) {
    case -4:
        return;
    case -3:
        sel = F_14C8B();
        break;
    case 0x3ea:
        game_message(0x0f4e);
        return;
    }
    y = g_4806[car->slot12->idx].f2;
    cur = g_4806[tbl[sel]->idx].f2;
    F_07741(F_077B7(tbl[sel]), &car->slot12);
    F_1B213(g_5030[y], g_5030[cur]);
    g_05fc += 0x444L;
}

/* build_car_info @ 0000:78F1 (linear 0x078F1, 250 bytes) */

static void near build_car_info(int *ids, char *vals)
{
    int n;
    struct Item *it;
    int count;
    struct Node *node;
    struct Node *gear[7];

    for (n = 0; n < 5; n++)
        gear[n] = a_7eaap.m7eb0->rack.s[n];
    gear[n++] = a_7eaap.m7eb0->slot6;
    gear[n] = a_7eaap.m7eb0->slot12;

    for (count = 0, n = 0; n < 7; n++) {
        if (node = gear[n]) {
            count++;
            it = &g_4806[node->idx];
            ids[count] = g_50ce[n];
            g_list[count] = it->f6;
            if (node->b != -0x80)
                vals[count] = node->b / 100;
            else
                vals[count] = 0x80;
        }
    }
    for (n = 1; n <= 4; n++) {
        g_list[++count] = -n;
        vals[count] = 0x80;
        ids[count] = 0;
    }
    g_list[0] = count;
}

char *g_511e[4] = {"", "Fuel                  ", "", "Max speed      mph"};   /* DS:511E */
int g_5126[5] = {1, 1, 3, 2, 2};   /* DS:5126 */
int g_5130[6] = {1, 2, 2, 2, 2, 2};   /* DS:5130 */
int g_513c[6] = {0, 3, 2, 2, 2, 2};   /* DS:513C */
int g_5148[5][6] = {{5, 5, 0, 0, 0, 0}, {6, 6, 0, 0, 0, 0}, {7, 7, 8, 8, 9, 9}, {10, 10, 11, 11, 0, 0}, {12, 12, 13, 13, 0, 0}};   /* DS:5148 */
int g_5184[1] = {0};   /* DS:5184 */
int g_5186[9] = {0, 1, 1, 5, 5, 5, 5, 5, 5};   /* DS:5186 */
int g_5198[1] = {0};   /* DS:5198 */
int g_519a[9] = {0, 0, 1, 0, 1, 0, 1, 0, 1};   /* DS:519A */

/* F_079EB @ 0000:79EB (linear 0x079EB, 185 bytes) */

void F_079EB(void)
{
    int ids[143];
    char vals[144];

    if (a_7eaap.m7eb0 == 0) {
        game_message(0x0efa);
        return;
    }
    build_car_info(ids, vals);
    sprintf(g_511e[3] + 9, " %3i mph", F_07EE6(a_7eaap.m7eb0, 1) ? F_0E218(-1) : 0);
    sprintf(g_511e[1] + 5, " %2i gallons", a_7eaap.m7eb0->f22 / 10);
    switch (F_142CF(0x26, 1, ids, g_list, vals, g_511e)) {
    case -3:
        return;
    case 0x3ea:
        game_message(0x0f4e);
        return;
    }
}

/* F_07AA4 @ 0000:7AA4 (linear 0x07AA4, 1090 bytes) */

void near F_07AA4(void)
{
    int flags[3];   /* by key 1, 2; flags[0] is the original's unused 2-byte slot below flags[1] */
    char buf[142];
    int prev;
    struct Car *owner;
    int k;
    struct Node *tbl[142];

    owner = a_7eaap.m7eb0;
    if (!owner) {
        game_message(0x0efa);
        return;
    }
    for (k = 0; k < 2; k++) {
        if (prev = owner->b14[k])
            flags[k + 1] = prev == 1 ? 1 : 2;
        else
            flags[k + 1] = 0;
        g_1804[k].f0 = 11;
    }
    m_8272.m8286 = owner->slot6 ? g_49d2[g_4806[owner->slot6->idx].f2] : 0;
    r_vid.f16 = !m_8272.m8286;
    F_119B8(0x42e, 0x20, 10, -1);
    r_vid.f16 = 1;
    F_127DB(m_8272.m8286, owner->b14);
    menu_push(20);
    for (;;) {
        while ((k = sys_wait_input(3000)) == 0)
            ;
        switch (k) {
        case -1:
            menu_pop(1);
            return;
        case -2:
            F_076A7(1, tbl, buf, 0);
            for (;;) {
                switch (k = F_142CF(4, 1, 0, g_list, buf, 0)) {
                case -3:
                    if (owner->slot6) {
                        game_message(0x1875);
                        k = 0;
                    } else
                        k = F_14C8B();
                    break;
                case -4:
                    k = 0;
                    break;
                case 0x3ea:
                    game_message(0xf72);
                    k = 0;
                    break;
                }
                if (!k)
                    break;
                prev = m_8272.m8286;
                m_8272.m8286 = g_49d2[g_4806[tbl[k]->idx].f2];
                if (g7d86n[owner->f2].f4 & g_4806[tbl[k]->idx].f3 & 0xf) {
                    F_07741(F_077B7(tbl[k]), &owner->slot6);
                    cursor_ctl(-4);
                    F_16A5A(1);
                    F_1264E(m_8272.m8286, 0);
                    F_16A5A(0);
                    F_16B97();
                    g_05fc += 0x666L;
                    for (k = 2; k--;) {
                        flags[k + 1] = 1;
                        F_123F4(owner->b14[k] = 1, k);
                        F_16B61();
                        g_1804[k].f0 = 11;
                    }
                    sys_hover_reset();
                    cursor_ctl(-2);
                    break;
                }
                    F_1264E(m_8272.m8286, 1);
                    game_message(0x10cd);
                    F_12784(m_8272.m8286, 1);
                    g_05fc += 0x888L;
                    m_8272.m8286 = prev;
            }
            break;
        case 1:
        case 2:
            if (m_8272.m8286 == 0)
                break;
            if (flags[k--] == 2) {
                if (--owner->b14[k] <= 1) {
                    flags[k + 1] = owner->b14[k] = 0;
                    F_12525(k);
                    F_16B61();
                    sys_hover_reset();
                } else {
                    F_123F4(owner->b14[k], k);
                    F_16CF3();
                }
                if (flags[1] || flags[2])
                    break;
                F_12784(m_8272.m8286, 0);
                g_05fc += 0x666L;
                m_8272.m8286 = 0;
                owner->b14[0] = owner->b14[1] = 0;
                remove_part(&owner->slot6);
                sys_hover_reset();
            } else if (flags[k + 1] == 1) {
                if (owner->b14[k] == 3) {
                    flags[k + 1] = 2;
                    break;
                }
                F_123F4(++owner->b14[k], k);
                F_16CF3();
            } else {
                cursor_ctl(-4);
                F_123F4(owner->b14[k] = 1, k);
                F_16B61();
                cursor_ctl(-2);
                flags[k + 1] = 1;
            }
            break;
        }
    }
}

/* F_07EE6 @ 0000:7EE6 (linear 0x07EE6, 203 bytes) */

int F_07EE6(struct Car *car, int flag)
{
    int n, i;

    if (car->rack.s[0] == 0 || car->f1f <= 0 || car->rack.s[1] == 0)
        return 0;
    n = g_5126[g_4806[car->rack.s[1]->idx].f2];
    for (i = 2; i < n + 2; i++)
        if (car->rack.s[i] == 0)
            return 0;
    for (i = n * 2 + 3; --i >= 0; )
        if (car->b16[i] != 3)
            return 0;
    if (flag == 0)
        return 1;
    if (car->slot6 == 0)
        return 0;
    if (car->slot12 == 0)
        return 0;
    for (i = 2; --i >= 0; )
        if (car->b14[i] != 3)
            return 0;
    return 1;
}

/* tune_timing @ 0000:7FB1 (linear 0x07FB1, 506 bytes) */

static void near tune_timing(struct Car *car)
{
    struct Obj *p;
    long tmp;
    struct Blt blt;
    union Word w;
    int key;
    int y;
    unsigned n;
    int gear;

    menu_push(0x1c);
    blt.w = 8;
    blt.h = 11;
    blt.sx = 0x30;
    blt.sy = 0;
    blt.dx = 0xb0;
    F_119B8(0x42f, 0x60, 9, -1);
    p = bm7568 + video_anim_start(8);
    tmp = g_0600;
    sys_music_pause();
    g_voiceNum = 2;
    if (g_soundOn) {
        outp(0x61, inp(0x61) | 3);
        g_speakerOn = 1;
    }
    do {
        if (g_soundOn && g_soundOn != g_speakerOn) {
            outp(0x61, inp(0x61) | 3);
            g_speakerOn = 1;
        }
        gear = car->f20;
        w.w = (abs(gear) << 10) + 30000;
        outp(0x42, w.b[0]);
        outp(0x42, w.b[1]);
        y = blt.dy = gear * 2 + 0x4b;
        cursor_ctl(-4);
        (*b_78a2)(p->img, r_vid.back, &blt, 1);
        bitmap_copy_box(r_vid.back, r_vid.front, 0xb0, y, 0xb7, y + 10);
        cursor_ctl(-2);
        while ((key = sys_wait_input(0)) == 0) {
            if (g_soundOn && g_soundOn != g_speakerOn) {
                outp(0x61, inp(0x61) | 3);
                g_speakerOn = 1;
            }
        }
        switch (key) {
        case -10:
            car->f20++;
            if (car->f20 > 4)
                car->f20 = 4;
            break;
        case -7:
            car->f20--;
            if (car->f20 < -8)
                car->f20 = -8;
            break;
        }
    } while (key != -1);
    outp(0x61, inp(0x61) & 0xfc);
    g_speakerOn = 0;
    g_voiceNum = 1;
    sys_music_resume();
    video_anim_stop(8);
    n = ((unsigned)g_0600 - (unsigned)tmp) * 3;
    n = n > 0x444 ? 0x444 : n;
    g_05fc += n;
    menu_pop(1);
}

/* garage_hood @ 0000:81AB (linear 0x081AB, 2938 bytes)

   The garage screen: install, remove and tune parts */

void near garage_hood(struct Car *car)
{
    struct Node *list4[142];
    struct Node *p6;
    int cnt1;
    int sel2;
    int key;
    int ok;
    int k;
    int tmp29;
    int step;
    char buf[142];
    int n;
    int havek[10];

    if (car == 0) {
        game_message(0xefa);
        return;
    }
    for (key = 1; key <= 9; key++) {
        if (k = car->b16[key - 1]) {
            havek[key] = k == 1 ? 1 : 2;
            af829ax[key - 1] = g_49ca[k];
        } else {
            havek[key] = 0;
            af829ax[key - 1] = 0;
        }
    }
    ap82acz = car->f1f != 0 ? (car->f1f > 0 ? 0x447 : 0x446) : 0;
    if (p6 = car->rack.s[0])
        g_4998[1][0] = g_4806[p6->idx].f2 == 0 ? 0x438 : 0x439;
    for (key = 0; key < 5; key++)
        o_8290[key] = (p6 = car->rack.s[key]) ? g_4998[key][g_4806[p6->idx].f2] : 0;
    g_5148[0][0] = g_5148[0][1] = o_8290[0] == 0x434 ? 4 : 5;
    cnt1 = (o_8290[0] != 0) + (o_8290[1] != 0) + (o_8290[2] != 0) + (o_8290[3] != 0) + (o_8290[4] != 0);
    n = (p6 = car->rack.s[1]) ? g_5126[g_4806[p6->idx].f2] + 2 : 5;
    if (p6) {
        tmp29 = g_4806[p6->idx].f2;
        for (k = 3; k < 9; k++)
            g_5186[k] = g_5148[tmp29][k - 3];
    }
    cursor_ctl(-4);
    gargfx_draw_parts(car);
    gargfx_draw_bolts(g_5186, g_519a, car == a_7eaap.m7eb0);
    cursor_ctl(-2);
    if (car != a_7eaap.m7eb0)
        return;
    gargfx_build_hots(g_5186, g_519a);
    menu_push(0x15);
    for (;;) {
        while ((key = sys_wait_input(3000)) == 0)
            ;
        switch (key) {
        case -1:
            menu_pop(1);
            r_vid.f8 = 0;
            marks_release();
            return;
        case 13:
            car->f1f = -car->f1f;
            if (car->f1f == 0)
                continue;
            if (car->f1f > 0) {
                ap82acz = 0x447;
                gargfx_draw_wires();
                g_05fc += 0x6d;
                gargfx_build_hots(g_5186, g_519a);
                continue;
            }
            ap82acz = 0x446;
            gargfx_draw_wires();
            g_05fc += 0x6d;
            hz82ae = 0;
            gargfx_build_hots(g_5186, g_519a);
            break;
        case -3:
            if (F_07EE6(car, 0)) {
                tune_timing(car);
                cursor_ctl(-4);
                gargfx_draw_bolts(g_5186, g_519a, 1);
                cursor_ctl(-2);
            } else
                game_message(0xea1);
            continue;
        case -2:
            F_076A7(g_513c[cnt1], list4, buf, 0);
        again:
            key = F_142CF(4, 1, 0, g_list, buf, 0);
            switch (key) {
            case -3:
                key = F_14C8B();
                break;
            case 0x3ea:
                if (g_513c[cnt1] == 0)
                    game_message(0xf97);
                if (g_513c[cnt1] == 3)
                    game_message(0xfd7);
                if (g_513c[cnt1] == 2)
                    game_message(0xfb5);
                key = 0;
                break;
            case -4:
                key = 0;
                break;
            }
            if (key == 0)
                continue;
            if (cnt1 >= n) {
                game_message(0x1875);
                continue;
            }
            sel2 = cnt1;
            switch (cnt1) {
            default:
                goto again;
            case 0:
                ok = g_4806[list4[key]->idx].f3 & g7d86n[car->f2].f4 & 0xf;
                hz82ae = 0;
                step = 0;
                car->f1f = -1;
                ap82acz = 0x446;
                g_05fc += ok ? 0x666 : 0x888;
                break;
            case 1:
                ok = (g_4806[k = list4[key]->idx].f3 & g7d86n[car->f2].f4 & 0xf) != 0
                     && g_4978[g_4806[car->rack.s[0]->idx].f2][tmp29 = g_4806[k].f2] != 0;
                hz82ae = 1;
                step = 1;
                n = g_5126[g_4806[list4[key]->idx].f2] + 2;
                for (k = 4; k < 10; k++)
                    g_5184[k] = g_5148[tmp29][k - 4];
                g_05fc += ok ? 0x333u : 0x444u;
                break;
            case 2:
            case 3:
            case 4:
                ok = (g_4806[k = list4[key]->idx].f3 & g7d86n[car->f2].f4 & 0xf) != 0
                     && g_4988[tmp29 = g_4806[car->rack.s[1]->idx].f2][g_4806[k].f2] != 0;
                g_05fc += ok ? 0x199u : 0x222u;
                step = 3;
                sel2 = 2;
                switch (hz82ae = g_5148[tmp29][0]) {
                case 4:
                case 5:
                case 6:
                    break;
                case 7:
                    if (car->rack.s[2] == 0)
                        break;
                    if (car->rack.s[3]) {
                        hz82ae = 9;
                        step += 4;
                        sel2 = 4;
                    } else {
                        hz82ae = 8;
                        step += 2;
                        sel2 = 3;
                    }
                    break;
                case 10:
                case 12:
                    if (car->rack.s[2] == 0)
                        break;
                    hz82ae++;
                    step += 2;
                    sel2++;
                    break;
                default:
                    goto again;
                }
            }
            o_8290[sel2] = k = g_4998[sel2][g_4806[list4[key]->idx].f2];
            if (ok) {
                F_07741(F_077B7(list4[key]), &car->rack.s[sel2]);
                cursor_ctl(-4);
                F_16A5A(1);
                gargfx_draw_part(o_8290[sel2], 0);
                F_16A5A(0);
                F_16B97();
                if (sel2 == 0) {
                    g_4998[1][0] = k == 0x434 ? 0x438 : 0x439;
                    g_5148[0][0] = g_5148[0][1] = k == 0x434 ? 4 : 5;
                    ap82acz = 0x446;
                    gargfx_draw_wires();
                }
                for (k = g_5130[cnt1]; --k >= 0;) {
                    g82b2t = k;
                    gargfx_draw_bolt(af829ax[step + k] = g_49ca[1]);
                    F_16B61();
                    havek[step + k + 1] = 1;
                    car->b16[step + k] = 1;
                }
                gargfx_build_hots(g_5186, g_519a);
                cursor_ctl(-2);
                cnt1++;
                car->f20 = random_int(13) - 8;
                continue;
            } else {
                F_16A5A(1);
                gargfx_draw_part(o_8290[sel2], 1);
                F_16A5A(0);
                F_16B97();
                game_message(0x10cd);
                F_16A5A(1);
                gargfx_erase_part(o_8290[sel2], 1);
                F_16A5A(0);
                o_8290[sel2] = 0;
                if (sel2 == 0) {
                    car->f1f = 0;
                    ap82acz = 0;
                }
                goto again;
            }
        default:
            if (key < 1 || key > 9)
                continue;
            k = tmp29 = 0;
            for (; k < 5; k++)
                if ((tmp29 += g_5130[k]) >= key)
                    break;
            if (o_8290[k] == 0)
                continue;
            g_05fc += 0x36;
            if (havek[key] == 2) {
                hz82ae = g_5184[key];
                g82b2t = g_5198[key];
                if (--car->b16[key - 1] <= 1) {
                    af829ax[key - 1] = havek[key] = car->b16[key - 1] = 0;
                    gargfx_drop_bolt();
                    F_16B61();
                } else {
                    gargfx_draw_bolt(af829ax[key - 1] = g_49ca[car->b16[key - 1]]);
                    F_16CF3();
                }
            done:
                sel2 = -1;
                ok = 0;
                switch (hz82ae) {
                default:
                    if (g_5198[k = key] == 1)
                        k--;
                    if (havek[k] != 0 || havek[k + 1] != 0)
                        goto strip;
                    sel2 = (k - 4) / 2 + 2;
                    if (cnt1 == 3)
                        ok = 2;
                    g_05fc += 0x199;
                    goto strip;
                case 0:
                    break;
                case 1:
                    if (cnt1 != 2)
                        goto strip;
                    for (key = 2; key < 4 && havek[key] == 0; key++)
                        ;
                    if (key != 4)
                        goto strip;
                    sel2 = 1;
                    ok = 1;
                    g_05fc += 0x333;
                    goto strip;
                }
            } else {
                if (havek[key] == 1) {
                    if (car->b16[key - 1] == 3) {
                        havek[key] = 2;
                        continue;
                    }
                    af829ax[key - 1] = g_49ca[++car->b16[key - 1]];
                    hz82ae = g_5184[key];
                    g82b2t = g_5198[key];
                    gargfx_draw_bolt(af829ax[key - 1]);
                    F_16CF3();
                    continue;
                }
                hz82ae = g_5184[key];
                g82b2t = g_5198[key];
                gargfx_draw_bolt(af829ax[key - 1] = g_49ca[1]);
                F_16B61();
                havek[key] = 1;
                car->b16[key - 1] = 1;
                continue;
            }
        }
        sel2 = -1;
        ok = 0;
        if (cnt1 == 1 && car->f1f == -1) {
            for (key = 1; key < 2 && !havek[key]; key++)
                continue;
            if (key == 2) {
                sel2 = 0;
                car->f1f = 0;
                ap82acz = 0;
                g_05fc += 0x666;
            }
        }
    strip:
        if (sel2 >= 0) {
            cnt1--;
            remove_part(&car->rack.s[sel2]);
            cursor_ctl(-4);
            o_8290[sel2] = 0;
            if (ok)
                hz82ae = ok - 1;
            gargfx_draw_parts(car);
            gargfx_draw_bolts(g_5186, g_519a, 1);
            gargfx_build_hots(g_5186, g_519a);
            car->f20 = random_int(13) - 8;
            cursor_ctl(-2);
        }
        if (ok == 0)
            continue;
        goto done;
    }
}
