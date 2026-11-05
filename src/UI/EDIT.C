#include <string.h>
#include <stdlib.h>
#include "EDIT.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRAW.H"
#include "KBD.H"
#include "KEYMAPS.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

static char g_editDigits = 0;

/* Line editor. Edit the string buf (at most max characters, clipped to the width of
   the back bitmap) at x, y on the back page until Enter or a negative event; backspace, printable
   characters 0x20..0x5A (digits only when g_editDigits), a blinking block cursor. Returns the last key. */

int ed_line(int x, int y, char *buf, int max)
{
    char colr;
    char dest[2];
    char pos, oc;
    int end, w, px;
    char ip;
    int key, mono, cx, ret, cy, n;

    pos = x_colors.fill;
    oc = x_colors.line;
    mono = (unsigned char)(r_vid.back->kind <= BITMAP_WORK);
    if ((w = 6 * max) + x >= r_vid.back->width) {
        max = (r_vid.back->width - x - 1) / 6;
        buf[max] = 0;
        w = 6 * max;
    }
    x_colors.fill = x_colors.textBg;
    x_colors.line = x_colors.text;
    cu_ctl(CURSOR_HIDE);
    dr_fill_rect(r_vid.back, x, y, w, 8);
    (*g_videoOps.drawText)(r_vid.back, x, y, buf);
    dr_vline(r_vid.back, cx = g_penPoint.x, cy = g_penPoint.y, 8);
    colr = -1;
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, x, y, x + w, y + 8);
    ip = g_msgClickToEdit[0];
    g_msgClickToEdit[0] = 0;
    sy_hover_reset();
    ret = sy_keymap_restore(1);
    kb_text_on(g_editDigits);
    n = g_input.any;
    g_input.any = 1;
    g_input.key = 0;
    dest[1] = 0;
    end = strlen(buf);
    for (;; g_input.key = 0) {
        if ((key = sy_wait_input(0x23)) != 0) {
            if (key < 0)
                break;
            key = g_input.key;
            if (key == 13) {
                if (!g_editDigits || *buf)
                    break;
                so_buzz();
                continue;
            }
            if (key != 0) {
                if (key != 8) {
                    if (key < 0x20 || key > 0x5a)
                        continue;
                    if (g_editDigits && (key < 0x30 || key > 0x39)) {
                        so_buzz();
                        continue;
                    }
                }
                cu_ctl(CURSOR_HIDE);
                x_colors.line = x_colors.textBg;
                dr_vline(r_vid.back, cx, cy, 8);
                if (key == 8) {
                    if (end > 0) {
                        dest[0] = buf[--end];
                        cx -= da_text_width(dest);
                        px = cx;
                        (*g_videoOps.drawText)(r_vid.back, cx, cy, " ");
                    } else
                        so_buzz();
                } else if (end < max) {
                    buf[end++] = key;
                    buf[end] = 0;
                    (*g_videoOps.drawText)(r_vid.back, px = cx, cy, buf + end - 1);
                    cx = g_penPoint.x;
                } else
                    so_buzz();
                x_colors.line = x_colors.text;
                dr_vline(r_vid.back, cx, cy, 8);
                colr = -1;
                cu_ctl(CURSOR_SHOW);
                if (r_vid.copyToFront)
                    bm_copy_box(r_vid.back, r_vid.front, px, cy, px + 6, cy + 8);
            }
        } else {
            cu_ctl(CURSOR_HIDE);
            x_colors.line = colr ? x_colors.textBg : x_colors.text;
            dr_vline(r_vid.back, cx, cy, 8);
            colr = ~colr;
            cu_ctl(CURSOR_SHOW);
            if (r_vid.copyToFront)
                bm_copy_box(r_vid.back, r_vid.front, cx, cy, cx + 1, cy + 8);
        }
    }
    buf[end] = 0;
    cu_ctl(CURSOR_HIDE);
    x_colors.line = x_colors.textBg;
    dr_vline(r_vid.back, cx, cy, 8);
    cu_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bm_copy_box(r_vid.back, r_vid.front, cx - 6, y, cx, y + 8);
    x_colors.line = oc;
    x_colors.fill = pos;
    g_msgClickToEdit[0] = ip;
    sy_hover_reset();
    g_input.any = n;
    kb_text_off();
    if (ret)
        sy_keymap_set(ret);
    return key;
}

/* Number prompt: draw the prompt s at (x, y) in the style's colours, let the player edit a
   text of up to 20 characters at (g_penPoint.x, g_penPoint.y) and store its value in *out. Returns the result
   of the editor when that is 0 or negative, else 0 */

int ed_number(int x, int y, char *s, int *out)
{
    char buf[20];
    int rl;

    x_colors = bustyle->schemes[STYLE_TEXT];
    sc_text(x, y, s);
    g_editDigits = 1;
    buf[0] = 0;
    rl = ed_line(g_penPoint.x, g_penPoint.y, buf, 20);
    g_editDigits = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* Number field of the current hot spot g_menuList.header: edit a text into buf (at most 10 characters,
   fewer when the box is narrow: 6 pixels each) with the spot's colours and store its value in *out.
   Returns the editor's result when that is 0 or negative, else 0 */

int ed_spot_number(int *out, char *buf)
{
    int rl;

    x_colors = bustyle->schemes[g_menuList.header->styleIndex];
    g_editDigits = 1;
    rl = ed_line(g_menuList.header->x1, g_menuList.header->y1, buf,
                 (g_menuList.header->x2 - g_menuList.header->x1) / 6 > 10
                     ? 10
                     : (g_menuList.header->x2 - g_menuList.header->x1) / 6);
    g_editDigits = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* Show hot spot id, read a number into *out with ed_spot_number and close the spot again;
   returns the editor's result */

int ed_number_box(enum ScreenId id, int *out)
{
    int res;
    char buf[12];

    mn_push(id);
    buf[0] = 0;
    res = ed_spot_number(out, buf);
    mn_pop(1);
    return res;
}

/* Show hot spot id and let the player edit the text s (at most len characters) in its box
   with the spot's colours; returns the editor's result when that is 0 or negative, else 0 */

int ed_text_box(enum ScreenId id, int len, char *s)
{
    int rl;

    mn_push(id);
    x_colors = bustyle->schemes[g_menuList.header->styleIndex];
    rl = ed_line(g_menuList.header->x1, g_menuList.header->y1, s, len);
    mn_pop(1);
    return rl > 0 ? 0 : rl;
}
