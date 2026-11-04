/* EDIT.H - declarations of what EDIT.C defines. */

#ifndef EDIT_H
#define EDIT_H
#include "STRUCTS.H"
#include "KEYMAPS.H"

extern char g_editDigits;
int ed_line(int x, int y, char *s, int color);
int ed_number(int x, int y, char *s, int *out);
int ed_spot_number(int *out, char *buf);
int ed_number_box(enum ScreenId id, int *out);
int ed_text_box(enum ScreenId id, int len, char *s);

#endif
