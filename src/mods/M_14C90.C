#include <string.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "M_14C90.H"
#include "MESSAGES.H"
#include "M_00000.H"
#include "M_00D58.H"
#include "M_02C04.H"
#include "M_0F38A.H"
#include "M_10B7C.H"
#include "M_113A2.H"
#include "M_11E9C.H"
#include "M_13682.H"
#include "M_139DE.H"
#include "M_163BE.H"
#include "M_19ED0.H"

char g_09f2 = 0;   /* DS:09F2 */

/* F_14C90 @ 0F38:5910 (linear 0x14C90, 1063 bytes)

   Line editor. Edit the string buf (at most max characters, clipped to the width of
   the back bitmap) at x, y on the back page until Enter or a negative event; backspace, printable
   characters 0x20..0x5A (digits only when g_09f2), a blinking block cursor. Returns the last key. */

int F_14C90(int x, int y, char *buf, int max)
{
    char colr;
    char dest[2];
    char pos, oc;
    int end, w, px;
    char ip;
    int key, mono, cx, ret, cy, n;

    pos = x_colors.c[2];
    oc = x_colors.c[3];
    mono = (unsigned char)(r_vid.back->flags <= -1);
    if ((w = 6 * max) + x >= r_vid.back->width) {
        max = (r_vid.back->width - x - 1) / 6;
        buf[max] = 0;
        w = 6 * max;
    }
    x_colors.c[2] = x_colors.c[1];
    x_colors.c[3] = x_colors.c[0];
    F_120FE(-3);
    F_13682(r_vid.back, x, y, w, 8);
    (*a_78aem)(r_vid.back, x, y, buf);
    F_1371C(r_vid.back, cx = g824cd, cy = bq824e, 8);
    colr = -1;
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, x, y, x + w, y + 8);
    ip = g_290c;
    g_290c = 0;
    F_00D78();
    ret = F_01391(1);
    F_02C04(g_09f2);
    n = g_05d0;
    g_05d0 = 1;
    g_05d2 = 0;
    dest[1] = 0;
    end = strlen(buf);
    for (;; g_05d2 = 0) {
        if ((key = F_01417(0x23)) != 0) {
            if (key < 0)
                break;
            key = g_05d2;
            if (key == 13) {
                if (!g_09f2 || *buf)
                    break;
                F_16B3A();
                continue;
            }
            if (key != 0) {
                if (key != 8) {
                    if (key < 0x20 || key > 0x5a)
                        continue;
                    if (g_09f2 && (key < 0x30 || key > 0x39)) {
                        F_16B3A();
                        continue;
                    }
                }
                F_120FE(-3);
                x_colors.c[3] = x_colors.c[1];
                F_1371C(r_vid.back, cx, cy, 8);
                if (key == 8) {
                    if (end > 0) {
                        dest[0] = buf[--end];
                        cx -= F_10B48(dest);
                        px = cx;
                        (*a_78aem)(r_vid.back, cx, cy, " ");
                    } else
                        F_16B3A();
                } else if (end < max) {
                    buf[end++] = key;
                    buf[end] = 0;
                    (*a_78aem)(r_vid.back, px = cx, cy, buf + end - 1);
                    cx = g824cd;
                } else
                    F_16B3A();
                x_colors.c[3] = x_colors.c[0];
                F_1371C(r_vid.back, cx, cy, 8);
                colr = -1;
                F_120FE(-1);
                if (r_vid.f16)
                    F_1A92A(r_vid.back, r_vid.front, px, cy, px + 6, cy + 8);
            }
        } else {
            F_120FE(-3);
            x_colors.c[3] = colr ? x_colors.c[1] : x_colors.c[0];
            F_1371C(r_vid.back, cx, cy, 8);
            colr = ~colr;
            F_120FE(-1);
            if (r_vid.f16)
                F_1A92A(r_vid.back, r_vid.front, cx, cy, cx + 1, cy + 8);
        }
    }
    buf[end] = 0;
    F_120FE(-3);
    x_colors.c[3] = x_colors.c[1];
    F_1371C(r_vid.back, cx, cy, 8);
    F_120FE(-1);
    if (r_vid.f16)
        F_1A92A(r_vid.back, r_vid.front, cx - 6, y, cx, y + 8);
    x_colors.c[3] = oc;
    x_colors.c[2] = pos;
    g_290c = ip;
    F_00D78();
    g_05d0 = n;
    F_02C2B();
    if (ret)
        F_012DE(ret);
    return key;
}

/* F_150B7 @ 0F38:5D37 (linear 0x150B7, 112 bytes)

   Number prompt: draw the prompt s at (x, y) in the style's colours, let the player edit a
   text of up to 20 characters at (g824cd, bq824e) and store its value in *out. Returns the result
   of the editor when that is 0 or negative, else 0 */

int F_150B7(int x, int y, char *s, int *out)
{
    char buf[20];
    int rl;

    x_colors = bustyle->pal[5];
    F_119D6(x, y, s);
    g_09f2 = 1;
    buf[0] = 0;
    rl = F_14C90(g824cd, bq824e, buf, 20);
    g_09f2 = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* F_15127 @ 0F38:5DA7 (linear 0x15127, 119 bytes)

   Number field of the current hot spot az8ba2t: edit a text into buf (at most 10 characters,
   fewer when the box is narrow: 6 pixels each) with the spot's colours and store its value in *out.
   Returns the editor's result when that is 0 or negative, else 0 */

int F_15127(int *out, char *buf)
{
    int rl;

    x_colors = bustyle->pal[az8ba2t->f2];
    g_09f2 = 1;
    rl = F_14C90(az8ba2t->x1, az8ba2t->y1, buf,
        (az8ba2t->x2 - az8ba2t->x1) / 6 > 10 ? 10 : (az8ba2t->x2 - az8ba2t->x1) / 6);
    g_09f2 = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* F_1519E @ 0F38:5E1E (linear 0x1519E, 54 bytes)

   Show hot spot id, read a number into *out with F_15127 and close the spot again;
   returns the editor's result */

int F_1519E(int id, int *out)
{
    int res;
    char buf[12];

    F_13DE6(id);
    buf[0] = 0;
    res = F_15127(out, buf);
    F_13F21(1);
    return res;
}

/* F_151D4 @ 0F38:5E54 (linear 0x151D4, 98 bytes)

   Show hot spot id and let the player edit the text s (at most len characters) in its box
   with the spot's colours; returns the editor's result when that is 0 or negative, else 0 */

int F_151D4(int id, int len, char *s)
{
    int rl;

    F_13DE6(id);
    x_colors = bustyle->pal[az8ba2t->f2];
    rl = F_14C90(az8ba2t->x1, az8ba2t->y1, s, len);
    F_13F21(1);
    return rl > 0 ? 0 : rl;
}
