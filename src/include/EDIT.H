/* EDIT.H - declarations of what EDIT.C defines. */

#ifndef EDIT_H
#define EDIT_H
#include "STRUCTS.H"

extern char g_09f2;                                     /* DS:09F2 */
int F_14C90(int x, int y, char *s, int color);          /* 0x14C90 */
int F_150B7(int x, int y, char *s, int *out);           /* 0x150B7 */
int F_15127(int *out, char *buf);                       /* 0x15127 */
int F_1519E(int id, int *out);                          /* 0x1519E */
int F_151D4(int id, int len, char *s);                  /* 0x151D4 */

#endif
