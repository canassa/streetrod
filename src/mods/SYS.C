#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include "STRUCTS.H"
#include "SYS.H"
#include "BOBS.H"
#include "CARPIC.H"
#include "CURSOR.H"
#include "DASH.H"
#include "FAME.H"
#include "GAME.H"
#include "GARAGE.H"
#include "HERCDET.H"
#include "HOTSPOT.H"
#include "JOY.H"
#include "KBDGET.H"
#include "KBDOFF.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "MOUSE.H"
#include "MUSIC.H"
#include "M_02B9A.H"
#include "RACE.H"
#include "RANDOM.H"
#include "ROAD.H"
#include "SCREEN.H"
#include "SONG.H"
#include "SOUND.H"
#include "VIDDET.H"
#include "VIDEO.H"

#pragma intrinsic(inp, outp, _enable, _disable, abs)

/* File-local storage (_BSS): names chosen to fit MSC's name-hash order (docs/RECONSTRUCTION.md). */
static int g_6c46;                         /* DS:6C46 */
static int g_6c48;                         /* DS:6C48 */
static char g_6c4a;                        /* DS:6C4A */
static int g_6c4c;                         /* DS:6C4C */
static char g_6c4e;                        /* DS:6C4E */
static int g_6c50a;                        /* DS:6C50 */
static int g_6c52a;                        /* DS:6C52 */
static int g_6c54a;                        /* DS:6C54 */

/* Uninitialized storage: communals, allocated by LINK (DS:6E60..8F02); names chosen to fit LINK's hash buckets (docs/RECONSTRUCTION.md). */
struct Dev bs79aa;
struct Dev ao79c6t;
void (interrupt far *w_7aaa)(void);
char g7f32s[80];
int g8b8eb;
struct Lims ao8b90v;
struct Hot *at8ba8v;
int az8baa;
int cz8bc0y;
int g8ef6q;
int g_8ef8;
int aj8efax;
int bw8efc;

int g_0566 = -1;   /* DS:0566 */
int g_0568 = -1;   /* DS:0568 */
int g_056a = 0;   /* DS:056A */
long g_056c = 2147483647L;   /* DS:056C */
char g_0570 = 0;   /* DS:0570 */
char g_0571 = 0;   /* DS:0571 */
struct Link g_0572[4] = {{0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}, {0, 0, 0L, 0, 0, 0, ""}};   /* DS:0572 */
int g_05ba = 0;   /* DS:05BA */
int g_05bc[8] = {0, 0, 0, 0, 0, 0, 0, 0};   /* DS:05BC */
long g_05cc = 0L;   /* DS:05CC */
int g_05d0 = 0;   /* DS:05D0 */
int g_05d2 = 0;   /* DS:05D2 */
int g_05d4 = 32767;   /* DS:05D4 */
int g_05d6 = 0;   /* DS:05D6 */
int g_05d8 = 0;   /* DS:05D8 */
int g_05da = 0;   /* DS:05DA */
int g_05dc = 350;   /* DS:05DC */
int g_05de = 0;   /* DS:05DE */
int g_05e0 = 0;   /* DS:05E0 */
int g_05e2 = 0;   /* DS:05E2 */
int g_05e4 = 0;   /* DS:05E4 */
int g_05e6 = 0;   /* DS:05E6 */
int g_05e8 = 0;   /* DS:05E8 */
int g_05ea = 0;   /* DS:05EA */
int g_05ec = 0;   /* DS:05EC */
int g_05ee = 0;   /* DS:05EE */
int g_05f0 = 10;   /* DS:05F0 */
char g_05f2 = 3;   /* DS:05F2 */
char g_05f3 = 0;   /* DS:05F3 */
int g_05f4[2] = {0, 0};   /* DS:05F4 */
long g_tick = 0L;   /* DS:05F8 */
long g_05fc = 0L;   /* DS:05FC */
long g_0600 = 0L;   /* DS:0600 */
long g_0604 = 0L;   /* DS:0604 */
char g_0608 = 0;   /* DS:0608 */
char g_0609 = 0;   /* DS:0609 */
int g_060a = 0;   /* DS:060A */
char *g_060c = "cannot read drive ?";   /* DS:060C */
char *g_060e = "disk ? is write protected";   /* DS:060E */
int g_0610 = 0;   /* DS:0610 */
int g_0612 = 0;   /* DS:0612 */

/* sys_hover_reset @ 0000:0D78 (linear 0x00D78, 33 bytes) */

void sys_hover_reset(void)
{
    g_6c4a = 1;
    ao8b90v.x2 = -1;
    az8baa = 0x7d00;
    g_4736 = 0;
    g_4737 = 0;
    kbdget_key();
}

/* sys_music_pause @ 0000:0D99 (linear 0x00D99, 63 bytes) */

void sys_music_pause(void)
{
    if (g_58d2->off) {
        g_58d2->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2bab;
        sys_hover_reset();
    }
}

/* sys_music_resume @ 0000:0DD8 (linear 0x00DD8, 76 bytes) */

void sys_music_resume(void)
{
    if (g_58d0 && !g_58d2->off && g_58d2->lc != g_58d2->l8) {
        g_58d2->off = 1;
        g_58d2->dirty = 1;
        g_09f4[F_06827(-40)].f4 = g_2ba9;
        sys_hover_reset();
    }
}

