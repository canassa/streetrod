/* SCREEN.H - declarations of what SCREEN.C defines. */

#ifndef SCREEN_H
#define SCREEN_H
#include "STRUCTS.H"

extern struct TextEnd g_penPoint;
extern struct StatusText g_statusLine;

#define SCREEN_BODY_HEIGHT 190
#define STATUS_BAR_HEIGHT 10

extern char *g_gearNames[];
extern char g_promptPalette[];
extern struct PicturePlacement g_picturePlacement;
extern struct DriverState g_driver;
extern char g_displaySplitYVal;
extern int g_xMoneyRhsVal;
void sc_status_init(int y);
void sc_status_text(int x, char *s, int w);
void sc_add_money(int n);
void sc_label(char *s);
void sc_show_gear(int i);
void sc_fill_rect(int x, int y, int w, int h, char color);
void sc_msg_box(char *s);
void sc_wait_msg(char *s);
int sc_picture(int a, int b);
int sc_picture_at(int a, int b, int c, int d);
void sc_text(int x, int y, char *s);
void sc_hline(int x, int y, int w);

#endif
