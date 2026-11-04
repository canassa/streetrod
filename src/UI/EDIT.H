/* EDIT.H - declarations of what EDIT.C defines. */

#ifndef EDIT_H
#define EDIT_H
#include "STRUCTS.H"
#include "KEYMAPS.H"

extern char g_editDigits;                               /* DS:09F2 */
int ed_line(int x, int y, char *s, int color);        /* 0x14C90 */
int ed_number(int x, int y, char *s, int *out);       /* 0x150B7 */
int ed_spot_number(int *out, char *buf);              /* 0x15127 */
int ed_number_box(enum ScreenId id, int *out);                  /* 0x1519E */
int ed_text_box(enum ScreenId id, int len, char *s);            /* 0x151D4 */

#endif
