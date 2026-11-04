/* SCREEN.H - declarations of what SCREEN.C defines. */

#ifndef SCREEN_H
#define SCREEN_H
#include "STRUCTS.H"

extern struct TextEnd g_penPoint; /* DS:824C */
extern struct StatusText g_statusLine; /* DS:8250 */

#define SCREEN_BODY_HEIGHT 190
#define STATUS_BAR_HEIGHT 10

extern char *g_gearNames[];                             /* DS:0482 */
extern char g_promptPalette[];                          /* DS:04AC */
extern struct PicturePlacement g_picturePlacement; /* DS:04BE */
extern struct DriverState g_driver;                        /* DS:7EAA */
extern char g_displaySplitYVal;                                    /* DS:824A */
extern int g_xMoneyRhsVal;                                      /* DS:82C0 */
void sc_status_init(int y);                         /* 0x113A2 */
void sc_status_text(int x, char *s, int w);                    /* 0x11421 */
void sc_add_money(int n);                                    /* 0x11540 */
void sc_label(char *s);                                  /* 0x11593 */
void sc_show_gear(int i);                           /* 0x11669 */
void sc_fill_rect(int x, int y, int w, int h, char color);/* 0x1169A */
void sc_msg_box(char *s);                           /* 0x1171C */
void sc_wait_msg(char *s);                          /* 0x1174E */
int sc_picture(int a, int b);                              /* 0x118D4 */
int sc_picture_at(int a, int b, int c, int d);                /* 0x119B8 */
void sc_text(int x, int y, char *s);                /* 0x119D6 */
void sc_hline(int x, int y, int w);                 /* 0x11A24 */

#endif