/* sys_input_reset @ 0000:0E24 (linear 0x00E24, 231 bytes) */

void sys_input_reset(void)
{
    int i;
    long deadline;

    g_0566 = g_0568 = -1;
    g_056a = 0;
    g_056c = 0x7fffffffL;
    g_0570 = g_0571 = 0;
    for (i = 0; i < 4; i++)
        g_0572[i].nxt = i + 1;
    g_05ba = -1;
    deadline = g_tick + 10;
    while (deadline > g_tick) {
        for (i = 0; kbdget_key(); i++)
            ;
        if (i)
            deadline = g_tick + 10;
    }
    g_05cc = 0;
    if (g_0610 == 0 && g_05f2) {
        mouse_poll();
        g_05e6 = g_05e8 = 0;
    }
    g_4736 = 0;
    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_4732 = g_472e = g_472f = g_4730 = g_4731 = 0;
}

/* sys_timer_remove @ 0000:0F0B (linear 0x00F0B, 172 bytes) */

void sys_timer_remove(int n)
{
    register int i;

    i = g_0572[n].nxt;
    if (i >= 0)
        g_0572[i].prv = g_0572[n].prv;
    else {
        i = g_0572[n].prv;
        g_0566 = i;
        if (i < 0)
            g_056c = 0x7fffffffL;
        else
            g_056c = g_0572[i].key;
    }
    i = g_0572[n].prv;
    if (i >= 0)
        g_0572[i].nxt = g_0572[n].nxt;
    else
        g_0568 = g_0572[n].nxt;
    g_0572[n].nxt = g_056a;
    g_056a = n;
}

/* sys_unhook @ 0000:0FB7 (linear 0x00FB7, 51 bytes) */

void sys_unhook(void)
{
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0);
    _dos_setvect(8, w_7aaa);
    _enable();
    kbdoff_unhook();
    g_05e0 = 0;
    g_0044 = 0;
}

/* sys_exit @ 0000:0FEA (linear 0x00FEA, 113 bytes) */

void sys_exit(void)
{
    if (g_0610) {
        g_0610 = (g_0610 == 0x62) ? 0 : 1;
        screen_wait_msg("Quit demo");
        screen_fill_rect(0, 0, 320, 200, 0);
    }
    if (g_0044)
        sys_unhook();
    if (g_0048) {
        (*m_present)();
        viddet_restore();
    }
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    exit(5);
}

char g_064d = 0;   /* DS:064D */
char g_064e = 0;   /* DS:064E */
struct KeyMap g_0650[30] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};   /* DS:0650 */

/* sys_crit_error @ 0000:105B (linear 0x0105B, 182 bytes) */

void sys_crit_error(unsigned deverr, unsigned char errcode)
{
    int disk;
    int action;

    disk = (deverr & 0x8000) == 0;
    if (disk) {
        if (deverr & 0x1000)
            action = 1;
        else
            action = 2;
    } else
        action = 1;
    switch (errcode) {
    case 0:
        g_060e[5] = deverr + 'A';
        screen_wait_msg(g_060e);
        break;
    case 2:
        g_060c[strlen(g_060c) - 1] = deverr + 'A';
        screen_wait_msg(g_060c);
        break;
    case 9:
        screen_wait_msg("printer out of paper");
        break;
    default:
        if (disk)
            screen_wait_msg("disk error");
        else {
            screen_wait_msg("hardware error");
            action = 2;
        }
    }
    if (action == 2)
        sys_exit();
    _hardresume(action);
}

/* sys_init @ 0000:1111 (linear 0x01111, 461 bytes) */

void sys_init(void)
{
    int sumx;
    int sumy;
    int first;
    int v;
    int j;

    sumx = 0;
    sumy = 0;
    g8ef6q = 10;
    g_8ef8 = 2;
    aj8efax = 5;
    bw8efc = 5;
    outp(0x201, 0);
    outp(0x201, 0);
    first = inp(0x201);
    for (j = 1; j; j++)
        if (((v = inp(0x201)) & 0xf3) == 0xf0)
            break;
    if (g_0610 == 0 && j != 0 && v != first) {
        g_471d = 1;
        joy_read();
        sumx = g_4718;
        sumy = g_471a;
        joy_read();
        sumx += g_4718;
        sumy += g_471a;
        joy_read();
        sumx += g_4718;
        sumy += g_471a;
        g_471e = sumx / 3;
        g_4720 = sumy / 3;
        g_4722 = -(g_471e >> 1);
        g_4724 = g_471e >> 1;
        g_4726 = -(g_4720 >> 1);
        g_4728 = g_4720 >> 1;
    }
    if (g_0610 == 0 && g_0262 && mouse_reset() == -1) {
        g_05f2 = 1;
        g_05e6 = g_05e8 = 0;
        g_05ea = 1;
        g_05ec = 1;
    } else
        g_05f2 = 0;
    g_6c46 = -1;
    g_6c48 = -1;
    bs79aa.off = 0;
    bs79aa.l8 = g_2e6b_0000;
    bs79aa.cur = bs79aa.lc = bs79aa.l8;
    bs79aa.f16 = bs79aa.f14 = g_5858;
    bs79aa.f1a = bs79aa.f18 = g_5882;
    ao79c6t.off = 0;
    ao79c6t.f1a = ao79c6t.f18 = g_58cc;
    _harderr(sys_crit_error);
    _disable();
    outp(0x43, 0x36);
    outp(0x40, 0);
    outp(0x40, 0x40);
    w_7aaa = _dos_getvect(8);
    _dos_setvect(8, sys_timer_isr);
    _enable();
    kbdget_hook();
    g_05e0 = 1;
    outp(0x43, 0xb6);
    sys_input_reset();
    g_0044 = 1;
}

