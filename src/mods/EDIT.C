#include <string.h>
#include <stdlib.h>
#include "STRUCTS.H"
#include "EDIT.H"
#include "BITMAP.H"
#include "CURSOR.H"
#include "DASH.H"
#include "DRAW.H"
#include "KBD.H"
#include "MAIN.H"
#include "MENU.H"
#include "MESSAGES.H"
#include "SCREEN.H"
#include "SOUND.H"
#include "SYS.H"
#include "VIDEO.H"

char g_editDigits = 0;   /* DS:09F2 */

/* edit_line @ 0F38:5910 (linear 0x14C90, 1063 bytes)

   Line editor. Edit the string buf (at most max characters, clipped to the width of
   the back bitmap) at x, y on the back page until Enter or a negative event; backspace, printable
   characters 0x20..0x5A (digits only when g_editDigits), a blinking block cursor. Returns the last key. */

int edit_line(int x, int y, char *buf, int max)
{
    char colr;
    char dest[2];
    char pos, oc;
    int end, w, px;
    char ip;
    int key, mono, cx, ret, cy, n;

    pos = x_colors.fill;
    oc = x_colors.line;
    mono = (unsigned char)(r_vid.back->flags <= -1);
    if ((w = 6 * max) + x >= r_vid.back->width) {
        max = (r_vid.back->width - x - 1) / 6;
        buf[max] = 0;
        w = 6 * max;
    }
    x_colors.fill = x_colors.textBg;
    x_colors.line = x_colors.text;
    cursor_ctl(CURSOR_HIDE);
    draw_fill_rect(r_vid.back, x, y, w, 8);
    (*a_78aem)(r_vid.back, x, y, buf);
    draw_vline(r_vid.back, cx = g_xTextR, cy = g_yLastString, 8);
    colr = -1;
    cursor_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bitmap_copy_box(r_vid.back, r_vid.front, x, y, x + w, y + 8);
    ip = g_msgClickToEdit[0];
    g_msgClickToEdit[0] = 0;
    sys_hover_reset();
    ret = sys_keymap_restore(1);
    kbd_text_on(g_editDigits);
    n = g_anyInput;
    g_anyInput = 1;
    g_inputKey = 0;
    dest[1] = 0;
    end = strlen(buf);
    for (;; g_inputKey = 0) {
        if ((key = sys_wait_input(0x23)) != 0) {
            if (key < 0)
                break;
            key = g_inputKey;
            if (key == 13) {
                if (!g_editDigits || *buf)
                    break;
                sound_buzz();
                continue;
            }
            if (key != 0) {
                if (key != 8) {
                    if (key < 0x20 || key > 0x5a)
                        continue;
                    if (g_editDigits && (key < 0x30 || key > 0x39)) {
                        sound_buzz();
                        continue;
                    }
                }
                cursor_ctl(CURSOR_HIDE);
                x_colors.line = x_colors.textBg;
                draw_vline(r_vid.back, cx, cy, 8);
                if (key == 8) {
                    if (end > 0) {
                        dest[0] = buf[--end];
                        cx -= dash_text_width(dest);
                        px = cx;
                        (*a_78aem)(r_vid.back, cx, cy, " ");
                    } else
                        sound_buzz();
                } else if (end < max) {
                    buf[end++] = key;
                    buf[end] = 0;
                    (*a_78aem)(r_vid.back, px = cx, cy, buf + end - 1);
                    cx = g_xTextR;
                } else
                    sound_buzz();
                x_colors.line = x_colors.text;
                draw_vline(r_vid.back, cx, cy, 8);
                colr = -1;
                cursor_ctl(CURSOR_SHOW);
                if (r_vid.copyToFront)
                    bitmap_copy_box(r_vid.back, r_vid.front, px, cy, px + 6, cy + 8);
            }
        } else {
            cursor_ctl(CURSOR_HIDE);
            x_colors.line = colr ? x_colors.textBg : x_colors.text;
            draw_vline(r_vid.back, cx, cy, 8);
            colr = ~colr;
            cursor_ctl(CURSOR_SHOW);
            if (r_vid.copyToFront)
                bitmap_copy_box(r_vid.back, r_vid.front, cx, cy, cx + 1, cy + 8);
        }
    }
    buf[end] = 0;
    cursor_ctl(CURSOR_HIDE);
    x_colors.line = x_colors.textBg;
    draw_vline(r_vid.back, cx, cy, 8);
    cursor_ctl(CURSOR_SHOW);
    if (r_vid.copyToFront)
        bitmap_copy_box(r_vid.back, r_vid.front, cx - 6, y, cx, y + 8);
    x_colors.line = oc;
    x_colors.fill = pos;
    g_msgClickToEdit[0] = ip;
    sys_hover_reset();
    g_anyInput = n;
    kbd_text_off();
    if (ret)
        sys_keymap_set(ret);
    return key;
}

/* edit_number @ 0F38:5D37 (linear 0x150B7, 112 bytes)

   Number prompt: draw the prompt s at (x, y) in the style's colours, let the player edit a
   text of up to 20 characters at (g_xTextR, g_yLastString) and store its value in *out. Returns the result
   of the editor when that is 0 or negative, else 0 */

int edit_number(int x, int y, char *s, int *out)
{
    char buf[20];
    int rl;

    x_colors = bustyle->pal[5];
    screen_text(x, y, s);
    g_editDigits = 1;
    buf[0] = 0;
    rl = edit_line(g_xTextR, g_yLastString, buf, 20);
    g_editDigits = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* edit_spot_number @ 0F38:5DA7 (linear 0x15127, 119 bytes)

   Number field of the current hot spot g_hotTextFld: edit a text into buf (at most 10 characters,
   fewer when the box is narrow: 6 pixels each) with the spot's colours and store its value in *out.
   Returns the editor's result when that is 0 or negative, else 0 */

int edit_spot_number(int *out, char *buf)
{
    int rl;

    x_colors = bustyle->pal[g_hotTextFld->palIndex];
    g_editDigits = 1;
    rl = edit_line(g_hotTextFld->x1, g_hotTextFld->y1, buf,
        (g_hotTextFld->x2 - g_hotTextFld->x1) / 6 > 10 ? 10 : (g_hotTextFld->x2 - g_hotTextFld->x1) / 6);
    g_editDigits = 0;
    *out = atoi(buf);
    return rl > 0 ? 0 : rl;
}

/* edit_number_box @ 0F38:5E1E (linear 0x1519E, 54 bytes)

   Show hot spot id, read a number into *out with edit_spot_number and close the spot again;
   returns the editor's result */

int edit_number_box(int id, int *out)
{
    int res;
    char buf[12];

    menu_push(id);
    buf[0] = 0;
    res = edit_spot_number(out, buf);
    menu_pop(1);
    return res;
}

/* edit_text_box @ 0F38:5E54 (linear 0x151D4, 98 bytes)

   Show hot spot id and let the player edit the text s (at most len characters) in its box
   with the spot's colours; returns the editor's result when that is 0 or negative, else 0 */

int edit_text_box(int id, int len, char *s)
{
    int rl;

    menu_push(id);
    x_colors = bustyle->pal[g_hotTextFld->palIndex];
    rl = edit_line(g_hotTextFld->x1, g_hotTextFld->y1, s, len);
    menu_pop(1);
    return rl > 0 ? 0 : rl;
}