/* sys_keymap_set @ 0000:12DE (linear 0x012DE, 179 bytes) */

void sys_keymap_set(int n)
{
    struct KeyMap *p;
    int num;
    struct KeyMap *q;
    char c;

    num = 1;
    if (g_0610)
        g_47cc = 0;
    g_6c4c = n;
    sys_keymap_restore(0);
    g_064e = n;
    p = g_579f[n];
    q = g_0650;
    for (; p->key; p++, q++, num++) {
        if (num >= 30)
            break;
        q->val = g_4766[q->key = c = p->key];
        g_4766[c] = p->val;
    }
    p = g_579f[42];
    q->val = g_4766[q->key = c = p->key];
    g_4766[c] = p->val;
    q[1].key = 0;
}

/* sys_keymap_restore @ 0000:1391 (linear 0x01391, 134 bytes) */

int sys_keymap_restore(int keep)
{
    int i;
    struct KeyMap *p;

    for (i = 0; i < 8; i++)
        g_47d8[i] = 0;
    g_472e = g_472f = g_4730 = g_4731 = 0;
    i = g_064e;
    if (i) {
        if (keep) {
            for (p = g_0650; p->key; p++)
                if (p->val >= 0x20)
                    g_4766[p->key] = p->val;
        } else {
            for (p = g_0650; p->key; p++)
                g_4766[p->key] = p->val;
            g_064e = 0;
        }
    }
    return i;
}

char g_0692[16] = {0, 0, 8, 7, 15, 7, 15, 8, 7, 2, 4, 12, 5, 10, 6, 14};   /* DS:0692 */
int g_06a2 = 0;   /* DS:06A2 */
int g_06a4 = 0;   /* DS:06A4 */
char g_06a6 = 0;   /* DS:06A6 */
char g_06a7 = 0;   /* DS:06A7 */
int g_06a8 = 0;   /* DS:06A8 */
int g_06aa = 0;   /* DS:06AA */
int g_06ac = 0;   /* DS:06AC */
int g_06ae = 0;   /* DS:06AE */
int g_06b0 = 0;   /* DS:06B0 */
int g_06b2 = 0;   /* DS:06B2 */
int g_06b4[22] = {-1, 0, -1, 0, 1, 1, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 0, 0, 2};   /* DS:06B4 */
char g_06e0 = 0;   /* DS:06E0 */
int g_06e2 = 1;   /* DS:06E2 */
int g_06e4 = 0;   /* DS:06E4 */
int g_06e6 = 0;   /* DS:06E6 */
char g_06e8 = 0;   /* DS:06E8 */
char g_06e9 = 0;   /* DS:06E9 */
char g_06ea = 0;   /* DS:06EA */

/* sys_wait_input @ 0000:1417 (linear 0x01417, 1068 bytes) */

int sys_wait_input(int ticks)
{
    register struct Link *p;
    long end;
    int x;
    register int i;
    int y;
    int res;
    char flag;
    int j;
    char old;
    int bit;

    res = 0;
    j = 0;
    bit = 0;
    if (g_6c4c == 0x16) {
        aq8256z = 1;
        F_11421(-1, "", 0);
        aq8256z = 1;
        F_11421(g_51ee, "", cz8bc0y);
        g7f32s[0] = 0;
    }
    flag = g_05cc != 0L;
    end = (long)ticks + g_tick;
    do {
        if (g_0610 == 0x63 && g_06a4 != 0x63) {
            g_06a4 = 0x63;
            sys_exit();
        }
        if (g_4bc8 && (r_vid.mode == -2 || r_vid.mode == -6)
            && !(g_tick & 0xf) && g_58d0 && g_47cf) {
            old = g_0692[8];
            g_0692[8] = g_0692[7];
            g_0692[7] = g_0692[6];
            g_0692[6] = old;
            F_112CB(6, old);
            F_112CB(7, g_0692[7]);
            F_112CB(8, g_0692[8]);
        }
        res = kbdget_key();
        if (res > 0) {
            if (g_05d0) {
                g_05d2 = res;
                return 0x3eb;
            }
            if (res == 0x7f)
                sys_exit();
        } else if (res < 0) {
            res += 0x32;
            if (res == -0x28) {
                if (g_58d4 == 1) {
                    if (g_58d0) {
                        sys_music_pause();
                        g_58d0 = 0;
                    } else {
                        g_58d0 = 1;
                        sys_music_resume();
                    }
                }
            } else
                return res;
        }
        if (g_056c <= g_tick) {
            p = &g_0572[i = g_0566];
            switch (p->fa) {
            case 0:
            default:
                sys_timer_remove(i);
                break;
            case 1:
                res = p->fc;
                sys_timer_remove(i);
                video_anim_resume(res);
                break;
            }
        }
        if (g_0610 == 0 && g_05f2) {
            mouse_poll();
            if (g_6c4a |= (g_05e2 != 0 || g_05e4 != 0)) {
                g_472a += g_05e2;
                g_472c += g_05e4;
            }
        }
        if (g_4736 | g_4737) {
            _disable();
            g_6c4e = g_4736;
            x = g_4738;
            y = g_473a;
            g_4737 = g_4736 = 0;
            _enable();
            if (g_05de) {
                g_05d2 = 0;
                return 0x3eb;
            }
            if (g8b8eb) {
                if (i = (x >= ao8b90v.x1 && x < ao8b90v.x2 && y >= ao8b90v.y1 && y < ao8b90v.y2)
                        ? az8baa : menu_hit(x, y))
                    return i;
            }
            if (g_05d0) {
                g_05d2 = 0;
                return 0x3eb;
            }
        } else if (flag && g_05cc < g_tick) {
            g_05cc = 0L;
            return 0x3e8;
        }
        if (g_4733 > 0 && g_6c4a) {
            _disable();
            if ((g_6c46 = g_472a) < 0)
                g_6c46 = g_472a = 0;
            else if (g_6c46 > 0x13a)
                g_6c46 = g_472a = 0x13a;
            if ((g_6c48 = g_472c) < 0)
                g_6c48 = g_472c = 0;
            else if (g_6c48 > 0xc2)
                g_6c48 = g_472c = 0xc2;
            g_6c4a = 0;
            _enable();
            cursor_draw(x = g_6c46, y = g_6c48);
            if (g8b8eb
                && (x < ao8b90v.x1 || x >= ao8b90v.x2 || y < ao8b90v.y1 || y >= ao8b90v.y2)) {
                res = az8baa;
                if (i = menu_hit(x, y)) {
                    if (res != i) {
                        if (g_6c4c == 0x25) {
                            aq8256z = 1;
                            F_11421(-1, "", 0);
                            aq8256z = 1;
                            F_11421(g_51ee, "", cz8bc0y);
                        }
                        F_11421(-1, g_239e + at8ba8v->f4, 0);
                        bit = 1;
                    }
                } else {
                    if (bit) {
                        F_11421(-1, "", 0);
                        bit = 0;
                    }
                    if (g_6c4c == 0x25)
                        F_11421(g_51ee, g7f32s, cz8bc0y);
                    else if (res)
                        F_11421(-1, "", 0);
                }
            }
        }
    } while (end > g_tick);
    return 0;
}

struct Eng e_78e0;
int t_72ec;
struct Tone *bn82c4;

/* sys_timer_add @ 0000:1843 (linear 0x01843, 327 bytes) */

int sys_timer_add(int delay, int pri)
{
    register int i, cur3;
    int prev;
    long when, key;

    g_0571 = 1;
    i = g_056a;
    if (i < 0) {
        g_0571 = 0;
        return -1;
    }
    cur3 = g_0568;
    prev = -1;
    when = g_tick + delay;
    if (delay) {
        for (;;) {
            if (cur3 >= 0) {
                if ((key = g_0572[cur3].key) > when) {
                    cur3 = g_0572[prev = cur3].nxt;
                    continue;
                }
                if (when > key)
                    break;
                for (;;) {
                    if (g_0572[cur3].f8 >= pri)
                        break;
                    cur3 = g_0572[prev = cur3].nxt;
                    if (cur3 < 0 || g_0572[cur3].key != key)
                        break;
                }
            }
            break;
        }
    }
    g_0572[i].key = when;
    g_0572[i].f8 = pri;
    g_056a = g_0572[i].nxt;
    g_0572[i].nxt = cur3;
    if (cur3 < 0) {
        g_0566 = i;
        g_056c = when;
    } else
        g_0572[cur3].prv = i;
    g_0572[i].prv = prev;
    if (prev < 0)
        g_0568 = i;
    else
        g_0572[prev].nxt = i;
    g_0571 = 0;
    return i;
}

/* sys_timer_anim @ 0000:198A (linear 0x0198A, 50 bytes) */

void sys_timer_anim(int fc, int delay)
{
    int n;

    n = sys_timer_add(delay, g8ef6q);
    g_0572[n].fa = 1;
    g_0572[n].fc = fc;
}

/* sys_timer_cancel @ 0000:19BC (linear 0x019BC, 73 bytes) */

void sys_timer_cancel(int n)
{
    struct Link *p;
    int i;

    for (i = g_0566; i >= 0; i = g_0572[i].nxt)
        if ((p = g_0572 + i)->fa == 1 && p->fc == n)
            p->fa = 0;
}

/* sys_music_start @ 0000:1A05 (linear 0x01A05, 119 bytes) */

void sys_music_start(int n, int cnt)
{
    if (g_58d0) {
        g_58d2->off = 0;
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
        g_58d2->f4 = 0;
        g_58d2->dirty = 1;
        g_58d2->lc = g_58d2->cur = g_58d2->l8 + n;
        g_58d2->f2 = cnt + 1;
        g_58d2->f1a = g_58d2->f18;
        bn82c4 = g_tones + *g_58d2->f1a;
        t_72ec = 1;
        g_58d2->off = 1;
    } else {
        g_58d2->off = 0;
    }
}

/* sys_music_stop @ 0000:1A7C (linear 0x01A7C, 49 bytes) */

void sys_music_stop(void)
{
    g_58d2->off = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    g_58d2->dirty = 1;
    g_58d2->lc = g_58d2->cur = g_58d2->l8;
}

/* sys_drive_start @ 0000:1AAD (linear 0x01AAD, 122 bytes) */

void sys_drive_start(void)
{
    g_060a = 0;
    _dos_setvect(8, sys_drive_isr);
    if (!g_0610 && g_05f2) {
        mouse_poll();
        g_05e6 = g_05e8 = 0;
        g_05ea = 1;
        g_05ec = 6;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
    e_78e0.f2 = e_78e0.f4 = 0;
    g_0609 = 0;
    outp(0x42, 0x1e);
    outp(0x42, 0);
    if (g_47ce) {
        outp(0x61, inp(0x61) | 3);
        g_47cf = 1;
    }
}

/* sys_drive_stop @ 0000:1B27 (linear 0x01B27, 106 bytes) */

void sys_drive_stop(void)
{
    _dos_setvect(8, sys_timer_isr);
    g_58dc = 0;
    g_58da = 0;
    g_5e0e = 0;
    outp(0x61, inp(0x61) & 0xfc);
    g_47cf = 0;
    if (!g_0610 && g_05f2) {
        mouse_poll();
        g_05e6 = g_05e8 = 0;
        g_05ea = 1;
        g_05ec = 1;
    }
    g_4730 = g_4731 = g_4732 = g_4736 = 0;
}

/* sys_sign @ 0000:1B91 (linear 0x01B91, 29 bytes) */

int sys_sign(int x)
{
    if (x == 0)
        return 0;
    if (x > 0)
        return 1;
    return -1;
}

/* Loop optimization is off from here to the end of the file. The file is
   compiled /Oalsi, but in the shipped code the loops of sys_demo_step, sys_timer_isr and
   sys_drive_isr (the timer interrupt handler) are plain /Ol-off loops: counters and
   pointers stay in memory, the test is reached by a jmp, no induction
   variables. No for/while/do formulation reproduces that under /Ol (/Ol takes
   every such loop here); this one pragma reproduces all three functions. */
#pragma loop_opt(off)

/* sys_demo_step @ 0000:1BAE (linear 0x01BAE, 1076 bytes) */

void sys_demo_step(void)
{
    int num;
    int cx;
    int k;
    int mask;
    int step;
    int cy;

    if (g_6c4c == 2)
        random_int(-1);
    if (g_06a6) {
        g_47cc |= 0x80;
        g_06a6 = 0;
        joy_demo_key();
        return;
    }
    if (!g_06a7) {
        if (g_06ac && g_6c4c == 10) {
            g_06ac = 0;
            g_06a8 = 15;
        }
        num = (int)strlen((char *)g_579f[g_6c4c]) / 2;
        k = 0;
        switch (g_6c4c) {
        case 14:
        case 18:
        case 19:
        case 22:
        case 36:
            num -= 2;
            break;
        case 11:
            num -= 2;
        case 37:
            k = 1;
            break;
        case 23:
            g_06a8 = 4;
            break;
        case 8:
            k = 4;
            break;
        case 28:
        case 35:
            k = 2;
            break;
        case 9:
            if (a_7eaap.m7eb0)
                break;
        case 13:
            num--;
            break;
        }
        if (g_6c4c == 10 && g_06aa != 10) {
            g_6c50a = num - g_06a8++ - 1;
            g_06a8 = g_06a8 % num;
        } else
            g_6c50a = random_int(num + k);
        switch (g_6c4c) {
        case 21:
            if ((g_6c50a >= 0 && g_6c50a < 9) || g_6c50a == 12)
                g_6c50a = 9;
            if (g_06aa == 28)
                g_6c50a = 11;
            break;
        case 20:
            g_6c50a = g_06b4[g_06b2++];
            if (g_6c50a == 2)
                g_06b2 = 0;
            break;
        case 23:
            if (g_6c50a)
                break;
        case 2:
            g_6c50a = 1;
            break;
        case 4:
            if (g_06a8 < 11)
                g_6c50a = 0;
            else
                g_6c50a = num - 1;
            break;
        case 34:
        case 41:
            g_6c50a = 0;
            break;
        case 36:
            if (g_06aa == 36)
                g_6c50a = 0;
            break;
        case 11:
            if (g_06aa == 11)
                g_6c50a = 10;
            break;
        case 27:
            g_6c50a = 5;
            break;
        case 7:
            if (a_7eaap.m7eb0 == 0)
                g_6c50a = 0;
            break;
        }
        if (g_6c4c == 8)
            g_06ae = 160;
        if (g_6c50a >= 0) {
            if (num > g_6c50a) {
                g_47cc = g_579f[g_6c4c][g_6c50a].key;
                mask = g_579f[g_6c4c][g_6c50a].val + 0x32;
                step = g_2bad[g_6c4c];
                for (; step - g_2bad[g_6c4c] < 32 && g_09f4[step].f6 != mask; step++)
                    ;
                if (g_09f4[step].f6 == mask) {
                    cx = (g_09f4[step].x1 + g_09f4[step].x2) / 2;
                    if (cx)
                        g_06ae = cx;
                    if (g_6c4c == 8)
                        g_06ae = 160;
                    cy = (g_09f4[step].y1 + g_09f4[step].y2) / 2;
                    if (cy)
                        g_06b0 = cy;
                }
            } else {
                if (num == g_6c50a)
                    g_47cc = 0x1c;
                if (num + 1 == g_6c50a)
                    g_47cc = 0x39;
                if (num + 2 == g_6c50a)
                    g_47cc = 0x48;
                if (num + 3 == g_6c50a)
                    g_47cc = 0x50;
                if (num + 4 == g_6c50a)
                    g_47cc = 0x4b;
                if (num + 5 == g_6c50a)
                    g_47cc = 0x4d;
            }
        } else
            g_47cc = 0x4c;
    }
    if ((g_06ae && g_472a != g_06ae) || (g_06b0 && g_472c != g_06b0)) {
        g_06a7 = 1;
        return;
    }
    g_06a7 = 0;
    if (g_6c4c == 0x25 && g_06ac < 10 && g_47cc == 0x22)
        g_47cc = 0x1c;
    if (g_6c4c == 10 && g_06aa == 10) {
        g_47cc = 0x1c;
        g_06aa = 0;
    } else
        g_06aa = g_6c4c;
    if (a_7eaap.m7eb8 == 0 && g_6c4c == 7)
        game_add_part(0x2a, g_4806[42].f0);
    if (a_7eaap.m7eb0 == 0) {
        if (g_6c4c == 10)
            g_47cc = 0x31;
        if (g_6c4c == 8 && (g_47cc == 1 || g_47cc == 0x22 || g_47cc == 0x19))
            g_47cc = 0x39;
    }
    if (g_47cc == 0x46)
        g_47cc = 0x1c;
    g_06a6 = 1;
    joy_demo_key();
}

/* sys_timer_isr @ 0000:1FE2 (linear 0x01FE2, 914 bytes) */

void interrupt far sys_timer_isr(void)
{
    int x, y;
    register int delta;

    g_tick++;
    if (g_0608--) {
        outp(0x20, 0x20);
        _enable();
    } else {
        (*w_7aaa)();
        g_0608 = 3;
        g_0600++;
    }
    if (!g_47ce && g_47ce != g_47cf) {
        outp(0x61, inp(0x61) & 0xfc);
        g_47cf = 0;
    }
    if (g_0610) {
        if (((g_06a6 ? 0xf : 0x7f) & g_tick) == 0)
            sys_demo_step();
        if ((g_06ae || g_06b0) && (g_472a != g_06ae || g_472c != g_06b0)) {
            g_472a += sys_sign(g_06ae - g_472a);
            g_472c += sys_sign(g_06b0 - g_472c);
            g_6c4a = 1;
        }
    }
    if (g_58d2->off && --g_58d2->f2) {
        if (t_72ec && g_58d2->f0.w && !--g_06e2) {
            bn82c4->f0 = bn82c4->f0 % bn82c4->f3;
            g_6c54a = g_57f6[bn82c4->f1][bn82c4->f0++] * g_6c52a + g_58d2->f0.w;
            outp(0x42, g_6c54a);
            outp(0x42, g_6c54a >> 8);
            g_06e2 = bn82c4->f4;
        }
    } else if (g_58d2->off && g_58d2->f2 == 0) {
        while (!(g_58d2->f2 = *g_58d2->cur++)) {
            if (*g_58d2->cur++ == 0) {
                outp(0x61, inp(0x61) & 0xfc);
                g_47cf = 0;
                g_58d2->off = 0;
                g_58d2->dirty = 1;
                g_58d2->lc = g_58d2->cur = g_58d2->l8;
                goto tail;
            }
            g_58d2->f1a++;
            if (*g_58d2->f16 < ++g_58d2->f4) {
                if (*++g_58d2->f16 < 0) {
                    g_58d2->cur = g_58d2->l8 + 2;
                    g_58d2->f16 = g_58d2->f14;
                    g_58d2->f1a = g_58d2->f18;
                }
                g_58d2->lc = g_58d2->cur;
                g_58d2->f4 = 0;
            } else
                g_58d2->cur = g_58d2->lc;
            bn82c4 = g_tones + *g_58d2->f1a;
        }
        if (!(g_58d2->f0.w = *g_58d2->cur++)) {
            g_58d2->dirty = 1;
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        } else {
            if (t_72ec) {
                g_06e2 = bn82c4->f4;
                g_6c52a = (unsigned)bn82c4->f2 * g_58d2->f0.w / 1000;
            }
            outp(0x42, g_58d2->f0.b[0]);
            outp(0x42, g_58d2->f0.b[1]);
            if (g_47ce && (g_47ce != g_47cf || g_58d2->dirty)) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
                if (g_58d2->dirty)
                    g_58d2->dirty = 0;
            }
        }
    }
tail:
    if (g_4733 > 0) {
        _disable();
        if (!g_06e0) {
            g_06e0 = 1;
            _enable();
            x = y = 0;
            if (g_4740 == 0) {
                if (delta = g_472e)
                    x = delta > 0 ? 1 : -1;
                if (delta = g_472f)
                    y = delta > 0 ? 1 : -1;
            }
            if (g_0610 == 0 && g_471d) {
                joy_read();
                if ((delta = g_4718) < g_4722)
                    x = -1;
                else if (g_4724 < delta)
                    x = 1;
                if ((delta = g_471a) < g_4726)
                    y = -1;
                else if (g_4728 < delta)
                    y = 1;
            }
            if (x | y) {
                g_472a += g_474c[x];
                g_472c += g_4752[y];
                g_6c4a = 1;
            }
            g_06e0 = 0;
        }
    }
}

struct State7E8A ar7e8av;
struct State7D2E ao7d2ev;
int ao8228u;
long b_82bc;
int z_8b7c;

/* sys_car_halt @ 0000:2374 (linear 0x02374, 24 bytes) */

void sys_car_halt(void)
{
    e_78e0.f14 = e_78e0.f16 = e_78e0.f2 = e_78e0.f4 = g_05d6 = 0;
    e_78e0.fbe = e_78e0.fc0;
}

/* sys_drive_isr @ 0000:238C (linear 0x0238C, 2062 bytes) */

void interrupt far sys_drive_isr(void)
{
    struct Range *sp;
    char flag = 0;
    char s = 0;
    char prev = 0;
    char c = 0;
    char top = 0;
    union Word val;
    int i;
    int off;
    int n;
    unsigned r;

    g_tick++;
    if (g_0608--) {
        outp(0x20, 0x20);
        _enable();
        if (g_47ce) {
            if (g_47ce != g_47cf) {
                outp(0x61, inp(0x61) | 3);
                g_47cf = 1;
            }
            if (g_0608 & 1) {
                if (g_0286 == 0) {
                    r = (unsigned)(((long)e_78e0.fe * e_78e0.fe) >> 10);
                    r += r >> (r + 2) >> 3;
                    val.w = 50000 - ((e_78e0.fe == 5700 ? 10000 : 0) + r);
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                } else if (g_5a0e) {
                    off = ac825et - z_8b7c;
                    r = off < 0 ? -off : off;
                    r = (r >> 1) * r;
                    if (r > 20000)
                        r = 20000;
                    val.w = 60000 - r;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                }
            } else {
                g_06ea ^= 1;
                if (g_58dc) {
                    if (g_06e4++ < 10)
                        val.w = 0x54b;
                    else
                        val.w = 0x712;
                    outp(0x42, val.b[0]);
                    outp(0x42, val.b[1]);
                    if (g_06e4 > 20)
                        g_06e4 = 0;
                } else if (g_58da || g_5e0e) {
                    if (g_06ea) {
                        val.w = 0x95;
                        outp(0x42, val.b[0]);
                        outp(0x42, val.b[1]);
                    }
                }
            }
        } else if (g_47ce != g_47cf) {
            outp(0x61, inp(0x61) & 0xfc);
            g_47cf = 0;
        }
    } else {
        g_0600++;
        (*w_7aaa)();
        g_0608 = 3;
        if (bw8aca && !g_0286 && --g_0609 <= 0) {
            g_0609 = 3;
            _disable();
            if (!g_06e8) {
                g_06e8 = 1;
                _enable();
                flag = s = prev = c = 0;
                if (g_4740 == 0) {
                    if (n = g_472e) {
                        s = 1;
                        g_472a = n > 0 ? 1 : -1;
                    }
                    if (n = g_472f) {
                        if (n > 0)
                            prev = 1;
                        else
                            flag = 1;
                    }
                    if (g_0610 == 0 && g_471d) {
                        joy_read();
                        if ((n = g_4718) < g_4722) {
                            g_6c4a = s = 1;
                            g_472a += -1;
                        } else if (g_4724 < n) {
                            g_6c4a = s = 1;
                            g_472a++;
                        }
                        if ((n = g_471a) < g_4726) {
                            g_6c4a = flag = 1;
                            g_472c += -1;
                        } else if (g_4728 < n) {
                            g_6c4a = prev = 1;
                            g_472c++;
                        }
                    }
                    if (g_0610 == 0 && g_05f2) {
                        mouse_poll();
                        if (g_05e4 > 0) {
                            g_6c4a = 1;
                            g_472c++;
                            prev = 1;
                        }
                        if (n = g_05e2) {
                            g_6c4a = s = 1;
                            g_472a += n > 0 ? 1 : -1;
                        }
                        if (g_05f0)
                            flag = 1;
                    }
                }
                c = g_4732 && !g_06e9;
                g_06e9 = g_4732;
                if (g_47cd) {
                    c = 1;
                    g_47cd = 0;
                }
                if (g_0610 || g_0612) {
                    if (e_78e0.f14 * 4 + e_78e0.fe < 5700) {
                        if (ar7e8av.m7e8a > ao7d2ev.m7d30 ||
                            (!a_7eaap.m7ec0 ? 0x17c : 0x3fc) - 60 < ao7d2ev.m7d30)
                            flag = 1;
                    } else if (e_78e0.f18 < e_78e0.f1c && ao8228u) {
                        c = 1;
                        flag = 1;
                        g_06ac++;
                    }
                    if (e_78e0.fe < 2000 && e_78e0.f18 > 1 && !flag)
                        c = 1;
                    if (e_78e0.f20 && e_78e0.f18 > 0)
                        c = 0;
                }
                if (bw8aca) {
                    b_82bc += e_78e0.f0;
                    e_78e0.fc0++;
                }
                if ((!a_7eaap.m7ec0 ? 0x17c : 0x3fc) < ao7d2ev.m7d30) {
                    prev = 1;
                    flag = s = 0;
                    if (e_78e0.f0 > 15)
                        e_78e0.f0 -= 15;
                }
                if (g_54da > 0) {
                    prev = 1;
                    flag = s = 0;
                    g_54da--;
                }
                if (e_78e0.fe > 5000) {
                    a_7eaap.m7eb0->rack.s[0]->b += !a_7eaap.m7ec0 ? 1 : 1;
                    a_7eaap.m7eb0->slot6->b += !a_7eaap.m7ec0 ? 1 : 1;
                }
                if (!(g_0600 & (!a_7eaap.m7ec0 ? 0xf : 0xf)))
                    a_7eaap.m7eb0->rack.s[0]->b++;
                if ((i = a_7eaap.m7eb0->rack.s[0]->b) >= 9900) {
                    e_78e0.fbc |= 5;
                    sys_car_halt();
                }
                if (flag) {
                    e_78e0.fe += e_78e0.f14;
                    if (e_78e0.f20 && e_78e0.fe > 5690)
                        e_78e0.fe = 5690;
                    if (e_78e0.fe > 5700) {
                        e_78e0.fe = 5700;
                        if (15 - i / 1000 < ++g_06e6) {
                            e_78e0.fbc |= 5;
                            sys_car_halt();
                        }
                    }
                } else if (e_78e0.fe > 1500 ||
                           (e_78e0.f18 > 0 && e_78e0.fe > e_78e0.f16))
                    e_78e0.fe -= e_78e0.f16;
                if (e_78e0.fe < 5700)
                    g_06e6 = 0;
                top = e_78e0.f0 < 5 && !flag && e_78e0.f18 > 0;
                if (c || top ||
                    (e_78e0.f20 && e_78e0.f24 > 1 &&
                     ((flag && e_78e0.fe > e_78e0.f22 && e_78e0.f18 < e_78e0.f1c) ||
                      (!flag && e_78e0.f18 > 1 && !e_78e0.f42)))) {
                    a_7eaap.m7eb0->slot6->b += e_78e0.fe > 5000 ? 15 : 3;
                    if (g_0610 == 0) {
                        i = a_7eaap.m7eb0->slot6->b;
                        if (i >= 9900 ||
                            (e_78e0.f20 == 0 && flag && e_78e0.fe > 5000 &&
                             i / 100 + 100 > random_int(3277))) {
                            e_78e0.fbc |= 0x11;
                            sys_car_halt();
                        }
                    }
                    if ((prev || (!e_78e0.f42 && !flag)) && e_78e0.f18 > 0)
                        e_78e0.f18--;
                    else if ((flag || (e_78e0.f42 > 0 && !prev)) &&
                             e_78e0.f18 < e_78e0.f1c)
                        e_78e0.f18++;
                    if (e_78e0.f20) {
                        if (c)
                            e_78e0.f24 = e_78e0.f24 == 1 ? 2 : 1;
                        if (top || e_78e0.f18 == 0)
                            e_78e0.f24 = 1;
                        if (e_78e0.f24 == 1)
                            e_78e0.f18 = 0;
                    } else if (top)
                        e_78e0.f18 = 0;
                    e_78e0.f1a = 1;
                    e_78e0.f40 = e_78e0.f34[e_78e0.f18];
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) == 0)
                        e_78e0.fe = 1505;
                }
                i = -1;
                do {
                    sp = &e_78e0.gear[e_78e0.f18].r[++i];
                } while (i < 2 && sp->f2 < e_78e0.fe);
                if (i != e_78e0.f42 || e_78e0.f1a) {
                    e_78e0.f42 = i;
                    e_78e0.f14 = sp->f4;
                    e_78e0.f16 = sp->f6;
                }
                if (prev || (g_5e0e && e_78e0.f0 > 8)) {
                    if ((e_78e0.f0 -= (e_78e0.f0 >> 4) + 2) < 0)
                        e_78e0.f0 = 0;
                    if ((e_78e0.fe = e_78e0.f0 * e_78e0.f40) < 1500)
                        e_78e0.fe = 1500;
                } else if (e_78e0.f18 == 0 && e_78e0.f0) {
                    if (e_78e0.f0)
                        e_78e0.f0--;
                    e_78e0.fe = 30 * e_78e0.f0 + 1500;
                } else if (e_78e0.f40)
                    e_78e0.f0 = e_78e0.fe / e_78e0.f40;
                if (s && abs(e_78e0.f2 + g_472a) > abs(e_78e0.f2)) {
                    if (abs(g_472a) + abs(e_78e0.f4) == abs(e_78e0.f4 + g_472a) &&
                        abs(e_78e0.f2) < abs(e_78e0.f4))
                        e_78e0.f2 = e_78e0.f4;
                    e_78e0.f2 += g_472a << 1;
                    if (abs(e_78e0.f2) > 24)
                        e_78e0.f2 = e_78e0.f2 > 0 ? 24 : -24;
                    g_472a = 0;
                    e_78e0.f4 = e_78e0.f2;
                } else
                    e_78e0.f2 = 0;
                if (abs(e_78e0.f4) > 0)
                    e_78e0.f4 += e_78e0.f4 > 0 ? -((e_78e0.f4 >> 2) + 1)
                                               : ((-e_78e0.f4 >> 2) + 1);
                if (bw8aca && road_check_crash()) {
                    e_78e0.fbc |= 0x21;
                    sys_car_halt();
                }
                g_06e8 = 0;
            }
        }
    }
}
